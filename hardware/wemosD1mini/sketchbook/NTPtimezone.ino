
/*
ESP8266 : NTP Time + Timezone
Created 07 May 2016 by Ralf Bohnen - www.Arduinoclub.de
This example code is in the public domain.
*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <TimeLib.h>  //by Paul Stoffregen, not included in the Arduino IDE !!!
#include <Timezone.h> //by Jack Christensen, not included in the Arduino IDE !!!

const char WiFiSSID[] = "HP LaserJet P1505n";     //### your Router SSID
const char WiFiPSK[]  = "vQ1tASKOUu"; //### your Router Password

#define DBG_OUTPUT_PORT Serial

unsigned long previousMillis = 0;
const long interval = 1000;
volatile bool wasConnected = false;

bool isConnected(long timeOutSec) {
	timeOutSec = timeOutSec * 1000;
	int z = 0;
	while (WiFi.status() != WL_CONNECTED) {
		delay(200);
		DBG_OUTPUT_PORT.print(".");
		if (z == timeOutSec / 200) { return false; }
		z++;
	}
	return true;
}

//### NTP, TIME &amp; TIMEZONE ###################################################################


//UDP
WiFiUDP Udp;
unsigned int localPort = 123;

//NTP Server
char ntpServerName1[] = "ntp1.t-online.de";
char ntpServerName2[] = "time.nist.gov";

//Timezone
//Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = { "CEST", Last, Sun, Mar, 2, 120 };     //Central European Summer Time
TimeChangeRule CET = { "CET ", Last, Sun, Oct, 3, 60 };       //Central European Standard Time
Timezone CE(CEST, CET);
TimeChangeRule *tcr;        //pointer to the time change rule, use to get the TZ abbrev
time_t utc, local;


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
	DBG_OUTPUT_PORT.print(F("NTP request..."));
	if (timeStatus() == timeSet) {
		DBG_OUTPUT_PORT.println(F("not necessary"));
		return true;
	}

	IPAddress ntpServerIP; // NTP server's ip address

	while (Udp.parsePacket() > 0); // discard any previously received packets
	DBG_OUTPUT_PORT.println(F("Transmit NTP Request"));
	// get a random server from the pool
	WiFi.hostByName(ntpServerName, ntpServerIP);
	DBG_OUTPUT_PORT.print(ntpServerName);
	DBG_OUTPUT_PORT.print(": ");
	DBG_OUTPUT_PORT.println(ntpServerIP);
	sendNTPpacket(ntpServerIP);
	uint32_t beginWait = millis();
	while (millis() - beginWait < 1500) {
		int size = Udp.parsePacket();
		if (size >= NTP_PACKET_SIZE) {
			DBG_OUTPUT_PORT.println(F("Receive NTP Response"));
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
	DBG_OUTPUT_PORT.println(F("FATAL ERROR : No NTP Response."));
	return false; // return 0 if unable to get the time
}


//Function to return the compile date and time as a time_t value
time_t compileTime(void)
{
#define FUDGE 25        //fudge factor to allow for compile time (seconds, YMMV)
	char *compDate = __DATE__, *compTime = __TIME__, *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
	char chMon[3], *m;
	int d, y;
	tmElements_t tm;
	time_t t;
	strncpy(chMon, compDate, 3);
	chMon[3] = '\0';
	m = strstr(months, chMon);
	tm.Month = ((m - months) / 3 + 1);
	tm.Day = atoi(compDate + 4);
	tm.Year = atoi(compDate + 7) - 1970;
	tm.Hour = atoi(compTime);
	tm.Minute = atoi(compTime + 3);
	tm.Second = atoi(compTime + 6);
	t = makeTime(tm);
	return t + FUDGE;        //add fudge factor to allow for compile time
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

//############################################################################################

void setup()
{
	DBG_OUTPUT_PORT.begin(76400);
	DBG_OUTPUT_PORT.setDebugOutput(true);
	DBG_OUTPUT_PORT.println(F("ArduinoClub-NTP-Timezone"));

	WiFi.mode(WIFI_STA);
	WiFi.begin(WiFiSSID, WiFiPSK);

	if (isConnected(30)) {
		wasConnected = true;
		DBG_OUTPUT_PORT.println(F("Starting UDP"));
		Udp.begin(localPort);
		DBG_OUTPUT_PORT.print(F("Local port: "));
		DBG_OUTPUT_PORT.println(Udp.localPort());
		DBG_OUTPUT_PORT.println(F("waiting for sync"));
	}


    /* add setup code here */

}

void loop()
{
	unsigned long currentMillis = millis();
	if (currentMillis - previousMillis >= interval) {
		previousMillis = currentMillis;
		if (!isConnected(10) && wasConnected) { delay(200); ESP.restart(); }
		if (!getNtpTime(ntpServerName1)) { getNtpTime(ntpServerName2); }
		local = CE.toLocal(now(), &tcr);
		printTime(local);
	}

	/* add main program code here */

}
