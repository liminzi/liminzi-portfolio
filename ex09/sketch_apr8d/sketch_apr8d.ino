#include <WiFi.h>
#include <WebServer.h>

// ===================== 引脚定义 =====================
#define TOUCH_PIN 4  // 触摸引脚 T0 对应 GPIO4

// ===================== WiFi配置（已填好你的信息）=====================
const char* ssid = "limits";
const char* password = "LMZnice723";

// ===================== 触摸阈值配置（适配你的开发板）=====================
const int TOUCH_THRESHOLD = 20;  // 你的开发板阈值约20
const int TOUCH_MAX = 80;        // 无触摸时的基准值（适配ESP32）

// ===================== Web服务器初始化 =====================
WebServer server(80);

// ===================== 生成网页HTML（带AJAX实时刷新）=====================
String makePage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 实时触摸传感器仪表盘</title>
  <style>
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
      font-family: 'Arial', sans-serif;
    }
    body {
      background-color: #f0f2f5;
      display: flex;
      justify-content: center;
      align-items: center;
      min-height: 100vh;
      padding: 20px;
    }
    .dashboard {
      background: white;
      border-radius: 16px;
      box-shadow: 0 4px 20px rgba(0,0,0,0.1);
      padding: 40px;
      text-align: center;
      width: 100%;
      max-width: 500px;
    }
    h1 {
      color: #1a1a1a;
      margin-bottom: 30px;
      font-size: 24px;
    }
    .value-display {
      font-size: 80px;
      font-weight: bold;
      color: #2196F3;
      margin: 30px 0;
      font-family: 'Courier New', monospace;
    }
    .status {
      font-size: 18px;
      color: #666;
      margin-top: 20px;
    }
    .progress-bar {
      width: 100%;
      height: 20px;
      background: #e0e0e0;
      border-radius: 10px;
      overflow: hidden;
      margin: 20px 0;
    }
    .progress-fill {
      height: 100%;
      background: linear-gradient(90deg, #4CAF50, #FFC107, #F44336);
      width: 0%;
      transition: width 0.1s ease;
    }
  </style>
</head>
<body>
  <div class="dashboard">
    <h1>ESP32 实时触摸传感器监控</h1>
    <div class="progress-bar">
      <div class="progress-fill" id="progress"></div>
    </div>
    <div class="value-display" id="touchValue">--</div>
    <div class="status" id="status">等待数据...</div>
  </div>

  <script>
    // AJAX 实时拉取数据
    function updateData() {
      fetch('/data')
        .then(response => response.json())
        .then(data => {
          const value = data.touch;
          const valueElement = document.getElementById('touchValue');
          const progressElement = document.getElementById('progress');
          const statusElement = document.getElementById('status');
          
          // 更新数值显示
          valueElement.textContent = value;
          
          // 计算进度条百分比（无触摸80 → 触摸20，对应0%→100%）
          const maxVal = 80;
          const minVal = 20;
          let percent = ((maxVal - value) / (maxVal - minVal)) * 100;
          percent = Math.max(0, Math.min(100, percent));
          progressElement.style.width = percent + '%';
          
          // 更新状态提示
          if (value < 25) {
            statusElement.textContent = '⚠️ 触摸触发（阈值20）';
            statusElement.style.color = '#F44336';
          } else if (value < 40) {
            statusElement.textContent = '🖐️ 手指靠近';
            statusElement.style.color = '#FFC107';
          } else {
            statusElement.textContent = '✅ 正常（无触摸）';
            statusElement.style.color = '#4CAF50';
          }
        })
        .catch(error => {
          console.error('数据拉取失败:', error);
          document.getElementById('status').textContent = '❌ 连接断开';
          document.getElementById('status').style.color = '#F44336';
        });
    }

    // 每100ms刷新一次，实现实时效果
    setInterval(updateData, 100);
    // 页面加载时立即刷新一次
    updateData();
  </script>
</body>
</html>
)rawliteral";
  return html;
}

// ===================== 数据接口：返回触摸传感器JSON数据 =====================
void handleData() {
  int touchValue = touchRead(TOUCH_PIN);
  // 限制数值范围，避免异常
  touchValue = constrain(touchValue, 0, 100);
  
  String json = "{\"touch\":" + String(touchValue) + "}";
  server.send(200, "application/json", json);
}

// ===================== 主页路由：返回仪表盘页面 =====================
void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

// ===================== setup() 初始化 =====================
void setup() {
  Serial.begin(115200);
  delay(1000); // 等待串口稳定

  // ===================== 连接WiFi =====================
  WiFi.begin(ssid, password);
  Serial.print("正在连接WiFi: ");
  Serial.println(ssid);

  // 15秒超时，避免无限等待
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
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
    ESP.restart(); // 连接失败自动重启重试
  }

  // ===================== 绑定Web路由 =====================
  server.on("/", handleRoot);
  server.on("/data", handleData);

  // 启动Web服务器
  server.begin();
  Serial.println("✅ Web服务器已启动");
  Serial.println("📊 触摸阈值已适配: 20");
}

// ===================== loop() 主循环 =====================
void loop() {
  server.handleClient(); // 处理Web客户端请求，保证实时响应
}