// credentials.h
// Módulo encargado de la autenticación, los hashes y el bloqueo por intentos.
#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include <ctype.h>
#include "mbedtls/sha256.h"
#include "config.h"

// ---- Estado de seguridad en memoria RAM ----
// Los hashes se cargan desde NVS al iniciar el equipo.
struct Credenciales {
    String        hashUsuario;
    String        hashContrasena;
    uint8_t       intentosFallidos;
    bool          bloqueoActivo;
    unsigned long inicioBloqueoMs;
};

Credenciales credenciales;
Preferences  prefs;

// ---- Verifica si una cadena tiene formato SHA-256 hexadecimal ----
// Un hash SHA-256 en texto tiene 64 caracteres hexadecimales.
bool hashTieneFormatoValido(const String& valor) {
    if (valor.length() != 64) {
        return false;
    }

    for (size_t i = 0; i < valor.length(); i++) {
        if (!isxdigit(static_cast<unsigned char>(valor[i]))) {
            return false;
        }
    }

    return true;
}

// ---- Convierte un texto a SHA-256 en formato hexadecimal ----
// Esta función se usa tanto para guardar como para validar credenciales.
String hashTexto(const String& texto) {
    unsigned char hash[32];
    mbedtls_sha256_context contexto;

    mbedtls_sha256_init(&contexto);
    mbedtls_sha256_starts(&contexto, 0);
    mbedtls_sha256_update(&contexto, reinterpret_cast<const unsigned char*>(texto.c_str()), texto.length());
    mbedtls_sha256_finish(&contexto, hash);
    mbedtls_sha256_free(&contexto);

    String hashHex = "";
    for (int i = 0; i < 32; i++) {
        char buffer[3];
        snprintf(buffer, sizeof(buffer), "%02x", hash[i]);
        hashHex += buffer;
    }

    return hashHex;
}

// ---- Guarda solo los hashes actuales en NVS ----
// Antes de guardar, se eliminan claves antiguas para evitar mezclas de formato.
void guardarCredenciales() {
    prefs.begin("auth", false);
    prefs.remove("usuario");
    prefs.remove("password");
    prefs.remove("user");
    prefs.remove("userHash");
    prefs.remove("passHash");
    prefs.putString("userHash", credenciales.hashUsuario);
    prefs.putString("passHash", credenciales.hashContrasena);
    prefs.end();

    Serial.println("[NVS] Hashes de usuario y contraseña guardados.");
}

// ---- Guarda el número de intentos y el estado de bloqueo ----
// Este bloque se guarda aparte para no reescribir hashes si no es necesario.
void guardarEstadoSeguridad() {
    prefs.begin("auth", false);
    prefs.remove("intentos");
    prefs.putUChar("attempts", credenciales.intentosFallidos);
    prefs.putBool("locked", credenciales.bloqueoActivo);
    prefs.end();

    Serial.printf("[NVS] Estado guardado. Intentos: %u | Bloqueo: %s\n",
                  credenciales.intentosFallidos,
                  credenciales.bloqueoActivo ? "SI" : "NO");
}

// ---- Crea las credenciales iniciales del sistema ----
// Se usa solo cuando NVS no tiene datos válidos.
void cargarCredencialesPorDefecto() {
    credenciales.hashUsuario      = hashTexto(DEFAULT_USUARIO);
    credenciales.hashContrasena   = hashTexto(DEFAULT_PASS);
    credenciales.intentosFallidos = 0;
    credenciales.bloqueoActivo    = false;
    credenciales.inicioBloqueoMs  = 0;

    guardarCredenciales();
    guardarEstadoSeguridad();
    Serial.println("[AUTH] Se cargaron las credenciales iniciales con SHA-256.");
}

// ---- Reinicia intentos y quita el bloqueo ----
// Se llama cuando el login es correcto o cuando termina el tiempo de espera.
void resetearIntentos() {
    credenciales.intentosFallidos = 0;
    credenciales.bloqueoActivo    = false;
    credenciales.inicioBloqueoMs  = 0;
    guardarEstadoSeguridad();
    Serial.println("[AUTH] Se reinició el contador de intentos.");
}

// ---- Calcula cuánto tiempo falta para quitar el bloqueo ----
unsigned long milisegundosBloqueoRestantes() {
    if (!credenciales.bloqueoActivo) {
        return 0;
    }

    const unsigned long transcurrido = millis() - credenciales.inicioBloqueoMs;
    if (transcurrido >= TIEMPO_BLOQUEO) {
        return 0;
    }

    return TIEMPO_BLOQUEO - transcurrido;
}

// ---- Carga hashes y estado de seguridad desde NVS ----
// Si encuentra un formato anterior, intenta migrarlo para no perder acceso.
void loadCredentials() {
    prefs.begin("auth", false);

    String hashUsuarioGuardado    = prefs.getString("userHash", "");
    String hashContrasenaGuardada = prefs.getString("passHash", "");
    String usuarioPlanoLegacy     = prefs.getString("usuario", "");

    bool datosValidos = hashTieneFormatoValido(hashUsuarioGuardado) &&
                        hashTieneFormatoValido(hashContrasenaGuardada);

    // Compatibilidad con una versión anterior:
    // si el usuario estaba en texto plano pero la contraseña ya estaba en hash,
    // se convierte el usuario a SHA-256 y se guarda con el formato nuevo.
    if (!datosValidos && usuarioPlanoLegacy.length() > 0 && hashTieneFormatoValido(hashContrasenaGuardada)) {
        credenciales.hashUsuario    = hashTexto(usuarioPlanoLegacy);
        credenciales.hashContrasena = hashContrasenaGuardada;
        datosValidos = true;

        prefs.remove("usuario");
        prefs.putString("userHash", credenciales.hashUsuario);
        Serial.println("[AUTH] Se migró el usuario anterior a formato SHA-256.");
    }

    if (datosValidos) {
        credenciales.hashUsuario      = hashUsuarioGuardado.length() > 0 ? hashUsuarioGuardado : credenciales.hashUsuario;
        credenciales.hashContrasena   = hashContrasenaGuardada;
        credenciales.intentosFallidos = prefs.getUChar("attempts", static_cast<uint8_t>(prefs.getInt("intentos", 0)));
        credenciales.bloqueoActivo    = prefs.getBool("locked", credenciales.intentosFallidos >= MAX_INTENTOS);
        credenciales.inicioBloqueoMs  = credenciales.bloqueoActivo ? millis() : 0;

        prefs.remove("intentos");
        prefs.putUChar("attempts", credenciales.intentosFallidos);
        prefs.putBool("locked", credenciales.bloqueoActivo);
        prefs.end();

        Serial.printf("[NVS] Credenciales cargadas. Intentos: %u | Bloqueo: %s\n",
                      credenciales.intentosFallidos,
                      credenciales.bloqueoActivo ? "SI" : "NO");

        if (credenciales.bloqueoActivo) {
            Serial.println("[AUTH] Había un bloqueo pendiente. El conteo se reinicia desde este arranque.");
        }

        return;
    }

    prefs.end();
    Serial.println("[AUTH] No se encontraron datos válidos. Se restaurarán las credenciales iniciales.");
    cargarCredencialesPorDefecto();
}

// ---- Indica si el sistema sigue bloqueado ----
// Cuando el tiempo termina, libera automáticamente el acceso.
bool estaBloqueado() {
    if (!credenciales.bloqueoActivo) {
        return false;
    }

    const unsigned long restanteMs = milisegundosBloqueoRestantes();
    if (restanteMs > 0) {
        Serial.printf("[AUTH] Acceso bloqueado. Restan %lu segundos.\n",
                      (restanteMs + 999UL) / 1000UL);
        return true;
    }

    Serial.println("[AUTH] Terminó el tiempo de bloqueo.");
    resetearIntentos();
    return false;
}

// ---- Devuelve los segundos restantes del bloqueo ----
unsigned long segundosBloqueoRestantes() {
    const unsigned long restanteMs = milisegundosBloqueoRestantes();
    if (restanteMs == 0) {
        return 0;
    }

    return (restanteMs + 999UL) / 1000UL;
}

// ---- Valida las credenciales ingresadas ----
// El usuario y la contraseña se transforman a hash y luego se comparan con NVS.
bool validateLogin(const String& usuario, const String& contrasena) {
    const String hashUsuarioIngresado    = hashTexto(usuario);
    const String hashContrasenaIngresada = hashTexto(contrasena);

    const bool accesoValido = (hashUsuarioIngresado == credenciales.hashUsuario) &&
                              (hashContrasenaIngresada == credenciales.hashContrasena);

    Serial.printf("[AUTH] Intento de acceso %s. Huella usuario: %s...\n",
                  accesoValido ? "VALIDO" : "INVALIDO",
                  hashUsuarioIngresado.substring(0, 8).c_str());

    return accesoValido;
}

// ---- Registra un intento fallido ----
// Cuando se llega al límite, se activa el bloqueo por tiempo.
void registrarIntentoFallido() {
    if (credenciales.intentosFallidos < MAX_INTENTOS) {
        credenciales.intentosFallidos++;
    }

    if (credenciales.intentosFallidos >= MAX_INTENTOS) {
        credenciales.intentosFallidos = MAX_INTENTOS;
        credenciales.bloqueoActivo    = true;
        credenciales.inicioBloqueoMs  = millis();
        Serial.printf("[AUTH] Se alcanzó el límite. Bloqueo activo por %lu segundos.\n",
                      TIEMPO_BLOQUEO / 1000UL);
    } else {
        Serial.printf("[AUTH] Intento fallido. Restan %d intentos.\n",
                      MAX_INTENTOS - credenciales.intentosFallidos);
    }

    guardarEstadoSeguridad();
}

// ---- Reemplaza los hashes antiguos por nuevos valores ----
// El cambio puede ser solo de usuario, solo de contraseña o de ambos.
bool actualizarCredenciales(const String& nuevoUsuario, const String& nuevaContrasena) {
    bool huboCambios = false;

    if (!nuevoUsuario.isEmpty()) {
        const String nuevoHashUsuario = hashTexto(nuevoUsuario);
        if (nuevoHashUsuario != credenciales.hashUsuario) {
            credenciales.hashUsuario = nuevoHashUsuario;
            huboCambios = true;
            Serial.println("[AUTH] Se generó un nuevo hash de usuario.");
        }
    }

    if (!nuevaContrasena.isEmpty()) {
        const String nuevoHashContrasena = hashTexto(nuevaContrasena);
        if (nuevoHashContrasena != credenciales.hashContrasena) {
            credenciales.hashContrasena = nuevoHashContrasena;
            huboCambios = true;
            Serial.println("[AUTH] Se generó un nuevo hash de contraseña.");
        }
    }

    if (!huboCambios) {
        Serial.println("[AUTH] No se detectaron cambios reales en las credenciales.");
        return false;
    }

    guardarCredenciales();
    resetearIntentos();
    Serial.println("[AUTH] Se reemplazaron los hashes anteriores por los nuevos valores.");
    return true;
}
