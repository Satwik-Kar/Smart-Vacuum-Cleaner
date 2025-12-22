#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "DIRECT_SmartVacuumCleaner";
const char* password = "12345678";

ESP8266WebServer server(80);

String page = R"=====(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
<title>Vacuum Controller</title>
<style>
body { margin:0; padding:0; background:#121212; color:#00ffcc; font-family:Arial; overflow:hidden; width:100vw; height:100vh; user-select:none; }
.container { display:flex; flex-direction:column; align-items:center; justify-content:center; height:100%; gap:20px; }
.controls { display:grid; grid-template-columns:80px 80px 80px; grid-template-rows:80px 80px 80px; gap:10px; }
.btn { background:#1e1e1e; border:2px solid #00ffcc; border-radius:50%; display:flex; justify-content:center; align-items:center; font-size:30px; color:#00ffcc; touch-action:manipulation; }
.btn:active { background:#00ffcc; color:#000; box-shadow:0 0 20px #00ffcc; }
.stop { border-color:#ff0055; color:#ff0055; border-radius:15px; }
.stop:active { background:#ff0055; color:#fff; }
.panel { background:#222; padding:15px; border-radius:10px; border:1px solid #444; width:300px; text-align:center; }
.mode-btn { width:45%; padding:10px; border:none; border-radius:5px; font-weight:bold; cursor:pointer; }
.mode-active { background:#00ffcc; color:#000; }
.mode-inactive { background:#444; color:#888; }
.hidden { visibility: hidden; pointer-events: none; }
</style>
<script>
function send(c) { fetch("/cmd?c=" + c); }
function setMode(m) {
  send(m);
  document.getElementById('btnAuto').className = m === 'A' ? 'mode-btn mode-active' : 'mode-btn mode-inactive';
  document.getElementById('btnMan').className = m === 'M' ? 'mode-btn mode-active' : 'mode-btn mode-inactive';
  document.getElementById('dpad').className = m === 'M' ? 'controls' : 'controls hidden';
}
</script>
</head>
<body onload="setMode('M')">
<div class="container">
  
  <div class="panel">
    <h3>MODE</h3>
    <button id="btnAuto" class="mode-btn" onclick="setMode('A')">AUTO</button>
    <button id="btnMan" class="mode-btn" onclick="setMode('M')">MANUAL</button>
  </div>

  <div id="dpad" class="controls hidden">
    <div></div>
    <div class="btn" ontouchstart="send('F')" ontouchend="send('S')">&#9650;</div>
    <div></div>
    <div class="btn" ontouchstart="send('L')" ontouchend="send('S')">&#9664;</div>
    <div class="btn stop" ontouchstart="send('S')">&#9632;</div>
    <div class="btn" ontouchstart="send('R')" ontouchend="send('S')">&#9654;</div>
    <div></div>
    <div class="btn" ontouchstart="send('B')" ontouchend="send('S')">&#9660;</div>
    <div></div>
  </div>

</div>
</body>
</html>
)=====";

void setup() {
  Serial.begin(9600);
  WiFi.softAP(ssid, password);
  server.on("/", []() { server.send(200, "text/html", page); });
  server.on("/cmd", []() {
    String c = server.arg("c");
    Serial.print(c);
    server.send(200, "text/plain", "OK");
  });
  server.begin();
}

void loop() { server.handleClient(); }