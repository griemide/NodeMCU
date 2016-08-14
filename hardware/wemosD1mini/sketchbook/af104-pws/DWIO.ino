/*
 * SKETCH:
 *  DWIO.ino  (DWeet IO support module)
 * 
 *  (c) 2016, Michael Gries
 *  Creation: 2016-06-28 (based on SimpleTestDweetIO.ino)
 *  Modified: 2016-06-28 (function DweetIOmessaging added)
 *  Modified: 2016-07-04 (ESP.getFreeHeap() added to Dweet)
 *  Modified: 2016-07-31 (debug info of http server response improved)
 *  Modified: 2016-08-08 (DWEET_IO_THING_NAME changed to af104-pws)
 * 
 * PREREQUISITES:
 *   uses predefined local WiFi network
 *  
 * LINKS:
 *   see https://dweet.io/ (home page of service provider)
 *   see https://dweet.io/follow/af104-pws  (this application)
 *   see https://dweet.io/follow/af104-test (NodeMCU Lua project test data for reference)
 *   
 * 
 * ISSUES:
 *   none
 *   
 */
 
#include <ESP8266WiFi.h>
#include <TimeLib.h>      // by Paul Stoffregen, not included in the Arduino IDE !!!

#define  DWEET_IO_THING_NAME  "af104-pws"  // IoT test device name

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
  Serial.print(__func__); Serial.print(": TCP: port 80 connected, HTTP (sent data to): "); Serial.println(host);
  char TimeStamp[20]; 
  sprintf(TimeStamp, "%04d-%02d-%02d_%02d:%02d:%02d", year(), month(), day(), hour(), minute(), second() );
  //Serial.println(TimeStamp);
  char DATE_YMD[10]; 
  sprintf(DATE_YMD, "%02d-%02d-%02d", year()-2000, month(), day() );
  char TIME_HMS[10]; 
  sprintf(TIME_HMS, "%02d:%02d:%02d", hour(), minute(), second() );

  // Update Seconds elapsed since device reset
  SecondsElapsed = millis() / 1000;
  
  //
  // send the request to the server https://dweet.io/follow/af104-D1mini
  //
  String httpPayload;
  httpPayload = String("GET /dweet/for/af104-pws") 
                      + "?" 
                      + "Temp_Outdoor=" + gfTempOutdoor  // PWSM.ino
                      + "&" 
                      + "Date_Station="  + DATE_YMD        // DWIO.ino
                      + "&" 
                      + "Time_Local=" + TIME_HMS        // DWIO.ino
                   // + "&" 
                   // + "TimeStamp=" + TimeStamp        // DWIO.ino
                      + "&" 
                      + "RSSI_Station=" + WiFi.RSSI() 
                      + "&" 
                      + "Ping_Local=" + pingAverageLocal  //PING.ino 
                      + "&" 
                      + "Ping_Remote=" + pingAverageRemote //PING.ino 
                      + "&" 
                      + "Seconds_Elapsed=" + SecondsElapsed  //DWIO.ino
                      + "&" 
                      + "Free_Heap=" + ESP.getFreeHeap() 
                      + "&" 
                      + "Built_Date_Unix=" + FileCompiledUnix   // INIT.ino 
                      + "&" 
                      + "Built_Date=" + FileCompiled          // INIT.ino 
                      + "&" 
                      + "Tool_Version=" + __VERSION__ 
                      + " "
                      + "HTTP/1.1\r\n" 
                      + "Host: " + host + "\r\n" 
                      + "Connection: close\r\n\r\n"
                      ;
  //Serial.println(httpPayload); // for debug purposes only 
  client.print(httpPayload);
  delay(20);
  // Read all the lines of the reply from server and print them to Serial
  String httpStatusLine;
  bool   onlyfirstLine = true;
  while(client.available()){
    String line = client.readStringUntil('\r');
    if (onlyfirstLine) {
      httpStatusLine = line;
      onlyfirstLine = false;
    }
    // line.trim(); // get rid of whitespaces (CR, lF, etc.)
    // Serial.print(__func__); Serial.print(": HTTP (debug info): ");Serial.println(line); // only for debug purposes
  }
  Serial.print(__func__); Serial.print(": HTTP (server response): "); Serial.println(httpStatusLine);
}
