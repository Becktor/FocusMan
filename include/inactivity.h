#ifndef INACTIVITY_H
#define INACTIVITY_H

#include <Arduino.h>

// Function Declarations
void handleInactivity(unsigned long currentMillis);

// Extern variables to avoid multiple definitions
extern unsigned long lastActivityTime;
// extern volatile int menuIndex;
// extern volatile int lastMenuIndex;
// extern volatile unsigned long inactivityLimit;  // 3 minutes in milliseconds

#endif
