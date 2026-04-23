/*
 * esp32_encrip.ino
 * Proyecto de demostración para autenticación web y control seguro de LED en ESP32.
 *
 * Idea general:
 * - El ESP32 crea su propia red WiFi.
 * - El usuario entra a una página web local.
 * - El acceso se valida con hashes SHA-256 guardados en NVS.
 * - Si el login es correcto, se habilita el panel para controlar el LED.
 */

#include <WiFi.h>
#include <WebServer.h>
#include "config.h"
#include "credentials.h"
#include "web_pages.h"

#ifndef RGB_BUILTIN
#define RGB_BUILTIN PIN_LED_PRINCIPAL
#endif

// ---- Servidor HTTP ----
// El sitio web del proyecto se publica en el puerto 80.
WebServer servidor(80);

// ---- Estado de sesión ----
// La sesión se mantiene solo en RAM mientras el equipo esté encendido.
static String tokenSesion  = "";
static bool   sesionActiva = false;

// ---- Estado del LED ----
// Esta variable permite reflejar el estado del LED en la interfaz web.
static bool ledEncendido = false;

// ---- Control del LED principal RGB ----
// Esta placa usa un WS2812 integrado en GPIO48.
void escribirLedPrincipal(uint8_t rojo, uint8_t verde, uint8_t azul) {
    rgbLedWrite(RGB_BUILTIN, rojo, verde, azul);
}

// ---- Estado visual de la placa ----
// Se usa solo durante el arranque y diagnostico.
void mostrarEstadoPlaca(uint8_t rojo, uint8_t verde, uint8_t azul) {
    escribirLedPrincipal(rojo, verde, azul);
}

// ---- Sincroniza el estado del LED controlado por el usuario ----
// Se actualiza tanto el RGB integrado como la salida auxiliar.
void aplicarEstadoLedUsuario() {
    digitalWrite(PIN_LED_AUXILIAR, ledEncendido ? HIGH : LOW);
    escribirLedPrincipal(ledEncendido ? 0 : 0, ledEncendido ? 48 : 0, ledEncendido ? 12 : 0);
}

// ---- Genera el token de la sesión ----
// Se usa un valor aleatorio para identificar al usuario autenticado.
String generarTokenSesion() {
    String token = "";

    for (int i = 0; i < 16; i++) {
        char buffer[3];
        snprintf(buffer, sizeof(buffer), "%02x", static_cast<uint8_t>(esp_random()));
        token += buffer;
    }

    return token;
}

// ---- Devuelve la IP del servidor web ----
// Como el ESP32 trabaja en modo AP, la IP se obtiene desde softAPIP().
String obtenerIpServidor() {
    return WiFi.softAPIP().toString();
}

// ---- Calcula el tiempo activo del equipo ----
// Se muestra en la landing y en el panel para evidenciar que el sistema está en ejecución.
String obtenerTiempoActivo() {
    const unsigned long totalSegundos = millis() / 1000UL;
    const unsigned long horas         = totalSegundos / 3600UL;
    const unsigned long minutos       = (totalSegundos % 3600UL) / 60UL;
    const unsigned long segundos      = totalSegundos % 60UL;

    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%02lu:%02lu:%02lu", horas, minutos, segundos);
    return String(buffer);
}

// ---- Construye mensajes HTML reutilizables ----
// Esto evita repetir el mismo formato visual en varias vistas.
String construirMensajeHtml(const String& tipo, const String& texto) {
    if (texto.isEmpty()) {
        return "";
    }

    return "<div class='msg " + tipo + "'>" + texto + "</div>";
}

// ---- Construye la landing principal ----
// Muestra el estado del Access Point y el formulario de login.
String construirLoginHtml(const String& error, const String& warning, const String& success) {
    String html = String(LOGIN_HTML);

    html.replace("%AP_SSID%", AP_SSID);
    html.replace("%AP_IP%", obtenerIpServidor());
    html.replace("%UPTIME%", obtenerTiempoActivo());
    html.replace("%STATUS_CREDENCIALES%",
                 usaCredencialesPorDefecto()
                     ? "<div class='state warn'>Acceso inicial disponible.</div>"
                     : "<div class='state ok'>Acceso listo.</div>");
    html.replace("%CTA_SECUNDARIO%",
                 usaCredencialesPorDefecto()
                     ? "<a class='btn btn-secondary' href='/registro'>Crear acceso inicial</a>"
                     : "<a class='btn btn-secondary' href='/evidencia'>Ver evidencia SHA-256</a>");
    html.replace("%ACCESO_NOTA%",
                 usaCredencialesPorDefecto()
                     ? "Tambien puedes iniciar sesion si ya conoces las credenciales."
                     : "La evidencia sigue disponible aparte.");
    html.replace("%LOGIN_ERROR%", construirMensajeHtml("error", error));
    html.replace("%LOGIN_WARNING%", construirMensajeHtml("warn", warning));
    html.replace("%LOGIN_SUCCESS%", construirMensajeHtml("success", success));

    return html;
}

// ---- Construye el panel de control ----
// Aquí se muestran el estado del LED y los datos básicos de la red del ESP32.
String construirPanelHtml(const String& mensaje, const String& tipoMensaje) {
    String html = String(PANEL_HTML);

    html.replace("%AP_SSID%", AP_SSID);
    html.replace("%AP_IP%", obtenerIpServidor());
    html.replace("%UPTIME%", obtenerTiempoActivo());
    html.replace("%PANEL_MSG%", construirMensajeHtml(tipoMensaje, mensaje));
    html.replace("%STATUS_CREDENCIALES%",
                 usaCredencialesPorDefecto()
                     ? "<div class='state warn'>Sigues con el acceso inicial.</div>"
                     : "<div class='state ok'>Sesion protegida.</div>");

    if (ledEncendido) {
        html.replace("%ICONO_LED%", "💡");
        html.replace("%ESTADO_LED%", "ENCENDIDO");
        html.replace("%COLOR_LED%", "#22c55e");
        html.replace("%LED_DESCRIPCION%", "LED principal activo.");
    } else {
        html.replace("%ICONO_LED%", "⚫");
        html.replace("%ESTADO_LED%", "APAGADO");
        html.replace("%COLOR_LED%", "#64748b");
        html.replace("%LED_DESCRIPCION%", "LED principal apagado.");
    }

    html.replace("%LED_SALIDA%", "Control del RGB principal.");

    return html;
}

// ---- Construye la vista para actualizar credenciales ----
String construirCambioHtml(const String& mensaje, const String& tipoMensaje) {
    String html = String(CAMBIAR_HTML);
    html.replace("%CAMBIO_MENSAJE%", construirMensajeHtml(tipoMensaje, mensaje));
    return html;
}

// ---- Construye la vista de registro inicial ----
// Esta ruta existe para que el proyecto muestre un flujo completo y demostrable.
String construirRegistroHtml(const String& mensaje, const String& tipoMensaje) {
    String html = String(REGISTRO_HTML);
    html.replace("%STATUS_CREDENCIALES%",
                 usaCredencialesPorDefecto()
                     ? "<div class='state warn'>Crea tu acceso.</div>"
                     : "<div class='state ok'>Ya existe una cuenta.</div>");
    html.replace("%REGISTRO_MSG%", construirMensajeHtml(tipoMensaje, mensaje));
    return html;
}

// ---- Construye la vista de evidencia SHA-256 ----
// Expone de forma academica lo que se guarda en NVS y como se comparan los hashes.
String construirEvidenciaHtml(const String& mensaje,
                              const String& tipoMensaje,
                              const String& hashUsuarioCalculado,
                              const String& hashContrasenaCalculada) {
    String html = String(EVIDENCIA_HTML);

    const bool usuarioCalculado    = !hashUsuarioCalculado.isEmpty();
    const bool contrasenaCalculada = !hashContrasenaCalculada.isEmpty();

    html.replace("%STATUS_CREDENCIALES%",
                 usaCredencialesPorDefecto()
                     ? "<div class='state warn'>Actualmente el sistema conserva los hashes de las credenciales iniciales. Puedes mostrar esto y luego registrar tus propias credenciales.</div>"
                     : "<div class='state ok'>Actualmente el sistema usa hashes personalizados almacenados en NVS. Esta es la evidencia directa de persistencia segura.</div>");
    html.replace("%EVIDENCIA_MSG%", construirMensajeHtml(tipoMensaje, mensaje));
    html.replace("%HASH_USUARIO_GUARDADO%", obtenerHashUsuarioGuardado());
    html.replace("%HASH_PASS_GUARDADO%", obtenerHashContrasenaGuardada());
    html.replace("%HASH_USUARIO_CALCULADO%", usuarioCalculado ? hashUsuarioCalculado : "Aun no calculado");
    html.replace("%HASH_PASS_CALCULADO%", contrasenaCalculada ? hashContrasenaCalculada : "Aun no calculado");
    html.replace("%MATCH_USUARIO%",
                 !usuarioCalculado
                     ? "<div class='pill warn'>Ingresa un usuario para calcular su SHA-256.</div>"
                     : (hashUsuarioCalculado == obtenerHashUsuarioGuardado()
                            ? "<div class='pill ok'>El hash del usuario SI coincide con el valor almacenado en NVS.</div>"
                            : "<div class='pill warn'>El hash del usuario NO coincide con el valor almacenado en NVS.</div>"));
    html.replace("%MATCH_PASS%",
                 !contrasenaCalculada
                     ? "<div class='pill warn'>Ingresa una contrasena para calcular su SHA-256.</div>"
                     : (hashContrasenaCalculada == obtenerHashContrasenaGuardada()
                            ? "<div class='pill ok'>El hash de la contrasena SI coincide con el valor almacenado en NVS.</div>"
                            : "<div class='pill warn'>El hash de la contrasena NO coincide con el valor almacenado en NVS.</div>"));

    return html;
}

// ---- Envía una respuesta HTML al navegador ----
void responderHtml(int codigoHttp, const String& html) {
    servidor.send(codigoHttp, "text/html; charset=utf-8", html);
}

// ---- Redirige a otra ruta interna ----
void redirigir(const String& ruta) {
    servidor.sendHeader("Location", ruta);
    servidor.send(302, "text/plain", "");
}

// ---- Cierra la sesión actual ----
// Se borra el token y se invalida la cookie enviada al navegador.
void cerrarSesionLocal() {
    sesionActiva = false;
    tokenSesion  = "";
    servidor.sendHeader("Set-Cookie", "session=; Path=/; Max-Age=0; HttpOnly; SameSite=Strict");
    Serial.println("[SERVER] Sesión cerrada.");
}

// ---- Comprueba si la sesión recibida es válida ----
// Solo se considera válida si el token coincide con la cookie del cliente.
bool sesionValida() {
    if (!sesionActiva || tokenSesion.isEmpty()) {
        return false;
    }

    const String cookie = servidor.header("Cookie");
    return cookie.indexOf("session=" + tokenSesion) >= 0;
}

// ---- Restringe el acceso a rutas protegidas ----
// Si no hay sesión válida, el usuario vuelve a la landing.
bool requerirSesion() {
    if (sesionValida()) {
        return true;
    }

    Serial.println("[SERVER] Acceso denegado: no hay una sesión válida.");
    redirigir("/");
    return false;
}

// ---- Genera el mensaje de bloqueo ----
// Se usa cuando el usuario ya agotó sus intentos de acceso.
String obtenerMensajeBloqueo() {
    if (!estaBloqueado()) {
        return "";
    }

    return "Acceso bloqueado temporalmente. Espera " + String(segundosBloqueoRestantes()) + " segundos antes de volver a intentarlo.";
}

// =============================================================
// Rutas HTTP del proyecto
// =============================================================

// ---- GET / ----
// Muestra la landing o redirige al panel si ya existe sesión.
void handleRaiz() {
    if (sesionValida()) {
        redirigir("/panel");
        return;
    }

    redirigir(usaCredencialesPorDefecto() ? "/registro" : "/login");
}

// ---- GET /login ----
// Mantiene compatibilidad si el usuario entra directamente a la ruta de login.
void handleLoginGet() {
    if (sesionValida()) {
        redirigir("/panel");
        return;
    }

    responderHtml(200, construirLoginHtml("", obtenerMensajeBloqueo(), ""));
}

// ---- GET /registro ----
// Permite reemplazar las credenciales iniciales por unas propias.
void handleRegistroGet() {
    if (sesionValida()) {
        redirigir("/panel");
        return;
    }

    if (!usaCredencialesPorDefecto()) {
        redirigir("/login");
        return;
    }

    responderHtml(200, construirRegistroHtml("", usaCredencialesPorDefecto() ? "warn" : "success"));
}

// ---- GET /evidencia ----
// Muestra la demostracion tecnica de SHA-256 y NVS.
void handleEvidenciaGet() {
    responderHtml(200, construirEvidenciaHtml("", "success", "", ""));
}

// ---- POST /login ----
// Valida el usuario y la contraseña comparando hashes SHA-256.
void handleLoginPost() {
    if (estaBloqueado()) {
        responderHtml(423, construirLoginHtml("", obtenerMensajeBloqueo(), ""));
        return;
    }

    String usuario    = servidor.arg("usuario");
    String contrasena = servidor.arg("contrasena");
    usuario.trim();
    contrasena.trim();

    if (usuario.isEmpty() || contrasena.isEmpty()) {
        responderHtml(400, construirLoginHtml("Debes ingresar usuario y contraseña.", "", ""));
        return;
    }

    if (validateLogin(usuario, contrasena)) {
        resetearIntentos();
        tokenSesion  = generarTokenSesion();
        sesionActiva = true;

        servidor.sendHeader("Set-Cookie", "session=" + tokenSesion + "; Path=/; HttpOnly; SameSite=Strict");
        Serial.println("[SERVER] Inicio de sesión correcto.");
        redirigir("/panel");
        return;
    }

    registrarIntentoFallido();

    if (estaBloqueado()) {
        responderHtml(423, construirLoginHtml("", obtenerMensajeBloqueo(), ""));
        return;
    }

    responderHtml(401,
                  construirLoginHtml("Credenciales incorrectas. Intentos restantes: " +
                                     String(MAX_INTENTOS - credenciales.intentosFallidos),
                                     "",
                                     ""));
}

// ---- POST /registro ----
// Completa el registro inicial mientras el sistema siga usando admin / 1234.
void handleRegistroPost() {
    String nuevoUsuario  = servidor.arg("nuevo_usuario");
    String nuevaPass     = servidor.arg("nueva_pass");
    String confirmarPass = servidor.arg("confirmar_pass");

    nuevoUsuario.trim();
    nuevaPass.trim();
    confirmarPass.trim();

    if (nuevoUsuario.isEmpty() || nuevaPass.isEmpty() || confirmarPass.isEmpty()) {
        responderHtml(400, construirRegistroHtml("Debes completar todos los campos del registro inicial.", "error"));
        return;
    }

    if (nuevaPass != confirmarPass) {
        responderHtml(400, construirRegistroHtml("La confirmacion de la contrasena no coincide.", "error"));
        return;
    }

    if (!usaCredencialesPorDefecto()) {
        responderHtml(403,
                      construirRegistroHtml("El registro inicial ya fue completado. Inicia sesion y usa la opcion Cambiar credenciales para cualquier ajuste posterior.",
                                            "warn"));
        return;
    }

    if (!registrarCredencialesIniciales(nuevoUsuario, nuevaPass)) {
        responderHtml(400,
                      construirRegistroHtml("No se detectaron cambios reales. Usa valores distintos a las credenciales iniciales para que la demostracion tenga sentido.",
                                            "warn"));
        return;
    }

    responderHtml(200,
                  construirLoginHtml("",
                                     "",
                                     "Registro completado. Las nuevas credenciales quedaron almacenadas como hashes SHA-256 en NVS. Ahora ya puedes iniciar sesion y abrir la vista de evidencia."));
}

// ---- POST /evidencia ----
// Calcula hashes en tiempo real para compararlos contra lo almacenado.
void handleEvidenciaPost() {
    String usuarioDemo    = servidor.arg("usuario_demo");
    String contrasenaDemo = servidor.arg("contrasena_demo");

    usuarioDemo.trim();
    contrasenaDemo.trim();

    const String hashUsuarioCalculado =
        usuarioDemo.isEmpty() ? "" : hashTexto(usuarioDemo);
    const String hashContrasenaCalculada =
        contrasenaDemo.isEmpty() ? "" : hashTexto(contrasenaDemo);

    if (hashUsuarioCalculado.isEmpty() && hashContrasenaCalculada.isEmpty()) {
        responderHtml(400,
                      construirEvidenciaHtml("Ingresa al menos un valor para que el ESP32 calcule su SHA-256 y puedas compararlo con lo almacenado.",
                                             "warn",
                                             "",
                                             ""));
        return;
    }

    responderHtml(200,
                  construirEvidenciaHtml("Hashes calculados en el propio ESP32. Usa las etiquetas de coincidencia para demostrar la comparacion contra NVS.",
                                         "success",
                                         hashUsuarioCalculado,
                                         hashContrasenaCalculada));
}

// ---- GET /panel ----
// Muestra el panel seguro si el usuario ya fue autenticado.
void handlePanel() {
    if (!requerirSesion()) {
        return;
    }

    responderHtml(200, construirPanelHtml("", "success"));
}

// ---- POST /led ----
// Cambia el estado del LED solo desde una sesión válida.
void handleLed() {
    if (!requerirSesion()) {
        return;
    }

    const String estado = servidor.arg("estado");
    if (estado == "on") {
        ledEncendido = true;
        aplicarEstadoLedUsuario();
        Serial.println("[LED] El LED principal fue encendido desde el panel.");
        responderHtml(200, construirPanelHtml("El LED principal de la placa fue encendido.", "success"));
        return;
    }

    if (estado == "off") {
        ledEncendido = false;
        aplicarEstadoLedUsuario();
        Serial.println("[LED] El LED principal fue apagado desde el panel.");
        responderHtml(200, construirPanelHtml("El LED principal de la placa fue apagado.", "success"));
        return;
    }

    Serial.println("[LED] Se recibió una orden inválida para el LED.");
    responderHtml(400, construirPanelHtml("No se reconoció la orden enviada al LED.", "error"));
}

// ---- POST /logout ----
// Cierra la sesión y vuelve a la landing.
void handleLogout() {
    cerrarSesionLocal();
    redirigir("/");
}

// ---- GET /cambiar ----
// Muestra el formulario para actualizar usuario y contraseña.
void handleCambiarGet() {
    if (!requerirSesion()) {
        return;
    }

    responderHtml(200, construirCambioHtml("", "success"));
}

// ---- POST /cambiar ----
// Valida las credenciales actuales y reemplaza los hashes por nuevos valores.
void handleCambiarPost() {
    if (!requerirSesion()) {
        return;
    }

    String usuarioActual  = servidor.arg("usuario_actual");
    String passActual     = servidor.arg("pass_actual");
    String nuevoUsuario   = servidor.arg("nuevo_usuario");
    String nuevaPass      = servidor.arg("nueva_pass");
    String confirmarPass  = servidor.arg("confirmar_pass");

    usuarioActual.trim();
    passActual.trim();
    nuevoUsuario.trim();
    nuevaPass.trim();
    confirmarPass.trim();

    if (usuarioActual.isEmpty() || passActual.isEmpty()) {
        responderHtml(400, construirCambioHtml("Debes validar primero el usuario y la contraseña actuales.", "error"));
        return;
    }

    if (!validateLogin(usuarioActual, passActual)) {
        responderHtml(401, construirCambioHtml("El usuario actual o la contraseña actual no coinciden.", "error"));
        return;
    }

    if (nuevoUsuario.isEmpty() && nuevaPass.isEmpty()) {
        responderHtml(400, construirCambioHtml("Debes escribir al menos un dato nuevo para reemplazar los hashes guardados.", "warn"));
        return;
    }

    if (!nuevaPass.isEmpty() && nuevaPass != confirmarPass) {
        responderHtml(400, construirCambioHtml("La confirmación de la nueva contraseña no coincide.", "error"));
        return;
    }

    if (!actualizarCredenciales(nuevoUsuario, nuevaPass)) {
        responderHtml(400, construirCambioHtml("No se detectaron cambios reales en las credenciales.", "warn"));
        return;
    }

    cerrarSesionLocal();
    responderHtml(200, construirLoginHtml("", "", "Credenciales actualizadas. Inicia sesión con los nuevos datos."));
}

// ---- Ruta no encontrada ----
// Cualquier ruta desconocida vuelve a la página principal.
void handleNoEncontrado() {
    Serial.println("[SERVER] Ruta no encontrada. Redirección a la landing.");
    redirigir("/");
}

// =============================================================
// Configuración de red
// =============================================================

// ---- Inicia el Access Point del ESP32 ----
// Aquí se crea la red local y se asigna la IP fija del servidor.
void setupWiFi() {
    Serial.println("[WiFi] Iniciando Access Point del ESP32...");
    mostrarEstadoPlaca(0, 0, 32);

    WiFi.persistent(false);
    WiFi.disconnect(true, true);
    WiFi.mode(WIFI_MODE_NULL);
    delay(250);
    WiFi.mode(WIFI_AP);
    WiFi.setSleep(false);
    WiFi.setTxPower(WIFI_POWER_19_5dBm);
    delay(250);

    if (!WiFi.softAPConfig(AP_IP, AP_GATEWAY, AP_MASK)) {
        Serial.println("[WiFi] No se pudo aplicar la IP fija. Se usará la configuración por defecto.");
    } else {
        Serial.println("[WiFi] IP fija aplicada correctamente.");
    }

    bool apIniciado = WiFi.softAP(AP_SSID, AP_PASSWORD, AP_CANAL, 0, AP_MAX_CLIENTES);

    if (!apIniciado) {
        mostrarEstadoPlaca(32, 16, 0);
        Serial.println("[WiFi] El AP con canal fijo no inicio. Reintentando con configuracion basica...");
        delay(250);
        apIniciado = WiFi.softAP(AP_SSID, AP_PASSWORD);
    }

    if (!apIniciado) {
        mostrarEstadoPlaca(32, 32, 0);
        Serial.println("[WiFi] El AP seguro no inicio. Activando AP de diagnostico...");
        delay(250);
        apIniciado = WiFi.softAP("ESP32_Control_LED_DIAG");
    }

    if (!apIniciado) {
        mostrarEstadoPlaca(32, 0, 0);
        Serial.println("[WiFi] Error critico: no se pudo iniciar ningun Access Point.");
        return;
    }

    mostrarEstadoPlaca(0, 32, 0);
    Serial.println("[WiFi] Access Point activo y listo para recibir clientes.");
    Serial.println("[WiFi] SSID: " + String(AP_SSID));
    Serial.println("[WiFi] Clave del Access Point configurada correctamente.");
    Serial.println("[WiFi] IP del servidor: " + obtenerIpServidor());
    Serial.printf("[WiFi] Clientes máximos permitidos: %d\n", AP_MAX_CLIENTES);
}

// ---- Registra las rutas del servidor ----
// Cada ruta se asocia a la función que atenderá la solicitud.
void setupServer() {
    static const char* headersPermitidos[] = {"Cookie"};
    servidor.collectHeaders(headersPermitidos, 1);

    servidor.on("/", HTTP_GET, handleRaiz);
    servidor.on("/login", HTTP_GET, handleLoginGet);
    servidor.on("/login", HTTP_POST, handleLoginPost);
    servidor.on("/registro", HTTP_GET, handleRegistroGet);
    servidor.on("/registro", HTTP_POST, handleRegistroPost);
    servidor.on("/evidencia", HTTP_GET, handleEvidenciaGet);
    servidor.on("/evidencia", HTTP_POST, handleEvidenciaPost);
    servidor.on("/panel", HTTP_GET, handlePanel);
    servidor.on("/led", HTTP_POST, handleLed);
    servidor.on("/logout", HTTP_POST, handleLogout);
    servidor.on("/cambiar", HTTP_GET, handleCambiarGet);
    servidor.on("/cambiar", HTTP_POST, handleCambiarPost);
    servidor.onNotFound(handleNoEncontrado);

    servidor.begin();
    Serial.println("[SERVER] Servidor web iniciado en el puerto 80.");
}

// =============================================================
// Inicio del sistema
// =============================================================

// ---- setup() ----
// Configura el hardware, carga credenciales, crea la red y arranca el servidor.
void setup() {
    Serial.begin(115200);
    delay(1200);
    mostrarEstadoPlaca(0, 0, 8);
    Serial.println("\n=== Demostración: autenticación web segura en ESP32 ===");

    pinMode(PIN_LED_AUXILIAR, OUTPUT);
    ledEncendido = false;
    aplicarEstadoLedUsuario();
    Serial.println("[HW] LED principal y salida auxiliar inicializados en estado APAGADO.");

    mostrarEstadoPlaca(16, 0, 16);
    loadCredentials();
    setupWiFi();
    setupServer();
    mostrarEstadoPlaca(0, 32, 8);
    delay(250);
    aplicarEstadoLedUsuario();

    Serial.println("[SISTEMA] El sistema está listo. Conéctate al WiFi del ESP32 y abre la IP mostrada.\n");
}

// ---- loop() ----
// Atiende continuamente las solicitudes del navegador.
void loop() {
    servidor.handleClient();
}
