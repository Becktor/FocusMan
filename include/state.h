#ifndef STATE_H
#define STATE_H

#include <Arduino.h>

// ✅ Global State Variables
extern int flowMinutes;
extern int menuIndex;
extern String menuOptions[3];
extern bool focusMode;
extern bool lastFocusModeState;
extern int lastMenuIndex; 
extern bool buttonWasPressed;
extern int elapsedMinutes;
extern int countdownValue;
extern bool isCounting;
extern int initialCountdownValue;
extern unsigned long previousMillis;  // For counting logic
extern int menuChangeFlag; // Flag to indicate encoder movement
extern unsigned long lastRotaryTime; // Faster debounce for rotary encoder
extern bool rotaryChanged;
extern volatile bool buttonInterruptFlag;
extern volatile unsigned long lastButtonPressTime;
// ✅ Enum for System States
enum State { MENU, COUNTING_UP, COUNTING_DOWN, SELECTING_DOWN_DURATION, IDLE };
extern State currentState;

// ✅ Timing Variables
extern unsigned long lastActivityTime;
extern const unsigned long inactivityLimit;
extern const unsigned long displayOffTimeLimit;

#endif
