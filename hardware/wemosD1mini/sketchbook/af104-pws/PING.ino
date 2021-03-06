/*
 * SKETCH:
 *  PING.ino  (PING support module)
 * 
 *  (c) 2016, Michael Gries
 *  Creation: 2016-06-22 (based on SimpleDeepSleepTest.ino)
 *  Modified: 2016-06-23 (editorial changes)
 *  Modified: 2016-10-23 (logging enhancements)
 * 
 * PREREQUISITES:
 *   uses predefined local IP addresses or global hostnames via DNS
 *  
 * LINKS:
 *   see https://github.com/griemide/NodeMCU/tree/master/hardware/wemosD1mini/sketchbook/SimpleDeepSleepModule/PING.ino 
 * 
 * ISSUES:
 *   none
 *   
 */

#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>

//// DECLARATIONS
const IPAddress local_ip(192, 168, 0, 10);
const char* remote_host = "www.gries.name";


//// FUNCTIONS
void PingLocalClients()
{
  Serial.print(__func__); Serial.print(": Pinging ");   Serial.print(local_ip); Serial.println("(5 times, default) ...");
  //sprintf(logMessageBuffer, "pinging  %s (5 times, default) ...", IP); SerialLog(__func__, logMessageBuffer); // IP from WLAN.ino // ???
  if(Ping.remoteIP(local_ip)) {
    pingAverageLocal = Ping.averageTime();
    sprintf(logMessageBuffer, "average time %d ms", pingAverageLocal ); SerialLog(__func__, logMessageBuffer);
  } else {
    Serial.print(__func__); Serial.println(": Error (local IP)"); 
  }
}

void PingRemoteServer()
{
  //Serial.print(__func__); Serial.print(": Pinging ");   Serial.print(remote_host); Serial.println("(5 times, default) ...");
  sprintf(logMessageBuffer, "pinging  %s (5 times, default) ...", remote_host); SerialLog(__func__, logMessageBuffer);
  if(Ping.remoteHOST(remote_host)) {
    pingAverageRemote = Ping.averageTime();
    Serial.print(__func__); Serial.print(": average time "); Serial.print(pingAverageRemote); Serial.println(" ms");
  } else {
    Serial.print(__func__); Serial.println(": Error (remote server)"); 
    sprintf(logMessageBuffer, "Error pinging %s", remote_host ); SerialLog(__func__, logMessageBuffer);
  }
}

