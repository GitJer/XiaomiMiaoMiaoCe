/*
 * Clock for the ESP8266 with the XiaomiMiaoMiaoCe e-ink display
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "XiaomiMiaoMiaoCe.h"

XiaomiMiaoMiaoCe my_display;

// ============================================================= CLOCK SETTINGS
// change the clock once every SECONDS_DEEP_SLEEP seconds
#define SECONDS_DEEP_SLEEP 60
// update via NTP once every DO_NTP_UPDATE seconds (typically an hour)
#define DO_NTP_UPDATE 3600
// NTP Server
static const char ntpServerName[] = "nl.pool.ntp.org";
// Central European Time
const int timeZone = 2;
// function prototypes
uint32_t getNtpTime();
void sendNTPpacket(IPAddress &address);

// ============================================================== WIFI SETTINGS
// wifi settings
const char ssid[] = "**********"; // your network SSID (name)
const char pass[] = "**********"; // your network password
// UDP settings
WiFiUDP Udp;
unsigned int localPort = 8888; // local port to listen for UDP packets

// ============================= STRUCT TO SAVE VARIABLES TO SURVIVE DEEP-SLEEP
struct
{
  int seconds = 0; // current time
  int minutes = 0;
  int hours = 0;
  int secs_since_NTP = 0; // the number of seconds since the last NTP update
  int valid_time = 0;     // indicate if the NTP update was unsuccessful
} rtc_data;

// ============================================== FUNCTION TO GET TIME FROM NTP

void do_NTP_update()
{ // Get the time via NTP
  // TODO: no compensation for things like summer/winter-time,
  // so there may be issues!

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
      rtc_data.seconds += 50; // 500ms delay * 100 tries = 50 seconds
      return;
    }
  }

  // start UDP
  Udp.begin(localPort);

  // get the time in seconds since 1970
  uint32_t my_time = getNtpTime();
  if (my_time > 0)
  { // there was a valid response from the NTP server

    // calculate time from seconds since 1970,
    // here we only need the time of day

    // the seconds since the start of today
    uint32_t seconds_today = my_time % 86400;
    uint32_t hours = seconds_today / 3600;
    uint32_t minutes = (seconds_today % 3600) / 60;
    uint32_t seconds = seconds_today % 60;

    // store the valid time in the appropriate variables,
    // and reset the time since last update
    rtc_data.hours = hours;
    rtc_data.minutes = minutes;
    rtc_data.seconds = seconds;
    rtc_data.valid_time = 1;
    rtc_data.secs_since_NTP = 0;
  }
  else
  { // the NTP server did not respond
    rtc_data.valid_time = 0;
  }
}

void update_display()
{ // display the time

  // start building a new screen (non-inverted)
  my_display.start_new_screen(0);

  // the minutes on the bottom row
  my_display.set_number(rtc_data.minutes % 10, BOTTOM_RIGHT);
  my_display.set_number(rtc_data.minutes / 10, BOTTOM_LEFT);
  // the hours on the top row
  my_display.set_number(rtc_data.hours % 10, TOP_MIDDLE);
  my_display.set_number(rtc_data.hours / 10, TOP_LEFT);
  // the smiley/frowny indicates if the time is valid
  // i.e. the latest NTP update was successful
  if (rtc_data.valid_time == 0)
    my_display.set_shape(FACE_FROWN);
  else
    my_display.set_shape(FACE_SMILE);
  // write the screen to the display
  my_display.write_display();
}

void correct_s_m_h()
{ // make sure that seconds and minutes are [0, 59] and hours [0, 23]

  if (rtc_data.seconds >= 60)
  {
    rtc_data.minutes++;
    rtc_data.seconds %= 60;
    if (rtc_data.minutes >= 60)
    {
      rtc_data.hours++;
      rtc_data.minutes %= 60;
      if (rtc_data.hours >= 24)
      {
        rtc_data.hours = 0;
      }
    }
  }
}

void setup()
{ // get the reason for the wake-up
  String reset_reason = ESP.getResetReason();
  if (reset_reason != "Deep-Sleep Wake")
  { // if it is not a deep sleep wake, then initialize

    // initialize the e-ink display and do a redraw
    my_display.init(1);
    // init time variables
    rtc_data.seconds = 0;
    rtc_data.minutes = 0;
    rtc_data.hours = 0;
    rtc_data.secs_since_NTP = DO_NTP_UPDATE;
    // rtc_data.delta_t = SECONDS_DEEP_SLEEP;
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
    rtc_data.seconds += SECONDS_DEEP_SLEEP;
    rtc_data.secs_since_NTP += SECONDS_DEEP_SLEEP;
    if (rtc_data.secs_since_NTP > DO_NTP_UPDATE)
      // get the current time via NTP
      do_NTP_update();
    else
      // make sure that seconds and minutes are [0, 59] and hours [0, 23]
      correct_s_m_h();

    // initialize the e-ink display without redraw
    my_display.init(0);
    // write variables that need to survive deep-sleep to rtc memory
    ESP.rtcUserMemoryWrite(0, (uint32_t *)&rtc_data, sizeof(rtc_data));
  }

  // display the current time (hours, minutes) on the display
  update_display();

  // calculate the change in seconds to make the deep-sleep
  // wake up on a whole minute
  uint32_t seconds_to_add = 60 - rtc_data.seconds;
  if ((seconds_to_add < 30) and (seconds_to_add > 1))
  { // wake up at the start of the next minute
    rtc_data.seconds = 0;
    rtc_data.minutes += 1;
    // make sure that seconds and minutes are [0, 59] and hours [0, 23]
    correct_s_m_h();
    // write variables that need to survive deep-sleep to rtc memory
    ESP.rtcUserMemoryWrite(0, (uint32_t *)&rtc_data, sizeof(rtc_data));
    // go to sleep
    ESP.deepSleep((SECONDS_DEEP_SLEEP + seconds_to_add) * 1e6, WAKE_RF_DEFAULT);
  }
  else if ((seconds_to_add < 59) and (seconds_to_add > 1))
  { // wake up at the start of the minute
    rtc_data.seconds = 0;
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

/*-------- NTP code ----------*/
// https://github.com/PaulStoffregen/Time

const int NTP_PACKET_SIZE = 48;     // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

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
      return secsSince1900 - 2208988800UL + timeZone * 3600; // 3600 = SECS_PER_HOUR
    }
  }
  return 0; // return 0 if unable to get the time
}

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