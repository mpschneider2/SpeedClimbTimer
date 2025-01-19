#include "SevSeg.h"
#include "OneButton.h"
SevSeg sevseg; //Initiate a seven segment controller object

unsigned long timerStart = 0;
int timer = 0;
int highScore = 9999;

int Button2 = A0;

int DisplayState = 0;
//0 is highscore, 1 is 00:00, 2 is timer, 3 is blinking time at end, 4 is blinking time NEW HIGH SCORE

OneButton Button1 = OneButton (
    A1,
    true,
    true
);

unsigned long ButtonHitMillis = 0;

void setup() {
    byte numDigits = 4;  
    // byte digitPins[] = {2, 3, 4, 5}; //Original Wiring
    byte digitPins[] = {7, 4, 3, 13}; //Custom Shield Wiring
    // byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13}; //Original Wiring
    byte segmentPins[] = {6, 2, 11, 9, 8, 5, 12, 10}; //Custom Shield Wiring
    bool resistorsOnSegments = 0;

    pinMode(Button2, INPUT_PULLUP);

    // variable above indicates that 4 resistors were placed on the digit pins.
    // set variable to 1 if you want to use 8 resistors on the segment pins.
    sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins, resistorsOnSegments);
    sevseg.setBrightness(90);

    Button1.attachPress([]() {
        if (DisplayState != 2) DisplayState = 0;
    });

    Button1.attachLongPressStart([]() {
        DisplayState = 1;
    });

    Button1.attachLongPressStop([]() {
        DisplayState = 2;
        timerStart = millis();
    });

    Button1.setPressMs(3000);
}

void runTimer() {
    timer = (millis()-timerStart)/10;
}

void loop() {
    if (DisplayState==2) {
        runTimer();
        sevseg.setNumber(timer, 2);
    } else if (DisplayState==3) {
        if (timer <= highScore) {
            highScore = timer;

            if ((millis()-ButtonHitMillis)/10%30 < 20) {
                sevseg.setNumber(timer, 2);
            } else {
                sevseg.blank();
            }

        } else {
            if ((millis()-ButtonHitMillis)/10%150 < 125) {
                sevseg.setNumber(timer, 2);
            } else {
                sevseg.blank();
            }
        }
        
    } else if (DisplayState == 0) {
        sevseg.setNumber(highScore, 2);
    } else if (DisplayState == 1) {
        timer = 0;
        sevseg.setNumber(timer, 2);
    }

    sevseg.refreshDisplay(); // Must run repeatedly

    if (digitalRead(Button2)==HIGH) {
        if (DisplayState==2) {
            DisplayState = 3;
            ButtonHitMillis = millis();
        }
    }

    Button1.tick();
}