//
// Created by fcl-93 on 07-06-2017.
//

#ifndef ARDUINO_PLATFORMIO_MAIN_H
#define ARDUINO_PLATFORMIO_MAIN_H

#include <../lib/Adafruit_NeoPixel/Adafruit_NeoPixel.h>
#include <Wire.h>

const int LED_NUM = 12;
const int LED_PIN = 6;
const int HEART_BEAT_PIN = 5;

const int THRESHOLD_GREEN = 50;
const int THRESHOLD_YELLOW = 70;
const int THRESHOLD_RED = 100;

struct RGB{
    byte r;
    byte g;
    byte b;
};

union DELAY{
    unsigned int value;
    byte bytes[4];
};

union POWER{
    unsigned int value;
    byte bytes[4];
};

struct STRIP_PARAM{
    byte relayState;
    byte personNear;
    byte leds_on;        //max number of leds that can be configured
    byte ledsConfigured; //This will show how many leds have been configured this is needed because I2C buffer only allows you to send 32 bytes of data
    DELAY delay;
    boolean canStartMovement;
    RGB generalColor;
};

struct SINGLE_LED_PARAM {
    int rotation;
    int inital_position;
    int current_position;
    RGB myColor;
    boolean isSelected;
};

#endif //ARDUINO_PLATFORMIO_MAIN_H
