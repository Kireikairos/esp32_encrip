# ESP32 Control Web con Autenticación

Servidor web en ESP32 con login seguro y control de LED desde el navegador.

## Características

- **Login web** con usuario y contraseña
- **Hash SHA-256** de la contraseña (nunca se guarda texto plano)
- **Persistencia NVS** (credenciales sobreviven al reinicio)
- **Sesión por cookie** con token aleatorio (hardware RNG)
- **Bloqueo automático** tras 3 intentos fallidos (30 segundos)
- **Cambio de credenciales** desde la interfaz web
- **Fallback a Access Point** si no conecta al WiFi configurado

## Hardware necesario

| Componente      | Detalle                        |
|-----------------|-------------------------------|
| ESP32           | Cualquier placa (DevKit, etc.) |
| LED             | Conectado en GPIO 2            |
| Resistencia     | 220 Ω en serie con el LED      |

> El GPIO 2 también controla el LED integrado en la mayoría de placas ESP32.

## Configuración antes de compilar

Edita **`config.h`** y cambia las credenciales WiFi:

```cpp
#define WIFI_SSID     "TU_WIFI_AQUI"
#define WIFI_PASSWORD "TU_PASSWORD_AQUI"
```

Opcionalmente cambia:
- `PIN_LED` → GPIO del LED (por defecto `2`)
- `MAX_INTENTOS` → Intentos antes de bloquear (por defecto `3`)
- `TIEMPO_BLOQUEO` → Duración del bloqueo en ms (por defecto `30000`)
- `DEFAULT_USUARIO` / `DEFAULT_PASS` → Credenciales del primer arranque

## Librerías requeridas

Todas incluidas con el **core ESP32 para Arduino IDE**. No se necesitan librerías externas.

| Librería              | Origen          |
|-----------------------|-----------------|
| `WiFi.h`              | Core ESP32      |
| `WebServer.h`         | Core ESP32      |
| `Preferences.h`       | Core ESP32      |
| `mbedtls/sha256.h`    | Core ESP32      |

## Instalación del core ESP32

1. Abre Arduino IDE → Archivo → Preferencias
2. En *URLs adicionales de gestor de tarjetas* agrega:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Herramientas → Placa → Gestor de tarjetas → busca **esp32** → instalar

## Cómo usar

1. Configura `config.h` con tu WiFi
2. Selecciona tu placa ESP32 en Arduino IDE
3. Sube el sketch
4. Abre el Monitor Serie (115200 baud) → anota la IP
5. En el navegador entra a `http://[IP]`
6. Login con usuario `admin` / contraseña `1234` (valores por defecto)

## Rutas del servidor

| Ruta        | Método | Descripción                        |
|-------------|--------|------------------------------------|
| `/`         | GET    | Redirige a login o panel           |
| `/login`    | GET    | Página de login                    |
| `/login`    | POST   | Procesa credenciales               |
| `/panel`    | GET    | Panel de control (requiere sesión) |
| `/led`      | POST   | Cambia estado del LED              |
| `/logout`   | POST   | Cierra sesión                      |
| `/cambiar`  | GET    | Formulario cambio de credenciales  |
| `/cambiar`  | POST   | Guarda nuevas credenciales         |

## Estructura del proyecto

```
esp32_encrip/
├── esp32_encrip.ino   → Sketch principal: WiFi, servidor, handlers, sesión
├── config.h           → Parámetros configurables (WiFi, LED, seguridad)
├── credentials.h      → NVS, hash SHA-256, validación y bloqueo
└── web_pages.h        → HTML de login, panel y cambio de credenciales
```

## Flujo de autenticación

```
Usuario accede a /login
        │
        ▼
¿Sistema bloqueado? ──► SÍ → Mostrar tiempo restante
        │ NO
        ▼
¿Credenciales correctas?
        │
        ├─ NO → Sumar intento fallido
        │       ¿intentos >= 3? → Bloquear 30s
        │
        └─ SÍ → Resetear intentos
                Crear token de sesión
                Redirigir a /panel
                        │
                        ▼
                  ON / OFF LED
                  Cambiar credenciales
                  Cerrar sesión
```

## Notas de seguridad

- La contraseña se guarda como **hash SHA-256**, nunca en texto plano.
- El token de sesión se genera con **hardware RNG** del ESP32 (`esp_random()`).
- La sesión **no persiste** al reiniciar (solo en RAM).
- El contador de intentos **sí persiste** al reiniciar (en NVS).
- Para uso en producción considera agregar HTTPS (requiere biblioteca adicional).
