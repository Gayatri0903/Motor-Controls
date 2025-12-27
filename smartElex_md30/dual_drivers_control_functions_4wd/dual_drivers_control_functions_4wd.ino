/*
 * SmartElex 30D – 4 Motor Motion Control
 * Arduino UNO + Serial Commands
 * PWM Independent Mode
 */

// ---------- Pin definitions ----------
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
// ------------------------------------

int currentSpeed = 0;   // 0–255

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
  Serial.println("SmartElex 30D – 4 Motor Serial Control Ready");
  Serial.println("Commands:");
  Serial.println("F <speed>  Forward");
  Serial.println("B <speed>  Reverse");
  Serial.println("L <speed>  Left");
  Serial.println("R <speed>  Right");
  Serial.println("S          Stop");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd.length() == 0) return;

    char motion = cmd.charAt(0);
    int speed = currentSpeed;

    if (cmd.length() > 1) {
      speed = cmd.substring(1).toInt();
      speed = constrain(speed, 0, 255);
    }

    switch (motion) {
      case 'F':
        moveForward(speed);
        break;
      case 'B':
        moveReverse(speed);
        break;
      case 'L':
        turnLeft(speed);
        break;
      case 'R':
        turnRight(speed);
        break;
      case 'S':
        stopAll();
        break;
      default:
        Serial.println("Unknown command");
        return;
    }

    currentSpeed = speed;
    Serial.print("CMD: ");
    Serial.print(motion);
    Serial.print(" SPEED: ");
    Serial.println(currentSpeed);
  }
}

// ---------- Motion primitives ----------

void moveForward(int speed) {
  setMotor(M1_DIR, M1_PWM, speed, true);
  setMotor(M2_DIR, M2_PWM, speed, true);
  setMotor(M3_DIR, M3_PWM, speed, true);
  setMotor(M4_DIR, M4_PWM, speed, true);
}

void moveReverse(int speed) {
  setMotor(M1_DIR, M1_PWM, speed, false);
  setMotor(M2_DIR, M2_PWM, speed, false);
  setMotor(M3_DIR, M3_PWM, speed, false);
  setMotor(M4_DIR, M4_PWM, speed, false);
}

void turnLeft(int speed) {
  setMotor(M1_DIR, M1_PWM, speed, false);
  setMotor(M2_DIR, M2_PWM, speed, true);
  setMotor(M3_DIR, M3_PWM, speed, false);
  setMotor(M4_DIR, M4_PWM, speed, true);
}

void turnRight(int speed) {
  setMotor(M1_DIR, M1_PWM, speed, true);
  setMotor(M2_DIR, M2_PWM, speed, false);
  setMotor(M3_DIR, M3_PWM, speed, true);
  setMotor(M4_DIR, M4_PWM, speed, false);
}

void stopAll() {
  analogWrite(M1_PWM, 0);
  analogWrite(M2_PWM, 0);
  analogWrite(M3_PWM, 0);
  analogWrite(M4_PWM, 0);
}

// ---------- Low-level motor control ----------

void setMotor(int dirPin, int pwmPin, int speed, bool forward) {
  digitalWrite(dirPin, forward ? HIGH : LOW);
  analogWrite(pwmPin, speed);
}
