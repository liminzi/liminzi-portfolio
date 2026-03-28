// ===================== 实验3：呼吸灯 PWM 定义 =====================
const int ledPin = 2;
const int freq = 5000;
const int resolution = 8;

// ===================== 实验4：触摸引脚定义 =====================
#define TOUCH_PIN 4
int threshold = 20;
int touchValue;

// ===================== 3档位速度 =====================
int speedLevel = 1;
int delayTime = 6;

// ===================== 触摸防抖 =====================
bool lastTouch = false;
unsigned long lastDebounce = 0;
const long debounceDelay = 80;

// ===================== 呼吸灯平滑变量 =====================
int brightness = 0;
int fadeAmount = 5;

void setup() {
  Serial.begin(115200);
  delay(1000);
  ledcAttach(ledPin, freq, resolution);
}

void loop() {
  // ========================== 触摸检测 ==========================
  touchValue = touchRead(TOUCH_PIN);
  unsigned long now = millis();
  bool currentTouch = (touchValue < threshold);

  if (currentTouch != lastTouch) {
    lastDebounce = now;
  }

  if (now - lastDebounce > debounceDelay) {
    if (currentTouch == true && lastTouch == false) {
      speedLevel++;
      if (speedLevel > 3) speedLevel = 1;

      // ====================== 三档差距拉得非常大 ======================
      if (speedLevel == 1) delayTime = 10;   // 慢档（流畅呼吸）
      if (speedLevel == 2) delayTime = 5;   // 中档（明显更快）
      if (speedLevel == 3) delayTime = 1;   // 快档（极快频闪）

      Serial.print("当前档位：");
      Serial.println(speedLevel);
    }
    lastTouch = currentTouch;
  }

  // ========================== 呼吸灯（不停） ==========================
  ledcWrite(ledPin, brightness);
  brightness += fadeAmount;

  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
  }

  delay(delayTime);
}