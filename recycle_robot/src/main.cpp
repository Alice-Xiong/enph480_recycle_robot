#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define DEBUG 0
#define MODE_SWITCH 1

// Arm and servos pins
#define GRAB_L PB_9
#define GRAB_R PB_8
#define ARM PB_6
#define CAN_SENSE PA_1

// Arm and servos values
#define ARM_PWM_FREQ 50
#define CAN_THRES 100
#define CLOSE_L 2700
#define CLOSE_R 300
#define OPEN_L 2100
#define OPEN_R 900
#define ARM_UP 2600
#define ARM_DOWN 1300

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
#define DRIVE_PWM_FREQ 2000

//Tape following
#define FAST 900
#define REG 850
#define SLOW 750

// OLED definitions
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 	-1 // This display does not have a reset pin accessible
#define OLED_SDA PB_11
#define OLED_SCL PB_10

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Function prototypes
void entertainment();
void score();
void engageBox(int time);
void lineFollow(float mod = 1);
void delayLineFollow(int duration);
void displayLineSensors();
void drive(int speedL, int speedR); 
void commandGrabber(int valueL, int valueR);

// Global variables for tape following
bool onTapeL=true, onTapeR=true, lastL=true, lastR=true;


void setup() {
    // OLED different SDA and SCL pins
    Wire.setSCL(OLED_SCL);
    Wire.setSDA(OLED_SDA);

    // mode switch
    pinMode(MODE_SWITCH, INPUT);

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
    displayLineSensors();
    delay(2500);
}


void loop() {
    if (MODE_SWITCH) {
        //Engage box
        commandGrabber(OPEN_L, OPEN_R);
        delay(200);
        engageBox(1000);
      

        while (1)
        {
            if (analogRead(CAN_SENSE) < CAN_THRES) {
                score();
            }        
            lineFollow();
        }
    } else {
        entertainment();
    }

}

void entertainment() {
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

/*
*
* Important motions
*
*/
void engageBox(int time) {
    double start_time = getCurrentMillis();
    while (getCurrentMillis() - start_time < time) {
        drive(-FAST, -FAST);
        lastL = analogRead(TAPE_L) > THRES_L;
        lastR = analogRead(TAPE_R) > THRES_R;
    }
}

void lineFollow(float mod){
    // Tape following
    onTapeL = analogRead(TAPE_L) > THRES_L;
    onTapeR = analogRead(TAPE_R) > THRES_R;

    if (onTapeL || onTapeR) {
        // Either sensor on tape
        if (onTapeL && onTapeR) {
            drive(FAST * mod, FAST * mod); 
        } else if (!onTapeL && onTapeR)
        {
            drive(FAST * mod, SLOW * mod);
        } else if (onTapeL && !onTapeR)
        {
            drive(SLOW * mod, FAST * mod);
        }
        lastL = onTapeL;
        lastR = onTapeR;
    }
    else {
        // Both sensors off tape
        // Not updating last values until one sensor is on tape again
        // lastL is last time left sensor was on tape..
        if (lastL) {
            drive(-REG * mod, FAST * mod);
        } else if (lastR)
        {
            drive(FAST * mod, -REG * mod);
        } else
        {
            drive(SLOW * mod, SLOW * mod);
        }  
    }

    // print stuff, only on debug 
    if (DEBUG) {
        displayLineSensors();
    }
}

// Functions like 3, but runs line following in the background
void delayLineFollow(int duration) {
    double startTime = getCurrentMillis();

    while (getCurrentMillis() - startTime < duration) {
        lineFollow(0.75);
    }
}

void score() {
    // Close grabber
    commandGrabber(CLOSE_L, CLOSE_R);
    delayLineFollow(300);  
    
    // Raise arm
    pwm_start(ARM, 100, ARM_UP, MICROSEC_COMPARE_FORMAT);
    delayLineFollow(450);
    
    // Open grabber
    commandGrabber(OPEN_L, OPEN_R);
    delayLineFollow(200);

    // Lower arm
    pwm_start(ARM, 100, ARM_DOWN, MICROSEC_COMPARE_FORMAT);
    delayLineFollow(300);
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

void drive(int speedL, int speedR) {
    if (speedL >= 0) {
        pwm_start(ML_F, DRIVE_PWM_FREQ, speedL, RESOLUTION_10B_COMPARE_FORMAT);
        pwm_start(ML_B, DRIVE_PWM_FREQ, 0, RESOLUTION_10B_COMPARE_FORMAT);
    } else {
        pwm_start(ML_F, DRIVE_PWM_FREQ, 0, RESOLUTION_10B_COMPARE_FORMAT);
        pwm_start(ML_B, DRIVE_PWM_FREQ, -speedL, RESOLUTION_10B_COMPARE_FORMAT);
    }
    
    if (speedR >= 0) {
        pwm_start(MR_F, DRIVE_PWM_FREQ, speedR, RESOLUTION_10B_COMPARE_FORMAT);
        pwm_start(MR_B, DRIVE_PWM_FREQ, 0, RESOLUTION_10B_COMPARE_FORMAT);    
    } else {
        pwm_start(MR_F, DRIVE_PWM_FREQ, 0, RESOLUTION_10B_COMPARE_FORMAT);
        pwm_start(MR_B, DRIVE_PWM_FREQ, -speedR, RESOLUTION_10B_COMPARE_FORMAT);
    }
}

void displayLineSensors() {
    display.clearDisplay();
    display.setCursor(0,0);
    if (MODE_SWITCH) {
        display.println("GAME MODE");
    } else {
        display.println("ENTERTAINMENT");
    }
    if (analogRead(TAPE_L) > THRES_L) {
        display.println("LEFT ON");
    } else {
        display.println("LEFT OFF");
    }
    if (analogRead(TAPE_R) > THRES_R) {
        display.println("RIGHT ON");
    } else {
        display.println("RIGHT OFF");
    }
    if (analogRead(CAN_SENSE) < CAN_THRES) {
        display.println("CAN ");
        
    } else {
        display.println("NO CAN");
    }
    display.display();

    delay(200);
}


void displayCAN(){
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

