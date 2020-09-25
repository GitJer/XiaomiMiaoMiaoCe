/*

This code is for controlling the e-ink screen of the 
Xiaomi Mijia MiaoMiaoCe Bluetooth Thermometer and Hygrometer Sensor 

*/

#include "XiaomiMiaoMiaoCe.h"

uint8_t all_segments[18] = {0x0c, 0x92, 0x49, 0x24, 0x92, 0x49, 0x24, 0x92, 0x64, 0x92, 0x49, 0x24, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff};


XiaomiMiaoMiaoCe my_display;

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

    Serial.printf("Setting segment #%d (%d, %d) to 1\r\n", number, bytes, bits);

    my_display.start_new_screen(is_screen_inverted);
    my_display.set_segment(bytes, bits, 1);

    // display the segment
    my_display.write_display();
    // increase the number of the segment
    number++;

    // Cycle through all bytes and bits
    bits++;
    if (bits > 7)
    {
        bytes++;    
        bits = 0;
    }
    if (bytes > 17)
    {
        bytes = 0;
		number = 0;
		
		// Alternate between inverted / non-inverted screen
        is_screen_inverted = !is_screen_inverted;

		// Make a full display refresh
        my_display.init(1);
    }

    // wait some time 3 seconds)
    delay(3000);
}