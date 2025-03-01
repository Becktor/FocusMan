#include "../include/state.h"
#include "../include/inactivity.h"
#include "../include/display.h"

// Track inactivity
unsigned long idleStartTime = 0;
bool displayOff = false;

void handleInactivity(unsigned long currentMillis) {
    unsigned long timeSinceLastActivity = currentMillis - lastActivityTime;

    const unsigned long inactivityLimit = 3 * 60000;  // 3 minutes in milliseconds
    // ✅ Enter IDLE mode if inactive for too long
    if ((currentState == MENU || currentState == SELECTING_DOWN_DURATION) &&
        timeSinceLastActivity > inactivityLimit) {
        
        if (currentState != IDLE) {
            currentState = IDLE;
            idleStartTime = millis();
            updateDisplay();
            Serial.println("Entered IDLE mode due to inactivity.");
        }
    }

    // ✅ Turn off display after extended inactivity
    if (currentState == IDLE && !displayOff &&
        (currentMillis - idleStartTime > displayOffTimeLimit)) {
        
        displayOff = true;
        turnDisplayOff();
        Serial.println("Display turned OFF after 30 minutes of IDLE.");
    }

    // ✅ Wake up on activity
    if (currentState == IDLE && (menuIndex != lastMenuIndex || buttonWasPressed)) {
        currentState = MENU;
        lastActivityTime = millis();  // Reset inactivity timer

        // ✅ Turn display back on if it was off
        if (displayOff) {
            turnDisplayOn();
            displayOff = false;
            Serial.println("Display turned back ON.");
        }

        updateDisplay();
        Serial.println("Exiting IDLE mode. Back to MENU.");
    }

    // ✅ Store last menu index for tracking changes
    lastMenuIndex = menuIndex;
}
