#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define PROG_CONTROL 1

/* USE GUIDE:
 * Copy function and relevant definesfrom recycle_robot main.cpp
 * Fix tune 
 * Copy back
 */

// DC motor definitions
#define MR_B PA_6
#define MR_F PA_7
#define ML_F PB_1
#define ML_B PB_0
#define DRIVE_PWM_FREQ 2000

// Arm and servos
#define ARM_PWM_FREQ 50
#define GRAB_L PB_9
#define GRAB_R PB_8
#define ARM PB_6
#define CAN_SENSE PA_1
#define CAN_THRES 100
#define CLOSE_L 2700
#define CLOSE_R 3d00
#define OPEN_L 2100
#define OPEN_R 900
#define ARM_UP 2600
#define ARM_DOWN 1300

// OLED definitions
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 	-1 // This display does not have a reset pin accessible
#define OLED_SDA PB_11
#define OLED_SCL PB_10

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void score();
void commandGrabber(int valueL, int valueR);

void setup() {
    Wire.setSCL(OLED_SCL);
    Wire.setSDA(OLED_SDA);

    // setup servos
    pinMode(ARM, OUTPUT);
    pinMode(GRAB_L, OUTPUT);
    pinMode(GRAB_R, OUTPUT);

    // start up the OLED screen
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.println("MAC");
    display.display();
}

void loop() {
    score();
    delay(3000);
}


void score() {
    // Close grabber
    commandGrabber(CLOSE_L, CLOSE_R);
    delay(300);  
    
    // Raise arm
    pwm_start(ARM, ARM_PWM_FREQ, ARM_UP, MICROSEC_COMPARE_FORMAT);
    delay(650);
    
    // Open grabber
    commandGrabber(OPEN_L, OPEN_R);
    delay(200);

    // Lower arm
    pwm_start(ARM, ARM_PWM_FREQ, ARM_DOWN, MICROSEC_COMPARE_FORMAT);
    delay(300);
}

void commandGrabber(int valueL, int valueR) {
    pwm_start(GRAB_L, ARM_PWM_FREQ, valueL, MICROSEC_COMPARE_FORMAT);
    pwm_start(GRAB_R, ARM_PWM_FREQ, valueR, MICROSEC_COMPARE_FORMAT);
}

