#include "../include/focus_webserver.h"
#include "../include/state.h"
#include "../include/display.h"
#include <ESPAsyncWebServer.h>

// ✅ Define AsyncWebServer globally
AsyncWebServer server(80);

// ✅ Handle incoming requests asynchronously
void initWebServer() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", focusMode ? "FOCUS ON" : "FOCUS OFF");
    });

    server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request) {
        focusMode = !focusMode;
        updateDisplay();
        request->send(200, "text/plain", focusMode ? "FOCUS ON" : "FOCUS OFF");
    });

    server.begin();
    Serial.println("✅ Async Web Server Started!");
    Serial.print("📡 Access it at: http://"); Serial.print(WiFi.localIP()); Serial.println("/");
}
