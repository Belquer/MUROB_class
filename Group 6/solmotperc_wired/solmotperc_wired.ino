/*
 * ESP32 Serial Control (Auto-Off for All Outputs)
 * ─────────────────────────────────────────────
 * DC Motor (12V)        → L298N   IN1/IN2   GPIO 18/19
 * Small Solenoid (5V)   → NPN     GPIO 22   (Buck to 7V)
 * JB-1250B (12V)        → XY-MOS  SIG       GPIO 26
 */

// ── Pin Definitions ───────────────────────────────────────────
const int MOTOR_IN1     = 18;
const int MOTOR_IN2     = 19;
const int SMALL_SOL_PIN = 22;
const int BIG_SOL_PIN   = 26;

// ── Global Timing Settings ────────────────────────────────────
const unsigned long PULSE_MS = 50; // Default duration for all actions

// ── State Tracking (Non-blocking) ─────────────────────────────
unsigned long motorStartTime    = 0;
unsigned long smallSolStartTime = 0;
unsigned long bigSolStartTime   = 0;

bool motorActive    = false;
bool smallSolActive = false;
bool bigSolActive   = false;

// ── Motor Functions ───────────────────────────────────────────
void motorForward() {
  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);
  motorStartTime = millis();
  motorActive = true;
  Serial.println("[MOTOR] Forward Pulse Start");
}

void motorBackward() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, HIGH);
  motorStartTime = millis();
  motorActive = true;
  Serial.println("[MOTOR] Backward Pulse Start");
}

void motorStop() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
  motorActive = false;
  Serial.println("[MOTOR] Stopped");
}

// ── Solenoid Functions ────────────────────────────────────────
void smallSolenoidOn() {
  digitalWrite(SMALL_SOL_PIN, HIGH);
  smallSolStartTime = millis();
  smallSolActive = true;
  Serial.println("[SMALL SOL] Pulse Start");
}

void smallSolenoidOff() {
  digitalWrite(SMALL_SOL_PIN, LOW);
  smallSolActive = false;
  Serial.println("[SMALL SOL] Pulse End");
}

void bigSolenoidOn() {
  digitalWrite(BIG_SOL_PIN, HIGH);
  bigSolStartTime = millis();
  bigSolActive = true;
  Serial.println("[BIG SOL] Pulse Start");
}

void bigSolenoidOff() {
  digitalWrite(BIG_SOL_PIN, LOW);
  bigSolActive = false;
  Serial.println("[BIG SOL] Pulse End");
}

// ── Setup ─────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);

  pinMode(MOTOR_IN1,     OUTPUT);
  pinMode(MOTOR_IN2,     OUTPUT);
  pinMode(SMALL_SOL_PIN, OUTPUT);
  pinMode(BIG_SOL_PIN,   OUTPUT);

  motorStop();
  digitalWrite(SMALL_SOL_PIN, LOW);
  digitalWrite(BIG_SOL_PIN,   LOW);

  Serial.println("[SYSTEM] Ready. All outputs timed to 50ms.");
}

// ── Loop ──────────────────────────────────────────────────────
void loop() {
  unsigned long currentMillis = millis();

  // 1. Listen for Commands from Max MSP
  if (Serial.available() > 0) {
    char command = Serial.read();

    switch (command) {
      case 'F': 
        if (!motorActive) motorForward(); 
        break;
      case 'B': 
        if (!motorActive) motorBackward(); 
        break;
      case 'S': 
        motorStop(); // Manual override stop
        break;
      
      case '1': 
        if (!smallSolActive) smallSolenoidOn(); 
        break;
        
      case 'X': 
        if (!bigSolActive) bigSolenoidOn(); 
        break;
    }
  }

  // 2. Safety Watchdog: Auto-off logic
  
  // Motor Auto-Stop
  if (motorActive && (currentMillis - motorStartTime >= PULSE_MS)) {
    motorStop();
  }

  // Small Solenoid Auto-Off
  if (smallSolActive && (currentMillis - smallSolStartTime >= PULSE_MS)) {
    smallSolenoidOff();
  }

  // Big Solenoid Auto-Off
  if (bigSolActive && (currentMillis - bigSolStartTime >= PULSE_MS)) {
    bigSolenoidOff();
  }
}