#include <WiFi.h>
#include <WebServer.h>

// ===================== 引脚定义 =====================
// ESP32 DevKit 板载LED通常为GPIO2
#define LED_PIN 2
// 触摸引脚 T0 对应 GPIO4
#define TOUCH_PIN 4

// ===================== WiFi配置 =====================
const char* ssid = "limits";
const char* password = "LMZnice723";

// ===================== 系统状态变量 =====================
// 布防状态：true=已布防，false=已撤防
bool armState = false;
// 报警状态：true=正在报警，false=未报警
bool alarmState = false;
// 触摸阈值（需通过串口监视器校准）
int threshold = 20;
// 实时触摸值
int touchValue;

// ===================== Web服务器初始化 =====================
WebServer server(80);

// ===================== 生成网页HTML =====================
String makePage() {
  // 显示当前系统状态
  String stateText = armState ? "已布防" : "已撤防";
  String alarmText = alarmState ? "🔴 报警中！" : "🟢 正常";
  
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 安防报警器</title>
  <style>
    body {font-family: Arial; text-align: center; margin-top: 80px; background-color: #f0f0f0;}
    h1 {color: #333;}
    .status {font-size: 24px; margin: 30px 0; padding: 20px; background: white; border-radius: 10px; display: inline-block;}
    button {font-size: 20px; padding: 15px 30px; margin: 10px; border: none; border-radius: 8px; cursor: pointer;}
    #armBtn {background-color: #ff4444; color: white;}
    #disarmBtn {background-color: #00C851; color: white;}
  </style>
</head>
<body>
  <h1>ESP32 物联网安防报警器</h1>
  <div class="status">
    <p>系统状态：<b>)rawliteral" + stateText + R"rawliteral(</b></p>
    <p>报警状态：<b>)rawliteral" + alarmText + R"rawliteral(</b></p>
  </div>
  <a href="/arm"><button id="armBtn">布防(Arm)</button></a>
  <a href="/disarm"><button id="disarmBtn">撤防(Disarm)</button></a>
</body>
</html>
)rawliteral";
  return html;
}

// ===================== 网页路由处理 =====================
// 主页路由
void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

// 布防路由
void handleArm() {
  armState = true;
  // 布防时不自动触发报警，仅开启检测
  Serial.println("系统已布防！");
  server.sendHeader("Location", "/");
  server.send(303);
}

// 撤防路由
void handleDisarm() {
  armState = false;
  alarmState = false; // 撤防时重置报警状态，停止闪烁
  digitalWrite(LED_PIN, LOW); // 确保LED熄灭
  Serial.println("系统已撤防，报警已重置！");
  server.sendHeader("Location", "/");
  server.send(303);
}

// =====================  setup() 初始化 =====================
void setup() {
  // 初始化串口，波特率115200
  Serial.begin(115200);
  delay(1000); // 等待串口稳定

  // 初始化LED引脚为输出模式，初始熄灭
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // ===================== 连接WiFi =====================
WiFi.begin(ssid, password);
Serial.print("正在连接WiFi: ");
Serial.println(ssid);

// 增加超时机制，避免无限等待
unsigned long startAttemptTime = millis();
while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) { // 15秒超时
  delay(500);
  Serial.print(".");
}

if (WiFi.status() == WL_CONNECTED) {
  Serial.println("\n✅ WiFi连接成功！");
  Serial.print("📶 信号强度: ");
  Serial.println(WiFi.RSSI());
  Serial.print("🌐 访问地址: http://");
  Serial.println(WiFi.localIP());
} else {
  Serial.println("\n❌ WiFi连接超时！请检查热点/密码");
  // 连接失败后重启ESP32，自动重试
  ESP.restart();
}

  // ===================== 绑定Web路由 =====================
  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);

  // 启动Web服务器
  server.begin();
  Serial.println("Web服务器已启动");
}

// =====================  loop() 主循环 =====================
void loop() {
  // 处理Web客户端请求（必须保留，否则网页无响应）
  server.handleClient();

  // ===================== 触摸检测逻辑 =====================
  // 仅在已布防且未报警时，检测触摸触发
  if (armState && !alarmState) {
    touchValue = touchRead(TOUCH_PIN);
    Serial.print("触摸值：");
    Serial.println(touchValue);

    // 触摸触发：值低于阈值时，触发报警
    if (touchValue < threshold) {
      alarmState = true;
      Serial.println("⚠️  触摸触发，报警启动！");
    }
  }

  // ===================== 报警闪烁逻辑 =====================
  if (alarmState) {
    // 高频闪烁：亮100ms，灭100ms，频率5Hz
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}