#include "../include/state.h"

// ✅ Initialize Global Variables
int flowMinutes = 0;
int menuIndex = 0;
String menuOptions[3] = {"UP", "DOWN", "Reset"};
bool focusMode = false;
bool lastFocusModeState = false;
int lastMenuIndex = 0;
bool buttonWasPressed = false;
int elapsedMinutes = 0;
int countdownValue = 20;
int initialCountdownValue = 20; // Default countdown value
bool isCounting = false;
unsigned long previousMillis = 0;  // For counting logic
int menuChangeFlag = 0; // Flag to indicate encoder movement
unsigned long lastRotaryTime = 0;
bool rotaryChanged = false; // Flag to indicate rotary encoder change
volatile bool buttonInterruptFlag = false;
volatile unsigned long lastButtonPressTime = 0; // For button debounce
// ✅ Initialize Current System State
State currentState = MENU;

// ✅ Timing Variables
unsigned long lastActivityTime = 0;
const unsigned long inactivityLimit = 3 * 60000;  // 3 minutes
const unsigned long displayOffTimeLimit = 30 * 60000; // 30 minutes
