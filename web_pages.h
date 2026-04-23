// web_pages.h
// Plantillas HTML del proyecto.
#pragma once

const char LOGIN_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>ESP32 - Iniciar sesion</title>
<style>
  :root{
    --bg:#08101d;--bg2:#0d1830;--card:rgba(15,24,45,.94);--line:rgba(255,255,255,.08);
    --text:#eef4ff;--muted:#9fb0cd;--blue:#2563eb;--teal:#0f766e;--slate:#334155;
  }
  *{box-sizing:border-box;margin:0;padding:0}
  body{
    min-height:100vh;padding:1rem;font-family:'Segoe UI',sans-serif;color:var(--text);
    background:
      radial-gradient(circle at top left, rgba(37,99,235,.16), transparent 22rem),
      radial-gradient(circle at bottom right, rgba(15,118,110,.14), transparent 18rem),
      linear-gradient(180deg,var(--bg),var(--bg2));
  }
  .shell{max-width:920px;margin:0 auto;display:grid;grid-template-columns:1fr 1fr;gap:1rem}
  .card{background:var(--card);border:1px solid var(--line);border-radius:24px;padding:1.35rem;box-shadow:0 20px 50px rgba(0,0,0,.28)}
  .badge{display:inline-flex;align-items:center;gap:.45rem;padding:.45rem .75rem;border-radius:999px;background:rgba(37,99,235,.16);border:1px solid rgba(37,99,235,.24);color:#bfdbfe;font-size:.86rem}
  h1{font-size:clamp(2rem,4vw,2.8rem);line-height:1.02;margin:.9rem 0 .55rem}
  h2{font-size:1.3rem;margin-bottom:.45rem}
  p{color:var(--muted);line-height:1.55}
  .state,.msg{margin-top:1rem;padding:.85rem 1rem;border-radius:16px;font-size:.93rem;line-height:1.45}
  .state.ok,.success{background:rgba(34,197,94,.12);border:1px solid rgba(34,197,94,.24);color:#bbf7d0}
  .state.warn,.warn{background:rgba(245,158,11,.12);border:1px solid rgba(245,158,11,.24);color:#fde68a}
  .error{background:rgba(239,68,68,.12);border:1px solid rgba(239,68,68,.24);color:#fecaca}
  .meta{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:.75rem;margin-top:1.2rem}
  .chip{background:rgba(255,255,255,.03);border:1px solid rgba(255,255,255,.05);border-radius:18px;padding:.9rem}
  .chip span{display:block;font-size:.78rem;color:#89a0c7;margin-bottom:.28rem}
  .chip strong{display:block;font-size:1rem;word-break:break-word}
  form{display:grid;gap:.85rem;margin-top:1rem}
  label{font-size:.88rem;color:#b8c8e6}
  input{width:100%;padding:.95rem 1rem;border-radius:14px;border:1px solid #2a4475;background:#0b1428;color:#fff;font-size:1rem}
  input:focus{outline:none;border-color:#60a5fa;box-shadow:0 0 0 3px rgba(96,165,250,.18)}
  .btn{display:inline-flex;align-items:center;justify-content:center;width:100%;padding:1rem;border:none;border-radius:14px;font-weight:700;font-size:1rem;color:#fff;text-decoration:none;cursor:pointer}
  .btn-primary{background:linear-gradient(135deg,var(--blue),#1d4ed8)}
  .btn-secondary{background:linear-gradient(135deg,var(--teal),#0f9f90)}
  .actions{display:grid;gap:.75rem;margin-top:1rem}
  .helper{margin-top:1rem;font-size:.92rem;color:#91a5c8}
  @media (max-width:820px){
    .shell,.meta{grid-template-columns:1fr}
  }
</style>
</head>
<body>
<div class="shell">
  <section class="card">
    <div class="badge">&#9679; ESP32 en red local</div>
    <h1>Entrar al sistema</h1>
    <p>Control del ESP32.</p>
    <div class="meta">
      <div class="chip"><span>SSID</span><strong>%AP_SSID%</strong></div>
      <div class="chip"><span>IP</span><strong>%AP_IP%</strong></div>
      <div class="chip"><span>Tiempo activo</span><strong>%UPTIME%</strong></div>
    </div>
    %STATUS_CREDENCIALES%
  </section>

  <section class="card">
    <h2>Iniciar sesion</h2>
    <p>Validacion con SHA-256.</p>
    %LOGIN_SUCCESS%
    %LOGIN_WARNING%
    %LOGIN_ERROR%
    <form method="POST" action="/login">
      <div>
        <label for="usuario">Usuario</label>
        <input id="usuario" name="usuario" type="text" placeholder="Tu usuario" required autocomplete="off">
      </div>
      <div>
        <label for="contrasena">Contrasena</label>
        <input id="contrasena" name="contrasena" type="password" placeholder="Tu contrasena" required>
      </div>
      <button class="btn btn-primary" type="submit">Entrar al panel</button>
    </form>
    <div class="actions">
      %CTA_SECUNDARIO%
    </div>
    <p class="helper">%ACCESO_NOTA%</p>
    <p class="helper">3 intentos. Luego se bloquea 1 minuto.</p>
  </section>
</div>
</body>
</html>
)rawliteral";

const char PANEL_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>ESP32 - Panel</title>
<style>
  :root{
    --bg:#08101d;--bg2:#0d1830;--card:rgba(15,24,45,.94);--line:rgba(255,255,255,.08);
    --text:#eef4ff;--muted:#9fb0cd;--blue:#2563eb;--green:#16a34a;--red:#dc2626;--slate:#334155;--teal:#0f766e;
  }
  *{box-sizing:border-box;margin:0;padding:0}
  body{
    min-height:100vh;padding:1rem;font-family:'Segoe UI',sans-serif;color:var(--text);
    background:
      radial-gradient(circle at top left, rgba(37,99,235,.16), transparent 20rem),
      radial-gradient(circle at bottom right, rgba(34,197,94,.12), transparent 18rem),
      linear-gradient(180deg,var(--bg),var(--bg2));
  }
  .shell{max-width:980px;margin:0 auto;display:grid;gap:1rem}
  .card{background:var(--card);border:1px solid var(--line);border-radius:24px;padding:1.3rem;box-shadow:0 20px 50px rgba(0,0,0,.28)}
  .top{display:flex;justify-content:space-between;gap:1rem;align-items:flex-start;flex-wrap:wrap}
  .badge{display:inline-flex;align-items:center;gap:.45rem;padding:.45rem .75rem;border-radius:999px;background:rgba(37,99,235,.16);border:1px solid rgba(37,99,235,.24);color:#bfdbfe;font-size:.86rem}
  h1{font-size:clamp(1.9rem,4vw,2.6rem);margin:.75rem 0 .45rem}
  h2{font-size:1.25rem;margin-bottom:.45rem}
  p{color:var(--muted);line-height:1.55}
  .state,.msg{margin-top:1rem;padding:.85rem 1rem;border-radius:16px;font-size:.93rem;line-height:1.45}
  .state.ok,.success{background:rgba(34,197,94,.12);border:1px solid rgba(34,197,94,.24);color:#bbf7d0}
  .state.warn,.warn{background:rgba(245,158,11,.12);border:1px solid rgba(245,158,11,.24);color:#fde68a}
  .error{background:rgba(239,68,68,.12);border:1px solid rgba(239,68,68,.24);color:#fecaca}
  .grid{display:grid;grid-template-columns:1.1fr .9fr;gap:1rem}
  .meta{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:.75rem;margin-top:1rem}
  .chip{background:rgba(255,255,255,.03);border:1px solid rgba(255,255,255,.05);border-radius:18px;padding:.9rem}
  .chip span{display:block;font-size:.78rem;color:#89a0c7;margin-bottom:.28rem}
  .chip strong{display:block;font-size:1rem;word-break:break-word}
  .lamp{width:88px;height:88px;border-radius:50%;background:%COLOR_LED%;box-shadow:0 0 0 10px rgba(255,255,255,.03),0 0 28px %COLOR_LED%;margin:1rem 0}
  .status{font-size:1.05rem;font-weight:700}
  .actions,.menu{display:grid;gap:.75rem}
  .actions{grid-template-columns:repeat(2,minmax(0,1fr));margin-top:1rem}
  .menu{margin-top:1rem}
  .btn{display:inline-flex;align-items:center;justify-content:center;width:100%;padding:1rem;border:none;border-radius:14px;font-weight:700;font-size:1rem;color:#fff;text-decoration:none;cursor:pointer}
  .btn-on{background:linear-gradient(135deg,var(--green),#15803d)}
  .btn-off{background:linear-gradient(135deg,var(--red),#b91c1c)}
  .btn-blue{background:linear-gradient(135deg,var(--blue),#1d4ed8)}
  .btn-teal{background:linear-gradient(135deg,var(--teal),#0f9f90)}
  .btn-dark{background:linear-gradient(135deg,#4b5563,var(--slate))}
  .helper{margin-top:.8rem;font-size:.92rem;color:#91a5c8}
  @media (max-width:820px){
    .grid,.meta,.actions{grid-template-columns:1fr}
  }
</style>
</head>
<body>
<div class="shell">
  <section class="card top">
    <div>
      <div class="badge">&#9679; Sesion activa</div>
      <h1>Panel del ESP32</h1>
      <p>Control y acceso rapido.</p>
    </div>
    %STATUS_CREDENCIALES%
  </section>

  <section class="grid">
    <article class="card">
      <h2>LED principal</h2>
      <div class="lamp"></div>
      <p class="status" style="color:%COLOR_LED%">Estado actual: %ESTADO_LED%</p>
      <p>%LED_DESCRIPCION%</p>
      <p class="helper">%LED_SALIDA%</p>
      <div class="actions">
        <form method="POST" action="/led">
          <input type="hidden" name="estado" value="on">
          <button class="btn btn-on" type="submit">Encender</button>
        </form>
        <form method="POST" action="/led">
          <input type="hidden" name="estado" value="off">
          <button class="btn btn-off" type="submit">Apagar</button>
        </form>
      </div>
      %PANEL_MSG%
    </article>

    <article class="card">
      <h2>Accesos</h2>
      <div class="meta">
        <div class="chip"><span>SSID</span><strong>%AP_SSID%</strong></div>
        <div class="chip"><span>IP</span><strong>%AP_IP%</strong></div>
        <div class="chip"><span>Tiempo activo</span><strong>%UPTIME%</strong></div>
      </div>
      <div class="menu">
        <a class="btn btn-teal" href="/evidencia">Ver evidencia SHA-256</a>
        <a class="btn btn-blue" href="/cambiar">Cambiar credenciales</a>
        <form method="POST" action="/logout">
          <button class="btn btn-dark" type="submit">Cerrar sesion</button>
        </form>
      </div>
    </article>
  </section>
</div>
</body>
</html>
)rawliteral";

const char CAMBIAR_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>ESP32 - Cambiar credenciales</title>
<style>
  :root{
    --bg:#09111f;--bg2:#0d1830;--card:#12213f;--line:rgba(255,255,255,.08);--text:#eef4ff;--muted:#9bacc9;
  }
  *{box-sizing:border-box;margin:0;padding:0}
  body{
    font-family:'Segoe UI',sans-serif;color:var(--text);min-height:100vh;padding:1rem;
    background:
      radial-gradient(circle at top left, rgba(56,189,248,.14), transparent 18rem),
      linear-gradient(180deg,var(--bg),var(--bg2));
    display:flex;align-items:center;justify-content:center;
  }
  .card{width:min(100%,640px);background:rgba(18,33,63,.95);border:1px solid var(--line);border-radius:22px;padding:1.3rem;box-shadow:0 20px 50px rgba(0,0,0,.28)}
  h1{font-size:1.8rem;margin-bottom:.55rem}
  p{color:var(--muted);line-height:1.55}
  .msg{margin:1rem 0;padding:.85rem 1rem;border-radius:14px;font-size:.93rem;line-height:1.45}
  .success{background:rgba(34,197,94,.12);border:1px solid rgba(34,197,94,.24);color:#bbf7d0}
  .warn{background:rgba(245,158,11,.12);border:1px solid rgba(245,158,11,.25);color:#fde68a}
  .error{background:rgba(239,68,68,.12);border:1px solid rgba(239,68,68,.25);color:#fecaca}
  form{display:grid;gap:.85rem;margin-top:1rem}
  .grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:.85rem}
  label{font-size:.88rem;color:#b8c8e6}
  input{width:100%;padding:.95rem 1rem;border-radius:14px;border:1px solid #2a4475;background:#0b1428;color:#fff;font-size:1rem}
  input:focus{outline:none;border-color:#60a5fa;box-shadow:0 0 0 3px rgba(96,165,250,.18)}
  .actions{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:.8rem}
  .btn{display:inline-flex;align-items:center;justify-content:center;width:100%;padding:1rem;border:none;border-radius:14px;font-weight:700;font-size:1rem;color:#fff;text-decoration:none;cursor:pointer}
  .btn-save{background:linear-gradient(135deg,#2563eb,#1d4ed8)}
  .btn-back{background:linear-gradient(135deg,#4b5563,#374151)}
  .hint{margin-top:1rem;font-size:.9rem;color:#95a9cd}
  @media (max-width:640px){.grid,.actions{grid-template-columns:1fr}}
</style>
</head>
<body>
<div class="card">
  <h1>Cambiar credenciales de forma segura</h1>
  <p>Esta vista se usa despues del registro inicial. Aqui el ESP32 vuelve a calcular SHA-256 y reemplaza los hashes antiguos en NVS solo si primero validas el acceso actual.</p>
  %CAMBIO_MENSAJE%
  <form method="POST" action="/cambiar">
    <div class="grid">
      <div>
        <label for="usuario_actual">Usuario actual</label>
        <input id="usuario_actual" name="usuario_actual" type="text" placeholder="Usuario actual" required autocomplete="off">
      </div>
      <div>
        <label for="pass_actual">Contrasena actual</label>
        <input id="pass_actual" name="pass_actual" type="password" placeholder="Contrasena actual" required>
      </div>
    </div>
    <div class="grid">
      <div>
        <label for="nuevo_usuario">Nuevo usuario</label>
        <input id="nuevo_usuario" name="nuevo_usuario" type="text" placeholder="Dejalo vacio si no cambia" autocomplete="off">
      </div>
      <div>
        <label for="nueva_pass">Nueva contrasena</label>
        <input id="nueva_pass" name="nueva_pass" type="password" placeholder="Dejala vacia si no cambia">
      </div>
    </div>
    <div>
      <label for="confirmar_pass">Confirmar nueva contrasena</label>
      <input id="confirmar_pass" name="confirmar_pass" type="password" placeholder="Repite la nueva contrasena">
    </div>
    <div class="actions">
      <button class="btn btn-save" type="submit">Guardar cambios</button>
      <a class="btn btn-back" href="/panel">Volver al panel</a>
    </div>
  </form>
  <p class="hint">Despues de guardar, deberas iniciar sesion otra vez con las nuevas credenciales.</p>
</div>
</body>
</html>
)rawliteral";

const char REGISTRO_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>ESP32 - Registro inicial</title>
<style>
  :root{
    --bg:#08101d;--bg2:#0d1830;--card:rgba(15,24,45,.95);--line:rgba(255,255,255,.08);
    --text:#eef4ff;--muted:#9fb0cd;--teal:#0f766e;--slate:#334155;
  }
  *{box-sizing:border-box;margin:0;padding:0}
  body{
    min-height:100vh;padding:1rem;font-family:'Segoe UI',sans-serif;color:var(--text);
    background:
      radial-gradient(circle at top left, rgba(34,197,94,.12), transparent 18rem),
      radial-gradient(circle at bottom right, rgba(37,99,235,.14), transparent 18rem),
      linear-gradient(180deg,var(--bg),var(--bg2));
    display:flex;align-items:center;justify-content:center;
  }
  .card{width:min(100%,720px);background:var(--card);border:1px solid var(--line);border-radius:24px;padding:1.35rem;box-shadow:0 20px 50px rgba(0,0,0,.28)}
  .badge{display:inline-flex;align-items:center;gap:.45rem;padding:.45rem .75rem;border-radius:999px;background:rgba(15,118,110,.16);border:1px solid rgba(15,118,110,.24);color:#99f6e4;font-size:.86rem}
  h1{font-size:clamp(2rem,4vw,2.7rem);margin:.85rem 0 .45rem}
  p{color:var(--muted);line-height:1.55}
  .state,.msg{margin-top:1rem;padding:.85rem 1rem;border-radius:16px;font-size:.93rem;line-height:1.45}
  .state.ok,.success{background:rgba(34,197,94,.12);border:1px solid rgba(34,197,94,.24);color:#bbf7d0}
  .state.warn,.warn{background:rgba(245,158,11,.12);border:1px solid rgba(245,158,11,.24);color:#fde68a}
  .error{background:rgba(239,68,68,.12);border:1px solid rgba(239,68,68,.24);color:#fecaca}
  form{display:grid;gap:.85rem;margin-top:1rem}
  .grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:.85rem}
  label{font-size:.88rem;color:#b8c8e6}
  input{width:100%;padding:.95rem 1rem;border-radius:14px;border:1px solid #2a4475;background:#0b1428;color:#fff;font-size:1rem}
  input:focus{outline:none;border-color:#60a5fa;box-shadow:0 0 0 3px rgba(96,165,250,.18)}
  .actions{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:.8rem}
  .btn{display:inline-flex;align-items:center;justify-content:center;width:100%;padding:1rem;border:none;border-radius:14px;font-weight:700;font-size:1rem;color:#fff;text-decoration:none;cursor:pointer}
  .btn-save{background:linear-gradient(135deg,var(--teal),#0f9f90)}
  .btn-back{background:linear-gradient(135deg,#4b5563,var(--slate))}
  .hint{margin-top:1rem;font-size:.92rem;color:#91a5c8}
  @media (max-width:680px){.grid,.actions{grid-template-columns:1fr}}
</style>
</head>
<body>
<div class="card">
  <div class="badge">&#9679; Primer acceso</div>
  <h1>Crear acceso inicial</h1>
  <p>Crea tu usuario inicial.</p>
  %STATUS_CREDENCIALES%
  %REGISTRO_MSG%
  <form method="POST" action="/registro">
    <div class="grid">
      <div>
        <label for="nuevo_usuario">Nuevo usuario</label>
        <input id="nuevo_usuario" name="nuevo_usuario" type="text" placeholder="Usuario para el sistema" required autocomplete="off">
      </div>
      <div>
        <label for="nueva_pass">Nueva contrasena</label>
        <input id="nueva_pass" name="nueva_pass" type="password" placeholder="Contrasena nueva" required>
      </div>
    </div>
    <div>
      <label for="confirmar_pass">Confirmar contrasena</label>
      <input id="confirmar_pass" name="confirmar_pass" type="password" placeholder="Repite la contrasena" required>
    </div>
    <div class="actions">
      <button class="btn btn-save" type="submit">Guardar registro</button>
      <a class="btn btn-back" href="/login">Ya tengo cuenta</a>
    </div>
  </form>
  <p class="hint">Si ya tienes cuenta, entra por login.</p>
</div>
</body>
</html>
)rawliteral";

const char EVIDENCIA_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>ESP32 - Evidencia SHA-256</title>
<style>
  :root{
    --bg:#08101d;--bg2:#0d1830;--card:rgba(15,24,45,.94);--card2:#0d1830;--line:rgba(255,255,255,.08);
    --text:#eef4ff;--muted:#9fb0cd;
  }
  *{box-sizing:border-box;margin:0;padding:0}
  body{
    font-family:'Segoe UI',sans-serif;color:var(--text);min-height:100vh;padding:1rem;
    background:
      radial-gradient(circle at top left, rgba(56,189,248,.12), transparent 20rem),
      radial-gradient(circle at bottom right, rgba(37,99,235,.12), transparent 20rem),
      linear-gradient(180deg,var(--bg),var(--bg2));
  }
  .app{max-width:1040px;margin:0 auto;display:grid;gap:1rem}
  .card{background:var(--card);border:1px solid var(--line);border-radius:24px;padding:1.25rem;box-shadow:0 20px 50px rgba(0,0,0,.28)}
  .hero{display:flex;justify-content:space-between;gap:1rem;align-items:flex-start;flex-wrap:wrap}
  .badge{display:inline-flex;align-items:center;gap:.45rem;padding:.45rem .75rem;border-radius:999px;background:rgba(37,99,235,.16);border:1px solid rgba(37,99,235,.24);color:#bfdbfe;font-size:.86rem;margin-bottom:.85rem}
  h1{font-size:clamp(1.9rem,4vw,2.7rem);margin-bottom:.45rem}
  h2{font-size:1.25rem;margin-bottom:.45rem}
  p{color:var(--muted);line-height:1.55}
  .state{padding:.75rem .95rem;border-radius:16px;font-size:.92rem;line-height:1.45;max-width:320px}
  .state.ok{background:rgba(34,197,94,.12);border:1px solid rgba(34,197,94,.24);color:#bbf7d0}
  .state.warn{background:rgba(245,158,11,.12);border:1px solid rgba(245,158,11,.25);color:#fde68a}
  .msg{margin-top:.9rem;padding:.85rem 1rem;border-radius:14px;font-size:.93rem;line-height:1.45}
  .success{background:rgba(34,197,94,.12);border:1px solid rgba(34,197,94,.24);color:#bbf7d0}
  .warn{background:rgba(245,158,11,.12);border:1px solid rgba(245,158,11,.25);color:#fde68a}
  .error{background:rgba(239,68,68,.12);border:1px solid rgba(239,68,68,.25);color:#fecaca}
  .grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:1rem}
  .hash-box,.meta-box{background:var(--card2);border:1px solid rgba(255,255,255,.05);border-radius:18px;padding:1rem}
  .hash-box span,.meta-box span{display:block;color:#89a0c7;font-size:.8rem;margin-bottom:.4rem}
  .hash{font-family:Consolas,'Courier New',monospace;font-size:.92rem;color:#dbeafe;word-break:break-all;line-height:1.55}
  .pill{display:inline-flex;align-items:center;gap:.45rem;padding:.45rem .75rem;border-radius:999px;font-size:.86rem}
  .pill.ok{background:rgba(34,197,94,.12);border:1px solid rgba(34,197,94,.24);color:#bbf7d0}
  .pill.warn{background:rgba(245,158,11,.12);border:1px solid rgba(245,158,11,.25);color:#fde68a}
  form{display:grid;gap:.85rem}
  .form-grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:.85rem}
  label{font-size:.88rem;color:#b8c8e6}
  input{width:100%;padding:.95rem 1rem;border-radius:14px;border:1px solid #2a4475;background:#0b1428;color:#fff;font-size:1rem}
  input:focus{outline:none;border-color:#60a5fa;box-shadow:0 0 0 3px rgba(96,165,250,.18)}
  .actions{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:.8rem}
  .btn{display:inline-flex;align-items:center;justify-content:center;width:100%;padding:1rem;border:none;border-radius:14px;font-weight:700;font-size:1rem;color:#fff;text-decoration:none;cursor:pointer}
  .btn-primary{background:linear-gradient(135deg,#2563eb,#1d4ed8)}
  .btn-dark{background:linear-gradient(135deg,#4b5563,#374151)}
  @media (max-width:760px){
    .grid,.form-grid,.actions{grid-template-columns:1fr}
  }
</style>
</head>
<body>
<div class="app">
  <section class="card">
    <div class="hero">
      <div>
        <div class="badge">&#9679; NVS + SHA-256</div>
        <h1>Evidencia tecnica del SHA-256</h1>
        <p>Hashes guardados en NVS.</p>
      </div>
      %STATUS_CREDENCIALES%
    </div>
    %EVIDENCIA_MSG%
  </section>

  <section class="grid">
    <article class="card">
      <h2>Hashes guardados</h2>
      <div class="hash-box" style="margin-top:1rem">
        <span>Clave NVS: auth / userHash</span>
        <div class="hash">%HASH_USUARIO_GUARDADO%</div>
      </div>
      <div class="hash-box" style="margin-top:.8rem">
        <span>Clave NVS: auth / passHash</span>
        <div class="hash">%HASH_PASS_GUARDADO%</div>
      </div>
      <div class="meta-box" style="margin-top:.8rem">
        <span>NVS</span>
        <p>Namespace <strong>auth</strong>: <strong>userHash</strong>, <strong>passHash</strong>, <strong>attempts</strong> y <strong>locked</strong>.</p>
      </div>
    </article>

    <article class="card">
      <h2>Probar coincidencia</h2>
      <p>Ingresa valores y compara.</p>
      <form method="POST" action="/evidencia" style="margin-top:1rem">
        <div class="form-grid">
          <div>
            <label for="usuario_demo">Usuario a comprobar</label>
            <input id="usuario_demo" name="usuario_demo" type="text" placeholder="Usuario de prueba" autocomplete="off">
          </div>
          <div>
            <label for="contrasena_demo">Contrasena a comprobar</label>
            <input id="contrasena_demo" name="contrasena_demo" type="password" placeholder="Contrasena de prueba">
          </div>
        </div>
        <div class="actions">
          <button class="btn btn-primary" type="submit">Calcular y comparar</button>
          <a class="btn btn-dark" href="/">Volver al inicio</a>
        </div>
      </form>
      <div class="hash-box" style="margin-top:1rem">
        <span>SHA-256 calculado del usuario ingresado</span>
        <div class="hash">%HASH_USUARIO_CALCULADO%</div>
      </div>
      <div style="margin-top:.6rem">%MATCH_USUARIO%</div>
      <div class="hash-box" style="margin-top:.8rem">
        <span>SHA-256 calculado de la contrasena ingresada</span>
        <div class="hash">%HASH_PASS_CALCULADO%</div>
      </div>
      <div style="margin-top:.6rem">%MATCH_PASS%</div>
    </article>
  </section>
</div>
</body>
</html>
)rawliteral";
