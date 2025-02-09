#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
const char* ssid = "YourWiFiName";
const char* password = "YourWiFiPassword";
ESP8266WebServer server(80);
int ledPin = 2;
String page = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Calculator</title>
  <style>
    * { box-sizing: border-box; }
    body, html { margin: 0; padding: 0; height: 100%; overflow: hidden; background: #000; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; color: #0ff; }
    #bgCanvas { position: fixed; top: 0; left: 0; width: 100%; height: 100%; z-index: 0; }
    .container { position: relative; z-index: 2; display: flex; flex-direction: column; align-items: center; justify-content: center; height: 100%; }
    .calculator { background: rgba(17, 17, 17, 0.85); border: 2px solid #0ff; border-radius: 20px; padding: 20px; box-shadow: 0 0 30px #0ff; width: 350px; max-width: 90%; transition: background 0.5s ease; }
    .opacity-control { width: 100%; margin-bottom: 10px; text-align: center; color: #fff; }
    .opacity-control input[type=range] { width: 80%; margin-top: 5px; }
    .display { background: #000; color: #0f0; font-size: 2em; text-align: right; padding: 15px; border: 2px solid #0ff; border-radius: 10px; min-height: 60px; overflow: hidden; text-shadow: 0 0 10px #0ff; margin-bottom: 15px; }
    .buttons { display: grid; grid-template-columns: repeat(4, 1fr); grid-gap: 10px; }
    button { padding: 20px; font-size: 1.5em; border: none; border-radius: 10px; background: #222; color: #0ff; cursor: pointer; transition: all 0.2s; box-shadow: 0 0 5px #0ff; }
    button:hover { background: #0ff; color: #000; box-shadow: 0 0 15px #0ff; }
    button:active { transform: scale(0.95); }
    .wide { grid-column: span 4; background: #ff073a; color: #fff; box-shadow: 0 0 5px #ff073a; }
  </style>
</head>
<body>
  <canvas id="bgCanvas"></canvas>
  <div class="container">
    <div class="calculator" id="calcContainer">
      <div class="opacity-control">
        Transparency:
        <input type="range" id="opacityRange" min="10" max="100" value="85">
      </div>
      <div id="display" class="display"></div>
      <div class="buttons">
        <button data-value="7">7</button>
        <button data-value="8">8</button>
        <button data-value="9">9</button>
        <button data-op="/">÷</button>
        <button data-value="4">4</button>
        <button data-value="5">5</button>
        <button data-value="6">6</button>
        <button data-op="*">×</button>
        <button data-value="1">1</button>
        <button data-value="2">2</button>
        <button data-value="3">3</button>
        <button data-op="-">−</button>
        <button data-value="0">0</button>
        <button data-value=".">.</button>
        <button id="equals">=</button>
        <button data-op="+">+</button>
        <button class="wide" id="clear">C</button>
      </div>
    </div>
  </div>
  <script>
    var canvas = document.getElementById("bgCanvas");
    var ctx = canvas.getContext("2d");
    function resizeCanvas() {
      canvas.width = window.innerWidth;
      canvas.height = window.innerHeight;
    }
    window.addEventListener("resize", resizeCanvas);
    resizeCanvas();
    var auroraBands = [];
    function initAurora() {
      auroraBands = [];
      for (var i = 0; i < 6; i++) {
        var band = {
          amplitude: Math.random() * 30 + 20,
          wavelength: Math.random() * 80 + 120,
          speed: Math.random() * 0.005 + 0.002,
          yOffset: Math.random() * canvas.height,
          color: "rgba(0,150,255,0.3)",
          phase: Math.random() * Math.PI * 2
        };
        auroraBands.push(band);
      }
    }
    initAurora();
    var auroraTime = 0;
    function drawAurora() {
      ctx.clearRect(0, 0, canvas.width, canvas.height);
      auroraTime += 0.7;
      for (var i = 0; i < auroraBands.length; i++) {
        var band = auroraBands[i];
        ctx.beginPath();
        ctx.moveTo(0, band.yOffset + Math.sin(0 * 2 * Math.PI / band.wavelength + band.phase + auroraTime * band.speed) * band.amplitude);
        for (var x = 0; x <= canvas.width; x += 8) {
          var y = band.yOffset + Math.sin(x * 2 * Math.PI / band.wavelength + band.phase + auroraTime * band.speed) * band.amplitude;
          ctx.lineTo(x, y);
        }
        ctx.lineTo(canvas.width, canvas.height);
        ctx.lineTo(0, canvas.height);
        ctx.closePath();
        var gradient = ctx.createLinearGradient(0, band.yOffset - band.amplitude, 0, band.yOffset + band.amplitude);
        gradient.addColorStop(0, "rgba(0,150,255,0.5)");
        gradient.addColorStop(1, "rgba(0,0,0,0)");
        ctx.fillStyle = gradient;
        ctx.fill();
      }
      requestAnimationFrame(drawAurora);
    }
    drawAurora();
  </script>
  <script>
    const display = document.getElementById("display");
    const opacityRange = document.getElementById("opacityRange");
    const calcContainer = document.getElementById("calcContainer");
    opacityRange.addEventListener("input", function() {
      calcContainer.style.background = "rgba(17, 17, 17, " + (opacityRange.value / 100) + ")";
    });
    let current = "";
    let stored = null;
    let op = "";
    document.querySelectorAll("button[data-value]").forEach(b => {
      b.addEventListener("click", e => {
        current += e.target.getAttribute("data-value");
        display.innerText = current;
      });
    });
    document.querySelectorAll("button[data-op]").forEach(b => {
      b.addEventListener("click", e => {
        if (current === "") return;
        stored = parseFloat(current);
        op = e.target.getAttribute("data-op");
        current = "";
        display.innerText = stored + " " + op;
      });
    });
    document.getElementById("clear").addEventListener("click", () => {
      current = "";
      stored = null;
      op = "";
      display.innerText = "";
    });
    document.getElementById("equals").addEventListener("click", () => {
      if (current === "") return;
      let second = parseFloat(current);
      fetch("/calc?x=" + stored + "&y=" + second + "&op=" + encodeURIComponent(op))
        .then(r => r.text())
        .then(result => {
          display.innerText = result;
          current = result;
          stored = null;
          op = "";
        });
    });
  </script>
  <script>
    var extraParticles = [];
    function initExtraParticles() {
      extraParticles = [];
      for (var i = 0; i < 40; i++) {
        extraParticles.push({
          x: Math.random() * canvas.width,
          y: Math.random() * canvas.height,
          radius: Math.random() * 3 + 1,
          dx: (Math.random() - 0.5) * 1.5,
          dy: (Math.random() - 0.5) * 1.5,
          opacity: Math.random() * 0.5 + 0.5
        });
      }
    }
    initExtraParticles();
    function drawExtraParticles() {
      extraParticles.forEach(function(p) {
        p.x += p.dx;
        p.y += p.dy;
        if (p.x > canvas.width) p.x = 0;
        if (p.x < 0) p.x = canvas.width;
        if (p.y > canvas.height) p.y = 0;
        if (p.y < 0) p.y = canvas.height;
        ctx.beginPath();
        ctx.arc(p.x, p.y, p.radius, 0, Math.PI * 2);
        ctx.fillStyle = "rgba(0,150,255," + p.opacity + ")";
        ctx.fill();
      });
      requestAnimationFrame(drawExtraParticles);
    }
    drawExtraParticles();
  </script>
</body>
</html>
)rawliteral";
void blinkLED() {
  digitalWrite(ledPin, LOW);
  delay(100);
  digitalWrite(ledPin, HIGH);
}
void handleRoot() {
  server.send(200, "text/html", page);
}
void handleCalc() {
  if (!server.hasArg("x") || !server.hasArg("y") || !server.hasArg("op")) {
    server.send(200, "text/plain", "Eksik");
    return;
  }
  double x = server.arg("x").toDouble();
  double y = server.arg("y").toDouble();
  String op = server.arg("op");
  double result = 0;
  if (op == "+")
    result = x + y;
  else if (op == "-")
    result = x - y;
  else if (op == "*")
    result = x * y;
  else if (op == "/") {
    if (y == 0) {
      server.send(200, "text/plain", "Tanimsiz");
      return;
    }
    result = x / y;
  }
  blinkLED();
  server.send(200, "text/plain", String(result));
}
void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(300); }
  server.on("/", handleRoot);
  server.on("/calc", handleCalc);
  server.begin();
}
void loop() {
  server.handleClient();
}
