/*
 * Clock for the ESP8266 with the MHO-C401 e-ink display
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "XiaomiMiaoMiaoCeBT.h"
#include <time.h>

XiaomiMiaoMiaoCeBT my_display;

// ============================================================= CLOCK SETTINGS
// change the clock once every SECONDS_DEEP_SLEEP seconds
#define SECONDS_DEEP_SLEEP 10
// update via NTP once every DO_NTP_UPDATE seconds (typically an hour)
#define DO_NTP_UPDATE 3600
// NTP Server
static const char ntpServerName[] = "uk.pool.ntp.org";
// Central European Time
const int timeZone = 0;
// function prototypes

// ============================================================== WIFI SETTINGS
// wifi settings
const char ssid[] = "Your SSID";   // your network SSID (name)
const char pass[] = "Your passwd"; // your network password
// UDP settings
WiFiUDP Udp;
unsigned int localPort = 8888; // local port to listen for UDP packets

// ============================= STRUCT TO SAVE VARIABLES TO SURVIVE DEEP-SLEEP
struct
{
  time_t unix_time;
  int secs_since_NTP = 0; // the number of seconds since the last NTP update
  int valid_time = 0;     // indicate if the NTP update was unsuccessful
} rtc_data;

// ============================================== FUNCTION TO GET TIME FROM NTP
// part of this comes from: https://github.com/PaulStoffregen/Time

const int NTP_PACKET_SIZE = 48;     // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011; // LI, Version, Mode
  packetBuffer[1] = 0;          // Stratum, or type of clock
  packetBuffer[2] = 6;          // Polling Interval
  packetBuffer[3] = 0xEC;       // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

uint32_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0)
    ; // discard any previously received packets
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500)
  {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE)
    {
      Udp.read(packetBuffer, NTP_PACKET_SIZE); // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 = (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];

      // 2208988800UL = the 70 years from 1900 to 1970
      rtc_data.unix_time = secsSince1900 - 2208988800UL;

      return 1; // valid response
    }
  }
  return 0; // return 0 if unable to get the time
}

void do_NTP_update()
{ // Get the time via NTP

  // start wifi
  WiFi.begin(ssid, pass);
  // check if connected
  int num_of_tries = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    num_of_tries++;
    if (num_of_tries > 100) // allow 100 retries
    {
      rtc_data.valid_time = 0;
      // failing costs time -> add that to the seconds
      rtc_data.unix_time += 50; // 500ms delay * 100 tries = 50 seconds
      return;
    }
  }

  // start UDP
  Udp.begin(localPort);

  // get the time in seconds since 1970
  uint32_t my_time = getNtpTime();
  if (my_time == 1)
  { // there was a valid response from the NTP server
    rtc_data.valid_time = 1;
    rtc_data.secs_since_NTP = 0;
  }
  else
  { // the NTP server did not respond
    rtc_data.valid_time = 0;
  }
}

// =========================================================== DISPLAY SETTINGS

void update_display()
{ // display the time
  tm *normal_time;
  normal_time = localtime(&rtc_data.unix_time);

  // start building a new screen (0 - normal, 1 - inverted)
  my_display.start_new_screen(0);
  

  // the 10th of the seconds on the top row
  my_display.set_digit(normal_time->tm_sec / 10, TOP_RIGHT);
  // the minutes on the bottom row
  my_display.set_digit(normal_time->tm_min % 10, BOTTOM_RIGHT);
  my_display.set_digit(normal_time->tm_min / 10, BOTTOM_LEFT);
  // the hours on the top row
  my_display.set_digit(normal_time->tm_hour % 10, TOP_MIDDLE);
  my_display.set_digit(normal_time->tm_hour / 10, TOP_LEFT);
  // the smiley/frowny indicates if the time is valid
  // i.e. the latest NTP update was successful
  if (rtc_data.valid_time == 0)
    my_display.set_shape(FACE_FROWN);
  else
    my_display.set_shape(FACE_SMILE);
  // write the screen to the display
  my_display.write_display();
}

// ============================================================= SETUP AND LOOP

void setup()
{ // get the reason for the wake-up
  String reset_reason = ESP.getResetReason();
  if (reset_reason != "Deep-Sleep Wake")
  { // if it is not a deep sleep wake, then initialize

    // initialize the e-ink display and do a redraw
    my_display.init(1);
    // init time variables
    rtc_data.unix_time = 0;
    rtc_data.secs_since_NTP = DO_NTP_UPDATE;
    rtc_data.valid_time = 0;
    // get the current time via NTP
    do_NTP_update();
    // write variables that need to survive deep-sleep to rtc memory
    ESP.rtcUserMemoryWrite(0, (uint32_t *)&rtc_data, sizeof(rtc_data));
  }
  else
  { // it is a wake-up from deep-sleep

    // read variables from rtc memory
    ESP.rtcUserMemoryRead(0, (uint32_t *)&rtc_data, sizeof(rtc_data));

    // is it time for a NTP update? If not: continue with current time values
    rtc_data.unix_time += SECONDS_DEEP_SLEEP;
    rtc_data.secs_since_NTP += SECONDS_DEEP_SLEEP;
    if (rtc_data.secs_since_NTP > DO_NTP_UPDATE)
      // get the current time via NTP
      do_NTP_update();

    // initialize the e-ink display without redraw
    my_display.init(0);
    // write variables that need to survive deep-sleep to rtc memory
    ESP.rtcUserMemoryWrite(0, (uint32_t *)&rtc_data, sizeof(rtc_data));
  }

  // Set timezone to British DST
  setenv("TZ","GMTGMT-1,M3.4.0/01,M10.4.0/02",1);
  tzset();

  // display the current time (hours, minutes) on the display
  update_display();

  // calculate the change in seconds to make the deep-sleep
  // wake up on a whole minute
  tm *normal_time;
  normal_time = localtime(&rtc_data.unix_time);

  uint32_t seconds_to_add = SECONDS_DEEP_SLEEP - normal_time->tm_sec;
  if ((seconds_to_add < 30) and (seconds_to_add > 1))
  {
    normal_time->tm_sec = 0;
    normal_time->tm_min += 1;
    rtc_data.unix_time = mktime(normal_time);
    // write variables that need to survive deep-sleep to rtc memory
    ESP.rtcUserMemoryWrite(0, (uint32_t *)&rtc_data, sizeof(rtc_data));
    // go to sleep
    ESP.deepSleep((SECONDS_DEEP_SLEEP + seconds_to_add) * 1e6, WAKE_RF_DEFAULT);
  }
  else if ((seconds_to_add < 59) and (seconds_to_add > 1))
  {
    normal_time->tm_sec = 0;
    rtc_data.unix_time = mktime(normal_time);

    // write variables that need to survive deep-sleep to rtc memory
    ESP.rtcUserMemoryWrite(0, (uint32_t *)&rtc_data, sizeof(rtc_data));
    // go to sleep
    ESP.deepSleep((seconds_to_add)*1e6, WAKE_RF_DEFAULT);
  }
  else
    // go to sleep
    // Note: indicate that the radio should be started at wake-up
    // If WAKE_RF_DISABLED is used, you can never start the radio again
    // ... weird!
    ESP.deepSleep(SECONDS_DEEP_SLEEP * 1e6, WAKE_RF_DEFAULT);
}

// not needed
void loop()
{
}