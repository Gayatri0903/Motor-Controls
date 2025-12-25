/*************************************************
 * ESP32-S2 | 3 Stepper Motors (DM542) + 5 Relays
 * Full UART Command Control
 * Supports simultaneous motor movement
 *************************************************/

#include <Arduino.h>

/************** PIN CONFIG ****************/
// Steppers
#define X_PULSE   16
#define X_DIR     17
#define Y1_PULSE  13
#define Y1_DIR    14
#define Y2_PULSE  11
#define Y2_DIR    10

// Relays
#define RELAY1    1
#define RELAY2    2
#define RELAY3    3
#define RELAY4    4
#define RELAY5    5

/************** MOTOR CONFIG ****************/
#define PULSES_PER_REV 3200   // adjust according to your DM542 DIP switch

/************** GLOBAL FLAGS ****************/
volatile bool stopAll = false;

/************** SERIAL BUFFER ***************/
String serialBuffer = "";

/************** MOTOR STRUCT ***************/
struct Motor {
  int pulsePin;
  int dirPin;
  long remainingSteps;
  long position;
  unsigned long stepInterval;
  unsigned long lastStepMicros;
  bool direction;
  bool enabled;
};

Motor motorX = {X_PULSE, X_DIR, 0, 0, 0, 0, true, true};
Motor motorY1 = {Y1_PULSE, Y1_DIR, 0, 0, 0, 0, true, true};
Motor motorY2 = {Y2_PULSE, Y2_DIR, 0, 0, 0, 0, true, true};

/************** GPIO INIT *******************/
void initGPIO() {
  int pins[] = { X_PULSE,X_DIR,Y1_PULSE,Y1_DIR,Y2_PULSE,Y2_DIR,
                 RELAY1,RELAY2,RELAY3,RELAY4,RELAY5 };
  for (int i = 0; i < sizeof(pins)/sizeof(pins[0]); i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
  }
}

/************** RELAY CONTROL ***************/
void relayControl(int relay, bool state) {
  digitalWrite(relay, state ? HIGH : LOW);
  Serial.print("Relay ");
  Serial.print(relay);
  Serial.print(" ");
  Serial.println(state ? "ON" : "OFF");
}

void relayAll(bool state) {
  relayControl(RELAY1, state);
  relayControl(RELAY2, state);
  relayControl(RELAY3, state);
  relayControl(RELAY4, state);
  relayControl(RELAY5, state);
  Serial.println(state ? "ALL RELAYS TURNED ON" : "ALL RELAYS TURNED OFF");
}

/************** START MOTOR ***************/
void startMotor(Motor &m, float rotations, int speed_us, bool direction) {
  m.remainingSteps = (long)(rotations * PULSES_PER_REV);
  m.direction = direction;
  m.stepInterval = speed_us * 2UL; // high+low pulse
  m.lastStepMicros = micros();
  m.enabled = true;
  digitalWrite(m.dirPin, direction ? HIGH : LOW);
}

/************** UPDATE MOTOR ***************/
void updateMotor(Motor &m) {
  if (!m.enabled || m.remainingSteps <= 0) return;
  if (stopAll) { m.remainingSteps = 0; return; }
  unsigned long now = micros();
  if (now - m.lastStepMicros >= m.stepInterval) {
    m.lastStepMicros = now;
    digitalWrite(m.pulsePin, HIGH);
    delayMicroseconds(m.stepInterval / 2);
    digitalWrite(m.pulsePin, LOW);
    delayMicroseconds(m.stepInterval / 2);
    m.remainingSteps--;
    m.position += (m.direction ? 1 : -1);
  }
}

/************** COMMAND PARSER ***************/
void processCommand(String cmd) {
  cmd.trim();
  if (cmd.length() == 0) return;

  Serial.print("CMD: ");
  Serial.println(cmd);

  // STOP ALL
  if (cmd.equalsIgnoreCase("STOP")) { stopAll = true; Serial.println("ALL MOTORS STOPPED"); return; }

  // ZERO
  if (cmd.equalsIgnoreCase("ZERO")) {
    stopAll = false;
    motorX.position = motorY1.position = motorY2.position = 0;
    Serial.println("ZERO POSITION SET"); return;
  }

  // MOTOR ENABLE/DISABLE
  if (cmd.equalsIgnoreCase("MX ON"))  { motorX.enabled=true;  Serial.println("MX ENABLED");  return; }
  if (cmd.equalsIgnoreCase("MX OFF")) { motorX.enabled=false; Serial.println("MX DISABLED"); return; }
  if (cmd.equalsIgnoreCase("MY1 ON")) { motorY1.enabled=true;  Serial.println("MY1 ENABLED"); return; }
  if (cmd.equalsIgnoreCase("MY1 OFF")){ motorY1.enabled=false; Serial.println("MY1 DISABLED"); return; }
  if (cmd.equalsIgnoreCase("MY2 ON")) { motorY2.enabled=true;  Serial.println("MY2 ENABLED"); return; }
  if (cmd.equalsIgnoreCase("MY2 OFF")){ motorY2.enabled=false; Serial.println("MY2 DISABLED"); return; }

  // RELAY CONTROL (supports 3-word command: ALL RELAYS ON/OFF)
  char word1[16], word2[16], state[4];
  if (sscanf(cmd.c_str(), "%15s %15s %3s", word1, word2, state) == 3) {
    if (strcasecmp(word1,"ALL")==0 && strcasecmp(word2,"RELAYS")==0) {
      if (strcasecmp(state,"ON")==0) { relayAll(true); return; }
      if (strcasecmp(state,"OFF")==0){ relayAll(false); return; }
    }
  }

  // Individual relay control (2-word: R1 ON/OFF)
  char relay[16], relayState[4];
  if (sscanf(cmd.c_str(), "%15s %3s", relay, relayState) == 2) {
    int rPin = -1;
    if (strcasecmp(relay,"R1")==0) rPin=RELAY1;
    if (strcasecmp(relay,"R2")==0) rPin=RELAY2;
    if (strcasecmp(relay,"R3")==0) rPin=RELAY3;
    if (strcasecmp(relay,"R4")==0) rPin=RELAY4;
    if (strcasecmp(relay,"R5")==0) rPin=RELAY5;
    if (rPin!=-1){
      if (strcasecmp(relayState,"ON")==0) relayControl(rPin,true);
      else if (strcasecmp(relayState,"OFF")==0) relayControl(rPin,false);
      else Serial.println("INVALID RELAY STATE");
      return;
    }
  }

  // SIMULTANEOUS MOTOR MOVE FORMAT: X,Y1,Y2 CW 1.0 500
  char motors[16], dir[4];
  float rotations;
  int speed;
  if (sscanf(cmd.c_str(), "%15s %3s %f %d", motors, dir, &rotations, &speed) == 4) {
    stopAll=false;
    bool direction=(strcasecmp(dir,"CW")==0);
    if (strstr(motors,"X"))  startMotor(motorX, rotations, speed, direction);
    if (strstr(motors,"Y1")) startMotor(motorY1, rotations, speed, direction);
    if (strstr(motors,"Y2")) startMotor(motorY2, rotations, speed, direction);
    Serial.println("MOTORS STARTED SIMULTANEOUSLY");
    return;
  }

  Serial.println("INVALID COMMAND");
}

/************** SETUP ***********************/
void setup() {
  Serial.begin(115200);
  delay(1000);
  initGPIO();
  Serial.println("ESP32-S2 Stepper Controller Ready");
  Serial.print("PULSES PER REV = "); Serial.println(PULSES_PER_REV);
  Serial.println("Commands:");
  Serial.println("Single Motor: X CW 1.0 500");
  Serial.println("Simultaneous: X,Y1,Y2 CW 1.0 500");
  Serial.println("STOP | ZERO | MX/MY1/MY2 ON/OFF | R1-R5 ON/OFF | ALL RELAYS ON/OFF");
}

/************** LOOP *************************/
void loop() {
  // Read serial input
  while (Serial.available()) {
    char c = Serial.read();
    if (c=='\n' || c=='\r') { processCommand(serialBuffer); serialBuffer=""; }
    else { serialBuffer += c; }
  }

  // Update all motors simultaneously
  updateMotor(motorX);
  updateMotor(motorY1);
  updateMotor(motorY2);
}
