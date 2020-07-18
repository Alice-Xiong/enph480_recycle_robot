#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define PROG_CONTROL 1

// DC motor definitions
#define M1_B PA_7
#define M1_F PA_6
#define M2_F PB_0
#define M2_B PB_1
#define PWM_FREQ 2000

// OLED definitions
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 	-1 // This display does not have a reset pin accessible
#define OLED_SDA PB_11
#define OLED_SCL PB_10

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
    Wire.setSCL(OLED_SCL);
    Wire.setSDA(OLED_SDA);

    // setup pwm out pins
    pinMode(M1_F, OUTPUT);
    pinMode(M1_B, OUTPUT);
    pinMode(M2_F, OUTPUT);
    pinMode(M2_B, OUTPUT);

    // start up the OLED screen
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
}

void loop() {
    while(digitalRead(PROG_CONTROL)) {
        //pwm_start(M1_F, PWM_FREQ, 3000, RESOLUTION_12B_COMPARE_FORMAT);
        //pwm_start(M2_B, PWM_FREQ, 3000, RESOLUTION_12B_COMPARE_FORMAT);

        display.clearDisplay();
        display.setCursor(0,0);
        display.println("Aloha");
        display.display();
        delay(50);
    }
}

