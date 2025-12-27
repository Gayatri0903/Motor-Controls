//#include <RH_ASK.h>
//#include <SPI.h>
//
//// RH_ASK(speed, rxPin, txPin)
//RH_ASK rf(2000, 11, 12);   // RX = D11, TX unused
//
//// ================= MOTOR PINS =================
//const int M1_DIR = 8;
//const int M1_PWM = 9;
//
//const int M2_DIR = 13;   // CHANGED
//const int M2_PWM = 10;
//
//const int M3_DIR = 7;
//const int M3_PWM = 6;
//
//const int M4_DIR = 4;
//const int M4_PWM = 5;
//// =============================================
//
//const int DEADZONE = 15;
//const unsigned long FAILSAFE_TIMEOUT = 300;
//
//uint8_t buf[5];
//uint8_t buflen = sizeof(buf);
//unsigned long lastRxTime = 0;
//
//void setup() {
//  pinMode(M1_DIR, OUTPUT); pinMode(M1_PWM, OUTPUT);
//  pinMode(M2_DIR, OUTPUT); pinMode(M2_PWM, OUTPUT);
//  pinMode(M3_DIR, OUTPUT); pinMode(M3_PWM, OUTPUT);
//  pinMode(M4_DIR, OUTPUT); pinMode(M4_PWM, OUTPUT);
//
//  rf.init();
//  stopAll();
//}
//
//void loop() {
//  if (rf.recv(buf, &buflen)) {
//    if (buflen == 5 && buf[0] == 0xAA) {
//      uint8_t cs = buf[0] ^ buf[1] ^ buf[2] ^ buf[3];
//      if (cs == buf[4]) {
//        lastRxTime = millis();
//        handleJoystick(buf[1], buf[2], buf[3]);
//      }
//    }
//  }
//
//  if (millis() - lastRxTime > FAILSAFE_TIMEOUT) {
//    stopAll();
//  }
//}
//
//void handleJoystick(uint8_t xRaw, uint8_t yRaw, uint8_t flags) {
//  if (flags & 0x01) {
//    stopAll();
//    return;
//  }
//
//  int x = xRaw - 128;
//  int y = yRaw - 128;
//
//  if (abs(x) < DEADZONE) x = 0;
//  if (abs(y) < DEADZONE) y = 0;
//
//  if (x == 0 && y == 0) {
//    stopAll();
//    return;
//  }
//
//  int left  = y + x;
//  int right = y - x;
//
//  left  = constrain(left,  -128, 128);
//  right = constrain(right, -128, 128);
//
//  int leftSpeed  = map(abs(left),  0, 128, 0, 255);
//  int rightSpeed = map(abs(right), 0, 128, 0, 255);
//
//  bool leftDir  = left  >= 0;
//  bool rightDir = right >= 0;
//
//  setMotor(M1_DIR, M1_PWM, leftSpeed,  leftDir);
//  setMotor(M3_DIR, M3_PWM, leftSpeed,  leftDir);
//
//  setMotor(M2_DIR, M2_PWM, rightSpeed, rightDir);
//  setMotor(M4_DIR, M4_PWM, rightSpeed, rightDir);
//}
//
//void setMotor(int dirPin, int pwmPin, int speed, bool forward) {
//  digitalWrite(dirPin, forward ? HIGH : LOW);
//  analogWrite(pwmPin, speed);
//}
//
//void stopAll() {
//  digitalWrite(M1_PWM, LOW);
//  digitalWrite(M2_PWM, LOW);
//  digitalWrite(M3_PWM, LOW);
//  digitalWrite(M4_PWM, LOW);
//
//  digitalWrite(M1_DIR, LOW);
//  digitalWrite(M2_DIR, LOW);
//  digitalWrite(M3_DIR, LOW);
//  digitalWrite(M4_DIR, LOW);
//}


#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile

RH_ASK driver;

void setup()
{
    Serial.begin(9600);  // Debugging only
    if (!driver.init())
         Serial.println("init failed");
}

void loop()
{
    uint8_t buf[12];
    uint8_t buflen = sizeof(buf);
    if (driver.recv(buf, &buflen)) // Non-blocking
    {
      int i;
      // Message with a good checksum received, dump it.
      Serial.print("Message: ");
      Serial.println((char*)buf);         
    }
}
