Clock
-----

This example is a simple, but nifty, clock using the e-ink display of the Xiaomi Miao Miao CE.

It uses an ESP8266 (Wemos D1 mini clone) to get the time via NTP and control the display. The clock has the following features:

-   Deep sleep
-   Sleeps for 10 seconds (can be adjusted)
-   Refreshes the screen every 10 seconds
-   Does NTP synchronization every hour (can be adjusted). The code is taken from [here](https://github.com/PaulStoffregen/Time)
-   Has adjustable time-zone (the setenv statement, see e.g. [here](https://remotemonitoringsystems.ca/time-zone-abbreviations.php)), and NTP server
-   Continuous in case of wifi connectivity problems
-   Gives an indication if the displayed time is valid or not (this is actually not completely true: if the power fails the e-ink will happily keep on displaying the last update)

In order to run this example, you have to connect D0 to RST, but only *after* uploading the firmware to the Wemos D1 mini. If this connection is not made, the ESP8266 won't get out of deep sleep.
