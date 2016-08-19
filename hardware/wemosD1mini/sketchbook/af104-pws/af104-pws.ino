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
 *  
 * MODULES:
 *   DWIO.ino version 16.8.19 (DWeet.IO data monitoring)
 *   EDSM.ino version 16.6.25 (Esp8266 Deep Sleep Mode)
 *   INIT.ino version 16.7.3  (INITialze hardware)
 *   OTAU.ino version 16.8.19 (Over The Air Update)
 *   PING.ino version 16.6.23 (PING clients and hosts)
 *   PWSM.ino version 16.8.8  (Personal Weather Station Messaging)
 *   SNTP.ino version 16.6.27 (Simple Network Time Protocol)
 *   WLAN.ino version 16.8.18 (connect to local Wireless LAN) 
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
char   FilenameWithPath[] = __FILE__;
const int   LED_BRIGHTNESS_LOW      = 1023 - 23 ;   // 0..1023 (0=full)
const int   LED_BRIGHTNESS_HIGH     = 512 + 256 ;   // 0..1023 (0=full)
const int   LED_BRIGHTNESS_FULL     = 0;            // 0..1023 (0=full)

String FileCompiled;
int    FileCompiledUnix;
float  gfTempOutdoor = 15.5; // °Celsius for Dweet IO reporting
int    periodRunNetworkChecks = 1000 * 15;  // every 15 seconds
int    periodRunUpdatePWSdata = 1000 * 60;  // every minute
int    pingAverageLocal;
int    pingAverageRemote;
int    SecondsElapsed;
int    ipAddressDevice = 234;
char   deviceIPstring[24];

void InitializeModule(char* sketchName); 
void ConnectingToWLAN();
void GetTimeNTPServer();
void HandleOTAUpdater();
void PingLocalClients();
void PingRemoteServer();
void DweetIOmessaging();
void PWSMessageUpdate();
void EspDeepSleepMode();

void runNetworkChecks();
void runUpdatePWSdata();


//// APPLICATION SETUP

void setup() 
{
  byte b = sizeof(FilenameWithPath);
  while ( (b > 0) && (FilenameWithPath[b] != '\\')) b--;
  char *sketchName = &FilenameWithPath[++b];
  
  InitializeModule(sketchName); // ESP-12E based boards
  
  ConnectingToWLAN(); // Connect to local WiFi network

  HandleOTAUpdater(); // Aktivate OTA Update functionality 

  GetTimeNTPServer(); // use NTP server time for timestamp logging
  
  // EspDeepSleepMode(); // activate ESP-12E Deep-Sleep Wake mode

  timer.every(periodRunNetworkChecks, runNetworkChecks);
  timer.every(periodRunUpdatePWSdata, runUpdatePWSdata);

}

//// APPLICATION MAIN

void loop() 
{
  httpServer.handleClient();
  timer.update();
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
  PWSMessageUpdate(); // sent Personal Weather Station PWS) Message to WU Server 
  analogWrite(BUILTIN_LED, LED_BRIGHTNESS_LOW);
}

