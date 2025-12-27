/*
 * SmartElex 30D Motor Driver Arduino Example (PWM Independent Mode)
 * Controls two DC motors with variable speed and direction.
 */

// Define the pins connected to the SmartElex 30D
const int motor1DirPin = 8;
const int motor1PWMPin = 9;  // Must be a PWM pin (~)
const int motor2DirPin = 11;
const int motor2PWMPin = 10; // Must be a PWM pin (~)

void setup() {
  // Initialize the motor control pins as outputs
  pinMode(motor1DirPin, OUTPUT);
  pinMode(motor1PWMPin, OUTPUT);
  pinMode(motor2DirPin, OUTPUT);
  pinMode(motor2PWMPin, OUTPUT);

  // Optional: Start Serial communication for debugging
  Serial.begin(9600);
  Serial.println("SmartElex 30D Initialized in PWM Independent Mode.");
}

void loop() {
  // Example 1: Motor 1 Forward, Motor 2 Backward at half speed for 3 seconds
  Serial.println("Motor 1 Forward, Motor 2 Backward (Half Speed)");
  setMotorSpeedDirection(1, 127, true); // Motor 1 speed 0-255, true for forward
  setMotorSpeedDirection(2, 127, false); // Motor 2 speed 0-255, false for backward
  delay(3000);

  // Example 2: Stop both motors for 1 second
  Serial.println("Stopping motors...");
  stopMotors();
  delay(1000);

  // Example 3: Motor 1 Backward, Motor 2 Forward at full speed for 3 seconds
  Serial.println("Motor 1 Backward, Motor 2 Forward (Full Speed)");
  setMotorSpeedDirection(1, 127, false);
  setMotorSpeedDirection(2, 127, true);
  delay(3000);

  // Stop again before repeating the loop
  stopMotors();
  delay(2000);
}

/**
 * Helper function to set the speed and direction of a specific motor.
 * 
 * @param motorChannel The motor number (1 or 2).
 * @param speed The PWM speed value (0-255).
 * @param forwardDirection True for forward (HIGH on Direction pin), False for backward (LOW).
 */
void setMotorSpeedDirection(int motorChannel, int speed, bool forwardDirection) {
  // Constrain speed to valid 0-255 range
  speed = constrain(speed, 0, 255);

  if (motorChannel == 1) {
    digitalWrite(motor1DirPin, forwardDirection ? HIGH : LOW);
    analogWrite(motor1PWMPin, speed);
  } else if (motorChannel == 2) {
    digitalWrite(motor2DirPin, forwardDirection ? HIGH : LOW);
    analogWrite(motor2PWMPin, speed);
  }
}

/**
 * Helper function to immediately stop both motors by writing 0 speed.
 */
void stopMotors() {
  analogWrite(motor1PWMPin, 0);
  analogWrite(motor2PWMPin, 0);
}
