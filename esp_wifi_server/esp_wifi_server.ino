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
<title>4WD Controller</title>
<style>
body {
  margin: 0; padding: 0; background: #121212; color: #00ffcc;
  font-family: Arial, sans-serif; overflow: hidden;
  width: 100vw; height: 100vh;
  user-select: none; -webkit-user-select: none;
}

.container {
  display: flex; width: 100%; height: 100%;
  justify-content: space-between; align-items: center;
  padding: 0 40px; box-sizing: border-box;
}

@media screen and (orientation:portrait) {
  .container {
    position: absolute;
    top: 50%; left: 50%;
    width: 100vh; height: 100vw;
    transform: translate(-50%, -50%) rotate(90deg);
  }
}

.dpad {
  display: grid; grid-template-columns: 80px 80px 80px;
  grid-template-rows: 80px 80px 80px; gap: 10px;
}

.btn {
  background: #1e1e1e; border: 2px solid #00ffcc; border-radius: 50%;
  display: flex; justify-content: center; align-items: center;
  font-size: 30px; color: #00ffcc; 
  box-shadow: 0 0 10px rgba(0, 255, 204, 0.2);
  touch-action: manipulation;
}
.btn:active { background: #00ffcc; color: #000; box-shadow: 0 0 20px #00ffcc; }
.stop { border-color: #ff0055; color: #ff0055; border-radius: 15px; }
.stop:active { background: #ff0055; color: #fff; box-shadow: 0 0 20px #ff0055; }

.slider-group {
  display: flex; flex-direction: row; align-items: center;
  gap: 20px;
}
.label { 
  font-size: 18px; font-weight: bold; letter-spacing: 2px; 
  transform: rotate(-90deg);
}

input[type=range] {
  -webkit-appearance: none; width: 200px; height: 60px;
  background: #222; border-radius: 30px; outline: none;
  border: 2px solid #555;
  transform: rotate(-90deg);
}
input[type=range]::-webkit-slider-thumb {
  -webkit-appearance: none; width: 50px; height: 50px;
  border-radius: 50%; background: #00ffcc; cursor: pointer;
  box-shadow: 0 0 10px #00ffcc;
}
</style>
<script>
function send(c) { fetch("/cmd?c=" + c); }
function setSpeed(val) { fetch("/speed?v=" + val); }
</script>
</head>
<body>
<div class="container">
  
  <div class="dpad">
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

  <div class="slider-group">
    <div class="label">SPEED</div>
    <input type="range" min="0" max="9" value="9" oninput="setSpeed(this.value)">
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

  server.on("/speed", []() {
    String v = server.arg("v");
    Serial.print(v); 
    server.send(200, "text/plain", "OK");
  });

  server.begin();
}

void loop() { server.handleClient(); }