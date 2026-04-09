// config.h
// Archivo de configuración general del proyecto.
// Aquí se definen la red del ESP32, el pin del LED y los parámetros de seguridad.
#pragma once

#include <WiFi.h>

// ---- Red WiFi creada por el ESP32 ----
// El equipo trabaja como Access Point, así que no depende de un router externo.
#define AP_SSID         "ESP32_Control_LED"
#define AP_PASSWORD     "esp32segura"
#define AP_CANAL        6
#define AP_MAX_CLIENTES 4

// ---- Dirección local del servidor web ----
// Esta IP es la que se escribe en el navegador cuando el usuario se conecta al WiFi del ESP32.
#define AP_IP      IPAddress(192, 168, 4, 1)
#define AP_GATEWAY IPAddress(192, 168, 4, 1)
#define AP_MASK    IPAddress(255, 255, 255, 0)

// ---- Hardware ----
// GPIO donde está conectado el LED de prueba.
#define PIN_LED 2

// ---- Seguridad ----
// Se permiten 3 intentos fallidos y luego se bloquea el acceso por 1 minuto.
#define MAX_INTENTOS   3
#define TIEMPO_BLOQUEO 60000UL

// ---- Credenciales iniciales ----
// Solo se usan como base en el primer arranque.
// Después se convierten a hash SHA-256 y se guardan en NVS.
#define DEFAULT_USUARIO "admin"
#define DEFAULT_PASS    "1234"
