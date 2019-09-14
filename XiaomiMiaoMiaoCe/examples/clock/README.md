Clock
-----

This example is a simple, but nifty, clock using the e-ink display of the Xiaomi Miao Miao CE.

It uses an ESP8266 (Wemos D1 mini clone) to get the time via NTP and control the display. The clock has the following features:

-   Deep sleep
-   Sleeps for one minute (can be adjusted)
-   Refreshes the screen at the start of a minute
-   Does NTP synchronization every hour (can be adjusted). The code is taken from [here](https://github.com/PaulStoffregen/Time)
-   Has adjustable time-zone and NTP server
-   Gives an indication if the displayed time is valid or not (this is actually not completely true: if the power fails the e-ink will happily keep on displaying the last update)

But: it does not have summer/winter time.

You need to do the following things:
- Do not forget to fill in your wifi credentials!
- You should also check if there is a NTP server near you, change that as well in the code.**
- To wake up from deep-sleep, put a jumper wire between D0 and RST
- You need to reset the ESP8266 after flashing it
- A full reset is pressing the reset button on the ESP8266 twice
