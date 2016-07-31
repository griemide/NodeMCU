/*
 * SKETCH:
 *  PING.ino  (PING support module)
 * 
 *  (c) 2016, Michael Gries
 *  Creation: 2016-06-22 (based on SimpleDeepSleepTest.ino)
 *  Modified: 2016-06-23 (editorial changes)
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
int   avg_time_ms;

//// FUNCTIONS
void PingLocalClients()
{
  Serial.print(__func__); Serial.print(": pinging ");   Serial.print(local_ip); Serial.println("(5 times, default) ...");
  if(Ping.remoteIP(local_ip)) {
    avg_time_ms = Ping.averageTime();
    Serial.print(__func__); Serial.print(": average time "); Serial.print(avg_time_ms); Serial.println(" ms");
  } else {
    Serial.print(__func__); Serial.println(": Error"); 
  }
}

void PingRemoteServer()
{
  Serial.print(__func__); Serial.print(": Pinging ");   Serial.print(remote_host); Serial.println("(5 times, default) ...");
  if(Ping.remoteHOST(remote_host)) {
    avg_time_ms = Ping.averageTime();
    Serial.print(__func__); Serial.print(": average time "); Serial.print(avg_time_ms); Serial.println(" ms");
  } else {
    Serial.print(__func__); Serial.println(": Error"); 
  }
}

