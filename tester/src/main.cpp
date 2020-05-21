#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 	-1 // This display does not have a reset pin accessible
#define TAPE_DETECT_PIN PA7
#define REF_PIN PA1
#define PROG_CONTROL 1

void handle_interrupt();
volatile int i = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {
    // setup taper detecter pin PA7
    pinMode(TAPE_DETECT_PIN, INPUT);
    pinMode(REF_PIN, INPUT);

    // start up the OLED screen
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.display();
    delay(2000);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
}

void loop() {
    while(digitalRead(PROG_CONTROL)) {
        display.clearDisplay();
        display.setCursor(0,0);
        display.println(analogRead(TAPE_DETECT_PIN));
        display.display();
        delay(300);
    }
}


void handle_interrupt() {

}
