// config.h - Configuración del sistema ESP32
// IMPORTANTE: Edita este archivo antes de compilar
#pragma once

// ---- Credenciales WiFi ----
#define WIFI_SSID     "TU_WIFI_AQUI"
#define WIFI_PASSWORD "TU_PASSWORD_AQUI"

// ---- Hardware ----
#define PIN_LED 2  // GPIO2 (LED integrado en la mayoría de placas ESP32)

// ---- Parámetros de seguridad ----
#define MAX_INTENTOS   3      // Intentos fallidos antes de bloquear
#define TIEMPO_BLOQUEO 30000  // Duración del bloqueo en ms (30 segundos)

// ---- Credenciales por defecto (solo primer arranque) ----
#define DEFAULT_USUARIO "admin"
#define DEFAULT_PASS    "1234"
