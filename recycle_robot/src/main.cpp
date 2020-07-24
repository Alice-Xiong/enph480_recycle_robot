#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define DEBUG 0
#define GAME_MODE 1

// Arm and servos
#define GRAB_L PB_8
#define GRAB_R PB_9
#define ARM PB_6
#define CAN_SENSE PA_1
#define CAN_THRES 100
#define CLOSE_L 2900
#define CLOSE_R 550
#define OPEN_L 1800
#define OPEN_R 1150
#define ARM_UP 2600
#define ARM_DOWN 1250

// Tape 
#define TAPE_L PA_3
#define TAPE_R PA_2
#define THRES_L 150
#define THRES_R 150

// DC motor definitions
#define MR_B PA_6
#define MR_F PA_7
#define ML_F PB_1
#define ML_B PB_0
#define PWM_FREQ FAST
#define STOP 0
#define FORWARD 1
#define BACK 2

//Tape following
#define FAST 3850
#define REG 3600
#define SLOW 3500
#define VSLOW 3400

// OLED definitions
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 	-1 // This display does not have a reset pin accessible
#define OLED_SDA PB_11
#define OLED_SCL PB_10

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Function prototypes
void score();
void engageBox(int time);
void lineFollow();
void drive(int direction, int speedL, int speedR); 
void commandGrabber(int valueL, int valueR);

// Global variables
bool onTapeL=true, onTapeR=true, lastL=true, lastR=true;


void setup() {
    Wire.setSCL(OLED_SCL);
    Wire.setSDA(OLED_SDA);
    // setup servos
    pinMode(ARM, OUTPUT);
    pinMode(GRAB_L, OUTPUT);
    pinMode(GRAB_R, OUTPUT);

    // setup tape sensors
    pinMode(TAPE_L, INPUT);
    pinMode(TAPE_R, INPUT);

    // setup dc motor pwm out pins
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
    //Engage box
    //engageBox(500);

    while (1)
    {
        /*
        if (analogRead(CAN_SENSE) < CAN_THRES) {
            score();
        }
        */
        if (DEBUG) {
            display.clearDisplay();
            display.setCursor(0,0);
            display.println(analogRead(CAN_SENSE));

            if (analogRead(CAN_SENSE) < CAN_THRES) {
                display.println("CAN ");
                
            } else {
                display.println("NO CAN");
            }
            display.display();
        }
        
       lineFollow();
    }
    
}

/*
*
* Important motions
*
*/
void engageBox(int time) {
    double start_time = getCurrentMillis();
    while (getCurrentMillis() - start_time < time) {
        drive(BACK, FAST, FAST);
        lastL = analogRead(TAPE_L) > THRES_L;
        lastR = analogRead(TAPE_R) > THRES_R;
    }
}

void lineFollow(){
    // Tape following
    onTapeL = analogRead(TAPE_L) > THRES_L;
    onTapeR = analogRead(TAPE_R) > THRES_R;
    
    if (onTapeL || onTapeR) {
        //Either sensor on tape
        if (onTapeL && onTapeR) {
            drive(FORWARD, REG, REG);
        } else if (!onTapeL && onTapeR)
        {
            drive(FORWARD, REG, SLOW);
        } else if (onTapeL && !onTapeR)
        {
            drive(FORWARD, SLOW, REG);
        }
        lastL = onTapeL;
        lastR = onTapeR;
    }
    else {
        // Both sensors off tape
        // Not updating last values until one sensor is on tape again
        // lastL is last time left sensor was on tape..
        if (lastL) {
            drive(FORWARD, 0, SLOW);
        } else if (lastR)
        {
            drive(FORWARD, SLOW, 0);
        } else
        {
            drive(FORWARD, SLOW, SLOW);
        }  
    }
    delay(20);

    // print stuff, only on debug 
    if (DEBUG) {
        display.clearDisplay();
        display.setCursor(0,0);
        display.println(analogRead(TAPE_L));
        display.println(analogRead(TAPE_R));
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

        delay(200);
    }

}

void score() {
    //Close grabber
    commandGrabber(CLOSE_L, CLOSE_R);
    delay(300);  
    
    pwm_start(ARM, 100, ARM_UP, MICROSEC_COMPARE_FORMAT);
    delay(450);
    
    commandGrabber(OPEN_L, OPEN_R);
    delay(200);

    pwm_start(ARM, 100, ARM_DOWN, MICROSEC_COMPARE_FORMAT);
    delay(300);
}


/*
*
* Lowest level, send signal to motors
*
*/

void commandGrabber(int valueL, int valueR) {
    pwm_start(GRAB_L, 100, valueL, MICROSEC_COMPARE_FORMAT);
    pwm_start(GRAB_R, 100, valueR, MICROSEC_COMPARE_FORMAT);
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

