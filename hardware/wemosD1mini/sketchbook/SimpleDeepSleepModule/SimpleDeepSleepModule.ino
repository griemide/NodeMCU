/*
 * SKETCH:
 *  SimpleDeepSleepModule.ino 
 * 
 *  (c) 2016, Michael Gries
 *  Creation: 2016-06-15 (based on Sparkfun tutorial ESP8266_Phant_Sleep)
 *  Modified: 2016-06-23 (Sketch SimpleDeepsSleepTest rearanged to module structure)
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

//// DECLARATIONS
void InitializeModule(); 
void ConnectingToWLAN();
void GetTimeNTPServer();
void PingLocalClients();
void PingRemoteServer();
void EspDeepSleepMode();


//// APPLICATION 
void setup() 
{
  InitializeModule(); // ESP-12E based boards
  
  ConnectingToWLAN(); // Connect to local WiFi network

  GetTimeNTPServer(); // use NTP server time for timestamp logging

  PingLocalClients(); // monitor reachability of local network clients 

  PingRemoteServer(); // monitor reachability of remote ftp server
  
  EspDeepSleepMode(); // activate ESP-12E Deep-Sleep Wake mode
}

void loop() 
{
  // no further actions taken due to ESP.deepSleep mode
}


