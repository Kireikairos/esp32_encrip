// web_pages.h
// Plantillas HTML del proyecto.
// Aquí se define la landing, el panel principal y la vista para cambiar credenciales.
#pragma once

// ---- Landing principal ----
// Muestra el estado del ESP32 y el formulario de acceso.
const char LOGIN_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>ESP32 - Demostración de acceso seguro</title>
<style>
  *{box-sizing:border-box;margin:0;padding:0}
  body{font-family:'Segoe UI',sans-serif;background:#101827;color:#e5eefc;min-height:100vh;display:flex;align-items:center;justify-content:center;padding:1.2rem}
  .shell{width:min(100%,920px);display:grid;grid-template-columns:1.1fr .9fr;gap:1rem}
  .card{background:#16213e;border:1px solid rgba(255,255,255,.08);border-radius:18px;padding:1.4rem;box-shadow:0 18px 45px rgba(0,0,0,.28)}
  .badge{display:inline-flex;align-items:center;gap:.45rem;background:rgba(34,197,94,.12);color:#7df0a3;border:1px solid rgba(125,240,163,.28);padding:.45rem .75rem;border-radius:999px;font-size:.88rem;margin-bottom:1rem}
  h1{font-size:1.9rem;margin-bottom:.75rem;color:#ffffff}
  .lead{color:#b6c3db;line-height:1.55;margin-bottom:1.15rem}
  .grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:.8rem}
  .info{background:#0f172a;border:1px solid rgba(255,255,255,.06);border-radius:14px;padding:.9rem}
  .info span{display:block;color:#8ea3c8;font-size:.8rem;margin-bottom:.35rem}
  .info strong{font-size:1rem;color:#ffffff;word-break:break-word}
  .full{grid-column:1/-1}
  .msg{margin-bottom:.85rem;padding:.8rem 1rem;border-radius:12px;font-size:.92rem;line-height:1.4}
  .error{background:rgba(239,68,68,.12);border:1px solid rgba(239,68,68,.25);color:#ffb3b3}
  .warn{background:rgba(245,158,11,.12);border:1px solid rgba(245,158,11,.25);color:#ffd58a}
  .success{background:rgba(34,197,94,.12);border:1px solid rgba(34,197,94,.25);color:#9df7b5}
  .login-title{font-size:1.35rem;margin-bottom:.35rem;color:#ffffff}
  .login-sub{color:#9db0cf;font-size:.95rem;line-height:1.45;margin-bottom:1rem}
  form{display:flex;flex-direction:column;gap:.85rem}
  label{color:#aac0e3;font-size:.88rem}
  input{width:100%;padding:.9rem 1rem;border-radius:12px;border:1px solid #29406b;background:#0b1324;color:#ffffff;font-size:1rem}
  input:focus{outline:none;border-color:#60a5fa;box-shadow:0 0 0 3px rgba(96,165,250,.15)}
  button{width:100%;padding:.95rem 1rem;border:none;border-radius:12px;background:linear-gradient(135deg,#2563eb,#1d4ed8);color:#fff;font-weight:700;font-size:1rem;cursor:pointer;transition:transform .2s ease,opacity .2s ease}
  button:hover{opacity:.94;transform:translateY(-1px)}
  .hint{margin-top:1rem;color:#8ea3c8;font-size:.88rem;line-height:1.45}
  .divider{height:1px;background:rgba(255,255,255,.08);margin:1rem 0}
  @media (max-width:760px){.shell{grid-template-columns:1fr}.grid{grid-template-columns:1fr}}
</style>
</head>
<body>
<div class="shell">
  <section class="card">
    <div class="badge">● Servidor web del ESP32 en funcionamiento</div>
    <h1>Demostración de acceso seguro</h1>
    <p class="lead">Esta página confirma que el navegador ya está conectado al WiFi creado por el ESP32. Desde aquí se puede verificar el estado del sistema y luego iniciar sesión para controlar el LED.</p>
    <div class="grid">
      <article class="info">
        <span>Modo de red</span>
        <strong>%MODO_RED%</strong>
      </article>
      <article class="info">
        <span>SSID del ESP32</span>
        <strong>%AP_SSID%</strong>
      </article>
      <article class="info">
        <span>IP del servidor</span>
        <strong>%AP_IP%</strong>
      </article>
      <article class="info">
        <span>Clientes conectados</span>
        <strong>%CLIENTES_AP%</strong>
      </article>
      <article class="info full">
        <span>Tiempo activo del equipo</span>
        <strong>%UPTIME%</strong>
      </article>
    </div>
    <div class="divider"></div>
    <p class="hint">El LED no se controla desde esta vista. El acceso al control queda protegido detrás del inicio de sesión.</p>
  </section>

  <section class="card">
    <h2 class="login-title">Inicio de sesión</h2>
    <p class="login-sub">El usuario y la contraseña se comparan mediante hashes SHA-256 almacenados en la memoria no volátil del ESP32.</p>
    %LOGIN_SUCCESS%
    %LOGIN_WARNING%
    %LOGIN_ERROR%
    <form method="POST" action="/login">
      <div>
        <label for="usuario">Usuario</label>
        <input id="usuario" name="usuario" type="text" placeholder="Ingresa tu usuario" required autocomplete="off">
      </div>
      <div>
        <label for="contrasena">Contraseña</label>
        <input id="contrasena" name="contrasena" type="password" placeholder="Ingresa tu contraseña" required>
      </div>
      <button type="submit">Entrar al panel</button>
    </form>
    <p class="hint">Después de 3 intentos fallidos, el acceso se bloquea durante 1 minuto.</p>
  </section>
</div>
</body>
</html>
)rawliteral";

// ---- Panel principal ----
// Muestra el estado del LED y los datos básicos de la red local.
const char PANEL_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>ESP32 - Panel autenticado</title>
<style>
  *{box-sizing:border-box;margin:0;padding:0}
  body{font-family:'Segoe UI',sans-serif;background:#101827;color:#e5eefc;min-height:100vh;padding:1.2rem}
  .container{max-width:940px;margin:0 auto;display:grid;gap:1rem}
  .card{background:#16213e;border:1px solid rgba(255,255,255,.08);border-radius:18px;padding:1.35rem;box-shadow:0 18px 45px rgba(0,0,0,.28)}
  .header{display:flex;justify-content:space-between;align-items:center;gap:1rem;flex-wrap:wrap}
  .header h1{font-size:1.7rem;color:#ffffff}
  .badge{display:inline-flex;align-items:center;gap:.45rem;background:rgba(59,130,246,.12);color:#9dccff;border:1px solid rgba(59,130,246,.28);padding:.45rem .75rem;border-radius:999px;font-size:.88rem}
  .grid{display:grid;grid-template-columns:repeat(4,minmax(0,1fr));gap:.8rem;margin-top:1rem}
  .info{background:#0f172a;border:1px solid rgba(255,255,255,.06);border-radius:14px;padding:.9rem}
  .info span{display:block;color:#8ea3c8;font-size:.8rem;margin-bottom:.35rem}
  .info strong{font-size:1rem;color:#ffffff;word-break:break-word}
  .msg{margin-top:1rem;padding:.8rem 1rem;border-radius:12px;font-size:.92rem;line-height:1.4}
  .success{background:rgba(34,197,94,.12);border:1px solid rgba(34,197,94,.25);color:#9df7b5}
  .error{background:rgba(239,68,68,.12);border:1px solid rgba(239,68,68,.25);color:#ffb3b3}
  .led-wrap{display:grid;grid-template-columns:1fr auto;gap:1rem;align-items:center}
  .led-icon{font-size:4rem;text-align:center}
  .led-state{font-size:1.2rem;font-weight:700;margin-top:.45rem}
  .led-sub{color:#9db0cf;margin-top:.35rem;line-height:1.45}
  .actions{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:.8rem;margin-top:1rem}
  button{width:100%;padding:.95rem 1rem;border:none;border-radius:12px;color:#fff;font-weight:700;font-size:1rem;cursor:pointer;transition:transform .2s ease,opacity .2s ease}
  button:hover{opacity:.94;transform:translateY(-1px)}
  .btn-on{background:linear-gradient(135deg,#16a34a,#15803d)}
  .btn-off{background:linear-gradient(135deg,#dc2626,#b91c1c)}
  .btn-secondary{background:linear-gradient(135deg,#2563eb,#1d4ed8)}
  .btn-logout{background:linear-gradient(135deg,#4b5563,#374151)}
  .menu{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:.8rem}
  @media (max-width:860px){.grid{grid-template-columns:repeat(2,minmax(0,1fr))}.led-wrap,.actions,.menu{grid-template-columns:1fr}}
  @media (max-width:520px){.grid{grid-template-columns:1fr}}
</style>
</head>
<body>
<div class="container">
  <section class="card">
    <div class="header">
      <h1>Panel autenticado del ESP32</h1>
      <div class="badge">● Sesión válida en la red local del dispositivo</div>
    </div>
    <div class="grid">
      <article class="info">
        <span>Modo de red</span>
        <strong>Access Point</strong>
      </article>
      <article class="info">
        <span>SSID del ESP32</span>
        <strong>%AP_SSID%</strong>
      </article>
      <article class="info">
        <span>IP del servidor</span>
        <strong>%AP_IP%</strong>
      </article>
      <article class="info">
        <span>Clientes conectados</span>
        <strong>%CLIENTES_AP%</strong>
      </article>
    </div>
    <div class="grid" style="grid-template-columns:1fr;margin-top:.8rem">
      <article class="info">
        <span>Tiempo activo del equipo</span>
        <strong>%UPTIME%</strong>
      </article>
    </div>
    %PANEL_MSG%
  </section>

  <section class="card">
    <div class="led-wrap">
      <div>
        <h2>Control del LED</h2>
        <p class="led-state" style="color:%COLOR_LED%">Estado actual: %ESTADO_LED%</p>
        <p class="led-sub">Esta acción solo está disponible después de validar correctamente las credenciales.</p>
      </div>
      <div class="led-icon">%ICONO_LED%</div>
    </div>
    <div class="actions">
      <form method="POST" action="/led">
        <input type="hidden" name="estado" value="on">
        <button class="btn-on" type="submit">Encender LED</button>
      </form>
      <form method="POST" action="/led">
        <input type="hidden" name="estado" value="off">
        <button class="btn-off" type="submit">Apagar LED</button>
      </form>
    </div>
  </section>

  <section class="card">
    <h2 style="margin-bottom:1rem">Opciones del sistema</h2>
    <div class="menu">
      <form method="GET" action="/cambiar">
        <button class="btn-secondary" type="submit">Cambiar credenciales</button>
      </form>
      <form method="POST" action="/logout">
        <button class="btn-logout" type="submit">Cerrar sesión</button>
      </form>
    </div>
  </section>
</div>
</body>
</html>
)rawliteral";

// ---- Vista de actualización de credenciales ----
// Permite reemplazar los hashes almacenados por nuevos valores.
const char CAMBIAR_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>ESP32 - Cambiar credenciales</title>
<style>
  *{box-sizing:border-box;margin:0;padding:0}
  body{font-family:'Segoe UI',sans-serif;background:#101827;color:#e5eefc;min-height:100vh;display:flex;align-items:center;justify-content:center;padding:1rem}
  .card{width:min(100%,560px);background:#16213e;border:1px solid rgba(255,255,255,.08);border-radius:18px;padding:1.5rem;box-shadow:0 18px 45px rgba(0,0,0,.28)}
  h1{font-size:1.7rem;color:#ffffff;margin-bottom:.65rem}
  .lead{color:#9db0cf;line-height:1.5;margin-bottom:1rem}
  .msg{margin-bottom:1rem;padding:.85rem 1rem;border-radius:12px;font-size:.92rem;line-height:1.4}
  .error{background:rgba(239,68,68,.12);border:1px solid rgba(239,68,68,.25);color:#ffb3b3}
  .success{background:rgba(34,197,94,.12);border:1px solid rgba(34,197,94,.25);color:#9df7b5}
  .warn{background:rgba(245,158,11,.12);border:1px solid rgba(245,158,11,.25);color:#ffd58a}
  form{display:grid;gap:.85rem}
  label{color:#aac0e3;font-size:.88rem}
  input{width:100%;padding:.9rem 1rem;border-radius:12px;border:1px solid #29406b;background:#0b1324;color:#ffffff;font-size:1rem}
  input:focus{outline:none;border-color:#60a5fa;box-shadow:0 0 0 3px rgba(96,165,250,.15)}
  .grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:.85rem}
  button,a{display:inline-flex;align-items:center;justify-content:center;width:100%;padding:.95rem 1rem;border:none;border-radius:12px;color:#fff;font-weight:700;font-size:1rem;text-decoration:none;cursor:pointer;transition:transform .2s ease,opacity .2s ease}
  button:hover,a:hover{opacity:.94;transform:translateY(-1px)}
  .btn-save{background:linear-gradient(135deg,#2563eb,#1d4ed8)}
  .btn-back{background:linear-gradient(135deg,#4b5563,#374151)}
  .hint{color:#8ea3c8;font-size:.88rem;line-height:1.45;margin-top:.35rem}
  @media (max-width:600px){.grid{grid-template-columns:1fr}}
</style>
</head>
<body>
<div class="card">
  <h1>Actualizar credenciales</h1>
  <p class="lead">Para reemplazar los hashes guardados debes validar primero el acceso actual. Puedes cambiar solo el usuario, solo la contraseña o ambos datos.</p>
  %CAMBIO_MENSAJE%
  <form method="POST" action="/cambiar">
    <div class="grid">
      <div>
        <label for="usuario_actual">Usuario actual</label>
        <input id="usuario_actual" name="usuario_actual" type="text" placeholder="Usuario actual" required autocomplete="off">
      </div>
      <div>
        <label for="pass_actual">Contraseña actual</label>
        <input id="pass_actual" name="pass_actual" type="password" placeholder="Contraseña actual" required>
      </div>
    </div>
    <div class="grid">
      <div>
        <label for="nuevo_usuario">Nuevo usuario</label>
        <input id="nuevo_usuario" name="nuevo_usuario" type="text" placeholder="Déjalo vacío si no cambia" autocomplete="off">
      </div>
      <div>
        <label for="nueva_pass">Nueva contraseña</label>
        <input id="nueva_pass" name="nueva_pass" type="password" placeholder="Déjala vacía si no cambia">
      </div>
    </div>
    <div>
      <label for="confirmar_pass">Confirmar nueva contraseña</label>
      <input id="confirmar_pass" name="confirmar_pass" type="password" placeholder="Repite la nueva contraseña">
    </div>
    <button class="btn-save" type="submit">Guardar cambios</button>
    <a class="btn-back" href="/panel">Volver al panel</a>
  </form>
  <p class="hint">Después de guardar los cambios, será necesario iniciar sesión otra vez con las nuevas credenciales.</p>
</div>
</body>
</html>
)rawliteral";
