#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "limits";
const char* password = "LMZnice723";
const int LED_PIN = 2;

WebServer server(80);

int brightness = 0;

String makePage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>实验2</title>
</head>
<body style="font-family:Arial; text-align:center; margin-top:50px;">
  <h1>第二部分：按钮控制 LED</h1>
  
  <h3>LED 亮度调节</h3>
  <input type="range" min="0" max="255" id="slider" style="width:300px;">
  <p>当前亮度：<span id="val">0</span></p>

  <script>
    const slider = document.getElementById('slider');
    const valueText = document.getElementById('val');
    
    slider.addEventListener('input', function() {
      valueText.textContent = this.value;
      fetch('/set?bright=' + this.value);
    });
  </script>
</body>
</html>
)rawliteral";
  return html;
}

void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

void handleSet() {
  if (server.hasArg("bright")) {
    brightness = server.arg("bright").toInt();
    if (brightness < 0) brightness = 0;
    if (brightness > 255) brightness = 255;
    analogWrite(LED_PIN, brightness);
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, 0);
  
  WiFi.begin(ssid, password);
  Serial.print("连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n连接成功");
  Serial.print("访问地址: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();
}

void loop() {
  server.handleClient();
}