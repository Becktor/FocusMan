#include "../include/deep_sleep.h"
#include "../include/display.h"
#include "../include/focus_wifi.h"

void enterDeepSleep() {
    
    Serial.println("Turning Off.");
    disconnectWiFi();
    turnDisplayOff();
    delay(1000);
    esp_deep_sleep_start();
}
