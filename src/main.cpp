#include "../include/hardware.h"
#include "../include/display.h"
#include "../include/focus_webserver.h"
#include "../include/focus_wifi.h"
#include "../include/state.h"
#include "../include/deep_sleep.h"
#include "../include/inactivity.h"
#include "../include/input.h"

void setup() {
    Serial.begin(9600);
    delay(2000); //delay five seconds.
    initHardware();
    Serial.println("Hardware Init.");
    initDisplay();
    Serial.println("Display Init");
    initWiFi();
    Serial.println("Wifi Init");
    initWebServer();
    Serial.println("Webserv Init");
    updateDisplay();
    Serial.println("Setup complete, starting loop...");
}

void loop() {
    unsigned long currentMillis = millis();
     // Handle rotary encoder input
    handleRotaryInput();
    // Handle button presses and states
    handleButtonPresses(currentMillis);
    handleCounting(currentMillis);
    //handleInactivity(currentMillis);
}
