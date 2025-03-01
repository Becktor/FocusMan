#include "../include/display.h"
#include "../include/state.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initDisplay() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }
    display.setRotation(2);
    display.clearDisplay();
}
void turnDisplayOff(){
    display.ssd1306_command(SSD1306_DISPLAYOFF); // Ensure display is controlled properly
}

void turnDisplayOn(){
    display.ssd1306_command(SSD1306_DISPLAYON); // Ensure display is controlled properly
}

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