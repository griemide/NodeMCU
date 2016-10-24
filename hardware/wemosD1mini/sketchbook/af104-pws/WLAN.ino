/*
 * SKETCH:
 *  WLAN.ino  (WLAN support module)
 * 
 *  (c) 2016, Michael Gries
 *  Creation: 2016-06-22 (based on SimpleDeepSleepTest.ino)
 *  Modified: 2016-06-27 (RSSI monitoring added)
 *  Modified: 2016-08-18 (received LocalIP prepared to be used by DWIO.ino)
 *  Modified: 2016-10-24 (MAC address evaluation / presentation added)
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


void GetMACaddress()
{
  byte mac[6];
  WiFi.macAddress(mac);
  sprintf(deviceMACaddress, "%2x:%2x:%2x:%2x:%2x:%2x",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
}
 
void ConnectingToWLAN()
{
  Serial.print(__func__); Serial.print(": SSID: "); Serial.print(ssid); Serial.print(" ..."); 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);  Serial.print(".");
  }
  IPAddress deviceIP = WiFi.localIP();
  IP = deviceIP.toString();
  sprintf(deviceIPstring, "%d.%d.%d.%d", deviceIP[0], deviceIP[1], deviceIP[2], deviceIP[3]);
  ipAddressDevice = (int)deviceIP[3];
  Serial.println();
  Serial.print(__func__); Serial.print(": IP: "); Serial.print(IP);
  Serial.print(" RSSI: "); Serial.print(WiFi.RSSI()); Serial.print(" dBm (");
  Serial.print(millis()); Serial.println(" ms after reset)"); 
  GetMACaddress();
  Serial.print(__func__); Serial.print(": MAC: "); Serial.println(deviceMACaddress); 
}


