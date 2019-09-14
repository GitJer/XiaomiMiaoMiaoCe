/*

This code is for controlling the e-ink screen of the 
xiaomi mijia Mi miaomiaoce Thermometer Temperature Humidity Sensor 

Note: after a lot of playing with this display the white becomes light gray
      and the black becomes dark gray. This restores itself after some time
      without activity (i.e. switched off for some time). 

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

int number = 0;

void loop()
{
    my_display.start_new_screen(number % 2);
    my_display.write_display();
    delay(3000);
    number++;
}