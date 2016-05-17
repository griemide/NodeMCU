/* M. Gries 2016-05-16 */

/* 
 *  Design Specification
 *  Based on ArduinoOTA/WebUpdater for OTA Uploads of any *.bin file the target application will be added here too
 *  an OTA uploader functionality  still remains
 *  so another target application can be uploaded later on or an update of the following target application:
 *  
 *  Target Application: FADE
 *  source code copied from original Arduino examples 01.Basics/Fade.ino
 *  
 */

//INCLUDES
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>

//DECLARATIONS
const int ledPin = BUILTIN_LED;  // the onboard LED
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
void wifi();
void ota();
void esp();
void fade();

//SETUP
void setup(void){
  Serial.begin(74880); /* 2016-05-15 modified from 115200 to 74880*/
  Serial.println("Setup: ");
  Serial.println(" configuring Wifi, OTA, ESP, Application (Fade) ... ");
  wifi();
  ota();
  esp();
  // Target Application (Fade) Setup:
  pinMode(ledPin, OUTPUT);  // initialize onboard LED as output
  Serial.println("Fade:");   
  Serial.println(" processing fading of built-in LED");
  Serial.println(" application running ...");
}

//LOOP
void loop(void){
  httpServer.handleClient();
  // Target Application Loop:
  fade();
}
