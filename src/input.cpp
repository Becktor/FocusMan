#include "../include/input.h"
#include "../include/hardware.h"
#include "../include/state.h"
#include "../include/display.h"
#include "../include/deep_sleep.h"


//=========================================================
// ✅ Non-blocking rotary handling function
void handleRotaryInput() {
    if (!rotaryChanged) return; // ✅ Skip if no change detected
    rotaryChanged = false; // ✅ Reset change flag

    lastActivityTime = millis(); // ✅ Reset inactivity timer

    // ✅ Ensure menuIndex stays within valid range (3 options: UP, DOWN, Reset)
    menuIndex = (menuIndex + 3) % 3;  

    // ✅ Update display if we're in MENU
    if (currentState == MENU) {
        updateDisplay();
        Serial.print("Menu option: "); Serial.println(menuOptions[menuIndex]);
    }
    
    // ✅ Adjust countdown value if selecting duration
    else if (currentState == SELECTING_DOWN_DURATION) {
        countdownValue = max(1, countdownValue + (menuIndex == 0 ? 1 : -1));
        updateDisplay();
        Serial.print("Countdown value: "); Serial.println(countdownValue);
    }
}

// Start counting up
void startCountingUp() {
  currentState = COUNTING_UP;
  elapsedMinutes = 0;
  isCounting = true;
  focusMode = true;
  lastActivityTime = millis();  // Reset inactivity timer
  Serial.println("Counting UP started.");

}

//=========================================================
// Start selecting the countdown duration
void startSelectingDownDuration() {
  currentState = SELECTING_DOWN_DURATION;
  countdownValue = 20;
  lastActivityTime = millis();  // Reset inactivity timer
  Serial.println("Selecting DOWN duration.");
}

//=========================================================
// Confirm countdown selection and start counting down
void confirmCountdownSelection() {
  initialCountdownValue = countdownValue;
  currentState = COUNTING_DOWN;
  isCounting = true;
  lastActivityTime = millis();  // Reset inactivity timer
  Serial.print("Counting DOWN started with "); Serial.print(countdownValue); Serial.println(" minutes.");
}

//=========================================================
// Stop counting up and return to menu
void stopCountingUp() {
    flowMinutes += elapsedMinutes;
    successAnimation();
    currentState = MENU;
    isCounting = false;
    Serial.println("Counting UP stopped. Returning to MENU.");
}
//=========================================================
// Stop counting up and return to menu
void quietStopCountingUp() {
    flowMinutes += elapsedMinutes;
    currentState = MENU;
    isCounting = false;
    Serial.println("Quiet Counting UP stopped. Returning to MENU.");
}
//=========================================================
// Stop counting down and return to menu
void stopCountingDown() {
  flowMinutes += (initialCountdownValue - countdownValue);
  successAnimation();
  currentState = MENU;
  isCounting = false;
  Serial.println("Counting DOWN stopped. Returning to MENU.");
}
void quietStopCountingDown() {
  flowMinutes += (initialCountdownValue - countdownValue);
  currentState = MENU;
  isCounting = false;
  Serial.println("Counting DOWN stopped. Returning to MENU.");
}
//=========================================================
// Reset the total flow minutes counter to 0
void resetFlowMinutes() {
  flowMinutes = 0;
  Serial.println("Flow minutes reset to 0.");
  updateDisplay();  // Update the display to show the reset value
}

//=========================================================
static long pressStartTime = 0;
static long lastClickTime = 0;
const long longPressThreshold = 2000;   // ✅ 800ms for Long Click
const long doubleClickThreshold = 500; // ✅ 500ms for Double Click

static bool firstClickDetected = false;
// ✅ Non-blocking button press detection with debounce

ClickType buttonPressed() {
    unsigned long currentTime = millis();
    if (digitalRead(SW) == LOW) {  // Button is pressed
        if (!buttonWasPressed) {   // Detect first press
            buttonWasPressed = true;
            pressStartTime = millis();  // Start timing the press duration
            Serial.println("press!");
        }
    } 
    else if (buttonWasPressed) {   // Button was released
        buttonWasPressed = false;
        unsigned long pressDuration = millis() - pressStartTime;
        lastActivityTime = millis();  // Reset inactivity timer
        Serial.println("release!");
        if (pressDuration >= longPressThreshold) {
            
            Serial.println("Long Click!");
            return LONG_CLICK;  // ✅ Return Long Click if held long enough
        } 
        else if (firstClickDetected && (currentTime - lastClickTime < doubleClickThreshold)) {
            firstClickDetected = false;  // Reset first click flag
                        
            Serial.println("Double Click!");
            return DOUBLE_CLICK;  // ✅ Return Double Click
        } 
        else {
            firstClickDetected = true;
            lastClickTime = currentTime;
        }
    }
    if (firstClickDetected && (currentTime - lastClickTime > doubleClickThreshold)){
        firstClickDetected=false;
                  
        Serial.println("Click!");
        return NORMAL_CLICK;  // ✅ Return Normal Click
    }
    return NO_CLICK;  // No button press detected
}

// ✅ Handle button presses and trigger actions
void handleButtonPresses(unsigned long currentMillis) {
    ClickType click = buttonPressed();

    if (click == NORMAL_CLICK) {
        if (currentState == MENU) {
            if (menuIndex == 0) startCountingUp();
            else if (menuIndex == 1) startSelectingDownDuration();
            else if (menuIndex == 2) resetFlowMinutes();
        } 
        else if (currentState == SELECTING_DOWN_DURATION) confirmCountdownSelection();
        else if (currentState == COUNTING_UP) stopCountingUp();
        else if (currentState == COUNTING_DOWN) stopCountingDown();
    } else if (click == DOUBLE_CLICK) {
        // ✅ Double click: Toggle between UP ↔ DOWN mode, but faster
        Serial.println("Double Click detected!");
        if (currentState == COUNTING_UP) {
            quietStopCountingUp();  // Stop counting up
            confirmCountdownSelection();  // Switch to DOWN mode
        } else if (currentState == COUNTING_DOWN) {
            quietStopCountingDown();  // Stop counting down
            startCountingUp();  // Switch to UP mode
        } else if ((currentState == MENU) || (currentState == IDLE)) {
            startCountingUp();  // Start counting up
        }
    } else if (click == LONG_CLICK) { // ✅ Fix comparison (was `click = LONG_CLICK`)
        Serial.println("Long Click detected!");
        enterDeepSleep();
    }

    updateDisplay();
}
//=========================================================
// Handle counting up or down logic
void handleCounting(unsigned long currentMillis) {
  if (!isCounting || (currentMillis - previousMillis < 60000)) return;

  previousMillis = currentMillis;
  
  if (currentState == COUNTING_UP) {
    elapsedMinutes++;
    updateDisplay();
    Serial.print("Counting UP: "); Serial.println(elapsedMinutes);
  } else if (currentState == COUNTING_DOWN) {
    countdownValue--;
    if (countdownValue <= 0) {
      flowMinutes += initialCountdownValue;
      successAnimation();
      currentState = MENU;
      isCounting = false;
      Serial.println("Countdown finished, returning to MENU.");
    }
    updateDisplay();
    Serial.print("Counting DOWN: "); Serial.println(countdownValue);
  }
}
