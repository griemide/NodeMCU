/*
 * SKETCH:
 *  DWIO.ino 
 * 
 *  (c) 2016, Michael Gries
 *  Creation: 2016-06-28 (based on SimpleTestDweetIO.ino)
 *  Modified: 2016-06-28 (function DweetIOmessaging added)
 * 
 * PREREQUISITES:
 *   uses predefined local WiFi network
 *  
 * LINKS:
 *   see https://dweet.io/ (home page of service provider)
 *   see https://dweet.io/follow/af104-D1mini (this application)
 *   see https://dweet.io/follow/af104-test (NodeMCU Lua project test data for reference)
 * 
 * ISSUES:
 *   none
 *   
 */
 
#include <ESP8266WiFi.h>
#define  DWEET_IO_THING_NAME  "af104-D1mini"  // IoT test device name

////  DECLARATIONS
const char* host = "dweet.io";


void DweetIOmessaging(){
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.print(__func__); Serial.println(": TCP connection failed (port 80)");
    return;
  }
  Serial.print(__func__); Serial.print(": TCP: port 80 connected, HTTP: sent data to "); Serial.println(host);
  //
  // send the request to the server https://dweet.io/follow/af104-D1mini
  //
  client.print(String("GET /dweet/for/af104-D1mini") 
                      + "?" 
                      + "RSSI=" + WiFi.RSSI() 
                      + "&" 
                      + "Millis=" + millis() 
                      + "&" 
                      + "Version=" + __VERSION__ 
                      + " "
                      + "HTTP/1.1\r\n" 
                      + "Host: " + host + "\r\n" 
                      + "Connection: close\r\n\r\n"
                      );
  delay(10);
  // Read all the lines of the reply from server and print them to Serial
  String httpStatusLine;
  bool   onlyfirstLine = true;
  while(client.available()){
    String line = client.readStringUntil('\r');
    if (onlyfirstLine) {
      httpStatusLine = line;
      onlyfirstLine = false;
    }
    // Serial.print(__func__); Serial.print(line); // only for debug purposes
  }
  Serial.print(__func__); Serial.print(": HTTP: received "); Serial.println(httpStatusLine);
}
