#include "../include/hardware.h"
#include "../include/state.h"



void IRAM_ATTR rotaryISR() {
    int clkState = digitalRead(CLK);
    int dtState = digitalRead(DT);
    if (clkState == LOW) {
        if (dtState == HIGH) menuIndex++;
        else menuIndex--;
    }
    rotaryChanged = true; // ✅ Mark state as changed
}

void initHardware() {
    pinMode(SW, INPUT_PULLUP);
    pinMode(CLK, INPUT_PULLUP);
    pinMode(DT, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(CLK), rotaryISR, FALLING);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_25, 0); // Wake up on button press
}
