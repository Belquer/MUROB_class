#include <esp_now.h>
#include <WiFi.h>

// ⚠️ REPLACE THIS WITH THE MAC ADDRESS OF YOUR RECEIVER ESP32 ⚠️
// Example format: {0x3C, 0x71, 0xBF, 0x4A, 0x28, 0x90}
uint8_t receiverAddress[] = {0x20, 0xE7, 0xC8, 0x5A, 0xC6, 0xF8};

// Structure to send data (Must match the receiver exactly)
typedef struct struct_message {
  uint8_t motorIndex; // 0 to 3
  uint8_t value;      // 0 to 127
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(10); // Fast serial parsing
  delay(500);

  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register peer (the receiver)
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  
  Serial.println("Sender ready. Listening to Max...");
}

void loop() {
  if (Serial.available() > 0) {
    // Parse incoming Max data: "M4 100\r" -> grabs 4 and 100
    int motor = Serial.parseInt(); 
    int value = Serial.parseInt(); 
    
    Serial.readStringUntil('\r'); // Clear buffer

    int motorIndex = motor - 1; // Convert 1-4 to 0-3

    // Validate
    if (motorIndex >= 0 && motorIndex <= 3 && value >= 0 && value <= 127) {
      myData.motorIndex = motorIndex;
      myData.value = value;

      // Send the data via ESP-NOW
      esp_err_t result = esp_now_send(receiverAddress, (uint8_t *) &myData, sizeof(myData));
      
      if (result == ESP_OK) {
        Serial.print("Sent -> Motor: "); Serial.print(motor);
        Serial.print(" | Value: "); Serial.println(value);
      } else {
        Serial.println("Error sending data");
      }
    }
  }
}