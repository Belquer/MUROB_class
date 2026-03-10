#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  
  // Turn on the Wi-Fi radio
  WiFi.mode(WIFI_STA);
  
  Serial.println("Waking up Wi-Fi radio...");
}

void loop() {
  // Ask for the MAC address every 2 seconds until you see it
  Serial.print("Receiver MAC Address: ");
  Serial.println(WiFi.macAddress());
  
  delay(2000); 
}