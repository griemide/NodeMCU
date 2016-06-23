/*
 * SKETCH:
 *  WLAN.ino 
 * 
 *  (c) 2016, Michael Gries
 *  Creation: 2016-06-22 (based on SimpleDeepSleepTest.ino)
 *  Modified: 2016-06-23 (editorial changes)
 * 
 * PREREQUISITES:
 *   uses predefined WiFi networks
 *  
 * LINKS:
 *   see https://github.com/griemide/NodeMCU/tree/master/hardware/wemosD1mini/sketchbook/SimpleDeepSleepModule/WLAN.ino 
 * 
 * ISSUES:
 *   none
 *   
 */

#include <ESP8266WiFi.h>

// DECLARATIONS
const char* ssid                = "HP LaserJet P1505n";
const char* password            = "vQ1tASKOUu";

void ConnectingToWLAN()
{
  Serial.print(__func__); Serial.print(": "); Serial.print(ssid); Serial.print(" ..."); 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);  Serial.print(".");
  }
  Serial.println();
  Serial.print(__func__); Serial.print(": IP: "); Serial.print(WiFi.localIP());
  Serial.print(" ("); Serial.print(millis()); Serial.println(" ms after reset)"); 
}


