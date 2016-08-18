/*
 * SKETCH:
 *  WLAN.ino  (WLAN support module)
 * 
 *  (c) 2016, Michael Gries
 *  Creation: 2016-06-22 (based on SimpleDeepSleepTest.ino)
 *  Modified: 2016-06-27 (RSSI monitoring added)
 *  Modified: 2016-08-17 (received LocalIP prepared to be used by DWIO.ino)
 * 
 * PREREQUISITES:
 *   uses predefined WiFi networks
 *  
 * LINKS:
 *   see https://github.com/griemide/NodeMCU/tree/master/hardware/wemosD1mini/sketchbook/SimpleDeepSleepModule/WLAN.ino 
 *   see https://de.wikipedia.org/wiki/Received_Signal_Strength_Indication (Wikipedia)
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
  Serial.print(__func__); Serial.print(": SSID: "); Serial.print(ssid); Serial.print(" ..."); 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);  Serial.print(".");
  }
  IPAddress deviceIP = WiFi.localIP();
  sprintf(deviceIPstring, "%d.%d.%d.%d", deviceIP[0], deviceIP[1], deviceIP[2], deviceIP[3]);
  ipAddressDevice = (int)deviceIP[3];
  Serial.println();
  Serial.print(__func__); Serial.print(": IP: "); Serial.print(deviceIP);
  Serial.print(" RSSI: "); Serial.print(WiFi.RSSI()); Serial.print(" dBm (");
  Serial.print(millis()); Serial.println(" ms after reset)"); 
}


