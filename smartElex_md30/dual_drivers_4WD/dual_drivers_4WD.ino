/*
 * SmartElex 30D – Dual Driver, 4 Motor Control
 * Arduino UNO – PWM Independent Mode
 */

// ---------- Motor pin definitions ----------
// Driver 1
const int M1_DIR = 8;
const int M1_PWM = 9;

const int M2_DIR = 11;
const int M2_PWM = 10;

// Driver 2
const int M3_DIR = 7;
const int M3_PWM = 6;

const int M4_DIR = 4;
const int M4_PWM = 5;

// -------------------------------------------

void setup() {
  pinMode(M1_DIR, OUTPUT);
  pinMode(M1_PWM, OUTPUT);
  pinMode(M2_DIR, OUTPUT);
  pinMode(M2_PWM, OUTPUT);

  pinMode(M3_DIR, OUTPUT);
  pinMode(M3_PWM, OUTPUT);
  pinMode(M4_DIR, OUTPUT);
  pinMode(M4_PWM, OUTPUT);

  Serial.begin(9600);
  Serial.println("SmartElex 30D – 4 Motor Control Initialized");
}

void loop() {

  Serial.println("All motors FORWARD @ 50%");
  setMotor(M1_DIR, M1_PWM, 127, true);
  setMotor(M2_DIR, M2_PWM, 127, true);
  setMotor(M3_DIR, M3_PWM, 127, true);
  setMotor(M4_DIR, M4_PWM, 127, true);
  delay(3000);

  Serial.println("Stopping all motors");
  stopAllMotors();
  delay(1000);

  Serial.println("All motors REVERSE @ 50%");
  setMotor(M1_DIR, M1_PWM, 127, false);
  setMotor(M2_DIR, M2_PWM, 127, false);
  setMotor(M3_DIR, M3_PWM, 127, false);
  setMotor(M4_DIR, M4_PWM, 127, false);
  delay(3000);

  Serial.println("Motors crossed directions");
  setMotor(M1_DIR, M1_PWM, 255, true);
  setMotor(M2_DIR, M2_PWM, 255, false);
  setMotor(M3_DIR, M3_PWM, 255, true);
  setMotor(M4_DIR, M4_PWM, 255, false);
  delay(3000);

  stopAllMotors();
  delay(2000);
}

// ---------- Helper functions ----------

void setMotor(int dirPin, int pwmPin, int speed, bool forward) {
  speed = constrain(speed, 0, 255);

  digitalWrite(dirPin, forward ? HIGH : LOW);
  analogWrite(pwmPin, speed);
}

void stopAllMotors() {
  analogWrite(M1_PWM, 0);
  analogWrite(M2_PWM, 0);
  analogWrite(M3_PWM, 0);
  analogWrite(M4_PWM, 0);
}
