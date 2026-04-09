# Sistema de autenticación web segura con ESP32

## Resumen

Este proyecto fue desarrollado como una **demostración práctica para la materia de Encriptación de Datos**. El sistema utiliza un **ESP32** como servidor web local, crea su propia red WiFi, solicita autenticación mediante usuario y contraseña, guarda las credenciales en forma de **hash SHA-256** y permite controlar un **LED** únicamente después de validar el acceso.

La idea principal del proyecto es mostrar cómo aplicar conceptos de seguridad básicos en un sistema embebido real, combinando:

- autenticación web
- almacenamiento en memoria no volátil
- hash criptográfico
- control de hardware desde una interfaz web

---

## Objetivo general

Diseñar e implementar un sistema web embebido en un ESP32 que permita autenticar usuarios de forma local y controlar un LED, utilizando técnicas básicas de protección de credenciales mediante hash criptográfico.

## Objetivos específicos

- Crear una red WiFi local usando el ESP32 en modo **Access Point**.
- Servir una interfaz web accesible desde un navegador.
- Validar usuario y contraseña desde una página de login.
- Almacenar las credenciales en **NVS** usando **SHA-256**.
- Permitir el control de un LED solo después de iniciar sesión.
- Limitar el acceso a **3 intentos fallidos** y bloquear temporalmente el sistema.
- Permitir el cambio de credenciales desde la misma interfaz web.

---

## Justificación de seguridad

En este proyecto **no se guardan las credenciales en texto plano**. En su lugar, tanto el usuario como la contraseña se transforman mediante **SHA-256** antes de almacenarse en la memoria no volátil del ESP32.

### ¿Por qué se usa hash?

El uso de hash mejora la seguridad porque:

- evita guardar la contraseña directamente en memoria
- dificulta recuperar el dato original si alguien accede al almacenamiento
- permite comparar credenciales sin exponer el valor real

### ¿Por qué SHA-256?

Se eligió **SHA-256** porque:

- es más fuerte que SHA-1
- cumple bien como demostración académica de protección básica
- está disponible dentro del entorno del ESP32 mediante `mbedtls`

> En este proyecto el hash se usa como mecanismo de protección y demostración académica. En un sistema de producción real se podrían aplicar técnicas adicionales como `salt`, HTTPS y manejo más robusto de sesiones.

---

## Funcionamiento general del sistema

1. El ESP32 enciende.
2. Crea su propia red WiFi local.
3. Inicia un servidor web en la IP `192.168.4.1`.
4. El usuario se conecta a esa red desde un celular o PC.
5. Abre la landing del sistema en el navegador.
6. Ingresa usuario y contraseña.
7. El ESP32 convierte ambos datos a **SHA-256**.
8. Compara esos hashes con los que están guardados en NVS.
9. Si el acceso es correcto, abre el panel.
10. Desde el panel se puede encender o apagar el LED.
11. Si el acceso falla 3 veces, se bloquea por 1 minuto.

---

## Arquitectura del proyecto

### Módulos principales

- **Configuración**
  - parámetros del Access Point
  - pin del LED
  - intentos y tiempo de bloqueo

- **Autenticación y seguridad**
  - generación de hashes SHA-256
  - carga de credenciales desde NVS
  - validación del login
  - conteo de intentos fallidos
  - bloqueo temporal

- **Servidor web**
  - landing principal
  - login
  - panel autenticado
  - cambio de credenciales

- **Control de hardware**
  - encendido del LED
  - apagado del LED

### Estructura de archivos

```text
esp32_encrip/
├── esp32_encrip.ino   -> Lógica principal del sistema
├── config.h           -> Configuración general
├── credentials.h      -> Seguridad, hashes y NVS
├── web_pages.h        -> Plantillas HTML
└── README.md          -> Documentación del proyecto
```

---

## Red local creada por el ESP32

El proyecto trabaja en modo **Access Point**, por lo que el ESP32 crea su propia red y no depende de un router externo.

### Parámetros actuales

```cpp
#define AP_SSID         "ESP32_Control_LED"
#define AP_PASSWORD     "esp32segura"
#define AP_IP           IPAddress(192, 168, 4, 1)
```

### Cómo conectarse

1. Encender el ESP32.
2. Buscar la red WiFi:
   - `ESP32_Control_LED`
3. Conectarse con la contraseña:
   - `esp32segura`
4. Abrir en el navegador:
   - `http://192.168.4.1`

---

## Credenciales iniciales

En el primer arranque se toman estos valores:

```cpp
#define DEFAULT_USUARIO "admin"
#define DEFAULT_PASS    "1234"
```

Después del primer inicio:

- el usuario se convierte a hash SHA-256
- la contraseña se convierte a hash SHA-256
- ambos hashes se guardan en **NVS**

---

## Flujo de autenticación

```text
Usuario entra a la landing
        │
        ▼
Ingresa usuario y contraseña
        │
        ▼
ESP32 aplica SHA-256 a ambos datos
        │
        ▼
Compara con los hashes guardados en NVS
        │
        ├── Correcto  -> crea sesión -> abre panel -> controla LED
        │
        └── Incorrecto -> suma intento fallido
                           │
                           └── si llega a 3 -> bloqueo por 1 minuto
```

---

## Rutas del servidor web

| Ruta | Método | Función |
|---|---|---|
| `/` | GET | Landing principal con estado del sistema |
| `/login` | GET | Vista de acceso |
| `/login` | POST | Validación de credenciales |
| `/panel` | GET | Panel autenticado |
| `/led` | POST | Encender o apagar LED |
| `/cambiar` | GET | Formulario de cambio de credenciales |
| `/cambiar` | POST | Reemplazo de hashes guardados |
| `/logout` | POST | Cierre de sesión |

---

## Carga paso a paso en Arduino IDE

## 1. Instalar Arduino IDE

- Descargar e instalar **Arduino IDE 2.x**.
- Abrirlo una vez para completar la configuración inicial.

## 2. Instalar el soporte para ESP32

En Arduino IDE:

- Ir a **Archivo > Preferencias**
- En **URLs adicionales del Gestor de tarjetas** agregar:

```text
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

- Luego entrar a:
  - **Herramientas > Placa > Gestor de tarjetas**
- Buscar:
  - **esp32**
- Instalar:
  - **esp32 by Espressif Systems**

## 3. Abrir el proyecto

- Abrir el archivo principal:
  - `esp32_encrip.ino`

## 4. Conectar la placa

- Conectar el ESP32 por USB usando un cable de datos.

## 5. Seleccionar la placa

En Arduino IDE:

- Ir a **Herramientas > Placa**
- Seleccionar una placa compatible, por ejemplo:
  - `ESP32 Dev Module`

## 6. Seleccionar el puerto

- Ir a **Herramientas > Puerto**
- Elegir el puerto COM correspondiente al ESP32.

## 7. Verificar la configuración

Si se desea, se pueden cambiar en `config.h`:

- nombre de la red del ESP32
- contraseña de la red
- IP local del servidor
- tiempo de bloqueo
- credenciales iniciales

## 8. Compilar

- Hacer clic en **Verificar**.

## 9. Subir el programa

- Hacer clic en **Subir**.
- Si la placa lo requiere:
  - mantener presionado **BOOT** durante el inicio de la carga

## 10. Abrir el monitor serie

- Abrir el **Monitor Serie**
- Usar velocidad:

```text
115200
```

Se deben ver mensajes como estos:

```text
[WiFi] Access Point activo y listo para recibir clientes.
[WiFi] SSID: ESP32_Control_LED
[WiFi] IP del servidor: 192.168.4.1
[SERVER] Servidor web iniciado en el puerto 80.
```

## 11. Conectarse al ESP32

Desde un celular o PC:

- conectarse a la red `ESP32_Control_LED`
- abrir `http://192.168.4.1`

## 12. Probar el sistema

- entrar a la landing
- revisar datos del sistema
- iniciar sesión con:
  - usuario: `admin`
  - contraseña: `1234`
- encender y apagar el LED
- probar el bloqueo con 3 intentos fallidos
- probar el cambio de credenciales

---

## Reglas del sistema

- El LED **no se controla desde la landing**.
- El LED **solo se controla después del login**.
- Hay **3 intentos** de acceso.
- Después de 3 errores, el sistema se bloquea durante **1 minuto**.
- Las credenciales actualizadas reemplazan a las anteriores en NVS.

---

## Puntos fuertes para la demostración

Este proyecto permite demostrar de forma clara:

- uso de **hash criptográfico** en un sistema embebido
- diferencia entre guardar texto plano y guardar hash
- uso de **memoria no volátil** para persistencia
- integración entre seguridad, red, interfaz web y hardware
- separación modular del código

---

## Guion breve para la sustentación

Si necesitas explicarlo oralmente, puedes seguir este orden:

1. **Problema**
   - Se necesita controlar un LED desde una web, pero con acceso protegido.

2. **Solución**
   - El ESP32 crea una red local, aloja una web y valida credenciales antes de permitir el control.

3. **Seguridad aplicada**
   - Usuario y contraseña no se guardan en texto plano.
   - Ambos se almacenan como hash SHA-256 en NVS.
   - Se bloquea el acceso tras 3 intentos fallidos.

4. **Demostración práctica**
   - Conexión al WiFi del ESP32.
   - Apertura de la landing.
   - Inicio de sesión.
   - Encendido y apagado del LED.
   - Cambio de credenciales.

5. **Valor académico**
   - El proyecto muestra una aplicación real de encriptación de datos en un sistema embebido.

---

## Conclusión

El proyecto demuestra que un ESP32 puede funcionar como plataforma de autenticación local y control web, aplicando conceptos básicos de protección de credenciales con hash SHA-256. Además, integra red, seguridad, persistencia y control físico en una solución pequeña, comprensible y fácil de presentar.
