#include <Adafruit_SSD1306.h>
#include <splash.h>
#include <Arduino_BuiltIn.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ✅ Change these to your WiFi credentials
const char* ssid = "ssid";
const char* password = "passwd";

// ✅ Initialize Web Server on port 80
ESP8266WebServer server(80);



#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//-----------------------------------------------
#define CLK    D6
#define DT     D7
#define SW     D4

//-----------------------------------------------
int flowMinutes = 0;   // Total flow minutes
int menuIndex = 0;     // 0 for UP, 1 for DOWN, 2 for Reset
String menuOptions[3] = {"UP", "DOWN", "Reset"};  // Label reset option as "Reset"
unsigned long lastActivityTime = 0;  // For inactivity detection
const unsigned long inactivityLimit = 3 * 60000;  // 3 minutes in milliseconds

enum ClickType { NO_CLICK, NORMAL_CLICK, LONG_CLICK, DOUBLE_CLICK };
enum State { MENU, COUNTING_UP, COUNTING_DOWN, SELECTING_DOWN_DURATION, IDLE };
State currentState = MENU;

int countdownValue = 20;  // Default value for countdown
int initialCountdownValue = 20;  // Store the countdown value when selected
unsigned long previousMillis = 0;  // For counting logic
int elapsedMinutes = 0;
bool isCounting = false;
unsigned long buttonDebounceTime = 0;
const unsigned long buttonDebounceDelay = 800;  // Debounce delay

// Rotary encoder debounce variables
unsigned long lastRotaryTime = 0;
const unsigned long rotaryDebounceDelay = 150;  // Faster debounce for rotary encoder

// IDLE mode extended behavior
const unsigned long displayOffTimeLimit = 30 * 60000;  // 30 minutes in milliseconds

unsigned long idleStartTime = 0;  // Track when IDLE mode starts
bool displayOff = false;  // Track if the display is off

// Double click
unsigned long lastButtonPressTime = 0;
const unsigned long doubleClickTime = 500; // Max time between clicks (milliseconds)
bool singleClickDetected = false;

// focusMode
bool focusMode = false; 

//=========================================================
void setup() {  
  initHardware();
  initDisplay();
  initWiFi();
  initWebServer();
  updateDisplay();
  Serial.println("Setup complete, starting loop...");
}

//=========================================================
void loop() {
  server.handleClient();
  unsigned long currentMillis = millis();
  
  // Handle rotary encoder input
  handleRotaryInput();

  // Handle button presses and states
  handleButtonPresses(currentMillis);

  // Handle counting logic
  handleCounting(currentMillis);

  // Handle inactivity
  handleInactivity(currentMillis);
}

//=========================================================
// Initialize hardware pins and serial communication
void initHardware() {
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);
  pinMode(SW, INPUT_PULLUP);
  Serial.begin(9600);
}

//=========================================================
// Initialize the OLED display
void initDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.setRotation(2);  // Rotate display 180 degrees

  display.clearDisplay(); 
  Serial.println("Display initialized.");
}

// ==========================================================
// ✅ Wi-Fi Initialization
void initWiFi() {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected!");
    Serial.print("ESP32 IP Address: ");
    Serial.println(WiFi.localIP());
}

// ==========================================================
// ✅ Web Server Initialization
void initWebServer() {
    server.on("/", []() {
        server.send(200, "text/plain", focusMode ? "FOCUS ON" : "FOCUS OFF");
    });

    server.on("/toggle", []() {
        focusMode = !focusMode;
        updateDisplay();
        server.send(200, "text/plain", focusMode ? "FOCUS ON" : "FOCUS OFF");
    });

    server.begin();
    Serial.println("Web Server Started!");
}
//=========================================================
// Update the OLED display with the current state
void updateDisplay() {
  display.setTextColor(WHITE);
  display.clearDisplay();

  // Move top row text to bottom row
  String bottomRowText;
  
  if (currentState == COUNTING_UP) {
    bottomRowText = "Focus! \x18";  // Upward triangle for counting UP
  } else if (currentState == COUNTING_DOWN) {
    bottomRowText = "Focus! \x19";  // Downward triangle for counting DOWN
  } else {
    bottomRowText = "Flow: " + String(flowMinutes);  // Display total flow minutes when not counting
  }

  int bottomRowTextWidth = bottomRowText.length() * 12;  // TextSize 2, so 12 pixels per char
  int bottomRowX = (128 - bottomRowTextWidth) / 2;  // Center the text

  display.setTextSize(2);  // Keep text size large
  display.setCursor(bottomRowX, 50);  // Move to bottom row (Y = 50 for 64px height)
  display.print(bottomRowText);

  // Keep main row text unchanged
  String mainRowText;
  
  if (currentState == MENU) {
    mainRowText = menuOptions[menuIndex];  // Display UP, DOWN, or Reset
  } else if (currentState == COUNTING_UP) {
    mainRowText = String(elapsedMinutes);  // Counting up minutes
  } else if (currentState == COUNTING_DOWN || currentState == SELECTING_DOWN_DURATION) {
    mainRowText = String(countdownValue);  // Countdown minutes
  } else if (currentState == IDLE) {
    mainRowText = "IDLE?";
  }
  
  int mainRowTextWidth = mainRowText.length() * 24;  // TextSize 4, so 24 pixels per char
  int mainRowX = (128 - mainRowTextWidth) / 2;  // Center text horizontally

  display.setTextSize(4);  // Keep main row text large
  display.setCursor(mainRowX, 10);  // Move main text higher
  display.print(mainRowText);
  
  display.display();  // Show the updated display
}


//=========================================================
// Detect button presses with debounce logic
// bool buttonPressed() {
//   if (digitalRead(SW) == LOW && (millis() - buttonDebounceTime > buttonDebounceDelay)) {
//     buttonDebounceTime = millis();  // Debounce
//     
//     return true;
//   }
//   return false;
// }
// bool buttonReleased() {
//   if (digitalRead(SW) == HIGH) {
    
//     Serial.println("Button released.");
//     return true;
//   }
//   return false;
// }
static bool buttonWasPressed = false;
ClickType buttonPressed() {

    static unsigned long pressStartTime = 0;
    static unsigned long lastClickTime = 0;
    static bool firstClickDetected = false;
    
    const unsigned long longPressThreshold = 800;  // Time to register a long press (ms)
    const unsigned long doubleClickThreshold = 500;  // Max time between clicks (ms)
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
            return LONG_CLICK;  // ✅ Return Long Click if held long enough
        } 
        else if (firstClickDetected && (currentTime - lastClickTime < doubleClickThreshold)) {
            firstClickDetected = false;  // Reset first click flag
            return DOUBLE_CLICK;  // ✅ Return Double Click
        } 
        else {
            firstClickDetected = true;
            lastClickTime = currentTime;
        }
    }
    if (firstClickDetected && (currentTime - lastClickTime > doubleClickThreshold)){
      firstClickDetected=false;
      return NORMAL_CLICK;  // ✅ Return Normal Click
    }
    return NO_CLICK;  // No button press detected
}

//=========================================================
// Handle button presses and manage state transitions
// void handleButtonPresses(unsigned long currentMillis) {
//   if (!buttonPressed()) return;
//   switch (currentState) {
//     case MENU:
//       if (menuIndex == 0) {  // UP selected
//         startCountingUp();
//       } else if (menuIndex == 1) {  // DOWN selected
//         startSelectingDownDuration();
//       } else if (menuIndex == 2) {  // Reset selected
//         resetFlowMinutes();  // Reset the total focus time to 0
//       }
//       break;
      
//     case SELECTING_DOWN_DURATION:
//       confirmCountdownSelection();
//       break;

//     case COUNTING_UP:
//       stopCountingUp();
//       break;

//     case COUNTING_DOWN:
//       stopCountingDown();
//       break;
//   }
//   updateDisplay();
// }
void handleButtonPresses(unsigned long currentMillis) {
    ClickType click = buttonPressed();  // Get the type of click

    if (click == NORMAL_CLICK) {
        // ✅ Normal click: Perform regular button functions
        switch (currentState) {
            case MENU:
                if (menuIndex == 0) { startCountingUp(); } 
                else if (menuIndex == 1) { startSelectingDownDuration(); } 
                else if (menuIndex == 2) { resetFlowMinutes(); }
                break;
            case SELECTING_DOWN_DURATION:
                confirmCountdownSelection();
                break;
            case COUNTING_UP:
                stopCountingUp();
                focusMode = false;
                break;
            case COUNTING_DOWN:
                stopCountingDown();
                focusMode = false;
                break;
        }
    } 
    else if (click == LONG_CLICK) {
        // ✅ Long click: Toggle between UP ↔ DOWN modes
        Serial.println("Long Click detected!");
        if (currentState == COUNTING_UP) {
            startSelectingDownDuration();  // Switch to DOWN mode
        } else if (currentState == COUNTING_DOWN) {
            startCountingUp();  // Switch to UP mode
        }
    }
    else if (click == DOUBLE_CLICK) {
        // ✅ Double click: Toggle between UP ↔ DOWN mode, but faster
        Serial.println("Double Click detected!");
        if (currentState == COUNTING_UP) {
            startSelectingDownDuration();  // Switch to DOWN mode
        } else if (currentState == COUNTING_DOWN) {
            startCountingUp();  // Switch to UP mode
        }
    }

    updateDisplay();
}

//=========================================================
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
// Stop counting down and return to menu
void stopCountingDown() {
  flowMinutes += (initialCountdownValue - countdownValue);
  successAnimation();
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

//=========================================================
// Success animation when a session ends
void successAnimation() {
  display.clearDisplay();
  int centerX = 64, centerY = 32;

  for (int radius = 2; radius <= 30; radius += 2) {
    display.drawCircle(centerX, centerY, radius, WHITE);
    display.display();
    delay(100);

    if (radius % 4 == 0) {
      display.clearDisplay();
      display.display();
      delay(2);
    }
  }
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(20, 20);
  display.print("SUCCESS!");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.display();
}

//=========================================================
// Rotary Encoder Rotation Detection
int getRotation() {
  static int previousCLK = digitalRead(CLK);
  int currentCLK = digitalRead(CLK);
  
  if (currentCLK == LOW && previousCLK == HIGH && (millis() - lastRotaryTime > rotaryDebounceDelay)) {
    lastRotaryTime = millis();  // Debounce
    int DTValue = digitalRead(DT);  // Read DT to determine direction

    previousCLK = currentCLK;  // Update previous CLK for next iteration

    return (DTValue != currentCLK) ? 1 : -1;  // Clockwise or counterclockwise
  }
  
  previousCLK = currentCLK;
  return 0;  // No rotation
}

//=========================================================
// Handle rotary input for menu and countdown selection
void handleRotaryInput() {
  int rotation = getRotation();
  if (rotation == 0) return;  // No rotation detected
  
  lastActivityTime = millis();  // Reset inactivity timer on any valid rotation
  Serial.print(millis());  // Print the current time in milliseconds
  Serial.print(" - Rotation detected, activity timer reset. Rotation: ");
  Serial.println(rotation);

  if (currentState == MENU) {
    menuIndex = (menuIndex + rotation + 3) % 3;  // Update for 3 menu options: UP, DOWN, Reset
    updateDisplay();
    Serial.print(millis());  // Print the current time in milliseconds
    Serial.print(" - Menu option: "); Serial.println(menuOptions[menuIndex]);
  } else if (currentState == SELECTING_DOWN_DURATION) {
    countdownValue = max(1, countdownValue + rotation);
    updateDisplay();
    Serial.print(millis());  // Print the current time in milliseconds
    Serial.print(" - Countdown value: "); Serial.println(countdownValue);
  }
}
//=========================================================
// Handle inactivity and switch to IDLE if necessary
void handleInactivity(unsigned long currentMillis) {
  // Comment out frequent serial prints to improve performance
  /*
  Serial.print(millis());
  Serial.print(" - Current time (millis): ");
  Serial.println(currentMillis);
  
  Serial.print(millis());
  Serial.print(" - Last activity time (millis): ");
  Serial.println(lastActivityTime);
  */

  // Make sure the subtraction does not cause an overflow/underflow
  if (currentMillis >= lastActivityTime) {
    unsigned long timeSinceLastActivity = currentMillis - lastActivityTime;

    /*
    Serial.print(millis());
    Serial.print(" - Time since last activity (ms): ");
    Serial.println(timeSinceLastActivity);
    */

    // Check if the user is in the MENU or selecting countdown duration mode
    if ((currentState == MENU || currentState == SELECTING_DOWN_DURATION) && 
        (timeSinceLastActivity > inactivityLimit)) {
      if (currentState != IDLE) {
        currentState = IDLE;
        idleStartTime = millis();  // Record when IDLE mode starts
        updateDisplay();
        Serial.print(millis());  // Print the current time in milliseconds
        Serial.println(" - IDLE state entered due to inactivity.");
      }
    }
  } else {
    // Comment out warning to reduce unnecessary serial prints
    // Serial.println(" - Warning: currentMillis is less than lastActivityTime!");
  }

  // Check if the user has been in IDLE for more than 30 minutes and turn off the display
  if (currentState == IDLE && !displayOff && 
      (currentMillis - idleStartTime > displayOffTimeLimit) && 
      (currentMillis > idleStartTime)) {
    displayOff = true;
    display.ssd1306_command(SSD1306_DISPLAYOFF);  // Turn off the display
    Serial.print(millis());  // Print the current time in milliseconds
    Serial.println(" - Display turned off after 30 minutes of IDLE.");
  }

  // Exit IDLE if any rotary or button action happens
  if (currentState == IDLE && (getRotation() != 0 || buttonPressed())) {
    currentState = MENU;
    lastActivityTime = millis();  // Reset inactivity timer upon exiting IDLE
    
    // Turn the display back on if it was off
    if (displayOff) {
      display.ssd1306_command(SSD1306_DISPLAYON);
      displayOff = false;
      Serial.print(millis());  // Print the current time in milliseconds
      Serial.println(" - Display turned back on.");
    }

    updateDisplay();
    Serial.print(millis());  // Print the current time in milliseconds
    Serial.println(" - Exiting IDLE mode. Back to MENU.");
  }
}   