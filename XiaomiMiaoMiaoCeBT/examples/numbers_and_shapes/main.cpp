/*

This code is for controlling the e-ink screen of the 
Xiaomi Mijia MiaoMiaoCe Bluetooth Thermometer and Hygrometer Sensor 

*/

#include "XiaomiMiaoMiaoCeBT.h"

uint8_t all_segments[18] = {0x0c, 0x92, 0x49, 0x24, 0x92, 0x49, 0x24, 0x92, 0x64, 0x92, 0x49, 0x24, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff};

XiaomiMiaoMiaoCeBT my_display;

void setup()
{
    // allow serial printing
    Serial.begin(115200);

    // Use built-in LED as kind of watchdog indicator...
    pinMode(LED_BUILTIN, OUTPUT);
    
#if DEBUG_SERIAL
    Serial.print("Initialising display, please wait...\r\n");
#endif

    // initialize the e-ink display
    my_display.init(1);

    // show all segments at once.
    // Original firmware does this on start-up
    my_display.write_display(all_segments);
    //delay(100);

#if DEBUG_SERIAL
    Serial.print("Display initialised. Continuing to the loop() function!\r\n\r\n");
#endif
}

// the number to be displayed
int number = 0;
int led_state = LOW;

int bytes = 0;
int bits = 0;
bool is_screen_inverted = false;

void loop()
{
    led_state = ~led_state;
    digitalWrite(LED_BUILTIN, led_state);

    // start drawing a new screen
    my_display.start_new_screen(is_screen_inverted);

    // display the same Hexadecimal digit on top 3 positions
    my_display.set_digit(number % 16, TOP_LEFT);
    my_display.set_digit(number % 16, TOP_MIDDLE);
    my_display.set_digit(number % 16, TOP_RIGHT);
    // display the same Decimal digit on bottom 2 positions
    my_display.set_digit(number % 10, BOTTOM_LEFT);
    my_display.set_digit(number % 10, BOTTOM_RIGHT);

    // display pre-defined shapes one at a time
    int remainder = number % 11;
    switch (remainder)
    {
		case 0: my_display.set_shape(TOP_LEFT_1); break;
		case 1: my_display.set_shape(BATTERY_LOW); break;
		case 2: my_display.set_shape(DASHES); break;
		case 3: my_display.set_shape(FACE); break;
		case 4: my_display.set_shape(FACE_SMILE); break;
		case 5: my_display.set_shape(FACE_FROWN); break;
		case 6: my_display.set_shape(FACE_NEUTRAL); break;
		case 7: my_display.set_shape(SUN); break;
		case 8: my_display.set_shape(FIXED); break;
		case 9: my_display.set_shape(FIXED_DEG_C); break;
		case 10: my_display.set_shape(FIXED_DEG_F); break;		
    }

    my_display.write_display();
    number++;

    // Re-initialise display every 16 cycles
    // and alternate between inverted / non-inverted display
    if ((number % 16) == 0)
    {
        is_screen_inverted = !is_screen_inverted;
        // NOTE: If screen is not inverted from time to time,
        // white segments become visible
        // To avoid this, re-initialise the display with redraw
        // after 15-20 partial update cycles:
        //my_display.init(1);
    }

    // wait some time (5 seconds)
    delay(5000);
}