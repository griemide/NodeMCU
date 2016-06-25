/*
 * SKETCH:
 *  SNTP.ino 
 * 
 *  (c) 2016, Michael Gries
 *  Creation: 2016-06-22 (based on NTPtimezone.ino)
 *  Modified: 2016-06-24 (editorial changes)
 * 
 * PREREQUISITES:
 *   uses BUILTIN_LED
 *  
 * LINKS:
 *   see https://github.com/griemide/NodeMCU/tree/master/hardware/wemosD1mini/sketchbook/SimpleDeepSleepModule/INIT.ino  
 *   see https://de.wikipedia.org/wiki/Network_Time_Protocol  for application layer
 *   see https://tools.ietf.org/html/rfc4330 (RFC 4330 for SNTP IPv4)
 *   see https://de.wikipedia.org/wiki/User_Datagram_Protocol for transport layer
 *  
 * ISSUES:
 *   none
 *   
 */


#include <ESP8266mDNS.h>  // for UDP
#include <TimeLib.h>      // by Paul Stoffregen, not included in the Arduino IDE !!!
#include <Timezone.h>     // by Jack Christensen, not included in the Arduino IDE !!!

//// DECLARATIONS
WiFiUDP        UDP;             // ESP8266mDNS.h
unsigned int   localPort = 123; // UDP port number assigned by the IANA to NTP is 123 (see RFC 4330)
char           ntpServerName1[] = "ntp1.t-online.de";
char           ntpServerName2[] = "time.nist.gov";
const int      NTP_PACKET_SIZE = 48;  // NTP time is in the first 48 bytes of message
byte           packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming &amp; outgoing packets
time_t         utc;
time_t         local;
TimeChangeRule *tcr;             // pointer to the time change rule, use to get the TZ abbrev
TimeChangeRule CEST = { "CEST", Last, Sun, Mar, 2, 120 };     //Central European Summer Time
TimeChangeRule CET = { "CET ", Last, Sun, Oct, 3, 60 };       //Central European Standard Time
Timezone       CE(CEST, CET);



void sendNTPpacket(IPAddress &address) // send an NTP request to the time server at the given address
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
               // 8 bytes of zero for Root Delay &amp; Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  UDP.beginPacket(address, 123); //NTP requests are to port 123
  UDP.write(packetBuffer, NTP_PACKET_SIZE);
  UDP.endPacket();
}


bool getNtpTime(char* ntpServerName)
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (UDP.parsePacket() > 0); // discard any previously received packets
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 2500) {
    int size = UDP.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      UDP.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900  = (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      setTime(secsSince1900 - 2208988800UL);
      //setTime(23, 55, 0, 30, 3, 2016); //simulate time for test
      return true;
    }
  }
    return false; // return 0 if unable to get the time
}

void GetTimeNTPServer()
{
  Serial.print(__func__); Serial.print(": starting UDP service on port ");
  UDP.begin(localPort);
  Serial.print(UDP.localPort()); Serial.println(" (for NTP) ...");
  if (!getNtpTime(ntpServerName1)) { 
    Serial.print(__func__); Serial.println(": no response by NTP service - try next server");
    getNtpTime(ntpServerName2); 
  }
  local = CE.toLocal(now(), &tcr);
  Serial.print(__func__); Serial.print(": Get time from NTP server (");   Serial.print(ntpServerName1); Serial.println(") ...");
  char sTimestamp[50]; 
  sprintf(sTimestamp, "NTP: %04d-%02d-%02d %02d:%02d:%02d UTC (%d)", year(), month(), day(), hour(), minute(), second(), now() );
  Serial.print(__func__); Serial.print(": ");  Serial.println(sTimestamp); 
}

