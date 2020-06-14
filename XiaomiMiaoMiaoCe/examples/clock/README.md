Clock
-----

This example is a simple, but nifty, clock using the e-ink display of the Xiaomi Miao Miao CE.

It uses an ESP8266 (Wemos D1 mini clone) to get the time via NTP and control the display. The clock has the following features:

-   Deep sleep
-   Sleeps for one minute (can be adjusted)
-   Refreshes the screen at the start of a minute
-   Does NTP synchronization every hour (can be adjusted). The code is taken from [here](https://github.com/PaulStoffregen/Time)
-   Has adjustable time-zone (the setenv statement, see e.g. [here](https://remotemonitoringsystems.ca/time-zone-abbreviations.php)), and NTP server
-   Continuous in case of wifi connectivity problems
-   Gives an indication if the displayed time is valid or not (this is actually not completely true: if the power fails the e-ink will happily keep on displaying the last update)
