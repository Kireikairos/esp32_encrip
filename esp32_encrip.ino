/*
 * esp32_encrip.ino
 * Servidor web con autenticación segura en ESP32.
 *
 * Funcionalidades:
 *  - Página web de login con usuario y contraseña
 *  - Credenciales almacenadas en NVS (Preferences) con hash SHA-256
 *  - Sesión por cookie de un solo token en RAM
 *  - Control de LED desde el panel web
 *  - Bloqueo automático tras 3 intentos fallidos (30 segundos)
 *  - Cambio de usuario y contraseña desde la interfaz
 *  - Fallback a Access Point si no conecta al WiFi configurado
 *
 * Librerías requeridas (incluidas con el core ESP32 de Arduino):
 *  WiFi.h, WebServer.h, Preferences.h, mbedtls/sha256.h
 */

#include <WiFi.h>
#include <WebServer.h>
#include "config.h"
#include "credentials.h"
#include "web_pages.h"

// ---- Servidor HTTP en puerto 80 ----
WebServer servidor(80);

// ---- Estado de sesión en RAM (se pierde al reiniciar) ----
static String tokenSesion  = "";
static bool   sesionActiva = false;

// ---- Estado actual del LED ----
static bool ledEncendido = false;

// ---- Genera token hexadecimal aleatorio de 16 bytes usando hardware RNG ----
static String generarToken() {
    String token = "";
    for (int i = 0; i < 16; i++) {
        char buf[3];
        sprintf(buf, "%02x", (uint8_t)esp_random());
        token += buf;
    }
    return token;
}

// ---- Verifica si la solicitud tiene una cookie de sesión válida ----
static bool sesionValida() {
    if (!sesionActiva || tokenSesion.isEmpty()) return false;
    String cookie = servidor.header("Cookie");
    return cookie.indexOf("session=" + tokenSesion) >= 0;
}

// ---- Redirige a /login si no hay sesión activa; retorna false al caller ----
static bool requerirSesion() {
    if (!sesionValida()) {
        servidor.sendHeader("Location", "/login");
        servidor.send(302, "text/plain", "");
        return false;
    }
    return true;
}

// =============================================================
// Handlers HTTP
// =============================================================

// ---- GET / → redirige según estado de sesión ----
void handleRaiz() {
    servidor.sendHeader("Location", sesionValida() ? "/panel" : "/login");
    servidor.send(302, "text/plain", "");
}

// ---- GET /login ----
void handleLoginGet() {
    String html = String(LOGIN_HTML);
    html.replace("%ERROR%",   "");
    html.replace("%WARNING%", "");
    servidor.send(200, "text/html", html);
}

// ---- POST /login ----
void handleLoginPost() {
    // Verificar bloqueo activo antes de procesar
    if (estaBloqueado()) {
        String html = String(LOGIN_HTML);
        html.replace("%ERROR%",   "");
        html.replace("%WARNING%", "<div class='msg warn'>⛔ Bloqueado por " +
                                  String(segundosBloqueo()) + " s más.</div>");
        servidor.send(403, "text/html", html);
        return;
    }

    String usuario    = servidor.arg("usuario");
    String contrasena = servidor.arg("contrasena");

    if (validateLogin(usuario, contrasena)) {
        // ---- Login correcto: crear sesión ----
        resetearIntentos();
        tokenSesion  = generarToken();
        sesionActiva = true;

        servidor.sendHeader("Set-Cookie", "session=" + tokenSesion + "; Path=/; HttpOnly");
        servidor.sendHeader("Location",   "/panel");
        servidor.send(302, "text/plain", "");
        Serial.println("[SERVER] Sesión iniciada. Token: " + tokenSesion);

    } else {
        // ---- Login fallido: registrar intento ----
        registrarIntentoFallido();
        int restantes = MAX_INTENTOS - credenciales.intentos;

        String msg;
        if (credenciales.intentos >= MAX_INTENTOS) {
            msg = "<div class='msg error'>❌ Bloqueado " +
                  String(TIEMPO_BLOQUEO / 1000) + " s por exceso de intentos.</div>";
        } else {
            msg = "<div class='msg error'>❌ Credenciales incorrectas. "
                  "Intentos restantes: " + String(restantes) + "</div>";
        }

        String html = String(LOGIN_HTML);
        html.replace("%ERROR%",   msg);
        html.replace("%WARNING%", "");
        servidor.send(401, "text/html", html);
    }
}

// ---- GET /panel ----
void handlePanel() {
    if (!requerirSesion()) return;

    String html = String(PANEL_HTML);
    html.replace("%USUARIO%", credenciales.usuario);
    html.replace("%SUCCESS%", "");

    if (ledEncendido) {
        html.replace("%LED_ICON%",  "💡");
        html.replace("%LED_STATE%", "ENCENDIDO");
        html.replace("%LED_COLOR%", "#2ecc71");
    } else {
        html.replace("%LED_ICON%",  "⚫");
        html.replace("%LED_STATE%", "APAGADO");
        html.replace("%LED_COLOR%", "#e74c3c");
    }

    servidor.send(200, "text/html", html);
}

// ---- POST /led ----
void handleLed() {
    if (!requerirSesion()) return;

    String estado = servidor.arg("estado");
    if (estado == "on") {
        ledEncendido = true;
        digitalWrite(PIN_LED, HIGH);
        Serial.println("[LED] Encendido.");
    } else if (estado == "off") {
        ledEncendido = false;
        digitalWrite(PIN_LED, LOW);
        Serial.println("[LED] Apagado.");
    }

    servidor.sendHeader("Location", "/panel");
    servidor.send(302, "text/plain", "");
}

// ---- POST /logout ----
void handleLogout() {
    sesionActiva = false;
    tokenSesion  = "";
    servidor.sendHeader("Set-Cookie", "session=; Path=/; Expires=Thu, 01 Jan 1970 00:00:00 GMT");
    servidor.sendHeader("Location",   "/login");
    servidor.send(302, "text/plain", "");
    Serial.println("[SERVER] Sesión cerrada.");
}

// ---- GET /cambiar ----
void handleCambiarGet() {
    if (!requerirSesion()) return;
    String html = String(CAMBIAR_HTML);
    html.replace("%MENSAJE%", "");
    servidor.send(200, "text/html", html);
}

// ---- POST /cambiar ----
void handleCambiarPost() {
    if (!requerirSesion()) return;

    String passActual    = servidor.arg("pass_actual");
    String nuevoUsuario  = servidor.arg("nuevo_usuario");
    String nuevaPass     = servidor.arg("nueva_pass");
    String confirmarPass = servidor.arg("confirmar_pass");
    String html          = String(CAMBIAR_HTML);

    // Verificar contraseña actual antes de cualquier cambio
    if (!validateLogin(credenciales.usuario, passActual)) {
        html.replace("%MENSAJE%", "<div class='msg error'>❌ Contraseña actual incorrecta.</div>");
        servidor.send(401, "text/html", html);
        return;
    }

    // Verificar que las contraseñas nuevas coincidan
    if (!nuevaPass.isEmpty() && nuevaPass != confirmarPass) {
        html.replace("%MENSAJE%", "<div class='msg error'>❌ Las contraseñas no coinciden.</div>");
        servidor.send(400, "text/html", html);
        return;
    }

    // Aplicar cambios si se proporcionaron
    bool cambio = false;
    if (!nuevoUsuario.isEmpty()) {
        credenciales.usuario = nuevoUsuario;
        cambio = true;
        Serial.println("[AUTH] Usuario actualizado: " + nuevoUsuario);
    }
    if (!nuevaPass.isEmpty()) {
        credenciales.hashContrasena = hashPassword(nuevaPass);
        cambio = true;
        Serial.println("[AUTH] Contraseña actualizada (hash SHA-256 guardado).");
    }

    if (cambio) saveCredentials();

    html.replace("%MENSAJE%", "<div class='msg success'>✅ Credenciales actualizadas.</div>");
    servidor.send(200, "text/html", html);
}

// =============================================================
// Configuración WiFi con fallback a Access Point
// =============================================================
void setupWiFi() {
    Serial.println("[WiFi] Conectando a: " + String(WIFI_SSID));
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int intentos = 0;
    while (WiFi.status() != WL_CONNECTED && intentos < 20) {
        delay(500);
        Serial.print(".");
        intentos++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[WiFi] Conectado. IP: " + WiFi.localIP().toString());
    } else {
        Serial.println("\n[WiFi] Sin conexión. Iniciando como Access Point...");
        WiFi.softAP("ESP32_Control", "esp32admin");
        Serial.println("[WiFi] AP activo. IP: " + WiFi.softAPIP().toString());
        Serial.println("[WiFi] SSID: ESP32_Control | Pass: esp32admin");
    }
}

// =============================================================
// Registro de rutas y arranque del servidor web
// =============================================================
void setupServer() {
    // Habilitar lectura del header Cookie en las solicitudes entrantes
    static const char* headersPermitidos[] = {"Cookie"};
    servidor.collectHeaders(headersPermitidos, 1);

    servidor.on("/",        HTTP_GET,  handleRaiz);
    servidor.on("/login",   HTTP_GET,  handleLoginGet);
    servidor.on("/login",   HTTP_POST, handleLoginPost);
    servidor.on("/panel",   HTTP_GET,  handlePanel);
    servidor.on("/led",     HTTP_POST, handleLed);
    servidor.on("/logout",  HTTP_POST, handleLogout);
    servidor.on("/cambiar", HTTP_GET,  handleCambiarGet);
    servidor.on("/cambiar", HTTP_POST, handleCambiarPost);

    servidor.begin();
    Serial.println("[SERVER] Servidor web iniciado en puerto 80.");
}

// =============================================================
void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=== ESP32 Control Web ===");

    // Inicializar pin del LED
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, LOW);
    Serial.println("[HW] LED en GPIO " + String(PIN_LED) + " listo.");

    // Cargar credenciales desde NVS (o usar defaults si es primer arranque)
    loadCredentials();

    // Conectar a red WiFi (o crear AP si falla)
    setupWiFi();

    // Registrar rutas e iniciar servidor HTTP
    setupServer();

    Serial.println("[SISTEMA] Listo. Abre el navegador en la IP indicada.\n");
}

void loop() {
    // Atender solicitudes HTTP entrantes
    servidor.handleClient();
}
