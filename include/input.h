#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>

// Function Declarations
void startCountingUp();
void startSelectingDownDuration();
void resetFlowMinutes();
// ✅ Enum for Click Types
enum ClickType { NO_CLICK, NORMAL_CLICK, LONG_CLICK, DOUBLE_CLICK };
ClickType buttonPressed();
void handleRotaryInput();
void handleButtonPresses(unsigned long currentMillis);
void handleInput(unsigned long currentMillis);
void handleCounting(unsigned long currentMillis);

// // Extern variables for shared access
// extern volatile bool buttonWasPressed;
// extern volatile int menuIndex;
// extern unsigned long lastActivityTime;

#endif
