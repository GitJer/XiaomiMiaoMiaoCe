/*

This code is for controlling the e-ink screen of the 
Xiaomi Mijia MiaoMiaoCe Bluetooth Thermometer and Hygrometer Sensor 

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