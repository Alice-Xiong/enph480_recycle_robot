#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define PROG_CONTROL PC_15
#define GAME_MODE 1

// Tape 
#define TAPE_L PA_3
#define TAPE_R PA_2
#define THRES_L 650
#define THRES_R 700

// DC motor definitions
#define MR_B PA_6
#define MR_F PA_7
#define ML_F PB_1
#define ML_B PB_0
#define PWM_FREQ 2000
#define STOP 0
#define FORWARD 1
#define BACK 2

// OLED definitions
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 	-1 // This display does not have a reset pin accessible
#define OLED_SDA PB_11
#define OLED_SCL PB_10

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void drive(int direction, int speedL, int speedR); 

void setup() {
    Wire.setSCL(OLED_SCL);
    Wire.setSDA(OLED_SDA);

    pinMode(TAPE_L, INPUT);
    pinMode(TAPE_R, INPUT);

    // setup pwm out pins
    pinMode(ML_F, OUTPUT);
    pinMode(ML_B, OUTPUT);
    pinMode(MR_F, OUTPUT);
    pinMode(MR_B, OUTPUT);

    // start up the OLED screen
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.display();
    delay(100);
}

void loop() {
    // Engage box
    //drive(BACK, 2000, 2000);
    //delay(2000);

    bool onTapeL=false, onTapeR=false, lastL=false, lastR = false;
    
    while(1) {
        // Tape following
        if (analogRead(TAPE_L) > THRES_L) {
            onTapeL = true;
        } else {
            onTapeL = false;
        }
        if (analogRead(TAPE_R) > THRES_R) {
            onTapeR = true;
        } else {
            onTapeR = false;
        }

        if (onTapeL && onTapeR) {
            drive(FORWARD, 2500, 2500);
        } else if (!onTapeL && onTapeR)
        {
            drive(FORWARD, 2500, 1800);
        } else if (onTapeL && !onTapeR)
        {
            drive(FORWARD, 1800, 2500);
        } else {
            if (lastL) {
                drive(FORWARD, 1500, 3000);
            } else if (lastR)
            {
                drive(FORWARD, 1500, 3000);
            } else
            {
                drive(STOP, 0, 0);
            }
            
        }

        display.clearDisplay();
        display.setCursor(0,0);
        display.println((analogRead(TAPE_L)-700)*10);
        display.println((analogRead(TAPE_R)-700)*10);
        if (onTapeL) {
            display.println("LEFT ON");
        } else {
            display.println("LEFT OFF");
        }
        if (onTapeR) {
            display.println("RIGHT ON");
        } else {
            display.println("RIGHT OFF");
        }
        display.display();

        lastL = onTapeL;
        lastR = onTapeR;

        delay(300);
    }
}

void drive(int direction, int speedL, int speedR) {
    switch (direction)
    {
    case STOP:
        pwm_start(ML_F, PWM_FREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
        pwm_start(MR_F, PWM_FREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
        pwm_start(ML_B, PWM_FREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
        pwm_start(MR_B, PWM_FREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
        break;
    
    case FORWARD:
        pwm_start(ML_F, PWM_FREQ, speedL, RESOLUTION_12B_COMPARE_FORMAT);
        pwm_start(MR_F, PWM_FREQ, speedR, RESOLUTION_12B_COMPARE_FORMAT);
        pwm_start(ML_B, PWM_FREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
        pwm_start(MR_B, PWM_FREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
        break;

     case BACK:
        pwm_start(ML_F, PWM_FREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
        pwm_start(MR_F, PWM_FREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
        pwm_start(ML_B, PWM_FREQ, speedL, RESOLUTION_12B_COMPARE_FORMAT);
        pwm_start(MR_B, PWM_FREQ, speedR, RESOLUTION_12B_COMPARE_FORMAT);
        break;
    
    default:
        break;
    }
}

