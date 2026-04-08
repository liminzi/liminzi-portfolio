// ex06 警车双闪灯效 - 丝滑同步反相呼吸（长周期版）
#define LED_A_PIN 2
#define LED_B_PIN 16

// 呼吸参数（核心控制周期和丝滑度）
int brightness = 0;
int fadeStep = 1;          // 渐变步长，越小越丝滑
const int stepDelay = 8;  // 每步延时，越大周期越长，呼吸越慢

void setup() {
  pinMode(LED_A_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);
  // 初始状态：A灭，B亮
  analogWrite(LED_A_PIN, 0);
  analogWrite(LED_B_PIN, 255);
}

void loop() {
  // 1. 更新亮度值（0→255→0循环）
  brightness += fadeStep;
  
  // 2. 到达边界反转方向
  if (brightness <= 0 || brightness >= 255) {
    fadeStep = -fadeStep;
  }
  
  // 3. 核心：同步写入两个灯的PWM值，完全反相
  analogWrite(LED_A_PIN, brightness);
  analogWrite(LED_B_PIN, 255 - brightness);
  
  // 4. 延时控制周期，保证丝滑过渡
  delay(stepDelay);
}