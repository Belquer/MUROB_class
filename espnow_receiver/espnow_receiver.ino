/*
 * ESP-NOW Wireless Blink - RECEIVER
 *
 * This sketch receives LED blink commands wirelessly from a transmitter ESP32 using ESP-NOW.
 * The receiver listens for commands and controls its LED based on the received state.
 *
 * SETUP INSTRUCTIONS:
 * 1. Upload this sketch to your ESP32 (receiver) first
 * 2. Open Serial Monitor at 115200 baud
 * 3. Copy the MAC address displayed in the Serial Monitor
 * 4. Paste that MAC address into the 'receiverAddress' array in the TRANSMITTER sketch
 * 5. Upload the transmitter sketch to your other ESP32
 * 6. Both LEDs should now blink in sync wirelessly!
 *
 * Hardware:
 * - Built-in LED on GPIO2 (most ESP32 boards)
 *
 * How ESP-NOW Receiver Works:
 * - No MAC address configuration needed on receiver side
 * - ESP-NOW allows any device to send to this receiver
 * - Callback function automatically triggers when data arrives
 * - LED state is updated immediately upon receiving data
 */

#include <esp_now.h>
#include <WiFi.h>

#define LED_PIN 2  // Built-in LED pin (GPIO2 for most ESP32 boards)

// Structure to receive data - must match transmitter structure
typedef struct struct_message {
  bool ledState;
  unsigned long timestamp;  // Timestamp from transmitter
} struct_message;

struct_message incomingData;

// Statistics
unsigned long packetsReceived = 0;
unsigned long lastPacketTime = 0;

// Callback function executed when data is received
void OnDataRecv(const esp_now_recv_info *recv_info, const uint8_t *incomingDataPtr, int len) {
  memcpy(&incomingData, incomingDataPtr, sizeof(incomingData));

  packetsReceived++;
  lastPacketTime = millis();

  // Control the LED based on received state
  digitalWrite(LED_PIN, incomingData.ledState ? HIGH : LOW);

  // Print received data for debugging
  Serial.print("[");
  Serial.print(millis() / 1000.0, 1);
  Serial.print("s] RX #");
  Serial.print(packetsReceived);
  Serial.print(" | LED: ");
  Serial.print(incomingData.ledState ? "ON " : "OFF");
  Serial.print(" | From: ");
  Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X",
                recv_info->src_addr[0], recv_info->src_addr[1], recv_info->src_addr[2],
                recv_info->src_addr[3], recv_info->src_addr[4], recv_info->src_addr[5]);
  Serial.print(" | Size: ");
  Serial.print(len);
  Serial.println(" bytes");
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n========================================");
  Serial.println("   ESP-NOW WIRELESS BLINK - RECEIVER");
  Serial.println("========================================\n");

  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.println("[OK] LED initialized on GPIO2");

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.println("[OK] WiFi set to Station mode");

  // Print MAC Address (THIS IS IMPORTANT - COPY THIS!)
  Serial.println("\n========================================");
  Serial.println("*** COPY THIS MAC ADDRESS ***");
  Serial.println("========================================");
  Serial.print("Receiver MAC Address: ");
  Serial.println(WiFi.macAddress());
  Serial.println("========================================");
  Serial.println("Use this MAC address in the transmitter");
  Serial.println("code's 'receiverAddress' array!");
  Serial.println("========================================\n");

  // Initialize ESP-NOW
  Serial.println("--- Initializing ESP-NOW ---");
  if (esp_now_init() != ESP_OK) {
    Serial.println("[ERROR] ESP-NOW initialization failed!");
    return;
  }
  Serial.println("[OK] ESP-NOW initialized");

  // Register callback function
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("[OK] Receive callback registered");

  Serial.println("\n========================================");
  Serial.println("Ready! Waiting for commands...");
  Serial.println("========================================\n");
}

void loop() {
  // Nothing needed in loop - everything is handled by the callback function
  // The OnDataRecv() callback automatically triggers when data arrives

  // Optional: Check if we haven't received data in a while
  if (packetsReceived > 0 && (millis() - lastPacketTime) > 5000) {
    // No data received for 5 seconds - could indicate connection issue
    // This check only runs after first packet to avoid false warnings on startup
    static bool warningShown = false;
    if (!warningShown) {
      Serial.println("\n[WARNING] No data received for 5 seconds");
      Serial.println("Check transmitter is powered on and MAC address is correct");
      warningShown = true;
    }
  } else {
    // Reset warning flag when data resumes
    static bool warningShown = false;
    warningShown = false;
  }

  delay(100);
}
