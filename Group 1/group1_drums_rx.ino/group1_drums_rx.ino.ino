#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// PCA9685 physical channels
#define SERVO_CHANNEL_0  0
#define SERVO_CHANNEL_1  1
#define SERVO_CHANNEL_2  2
#define SERVO_CHANNEL_3  3

// Motor positions — Adjust to match your physical setup
#define MOTOR0_POS_A  307
#define MOTOR0_POS_B  450
#define MOTOR1_POS_A  307
#define MOTOR1_POS_B  450
#define MOTOR2_POS_A  307
#define MOTOR2_POS_B  450
#define MOTOR3_POS_A  307
#define MOTOR3_POS_B  450

#define STEP_SIZE  5
#define STEP_DELAY 5 // Speed of the movement loop

int posA[4] = {MOTOR0_POS_A, MOTOR1_POS_A, MOTOR2_POS_A, MOTOR3_POS_A};
int posB[4] = {MOTOR0_POS_B, MOTOR1_POS_B, MOTOR2_POS_B, MOTOR3_POS_B};
int channels[4] = {SERVO_CHANNEL_0, SERVO_CHANNEL_1, SERVO_CHANNEL_2, SERVO_CHANNEL_3};

// Track current position AND target position separately
int currentPos[4] = {MOTOR0_POS_A, MOTOR1_POS_A, MOTOR2_POS_A, MOTOR3_POS_A};
int targetPos[4]  = {MOTOR0_POS_A, MOTOR1_POS_A, MOTOR2_POS_A, MOTOR3_POS_A};

// Structure to receive data (Must match sender)
typedef struct struct_message {
  uint8_t motorIndex;
  uint8_t value;
} struct_message;

struct_message incomingData;

int midiToServo(int midiVal, int minPulse, int maxPulse) {
  return map(midiVal, 0, 127, minPulse, maxPulse);
}

// Callback function executed when data is received
void onReceive(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  memcpy(&incomingData, data, sizeof(incomingData));
  
  uint8_t m = incomingData.motorIndex;
  uint8_t val = incomingData.value;

  if (m <= 3) {
    // Instantly update the target position
    targetPos[m] = midiToServo(val, posA[m], posB[m]);
    Serial.print("Wireless Command -> Motor "); Serial.print(m + 1);
    Serial.print(" Target Pulse: "); Serial.println(targetPos[m]);
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  Wire.begin(21, 22);
  pwm.begin();
  pwm.setPWMFreq(50);
  delay(500);

  // Move all motors to starting positions
  for (int m = 0; m <= 3; m++) {
    pwm.setPWM(channels[m], 0, posA[m]);
  }

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb(onReceive);
  Serial.println("Receiver ready and listening...");
}

void loop() {
  // Non-blocking smooth movement for all 4 motors simultaneously
  bool anyMotorMoved = false;

  for (int i = 0; i <= 3; i++) {
    if (currentPos[i] < targetPos[i]) {
      currentPos[i] += STEP_SIZE;
      if (currentPos[i] > targetPos[i]) currentPos[i] = targetPos[i]; // Prevent overshooting
      pwm.setPWM(channels[i], 0, currentPos[i]);
      anyMotorMoved = true;
    } 
    else if (currentPos[i] > targetPos[i]) {
      currentPos[i] -= STEP_SIZE;
      if (currentPos[i] < targetPos[i]) currentPos[i] = targetPos[i];
      pwm.setPWM(channels[i], 0, currentPos[i]);
      anyMotorMoved = true;
    }
  }

  // Only delay if motors are actually moving, pacing the speed safely
  if (anyMotorMoved) {
    delay(STEP_DELAY);
  }
}