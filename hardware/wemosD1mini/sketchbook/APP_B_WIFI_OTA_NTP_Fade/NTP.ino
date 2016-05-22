/*
 * M Gries 2016-02-20
 */

#include <TimeLib.h>  //by Paul Stoffregen, not included in the Arduino IDE !!!
#include <Timezone.h> //by Jack Christensen, not included in the Arduino IDE !!!


void ntp(void)
{
Serial.println("NTP: not yet supported");
}



//### NTP, TIME &amp; TIMEZONE ###################################################################


//UDP
//WiFiUDP Udp;
//unsigned int localPort = 123;


//Timezone
//Central European Time (Frankfurt, Paris)
time_t utc;


const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming &amp; outgoing packets

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
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
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}


bool getNtpTime(char* ntpServerName)
{
  Serial.print(F("NTP request..."));
  if (timeStatus() == timeSet) {
    Serial.println(F("not necessary"));
    return true;
  }

  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0); // discard any previously received packets
  Serial.println(F("Transmit NTP Request"));
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println(F("Receive NTP Response"));
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 = (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      setTime(secsSince1900 - 2208988800UL);
      //setTime(23, 55, 0, 30, 3, 2016); //simulate time for test
      return true;
    }
  }
  Serial.println(F("FATAL ERROR : No NTP Response."));
  return false; // return 0 if unable to get the time
}

//Print an integer in "00" format (with leading zero).
//Input value assumed to be between 0 and 99.
void sPrintI00(int val)
{
  if (val < 10) DBG_OUTPUT_PORT.print('0');
  DBG_OUTPUT_PORT.print(val, DEC);
  return;
}

//Print an integer in ":00" format (with leading zero).
//Input value assumed to be between 0 and 99.
void sPrintDigits(int val)
{
  DBG_OUTPUT_PORT.print(':');
  if (val < 10) DBG_OUTPUT_PORT.print('0');
  DBG_OUTPUT_PORT.print(val, DEC);
}

void printTime(time_t t)
{
  sPrintI00(hour(t));
  sPrintDigits(minute(t));
  sPrintDigits(second(t));
  DBG_OUTPUT_PORT.print(' ');
  DBG_OUTPUT_PORT.print(dayShortStr(weekday(t)));
  DBG_OUTPUT_PORT.print(' ');
  sPrintI00(day(t));
  DBG_OUTPUT_PORT.print(' ');
  DBG_OUTPUT_PORT.print(monthShortStr(month(t)));
  DBG_OUTPUT_PORT.print(' ');
  DBG_OUTPUT_PORT.print(year(t));
  DBG_OUTPUT_PORT.println(' ');
}
