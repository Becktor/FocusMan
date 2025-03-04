#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_SSD1306.h>

void initDisplay();
void updateDisplay();
void turnDisplayOff();
void turnDisplayOn();
void successAnimation();
// sda scl
#define SDA_PIN 21  // ESP32 default SDA
#define SCL_PIN 22  // ESP32 default SCL

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#endif