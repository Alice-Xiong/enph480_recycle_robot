#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 	-1 // This display does not have a reset pin accessible
#define COLLISION_PIN PA0
#define PROG_CONTROL 1

void handle_interrupt();
volatile int i = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {
    // setup collision switch PA0
    pinMode(COLLISION_PIN, INPUT_PULLUP);

    // start up the OLED screen
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.display();
    delay(2000);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // attach interrupt
    attachInterrupt(digitalPinToInterrupt(COLLISION_PIN), handle_interrupt, RISING);

}

void loop() {
    int loopcount = 0;
    while(digitalRead(PROG_CONTROL)) {
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("Loop counter:  ");
        display.println(loopcount);
        display.println("Collisions:  ");
        display.println(i);
        display.display();
        loopcount++;
        delay(300);
    }
}


void handle_interrupt() {
    i++;
}
