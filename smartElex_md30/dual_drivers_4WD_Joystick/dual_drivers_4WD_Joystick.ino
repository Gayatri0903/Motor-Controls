/*
   SmartElex 30D – 4 Motor Control with PS2 Joystick
   Differential Drive – Corrected Y Polarity
   Arduino UNO

   Motor mapping:
   M1 = FR
   M2 = FL
   M3 = RL
   M4 = RR
*/

// ================= MOTOR PINS =================
// Driver 1
const int M4_DIR = 8;    // RR
const int M4_PWM = 9;
const int M3_DIR = 11;   // RL
const int M3_PWM = 10;

// Driver 2
const int M2_DIR = 7;    // FL
const int M2_PWM = 6;
const int M1_DIR = 4;    // FR
const int M1_PWM = 5;
// =============================================

// ================= JOYSTICK PINS =================
const int JOY_X = A0;   // RX  (-X = Forward, +X = Backward)
const int JOY_Y = A1;   // RY  (+Y = Left, -Y = Right)
const int JOY_SW = 2;
// ================================================

const int DEADZONE = 20;
//const int MAX_PWM = 50;
const int MAX_PWM_LINEAR  = 50;   // Forward / Backward
const int MAX_PWM_ROTATE  = 80;   // In-place Left / Right


// ---------------- MOTION STATES ----------------
enum Motion {
  STOP,
  FORWARD,
  BACKWARD,
  ROTATE_LEFT,
  ROTATE_RIGHT
};

void setup() {
  pinMode(M1_DIR, OUTPUT); pinMode(M1_PWM, OUTPUT);
  pinMode(M2_DIR, OUTPUT); pinMode(M2_PWM, OUTPUT);
  pinMode(M3_DIR, OUTPUT); pinMode(M3_PWM, OUTPUT);
  pinMode(M4_DIR, OUTPUT); pinMode(M4_PWM, OUTPUT);

  pinMode(JOY_SW, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println("=== JOYSTICK CONTROL (Y POLARITY FIXED) ===");

  stopAll();
}

void loop() {

  // ---------- Emergency Stop ----------
  if (digitalRead(JOY_SW) == LOW) {
    Serial.println("[EMERGENCY STOP]");
    stopAll();
    delay(100);
    return;
  }

  int x = analogRead(JOY_X) - 512;
  int y = analogRead(JOY_Y) - 512;

  if (abs(x) < DEADZONE) x = 0;
  if (abs(y) < DEADZONE) y = 0;

  // ---------- Motion Detection (FIXED) ----------
  Motion motion = STOP;

  if (x < 0 && abs(x) >= abs(y))
    motion = FORWARD;
  else if (x > 0 && abs(x) >= abs(y))
    motion = BACKWARD;
  else if (y > 0 && abs(y) > abs(x))   // +Y = LEFT
    motion = ROTATE_LEFT;
  else if (y < 0 && abs(y) > abs(x))   // -Y = RIGHT
    motion = ROTATE_RIGHT;

  if (motion == STOP) 
  {
    stopAll();
    Serial.println("[STOP]");
    delay(50);
    return;
  }

  // ---------- Speed Scaling (KEY CHANGE) ----------
  int speed;

  if (motion == FORWARD || motion == BACKWARD) 
  {
    speed = map(abs(x), 0, 512, 0, MAX_PWM_LINEAR);
  } 
  
  if (motion == ROTATE_LEFT || motion == ROTATE_RIGHT) 
  {
    speed = map(abs(y), 0, 512, 0, MAX_PWM_ROTATE);
  }

  // ---------- Direction Truth Table ----------
  bool fl, rl, fr, rr;

  switch (motion) {

    case FORWARD:
      fl = rl = fr = rr = true;
      break;

    case BACKWARD:
      fl = rl = fr = rr = false;
      break;

    case ROTATE_LEFT:
      fl = rl = false;   // Left side reverse
      fr = rr = true;    // Right side forward
      break;

    case ROTATE_RIGHT:
      fl = rl = true;    // Left side forward
      fr = rr = false;   // Right side reverse
      break;

    default:
      fl = rl = fr = rr = false;
      speed = 0;
  }

  // ---------- Apply Motors ----------
  setMotor(M2_DIR, M2_PWM, speed, fl); // FL
  setMotor(M3_DIR, M3_PWM, speed, rl); // RL

  setMotor(M1_DIR, M1_PWM, speed, fr); // FR
  setMotor(M4_DIR, M4_PWM, speed, rr); // RR

  // ---------- Debug ----------
  Serial.print("X: "); Serial.print(x);
  Serial.print(" Y: "); Serial.print(y);
  Serial.print(" | MODE: ");

  switch (motion) {
    case FORWARD:       Serial.print("FORWARD"); break;
    case BACKWARD:      Serial.print("BACKWARD"); break;
    case ROTATE_LEFT:   Serial.print("ROTATE_LEFT"); break;
    case ROTATE_RIGHT:  Serial.print("ROTATE_RIGHT"); break;
    default:            Serial.print("STOP");
  }

  Serial.print(" | FL:"); Serial.print(fl ? "F" : "R");
  Serial.print(" RL:");   Serial.print(rl ? "F" : "R");
  Serial.print(" FR:");   Serial.print(fr ? "F" : "R");
  Serial.print(" RR:");   Serial.println(rr ? "F" : "R");

  delay(20);
}

// ================= LOW LEVEL =================

void setMotor(int dirPin, int pwmPin, int speed, bool forward) {
  digitalWrite(dirPin, forward ? HIGH : LOW);
  analogWrite(pwmPin, speed);
}

void stopAll() {
  analogWrite(M1_PWM, 0);
  analogWrite(M2_PWM, 0);
  analogWrite(M3_PWM, 0);
  analogWrite(M4_PWM, 0);
}
