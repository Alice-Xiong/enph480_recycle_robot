#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 	-1 // This display does not have a reset pin accessible
#define COLLISION_SENSOR PA0
#define PROG_CONTROL 1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  pinMode(COLLISION_SENSOR, INPUT_PULLUP);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
 
  // Displays Adafruit logo by default. call clearDisplay immediately if you don't want this.
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  while(digitalRead(PROG_CONTROL)) {
    display.setCursor(0,0);
    if (digitalRead(COLLISION_SENSOR)) {
      display.clearDisplay();
      display.println("No collision");
      display.display();
    } else {
      display.clearDisplay();
      display.println("Collision!");
      display.display();
    }
    delay(300);
  }
};
