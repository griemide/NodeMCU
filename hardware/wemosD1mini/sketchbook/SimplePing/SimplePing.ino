/*
 * https://github.com/dancol90/ESP8266Ping
 * With this library an ESP8266 can ping a remote machine and know if it's reachable. 
 * It provides some basic measurements on ping messages (avg response time).
 * 
 * (c) 2006, Michael Gries
 * Creation: 2016-05-27 (library ESP8266Ping modified)
 * Modified: 2016-06-14 (SPIFFS storage added)
 * 
 * function name 'ping' for both types (ip or host) separated into
 * function 'remoteIP' and 'remotHOST' (for library handling learning purposes only)
 * 
 * see https://github.com/griemide/NodeMCU/tree/master/hardware/wemosD1mini/sketchbook/SimplePing
 * 
 */

#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include <FS.h>

//DECLARATIONS
const int ledPin = BUILTIN_LED;  // the onboard LED
const short int BUILTIN_LED1 = 16;  //GPIO16 on Wemos D1 mini (ESP-12)
const short int BUILTIN_LED2 = 16;  //GPIO16 on NodeMCU (ESP-12)

const char* ssid     = "HP LaserJet P1505n";
const char* password = "vQ1tASKOUu";

const IPAddress remote_ip(192, 168, 0, 10);
const char* remote_host = "www.gries.name";

int avg_time_ms;

// SPIFFS file handle
File fh;

void signalError() {  // loop endless with LED blinking in case of error
  while(1) {
      digitalWrite(BUILTIN_LED, LOW);
      delay(300); // ms
      digitalWrite(BUILTIN_LED, HIGH);
      delay(300); // ms
  }
}

void readSPIFFS() {
  fh = SPIFFS.open("/SimplePing.txt", "r");
  if (!fh) {
    Serial.println(F("SPIFFS open fail"));
    return;
  }
  while (fh.available()) {
    Serial.write(fh.read());
  }
  fh.close();
}  // readSPIFFS()



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

  if (!SPIFFS.begin()) {
     Serial.println("SPIFFS failed, needs formatting");
     signalError();
  }

  Serial.print("\nPinging (5 times, default) Remote IP ");
  Serial.println(remote_ip);
  if(Ping.remoteIP(remote_ip)) {
    avg_time_ms = Ping.averageTime();
    Serial.print("remote_ip: Average Time ");
    Serial.print(avg_time_ms); Serial.println(" ms");
  } else {
    Serial.println("remote_ip: Error :(");
  }

  Serial.print(__func__);
  fh = SPIFFS.open("/SimplePing.txt", "a+");
  if (!fh) {
    Serial.println(F(": SPIFFS open fail for APPENDING"));
    signalError();
  } else {
    Serial.println(F(": SPIFFS value appended to SimplePing.txt"));
    fh.print(avg_time_ms); fh.println(" ms");
    fh.close();
  }
  

  
  Serial.print("\nPinging (5 times, default) Remote host ");
  Serial.println(remote_host);
  if(Ping.remoteHOST(remote_host)) {
    avg_time_ms = Ping.averageTime();
    Serial.print("remote_host: Average Time ");
    Serial.print(avg_time_ms); Serial.println(" ms");
  } else {
    Serial.println("remote_host: Error :(");
  }
  
  Serial.print("\nPinging (10 times, defined) Remote host ");
  Serial.println(remote_host);
  if(Ping.remoteHOST(remote_host, 10)) {
    avg_time_ms = Ping.averageTime();
    Serial.print("remote_host: Average Time ");
    Serial.print(avg_time_ms); Serial.println(" ms");
  } else {
    Serial.println("remote_host: Error :(");
  }

  Serial.println();
  Serial.print(__func__); Serial.println(": pinging example ended");

readSPIFFS();

}


void loop() { } // no further actions taken



