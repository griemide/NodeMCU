/*
 * https://github.com/dancol90/ESP8266Ping
 * With this library an ESP8266 can ping a remote machine and know if it's reachable. 
 * It provides some basic measurements on ping messages (avg response time).
 * 
 * 2016-05-27, Michael Gries
 * library ESP8266Ping modified 
 * function name 'ping' for both types (ip or host) separated into
 * function 'remoteIP' and 'remotHOST' (for library handling learning purposes only)
 * 
 * see https://github.com/griemide/NodeMCU/tree/master/hardware/wemosD1mini/sketchbook/SimplePing
 * 
 */

#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>

const char* ssid     = "HP LaserJet P1505n";
const char* password = "vQ1tASKOUu";

const IPAddress remote_ip(192, 168, 0, 10);
const char* remote_host = "www.gries.name";

void setup() {
  Serial.begin(74880);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println("Connecting to WiFi");
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("WiFi connected with ip ");  
  Serial.println(WiFi.localIP());

  Serial.print("\nPinging (5 times, default) Remote IP ");
  Serial.println(remote_ip);
  if(Ping.remoteIP(remote_ip)) {
    int avg_time_ms = Ping.averageTime();
    Serial.print("remote_ip: Average Time ");
    Serial.print(avg_time_ms); Serial.println(" ms");
  } else {
    Serial.println("remote_ip: Error :(");
  }
  
  Serial.print("\nPinging (5 times, default) Remote host ");
  Serial.println(remote_host);
  if(Ping.remoteHOST(remote_host)) {
    int avg_time_ms = Ping.averageTime();
    Serial.print("remote_host: Average Time ");
    Serial.print(avg_time_ms); Serial.println(" ms");
  } else {
    Serial.println("remote_host: Error :(");
  }
  
  Serial.print("\nPinging (10 times, defined) Remote host ");
  Serial.println(remote_host);
  if(Ping.remoteHOST(remote_host, 10)) {
    int avg_time_ms = Ping.averageTime();
    Serial.print("remote_host: Average Time ");
    Serial.print(avg_time_ms); Serial.println(" ms");
  } else {
    Serial.println("remote_host: Error :(");
  }

  Serial.println();
  Serial.print(__func__); Serial.println(": pinging example ended");
}


void loop() { } // no further actions taken


