/* M. Gries 2016-05-16 */

/* 
 *  Design Specification
 *  Based on ArduinoOTA/WebUpdater for OTA Uploads of any *.bin file the target application will be added here too
 *  an OTA uploader functionality  still remains
 *  so another target application can be uploaded later on or an update of the following target application:
 *  
 *  Target Application: FADE
 *  source code copied from original Arduino examples 01.Basics/Fade.ino
 *  
 */

//INCLUDES
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <TimeLib.h>  //by Paul Stoffregen, not included in the Arduino IDE !!!
#include <Timezone.h> //by Jack Christensen, not included in the Arduino IDE !!!

//DECLARATIONS
const int ledPin = BUILTIN_LED;  // the onboard LED

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


ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
void build();
void wifi();
void ntp();
void ota();
void esp();
void fade();
void sendNTPpacket(IPAddress &address);
bool getNtpTime(char* ntpServerName);
void printTime(time_t t);

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

time_t local;




//SETUP
void setup(void){
  Serial.begin(74880); /* 2016-05-15 modified from 115200 to 74880*/
  Serial.println("Setup: ");
  Serial.println(" configuring Wifi, OTA, ESP, Application (Fade) ... ");
  build();
  wifi();
  ota();
  esp();
  // Target Application (Fade) Setup:
  pinMode(ledPin, OUTPUT);  // initialize onboard LED as output

    if (isConnected(30)) {
    wasConnected = true;
    DBG_OUTPUT_PORT.println(F("Starting UDP"));
    Udp.begin(localPort);
    DBG_OUTPUT_PORT.print(F("Local port: "));
    DBG_OUTPUT_PORT.println(Udp.localPort());
    DBG_OUTPUT_PORT.println(F("waiting for sync"));
  }

  Serial.println("Fade:");   
  Serial.println(" processing fading of built-in LED");
  Serial.println(" application running ...");

  
}

//LOOP
void loop(void){
  httpServer.handleClient();
  // Target Application Loop:
  fade();
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (!isConnected(10) && wasConnected) { delay(200); ESP.restart(); }
    if (!getNtpTime(ntpServerName1)) { getNtpTime(ntpServerName2); }
    local = CE.toLocal(now(), &tcr);
    printTime(local);
  }
}
