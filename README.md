# ESP32 Encrip

Proyecto final para demostrar autenticacion web local en un ESP32-S3 usando hashes SHA-256 almacenados en NVS, con control de LED desde una interfaz web.

## Que hace

- Crea una red WiFi propia del ESP32.
- Publica una interfaz web en `http://192.168.4.1`.
- Permite registro inicial, login y cambio de credenciales.
- Guarda `userHash` y `passHash` en NVS en vez de guardar texto plano.
- Bloquea temporalmente el acceso despues de 3 intentos fallidos.
- Controla el LED principal RGB integrado de la placa.
- Muestra una vista de evidencia para demostrar el uso de SHA-256 y NVS.

## Hardware probado

- Placa: `ESP32-S3-WROOM-1 N16R8`
- Tipo de placa en Arduino IDE: `ESP32S3 Dev Module`
- Puerto recomendado: `USB-UART CH343`
- Sistema operativo probado: Windows

## Estructura del proyecto

```text
esp32_encrip/
|- esp32_encrip.ino
|- config.h
|- credentials.h
|- web_pages.h
`- README.md
```

## Dependencias necesarias

Para instalar y ejecutar el proyecto desde cero necesitas lo siguiente:

1. `Arduino IDE 2.x`
2. Paquete de placas `esp32 by Espressif Systems`
3. Driver USB del puente `CH343` si Windows no reconoce el puerto
4. Cable USB de datos

El proyecto usa librerias que ya vienen dentro del core de ESP32:

- `WiFi.h`
- `WebServer.h`
- `Preferences`
- `mbedtls` para SHA-256

No necesitas instalar librerias adicionales desde el Library Manager.

## Paso a paso desde cero

### 1. Instalar Arduino IDE

1. Descarga e instala `Arduino IDE 2.x`.
2. Abre Arduino IDE una vez y deja que termine la configuracion inicial.

### 2. Agregar el gestor de placas ESP32

1. Ve a `Archivo > Preferencias`.
2. En `URLs adicionales del Gestor de tarjetas` agrega esta URL:

```text
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

3. Guarda los cambios.

### 3. Instalar el core de ESP32

1. Ve a `Herramientas > Placa > Gestor de tarjetas`.
2. Busca `esp32`.
3. Instala `esp32 by Espressif Systems`.

## 4. Instalar el driver CH343 si hace falta

Esta placa tiene dos USB-C. Para este proyecto conviene usar el puerto que Windows detecta como `USB-Enhanced-SERIAL CH343`.

Si al conectar la placa no aparece un `COM` en Windows:

1. Abre `Administrador de dispositivos`.
2. Revisa `Puertos (COM y LPT)`.
3. Si no aparece el CH343, instala el driver del chip `CH343`.
4. Desconecta y vuelve a conectar la placa.

Resultado esperado:

```text
USB-Enhanced-SERIAL CH343 (COMxx)
```

## 5. Elegir el puerto USB correcto de la placa

Esta placa tiene dos puertos USB-C.

- Usa el puerto `USB-UART / CH343`.
- No uses por defecto el USB nativo del S3 para esta carga, porque en esta placa fue menos estable y puede caer en modo bootloader.

Verificacion rapida en Windows:

- Correcto: aparece un `COMxx` con `CH343`
- Incorrecto: aparece algo tipo `ESP32 Family Device` o `USB JTAG/Serial`

## 6. Abrir el proyecto

1. Abre Arduino IDE.
2. Ve a `Archivo > Abrir`.
3. Abre:

```text
esp32_encrip.ino
```

## 7. Seleccionar placa y puerto

En Arduino IDE configura lo siguiente:

- `Herramientas > Placa > ESP32 Arduino > ESP32S3 Dev Module`
- `Herramientas > Puerto > COMxx` del CH343

Configuracion recomendada para esta placa:

- Board: `ESP32S3 Dev Module`
- Flash Size: `16MB`
- Upload Mode: `default`
- Upload Speed: `115200`
- PSRAM: `Disabled` o el valor por defecto del core si no rompe la compilacion

Si no ves exactamente todas esas opciones, deja las equivalentes por defecto del core mientras mantengas `ESP32S3 Dev Module`.

## 8. Revisar la configuracion del proyecto

Los parametros principales estan en [config.h](C:/Users/Kevin%20Carrasquilla/Desktop/ALL/tarea/esp/esp32_encrip/config.h:1).

Valores actuales:

```cpp
#define AP_SSID         "ESP32_Control_LED"
#define AP_PASSWORD     "esp32segura"
#define AP_CANAL        6
#define AP_MAX_CLIENTES 4

#define AP_IP      IPAddress(192, 168, 4, 1)
#define AP_GATEWAY IPAddress(192, 168, 4, 1)
#define AP_MASK    IPAddress(255, 255, 255, 0)

#define PIN_LED_AUXILIAR 2
#define PIN_LED_PRINCIPAL 48

#define MAX_INTENTOS   3
#define TIEMPO_BLOQUEO 60000UL

#define DEFAULT_USUARIO "admin"
#define DEFAULT_PASS    "1234"
```

Si quieres cambiar nombre del WiFi, contrasena, tiempo de bloqueo o credenciales por defecto, hazlo aqui antes de grabar.

## 9. Compilar

1. Pulsa `Verificar`.
2. Espera a que Arduino IDE termine la compilacion.

Resultado esperado:

- No debe haber errores de compilacion.

## 10. Subir a la placa

1. Pulsa `Subir`.
2. Espera a que termine la carga.

Si la placa no entra sola en modo de carga:

1. Mantén presionado `BOOT`.
2. Pulsa `RST` o `EN`.
3. Suelta `RST`.
4. Cuando inicie la carga, suelta `BOOT`.

Resultado esperado al final:

- `Hard resetting via RTS pin`
- la placa reinicia sola

## 11. Abrir el monitor serie

1. Abre `Herramientas > Monitor Serie`.
2. Configura la velocidad en:

```text
115200
```

Mensajes esperados:

```text
[WiFi] Access Point activo y listo para recibir clientes.
[WiFi] SSID: ESP32_Control_LED
[WiFi] IP del servidor: 192.168.4.1
[SERVER] Servidor web iniciado en el puerto 80.
```

## 12. Conectarte al WiFi del ESP32

Desde el celular o la laptop:

1. Busca la red `ESP32_Control_LED`
2. Conectate con la clave `esp32segura`
3. Abre en el navegador:

```text
http://192.168.4.1
```

## 13. Flujo de uso del sistema

### Primer uso

Si el sistema esta con las credenciales iniciales, `/` te lleva al registro inicial.

Haz esto:

1. Abre `http://192.168.4.1`
2. Crea tu usuario y contrasena
3. Guarda el registro
4. Ve al login
5. Inicia sesion

### Usos posteriores

Si ya existe una cuenta registrada:

1. Abre `http://192.168.4.1`
2. Entra por login
3. Usa el panel

## 14. Que probar para validar que todo funciona

### WiFi

- La red `ESP32_Control_LED` debe aparecer
- Debes poder abrir `192.168.4.1`

### Registro

- Debe dejar crear un usuario inicial
- No debe permitir registro inicial una segunda vez

### Login

- Debe entrar con usuario y contrasena correctos
- Debe rechazar credenciales incorrectas
- Debe bloquear despues de 3 fallos

### LED

- Desde el panel, `Encender` debe activar el LED RGB principal de la placa
- `Apagar` debe apagarlo

### Evidencia SHA-256

- La ruta `/evidencia` debe mostrar:
  - `auth / userHash`
  - `auth / passHash`
  - `attempts`
  - `locked`
- Si ingresas los valores correctos, debe indicar coincidencia
- Si cambias un caracter, debe indicar que no coincide

## Rutas disponibles

| Ruta | Metodo | Uso |
|---|---|---|
| `/` | GET | Redirige a registro o login |
| `/login` | GET | Vista de login |
| `/login` | POST | Validacion de acceso |
| `/registro` | GET | Registro inicial |
| `/registro` | POST | Guarda credenciales iniciales |
| `/panel` | GET | Panel autenticado |
| `/led` | POST | Enciende o apaga el LED |
| `/cambiar` | GET | Vista para cambiar credenciales |
| `/cambiar` | POST | Reemplaza hashes guardados |
| `/evidencia` | GET | Evidencia de SHA-256 y NVS |
| `/evidencia` | POST | Calcula y compara hashes |
| `/logout` | POST | Cierra la sesion |

## Como se guarda la informacion

No hay una base de datos SQL.

La persistencia se guarda en la `NVS` interna del ESP32 dentro del namespace:

```text
auth
```

Claves usadas:

- `userHash`
- `passHash`
- `attempts`
- `locked`

## Seguridad aplicada

- Usuario y contrasena no se guardan en texto plano
- Ambos se convierten a SHA-256
- Solo se comparan hashes
- El acceso se bloquea temporalmente tras 3 intentos fallidos

Nota academica:

- Aqui se usa `SHA-256` como demostracion clara de hashing
- En un sistema real convendria usar `salt` y un esquema especifico para passwords como `PBKDF2`, `bcrypt` o `Argon2`

## Troubleshooting

### No aparece el WiFi del ESP32

1. Verifica que la carga termino bien
2. Pulsa `RST` o `EN`
3. Espera 5 a 10 segundos
4. Busca de nuevo `ESP32_Control_LED`

### No aparece el puerto COM

1. Cambia de cable USB
2. Usa el puerto USB-C correcto
3. Reinstala el driver CH343
4. Revisa `Administrador de dispositivos`

### No deja subir el sketch

1. Cierra Arduino IDE y cualquier monitor serie extra
2. Vuelve a abrir solo una instancia
3. Si hace falta, entra manualmente en bootloader con `BOOT` + `RST`

### El LED no responde

1. Asegurate de iniciar sesion primero
2. Usa el panel y no la pagina de evidencia
3. Esta version controla el LED RGB principal en `GPIO48`
4. Si tu clon de placa cablea el RGB diferente, ajusta `PIN_LED_PRINCIPAL` en [config.h](C:/Users/Kevin%20Carrasquilla/Desktop/ALL/tarea/esp/esp32_encrip/config.h:26)

### El navegador muestra una pagina vieja

Haz una de estas opciones:

1. Recarga fuerte
2. Cierra la pestaña y abre otra vez
3. Borra cache del navegador

## Resumen rapido de instalacion

1. Instalar Arduino IDE 2.x
2. Instalar `esp32 by Espressif Systems`
3. Instalar driver CH343 si hace falta
4. Conectar la placa por el USB-UART CH343
5. Seleccionar `ESP32S3 Dev Module`
6. Seleccionar el `COMxx`
7. Compilar
8. Subir
9. Conectarte a `ESP32_Control_LED`
10. Abrir `http://192.168.4.1`

## Estado actual del proyecto

Esta version ya incluye:

- flujo de registro inicial
- login limpio
- panel simplificado
- control del LED principal RGB
- evidencia de SHA-256
- almacenamiento en NVS

