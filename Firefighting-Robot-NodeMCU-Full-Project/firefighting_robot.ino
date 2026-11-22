/*
  Firefighting Robot using NodeMCU ESP8266
  Hardware: L298N, 3 digital flame sensors via MCP23017,
  servo via PCA9685, 12V pump via 3.3V-compatible MOSFET driver.

  REQUIRED ADD-ON MODULES:
    MCP23017 I2C GPIO expander at address 0x20
    PCA9685 I2C PWM/servo driver at address 0x40

  Check actual wiring and module input polarity before powering.
*/

#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_MCP23X17 mcp;
Adafruit_PWMServoDriver pwm(0x40);

// NodeMCU I2C pins
const uint8_t I2C_SDA = D2;  // GPIO4
const uint8_t I2C_SCL = D1;  // GPIO5

// L298N direction inputs
const uint8_t IN1 = D5;      // GPIO14
const uint8_t IN2 = D6;      // GPIO12
const uint8_t IN3 = D7;      // GPIO13
const uint8_t IN4 = D0;      // GPIO16

// Active-HIGH logic-level MOSFET driver input.
// D8/GPIO15 is a boot-strap pin and must remain LOW during reset.
// This arrangement is suitable only if the driver input does not pull D8 HIGH.
const uint8_t PUMP_PIN = D8;
const uint8_t PUMP_ON_LEVEL = HIGH;
const uint8_t PUMP_OFF_LEVEL = LOW;

// MCP23017 GPA0, GPA1, GPA2
const uint8_t FLAME_LEFT = 0;
const uint8_t FLAME_CENTER = 1;
const uint8_t FLAME_RIGHT = 2;

// Verify your sensor output. Many modules are active LOW.
const uint8_t FIRE_LEVEL = LOW;

// PCA9685 channel and nozzle angles
const uint8_t SERVO_CHANNEL = 0;
const int ANGLE_LEFT = 45;
const int ANGLE_CENTER = 90;
const int ANGLE_RIGHT = 135;

// Approximate servo pulse range; calibrate for your servo.
const int SERVO_MIN_TICK = 102;  // approx 500 us at 50 Hz
const int SERVO_MAX_TICK = 512;  // approx 2500 us at 50 Hz

const unsigned long PUMP_BURST_MS = 2500;
const unsigned long SERVO_SETTLE_MS = 500;

bool pumpRunning = false;
bool fireHandled = false;
unsigned long pumpStartedAt = 0;

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void moveForward() {
  // If one side moves backward, reverse that side's motor polarity
  // or swap the corresponding IN1/IN2 or IN3/IN4 logic.
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void pumpOff() {
  digitalWrite(PUMP_PIN, PUMP_OFF_LEVEL);
}

void pumpOn() {
  digitalWrite(PUMP_PIN, PUMP_ON_LEVEL);
}

void setNozzleAngle(int angle) {
  angle = constrain(angle, 0, 180);
  int tick = map(angle, 0, 180, SERVO_MIN_TICK, SERVO_MAX_TICK);
  pwm.setPWM(SERVO_CHANNEL, 0, tick);
}

bool readFire(uint8_t expanderPin) {
  return mcp.digitalRead(expanderPin) == FIRE_LEVEL;
}

void startExtinguishing(int angle, const char *label) {
  stopMotors();
  setNozzleAngle(angle);
  Serial.print(F("Fire detected: "));
  Serial.println(label);
  delay(SERVO_SETTLE_MS);

  pumpOn();
  pumpRunning = true;
  pumpStartedAt = millis();
  Serial.println(F("Pump ON"));
}

void setup() {
  Serial.begin(115200);

  // Set safe output states before other initialization.
  pinMode(PUMP_PIN, OUTPUT);
  pumpOff();

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  stopMotors();

  Wire.begin(I2C_SDA, I2C_SCL);

  if (!mcp.begin_I2C(0x20)) {
    Serial.println(F("ERROR: MCP23017 not found at 0x20. Outputs remain OFF."));
    while (true) {
      pumpOff();
      stopMotors();
      delay(1000);
    }
  }

  mcp.pinMode(FLAME_LEFT, INPUT_PULLUP);
  mcp.pinMode(FLAME_CENTER, INPUT_PULLUP);
  mcp.pinMode(FLAME_RIGHT, INPUT_PULLUP);

  if (!pwm.begin()) {
    Serial.println(F("ERROR: PCA9685 not found at 0x40. Outputs remain OFF."));
    while (true) {
      pumpOff();
      stopMotors();
      delay(1000);
    }
  }

  pwm.setPWMFreq(50);
  setNozzleAngle(ANGLE_CENTER);

  Serial.println();
  Serial.println(F("Firefighting Robot ready."));
  Serial.println(F("Test with pump disconnected and wheels lifted first."));
}

void loop() {
  const bool leftFire = readFire(FLAME_LEFT);
  const bool centerFire = readFire(FLAME_CENTER);
  const bool rightFire = readFire(FLAME_RIGHT);
  const bool anyFire = leftFire || centerFire || rightFire;

  Serial.print(F("L=")); Serial.print(leftFire);
  Serial.print(F(" C=")); Serial.print(centerFire);
  Serial.print(F(" R=")); Serial.println(rightFire);

  if (!anyFire) {
    fireHandled = false;
  }

  if (anyFire && !pumpRunning && !fireHandled) {
    if (centerFire) {
      startExtinguishing(ANGLE_CENTER, "CENTER");
    } else if (leftFire) {
      startExtinguishing(ANGLE_LEFT, "LEFT");
    } else {
      startExtinguishing(ANGLE_RIGHT, "RIGHT");
    }
    fireHandled = true;
  }

  if (pumpRunning && millis() - pumpStartedAt >= PUMP_BURST_MS) {
    pumpOff();
    pumpRunning = false;
    setNozzleAngle(ANGLE_CENTER);
    Serial.println(F("Pump OFF"));
  }

  if (!anyFire && !pumpRunning) {
    pumpOff();
    moveForward();
  } else {
    stopMotors();
  }

  delay(50);
}
