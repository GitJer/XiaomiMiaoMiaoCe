/*

This code is for controlling the e-ink screen of the 
xiaomi mijia Mi miaomiaoce Thermometer Temperature Humidity Sensor 

Note: after a lot of playing with this display the white becomes light gray
      and the black becomes dark gray. This restores itself after some time
      without activity (i.e. switched off for some time). 

TODO: make a library.json
*/

#include <Arduino.h>
#include "XiaomiMiaoMiaoCe.h"

XiaomiMiaoMiaoCe my_display;

void setup()
{
    // allow serial printing
    Serial.begin(115200);
    // initialize the e-ink display and do a redraw
    my_display.init(1);
}

// the number to be displayed
int number = 0;

void loop()
{
    // start drawing a new screen, non-inverted
    my_display.start_new_screen(0);

    // display the same number on all 5 positions
    my_display.set_number(number % 10, TOP_LEFT);
    my_display.set_number(number % 10, TOP_MIDDLE);
    my_display.set_number(number % 10, TOP_RIGHT);
    my_display.set_number(number % 10, BOTTOM_LEFT);
    my_display.set_number(number % 10, BOTTOM_RIGHT);

    // display pre-defined shapes one at a time
    int remainder = number % 9;
    switch (remainder)
    {
    case 0:
        my_display.set_shape(TOP_LEFT_1);
        break;
    case 1:
        my_display.set_shape(BATTERY_LOW);
        break;
    case 2:
        my_display.set_shape(DASHES);
        break;
    case 3:
        my_display.set_shape(FACE);
        break;
    case 4:
        my_display.set_shape(FACE_SMILE);
        break;
    case 5:
        my_display.set_shape(FACE_FROWN);
        break;
    case 6:
        my_display.set_shape(FACE_NEUTRAL);
        break;
    case 7:
        my_display.set_shape(SUN);
        break;
    case 8:
        my_display.set_shape(FIXED);
        break;
    }

    // display the numbers and shapes
    my_display.write_display();
    // increase the number to be displayed
    number++;
    // wait some time seconds
    delay(3000);
}