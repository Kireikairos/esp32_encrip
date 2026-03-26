// web_pages.h - Páginas HTML del servidor web
// Placeholders: %ERROR%, %WARNING%, %USUARIO%, %LED_ICON%, %LED_STATE%, %LED_COLOR%, %SUCCESS%, %MENSAJE%
#pragma once

// ---- Página de Login ----
const char LOGIN_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>ESP32 Login</title>
<style>
  *{box-sizing:border-box;margin:0;padding:0}
  body{font-family:'Segoe UI',sans-serif;background:#1a1a2e;display:flex;justify-content:center;align-items:center;min-height:100vh}
  .card{background:#16213e;padding:2rem;border-radius:12px;box-shadow:0 8px 32px rgba(0,0,0,.4);width:320px}
  h1{color:#e94560;text-align:center;margin-bottom:1.5rem;font-size:1.5rem}
  .icon{text-align:center;font-size:3rem;margin-bottom:1rem}
  input{width:100%;padding:.75rem;margin-bottom:1rem;border:1px solid #0f3460;border-radius:8px;background:#0f3460;color:#fff;font-size:1rem}
  input:focus{outline:none;border-color:#e94560}
  button{width:100%;padding:.75rem;background:#e94560;color:#fff;border:none;border-radius:8px;font-size:1rem;cursor:pointer;transition:background .3s}
  button:hover{background:#c73652}
  .msg{text-align:center;margin-bottom:1rem;font-size:.9rem;padding:.5rem;border-radius:6px}
  .error{color:#ff6b6b;background:rgba(233,69,96,.1)}
  .warn{color:#ffa500;background:rgba(255,165,0,.1)}
</style>
</head>
<body>
<div class="card">
  <div class="icon">🔐</div>
  <h1>ESP32 Control</h1>
  %ERROR%
  %WARNING%
  <form method="POST" action="/login">
    <input type="text" name="usuario" placeholder="Usuario" required autocomplete="off">
    <input type="password" name="contrasena" placeholder="Contraseña" required>
    <button type="submit">Ingresar</button>
  </form>
</div>
</body>
</html>
)rawliteral";

// ---- Página del Panel de Control ----
const char PANEL_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>Panel ESP32</title>
<style>
  *{box-sizing:border-box;margin:0;padding:0}
  body{font-family:'Segoe UI',sans-serif;background:#1a1a2e;min-height:100vh;padding:1.5rem}
  .container{max-width:460px;margin:0 auto}
  .header{background:#16213e;padding:1.2rem 1.5rem;border-radius:12px;margin-bottom:1.2rem;display:flex;justify-content:space-between;align-items:center}
  .header h1{color:#e94560;font-size:1.2rem}
  .usuario{color:#aaa;font-size:.85rem}
  .card{background:#16213e;padding:1.5rem;border-radius:12px;margin-bottom:1.2rem}
  .card h2{color:#fff;margin-bottom:1rem;font-size:1rem;border-bottom:1px solid #0f3460;padding-bottom:.5rem}
  .led-icon{text-align:center;font-size:3.5rem;margin:1rem 0}
  .led-txt{text-align:center;font-size:1.2rem;font-weight:bold;margin-bottom:1rem}
  .row{display:flex;gap:1rem}
  .btn{flex:1;padding:.75rem;border:none;border-radius:8px;font-size:1rem;cursor:pointer;font-weight:bold;transition:background .3s}
  .btn-on{background:#27ae60;color:#fff}.btn-on:hover{background:#219a52}
  .btn-off{background:#e74c3c;color:#fff}.btn-off:hover{background:#c0392b}
  .btn-sec{width:100%;padding:.75rem;border:none;border-radius:8px;font-size:1rem;cursor:pointer;margin-bottom:.75rem;transition:background .3s}
  .btn-change{background:#2980b9;color:#fff}.btn-change:hover{background:#2471a3}
  .btn-logout{background:#555;color:#fff}.btn-logout:hover{background:#444}
  .success{color:#2ecc71;text-align:center;background:rgba(46,204,113,.1);padding:.5rem;border-radius:6px;margin-bottom:1rem;font-size:.9rem}
</style>
</head>
<body>
<div class="container">
  <div class="header">
    <h1>🎛️ Panel ESP32</h1>
    <span class="usuario">👤 %USUARIO%</span>
  </div>
  <div class="card">
    <h2>💡 Control LED</h2>
    %SUCCESS%
    <div class="led-icon">%LED_ICON%</div>
    <div class="led-txt" style="color:%LED_COLOR%">LED: %LED_STATE%</div>
    <div class="row">
      <form method="POST" action="/led" style="flex:1">
        <input type="hidden" name="estado" value="on">
        <button class="btn btn-on" type="submit">ON</button>
      </form>
      <form method="POST" action="/led" style="flex:1">
        <input type="hidden" name="estado" value="off">
        <button class="btn btn-off" type="submit">OFF</button>
      </form>
    </div>
  </div>
  <div class="card">
    <h2>⚙️ Cuenta</h2>
    <form method="GET" action="/cambiar">
      <button class="btn-sec btn-change" type="submit">Cambiar Credenciales</button>
    </form>
    <form method="POST" action="/logout">
      <button class="btn-sec btn-logout" type="submit">Cerrar Sesión</button>
    </form>
  </div>
</div>
</body>
</html>
)rawliteral";

// ---- Página de Cambio de Credenciales ----
const char CAMBIAR_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>Cambiar Credenciales</title>
<style>
  *{box-sizing:border-box;margin:0;padding:0}
  body{font-family:'Segoe UI',sans-serif;background:#1a1a2e;display:flex;justify-content:center;align-items:center;min-height:100vh;padding:1rem}
  .card{background:#16213e;padding:2rem;border-radius:12px;box-shadow:0 8px 32px rgba(0,0,0,.4);width:360px}
  h1{color:#e94560;text-align:center;margin-bottom:1.5rem;font-size:1.3rem}
  label{color:#aaa;font-size:.85rem;display:block;margin-bottom:.3rem}
  input{width:100%;padding:.75rem;margin-bottom:1rem;border:1px solid #0f3460;border-radius:8px;background:#0f3460;color:#fff;font-size:1rem}
  input:focus{outline:none;border-color:#2980b9}
  hr{border:1px solid #0f3460;margin:1rem 0}
  .btn-save{width:100%;padding:.75rem;background:#2980b9;color:#fff;border:none;border-radius:8px;font-size:1rem;cursor:pointer;margin-bottom:.75rem}
  .btn-save:hover{background:#2471a3}
  .btn-back{width:100%;padding:.75rem;background:#555;color:#fff;border:none;border-radius:8px;font-size:1rem;cursor:pointer;text-decoration:none;display:block;text-align:center}
  .btn-back:hover{background:#444}
  .msg{text-align:center;margin-bottom:1rem;font-size:.9rem;padding:.5rem;border-radius:6px}
  .error{color:#ff6b6b;background:rgba(233,69,96,.1)}
  .success{color:#2ecc71;background:rgba(46,204,113,.1)}
</style>
</head>
<body>
<div class="card">
  <h1>🔑 Cambiar Credenciales</h1>
  %MENSAJE%
  <form method="POST" action="/cambiar">
    <label>Contraseña actual</label>
    <input type="password" name="pass_actual" placeholder="Contraseña actual" required>
    <hr>
    <label>Nuevo usuario (vacío = sin cambio)</label>
    <input type="text" name="nuevo_usuario" placeholder="Nuevo usuario" autocomplete="off">
    <label>Nueva contraseña (vacío = sin cambio)</label>
    <input type="password" name="nueva_pass" placeholder="Nueva contraseña">
    <label>Confirmar nueva contraseña</label>
    <input type="password" name="confirmar_pass" placeholder="Confirmar contraseña">
    <button class="btn-save" type="submit">Guardar Cambios</button>
  </form>
  <a href="/panel" class="btn-back">← Volver al Panel</a>
</div>
</body>
</html>
)rawliteral";
