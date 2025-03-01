#include "../include/focus_wifi.h"
#include <WiFiManager.h>  // ✅ Include WiFiManager library
#include <ESPmDNS.h>      // ✅ Enables hostname functionality on ESP32

WiFiManager wifiManager;  // ✅ Create WiFiManager instance

const char* desiredHostname = "FocusESP";  // ✅ Set desired hostname
void initWiFi() {
    Serial.println("🌍 Starting WiFiManager...");

    WiFi.hostname(desiredHostname);  // ✅ Set the hostname for the ESP32
    
    // ✅ Try to connect, if it fails, start a configuration AP
    if (!wifiManager.autoConnect("FocusAP", "password123")) {
        Serial.println("❌ Failed to connect & timeout reached. Restarting...");
        delay(3000);
        ESP.restart();
    }

    Serial.println("\n✅ WiFi Connected!");
    Serial.print("📡 ESP32 IP Address: ");
    Serial.println(WiFi.localIP());
    // ✅ Initialize mDNS with the custom hostname
    if (MDNS.begin(desiredHostname)) {
        Serial.println("✅ mDNS responder started. You can access it via:");
        Serial.print("📡 http://"); Serial.println(desiredHostname); 
    } else {
        Serial.println("❌ mDNS initialization failed!");
    }
}

// ✅ Disconnect WiFi (useful for deep sleep or saving power)
void disconnectWiFi() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    Serial.println("🌍 Disconnected from WiFi.");
}
