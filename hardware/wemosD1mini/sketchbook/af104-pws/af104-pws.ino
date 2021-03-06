/*
 * SKETCH:
 *  af104-pws.ino (main module) 
 * 
 *  (c) 2016, Michael Gries
 *  Creation: 2016-07-30 (copied from SimpleDeepSleepModule)
 *  Modified: 2016-07-31 (Personal Weather Station PWS for weatherstation.wunderground.com)
 *  Modified: 2016-08-03 (outdoor temperature support added)
 *  Modified: 2016-08-14 (Timer-master Library added)
 *  Modified: 2016-08-15 (OTA functionality added)
 *  Modified: 2016-08-19 (Dweet IO Reporting modified)
 *  Modified: 2016-08-29 (Telnet support added)
 *  Modified: 2016-08-30 (Serial logging support added)
 *  Modified: 2016-09-01 (Simple Mail Transfer Protocol added)
 *  Modified: 2016-10-24 (email content improved)
 *  Modified: 2017-02-04 (INIT corrected regarding IDE version printing)
 *  Modified: 2017-02-05 (JSON decoding module added for PWSM.ino)
 *  
 * MODULES:
 *   DWIO.ino version 16.8.19  (DWeet.IO data monitoring)
 *   EDSM.ino version 16.6.25  (Esp8266 Deep Sleep Mode)
 *   INIT.ino version 17.2.4   (INITialze hardware)
 *   JSON.ino version 17.2.5   (GET request to Arduino Yun VRC Server)
 *   OTAU.ino version 16.8.19  (Over The Air Update)
 *   PING.ino version 16.10.23 (PING clients and hosts)
 *   PWSM.ino version 17.2.5   (Personal Weather Station Messaging)
 *   SLOG.ino version 16.8.20  (Serial Log support)
 *   SMTP.ino version 17.2.5   (E-Mail support added 16.10.23)
 *   SNTP.ino version 16.6.27  (Simple Network Time Protocol)
 *   TNET.ino version 16.8.29  (Telnet support module)
 *   WLAN.ino version 16.10.24 (connect to local Wireless LAN) 
 * 
 * PREREQUISITES:
 *   ESP8266 based hardware (NodeMCU or Wemos D1 or Wemos D1 mini)
 *  
 * LINKS:
 *   see https://github.com/griemide/NodeMCU/tree/master/hardware/wemosD1mini/sketchbook/af104-pws
 *   see http://playground.arduino.cc/Code/Timer
 *   
 * 
 * LIBRARYS:
 *   see https://www.arduino.cc/en/Guide/Libraries
 *   see Contributed libraries: C:\...\Arduino\Sketchbook\libraries
 *   see https://github.com/JChristensen/Timer (Timer-Master.zip)
 * 
 * ISSUES:
 *   none
 *   
 * WISHLIST:
 *   see https://github.com/tzapu/WiFiManager
 *   
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <Event.h>
#include <Timer.h>

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
Timer timer;   

////// DECLARATIONS
char      FilenameWithPath[] = __FILE__;
char*     sketchName;
String    IP; // WLAN.ino, STMP.ino
String    PWStempAF104; // stored value received by Arduino Yun AF104 server in JSON.ino
bool      PWStempValid; // evaluated in JSON.ino 
const int LED_BRIGHTNESS_LOW      = 1023 - 23 ;   // 0..1023 (0=full)
const int LED_BRIGHTNESS_HIGH     = 512 + 256 ;   // 0..1023 (0=full)
const int LED_BRIGHTNESS_FULL     = 0;            // 0..1023 (0=full)

String FileCompiled;
int    FileCompiledUnix;

float  gfTempOutdoor = 15.5; // °Celsius for Dweet IO reporting
int    periodRunNetworkChecks = 1000 * 15;  // every 15 seconds
int    periodRunUpdatePWSdata = 1000 * 60;  // every minute
int    periodRunTelnetAliveHB = 1000 * 30;  // every five seconds
int    pingAverageLocal;
int    pingAverageRemote;
int    SecondsElapsed;
int    ipAddressDevice = 234;
char   deviceIPstring[24];
char   deviceMACaddress[18];
char   logMessageBuffer[61];
bool   telnetActive = false;  

void InitializeModule(char* sketchName); 
void ConnectingToWLAN();
void GetTimeNTPServer();
void HandleOTAUpdater();
void RunTelnetSession();
void PingLocalClients();
void PingRemoteServer();
void DweetIOmessaging();
void GetAF104tempData();
void PWSMessageUpdate();
void EspDeepSleepMode();
byte sendEmailViaSMTP(); 

void runNetworkChecks();
void runUpdatePWSdata();
void runTelnetAliveHB();

void SerialLog(String, String );
void handleTelnetServ();


//// APPLICATION SETUP

void setup() 
{
  byte b = sizeof(FilenameWithPath);
  while ( (b > 0) && (FilenameWithPath[b] != '\\')) b--;
  sketchName = &FilenameWithPath[++b];
  
  InitializeModule(sketchName); // ESP-12E based boards
  
  ConnectingToWLAN(); // Connect to local WiFi network

  HandleOTAUpdater(); // Aktivate OTA Update functionality 

  RunTelnetSession(); // activate Telnet session for serial monitoring
  
  GetTimeNTPServer(); // use NTP server time for timestamp logging

  /*
  EspDeepSleepMode(); // activate ESP-12E Deep-Sleep Wake mode
  */

  timer.every(periodRunNetworkChecks, runNetworkChecks);
  timer.every(periodRunUpdatePWSdata, runUpdatePWSdata);
  timer.every(periodRunTelnetAliveHB, runTelnetAliveHB);

  byte ret = sendEmailViaSMTP(); // for test purposes only

}

//// APPLICATION MAIN

void loop() 
{
  httpServer.handleClient();
  timer.update();
  handleTelnetServ();
}

//// APPLICATION TASKS

void runNetworkChecks()
{
  analogWrite(BUILTIN_LED, LED_BRIGHTNESS_HIGH);
  PingLocalClients(); // monitor reachability of local network clients 
  PingRemoteServer(); // monitor reachability of remote ftp server
  DweetIOmessaging(); // sharing Internet of Things data to server
  analogWrite(BUILTIN_LED, LED_BRIGHTNESS_LOW);
}

void runUpdatePWSdata()
{
  analogWrite(BUILTIN_LED, LED_BRIGHTNESS_FULL);
  GetAF104tempData(); // request AUSSENTEMPERATUR of AF104 Arduino Yun server
  PWSMessageUpdate(); // sent Personal Weather Station PWS) Message to WU Server 
  analogWrite(BUILTIN_LED, LED_BRIGHTNESS_LOW);
}



