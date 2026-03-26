// credentials.h - Gestión de credenciales con NVS y hash SHA-256
#pragma once

#include <Preferences.h>
#include "mbedtls/sha256.h"
#include "config.h"

// ---- Estructura de credenciales en memoria RAM ----
struct Credenciales {
    String        usuario;
    String        hashContrasena;
    int           intentos;
    unsigned long tiempoBloqueo;  // millis() en que se activó el bloqueo
};

Credenciales credenciales;
Preferences  prefs;

// ---- Genera hash SHA-256 de una cadena y lo retorna en hexadecimal ----
String hashPassword(const String& password) {
    unsigned char hash[32];
    mbedtls_sha256_context ctx;

    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0);  // 0 = SHA-256 (no SHA-224)
    mbedtls_sha256_update(&ctx, (const unsigned char*)password.c_str(), password.length());
    mbedtls_sha256_finish(&ctx, hash);
    mbedtls_sha256_free(&ctx);

    String hex = "";
    for (int i = 0; i < 32; i++) {
        char buf[3];
        sprintf(buf, "%02x", hash[i]);
        hex += buf;
    }
    return hex;
}

// ---- Carga credenciales desde NVS ----
void loadCredentials() {
    prefs.begin("auth", true);  // modo solo lectura
    credenciales.usuario        = prefs.getString("usuario",  DEFAULT_USUARIO);
    credenciales.hashContrasena = prefs.getString("passHash", hashPassword(DEFAULT_PASS));
    credenciales.intentos       = prefs.getInt("intentos", 0);
    prefs.end();

    // Si arrancó con bloqueo pendiente, iniciar timer desde ahora
    if (credenciales.intentos >= MAX_INTENTOS) {
        credenciales.tiempoBloqueo = millis();
        Serial.println("[AUTH] Bloqueo activo al iniciar.");
    } else {
        credenciales.tiempoBloqueo = 0;
    }

    Serial.printf("[NVS] Credenciales cargadas. Usuario: %s | Intentos: %d\n",
                  credenciales.usuario.c_str(), credenciales.intentos);
}

// ---- Guarda credenciales en NVS ----
void saveCredentials() {
    prefs.begin("auth", false);  // modo lectura/escritura
    prefs.putString("usuario",  credenciales.usuario);
    prefs.putString("passHash", credenciales.hashContrasena);
    prefs.putInt("intentos",    credenciales.intentos);
    prefs.end();
    Serial.println("[NVS] Credenciales guardadas.");
}

// ---- Retorna true si el sistema está bloqueado ----
bool estaBloqueado() {
    if (credenciales.intentos < MAX_INTENTOS) return false;

    unsigned long transcurrido = millis() - credenciales.tiempoBloqueo;
    if (transcurrido < TIEMPO_BLOQUEO) {
        Serial.printf("[AUTH] Bloqueado. Restante: %lu s\n",
                      (TIEMPO_BLOQUEO - transcurrido) / 1000);
        return true;
    }

    // Bloqueo expirado → resetear contador
    credenciales.intentos       = 0;
    credenciales.tiempoBloqueo  = 0;
    saveCredentials();
    Serial.println("[AUTH] Bloqueo expirado. Intentos reseteados.");
    return false;
}

// ---- Retorna los segundos restantes de bloqueo (0 si no está bloqueado) ----
unsigned long segundosBloqueo() {
    if (!estaBloqueado()) return 0;
    return (TIEMPO_BLOQUEO - (millis() - credenciales.tiempoBloqueo)) / 1000;
}

// ---- Valida usuario y contraseña; retorna true si son correctos ----
bool validateLogin(const String& usuario, const String& contrasena) {
    bool ok = (usuario == credenciales.usuario) &&
              (hashPassword(contrasena) == credenciales.hashContrasena);
    Serial.printf("[AUTH] Validación '%s': %s\n",
                  usuario.c_str(), ok ? "CORRECTO" : "FALLO");
    return ok;
}

// ---- Registra intento fallido y activa bloqueo si se llega al límite ----
void registrarIntentoFallido() {
    credenciales.intentos++;
    if (credenciales.intentos >= MAX_INTENTOS) {
        credenciales.tiempoBloqueo = millis();
        Serial.printf("[AUTH] Límite alcanzado (%d). Bloqueado por %d s.\n",
                      MAX_INTENTOS, TIEMPO_BLOQUEO / 1000);
    }
    saveCredentials();
}

// ---- Resetea el contador de intentos fallidos ----
void resetearIntentos() {
    credenciales.intentos      = 0;
    credenciales.tiempoBloqueo = 0;
    saveCredentials();
}
