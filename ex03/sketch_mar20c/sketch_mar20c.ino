const int ledPin = 2;          // LED引脚
unsigned long t = 0;           // 时间戳变量（简化命名）
int step = 0;                  // 当前步骤
// SOS时序：0=短亮,1=短灭,2=长亮,3=长灭,4=长停顿
int sos[19] = {0,1,0,1,0,1,2,3,2,3,2,3,0,1,0,1,0,1,4}; 

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  unsigned long now = millis();
  // 仅当时间间隔到了才执行下一步
  if(now - t >= (sos[step]==0||sos[step]==2 ? (sos[step]==0?200:600) : (sos[step]==4?3000:200))){
    t = now; // 更新时间戳
    
    // 控制LED亮灭
    digitalWrite(ledPin, (sos[step]==0||sos[step]==2) ? HIGH : LOW);
    
    // 步骤自增，到末尾重置
    step = (step+1)%19;
    if(step==0) Serial.println("SOS循环完成");
  }
}