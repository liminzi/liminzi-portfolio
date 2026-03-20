// 定义LED引脚，ESP32板载LED通常在GPIO 2
const int ledPin = 2;

// 定时相关变量
unsigned long previousMillis = 0;  // 记录上一次LED状态切换的时间
const long interval = 500;         // LED状态切换间隔（毫秒），500ms=0.5秒，对应1Hz闪烁
bool ledState = LOW;               // 记录当前LED状态，初始为熄灭

void setup() {
  // 初始化串口通信，波特率115200
  Serial.begin(115200);
  // 将LED引脚设置为输出模式
  pinMode(ledPin, OUTPUT);
  // 初始化LED状态（熄灭）
  digitalWrite(ledPin, ledState);
}

void loop() {
  // 获取当前系统运行时间（毫秒）
  unsigned long currentMillis = millis();

  // 检查是否达到状态切换间隔
  if (currentMillis - previousMillis >= interval) {
    // 更新上一次切换时间
    previousMillis = currentMillis;
    
    // 翻转LED状态（亮→灭 / 灭→亮）
    ledState = !ledState;
    // 应用新的LED状态
    digitalWrite(ledPin, ledState);
    
    // 串口输出当前状态
    if (ledState == HIGH) {
      Serial.println("LED ON");
    } else {
      Serial.println("LED OFF");
    }
  }

  // 这里可以添加其他需要并行执行的代码（不会被延时阻塞）
  // 例如：检测按键、读取传感器等
}