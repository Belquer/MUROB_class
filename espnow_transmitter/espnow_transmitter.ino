/*
 * ESP-NOW Wireless Control - TRANSMITTER (Max/MSP Compatible)
 *
 * This sketch receives serial commands from Max/MSP and sends LED control
 * commands wirelessly to a receiver ESP32 using ESP-NOW.
 *
 * SETUP INSTRUCTIONS:
 * 1. Upload the RECEIVER sketch to your second ESP32 first
 * 2. Open Serial Monitor for the receiver and copy its MAC address
 * 3. Update the 'receiverAddress' array below with the receiver's MAC address
 * 4. Upload this sketch to the first ESP32 (transmitter)
 * 5. Connect to serial port at 115200 baud from Max/MSP
 *
 * SERIAL PROTOCOL:
 * - Send '1' or '1\n' to turn LED ON
 * - Send '0' or '0\n' to turn LED OFF
 *
 * Hardware:
 * - Built-in LED on GPIO2 (most ESP32 boards)
 */

#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH YOUR RECEIVER'S MAC ADDRESS
// To get receiver MAC: Upload receiver code first and check Serial Monitor
uint8_t receiverAddress[] = {0x80, 0xF3, 0xDA, 0x65, 0x70, 0xCC};  // Receiver MAC: 80:f3:da:65:70:cc

#define LED_PIN 2  // Built-in LED pin (GPIO2 for most ESP32 boards)

// Structure to send data - must match receiver structure
typedef struct struct_message {
  bool ledState;
  unsigned long timestamp;  // Add timestamp for debugging
} struct_message;

struct_message outgoingData;

// Peer info
esp_now_peer_info_t peerInfo;

// Variables for LED control
bool ledState = false;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n========================================");
  Serial.println("  ESP-NOW WIRELESS BLINK - TRANSMITTER");
  Serial.println("========================================\n");

  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.println("[OK] LED initialized on GPIO2");

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.println("[OK] WiFi set to Station mode");

  // Print MAC Address
  Serial.println("\n--- Device Information ---");
  Serial.print("Transmitter MAC Address: ");
  Serial.println(WiFi.macAddress());

  Serial.println("\n--- Peer Configuration ---");
  Serial.print("Target Receiver MAC: ");
  Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\n",
                receiverAddress[0], receiverAddress[1], receiverAddress[2],
                receiverAddress[3], receiverAddress[4], receiverAddress[5]);

  // Check if default MAC address is still set
  if (receiverAddress[0] == 0xFF && receiverAddress[1] == 0xFF) {
    Serial.println("\n*** WARNING: Default receiver MAC detected! ***");
    Serial.println("*** Please update receiverAddress[] array with actual MAC ***");
  }

  // Initialize ESP-NOW
  Serial.println("\n--- Initializing ESP-NOW ---");
  if (esp_now_init() != ESP_OK) {
    Serial.println("[ERROR] ESP-NOW initialization failed!");
    return;
  }
  Serial.println("[OK] ESP-NOW initialized");

  // Register send callback
  esp_now_register_send_cb(OnDataSent);
  Serial.println("[OK] Send callback registered");

  // Register peer
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("[ERROR] Failed to add peer!");
    Serial.println("Check that the receiver MAC address is correct.");
    return;
  }
  Serial.println("[OK] Peer added successfully");

  Serial.println("\n========================================");
  Serial.println("Ready! Waiting for serial commands...");
  Serial.println("Send '1' to turn LED ON");
  Serial.println("Send '0' to turn LED OFF");
  Serial.println("========================================\n");
}

void loop() {
  // Check if serial data is available
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();

    // Process command
    if (incomingByte == '1') {
      ledState = true;
      sendLEDCommand();
    }
    else if (incomingByte == '0') {
      ledState = false;
      sendLEDCommand();
    }
    // Ignore newlines, carriage returns, and other characters
  }

  delay(10);  // Small delay to prevent overwhelming the serial port
}

void sendLEDCommand() {
  // Update local LED
  digitalWrite(LED_PIN, ledState);

  // Prepare data to send
  outgoingData.ledState = ledState;
  outgoingData.timestamp = millis();

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(receiverAddress, (uint8_t *) &outgoingData, sizeof(outgoingData));

  // Print status
  Serial.print("[");
  Serial.print(millis() / 1000.0, 1);
  Serial.print("s] Command: LED ");
  Serial.print(ledState ? "ON " : "OFF");
  Serial.print(" | TX: ");

  if (result == ESP_OK) {
    Serial.println("Sent");
  } else {
    Serial.println("FAIL");
  }
}
