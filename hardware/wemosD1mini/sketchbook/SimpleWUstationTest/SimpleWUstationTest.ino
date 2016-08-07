/*
 * SKETCH:
 *  SimpleWUstationTest.ino (main module)
 * 
 *  (c) 2016, Michael Gries
 *  Creation: 2016-07-30 (copied from SimpleDeepSleepModule)
 *  Modified: 2016-07-31 (Personal Weather Station PWS for weatherstation.wunderground.com)
 *  Modified: 2016-08-03 (outdoor temperature support added)
 *  
 * MODULES:
 *   DWIO.ino version 16.7.31 (DWeet.IO data monitoring)
 *   EDSM.ino version 16.6.25 (Esp8266 Deep Sleep Mode)
 *   INIT.ino version 16.7.3  (INITialze hardware)
 *   PING.ino version 16.6.23 (PING clients and hosts)
 *   PWSM.ino version 16.8.3  (Personal Weather Station Messaging)
 *   SNTP.ino version 16.6.27 (Simple Network Time Protocol)
 *   WLAN.ino version 16.6.27 (connect to local Wireless LAN) 
 * 
 * PREREQUISITES:
 *   ESP8266 based hardware (NodeMCU or Wemos D1 mini)
 *  
 * LINKS:
 *   see https://github.com/griemide/NodeMCU/tree/master/hardware/wemosD1mini/sketchbook/SimpleDeepSleep
 * 
 * ISSUES:
 *   none
 *   
 */

///// DECLARATIONS
char  FilenameWithPath[] = __FILE__;
char* CurrentTimestamp;
float gfTempOutdoor = 15.5; // °Celsius for Dweet IO reporting

void InitializeModule(char* sketchName); 
void ConnectingToWLAN();
char* GetTimeNTPServer();
void PingLocalClients();
void PingRemoteServer();
void DweetIOmessaging();
void PWSMessageUpdate();
void EspDeepSleepMode();



//// APPLICATION 
void setup() 
{
  byte b = sizeof(FilenameWithPath);
  while ( (b > 0) && (FilenameWithPath[b] != '\\')) b--;
  char *sketchName = &FilenameWithPath[++b];
  
  InitializeModule(sketchName); // ESP-12E based boards
  
  ConnectingToWLAN(); // Connect to local WiFi network

  CurrentTimestamp = GetTimeNTPServer(); // use NTP server time for timestamp logging

  PingLocalClients(); // monitor reachability of local network clients 

  PingRemoteServer(); // monitor reachability of remote ftp server

  DweetIOmessaging(); // sharing Internet of Things data to server

  PWSMessageUpdate(); // sent Personal Weather Station PWS) Message to WU Server
  
  EspDeepSleepMode(); // activate ESP-12E Deep-Sleep Wake mode
}

void loop() 
{
  // no further actions taken due to ESP.deepSleep mode
}


