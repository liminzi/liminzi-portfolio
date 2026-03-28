// 定义两个LED引脚
const int ledPin1 = 2;
const int ledPin2 = 16;

// PWM设置（和你实验3完全一样）
const int freq = 5000;
const int resolution = 8;

void setup() {
  Serial.begin(115200);

  // 绑定两个独立PWM通道
  ledcAttach(ledPin1, freq, resolution);
  ledcAttach(ledPin2, freq, resolution);
}

void loop() {
  // 通道1 从暗 → 亮
  // 通道2 从亮 → 暗
  for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
    ledcWrite(ledPin1, dutyCycle);
    ledcWrite(ledPin2, 255 - dutyCycle);  // 反相
    delay(6);
  }

  // 通道1 从亮 → 暗
  // 通道2 从暗 → 亮
  for(int dutyCycle = 255; dutyCycle >= 0; dutyCycle--){
    ledcWrite(ledPin1, dutyCycle);
    ledcWrite(ledPin2, 255 - dutyCycle);  // 反相
    delay(12);
  }
}