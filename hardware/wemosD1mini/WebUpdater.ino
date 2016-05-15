/* M. Gries 2016-05-15 */

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

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>

const char* host = "192.168.0.43"; // assign ESP to this IP in Router config
const char* ssid = "HP LaserJet P1505n";
const char* password = "vQ1tASKOUu";

// Target Application Declaration starts here
const int ledPin = BUILTIN_LED;  // the onboard LED
int brightness = 0;        // how bright the LED is (0 = full, 512 = dim, 1023 = off)
int fadeAmount = 5;        // how many points to fade the LED by
const int delayMillis = 10;      // how long to pause between each loop


ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

void setup(void){

  Serial.begin(74880); /* 2016-05-15 modified from 115200 to 74880*/
  Serial.println();
  Serial.println("Booting Sketch...");
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

  while(WiFi.waitForConnectResult() != WL_CONNECTED){
    WiFi.begin(ssid, password);
    Serial.println("WiFi failed, retrying.");
  }
  IPAddress localhost = WiFi.localIP();
  Serial.println(localhost); // not used yet due to conversion issue
  
  MDNS.begin(host);

  httpUpdater.setup(&httpServer);
  httpServer.begin();

  MDNS.addService("http", "tcp", 80);
  Serial.println("HTTPUpdateServer ready!");
  Serial.printf("Open \nhttp://%s/update \nin browser\n", host);

  // Target Application Setup starts here
  pinMode(ledPin, OUTPUT);  // initialize onboard LED as output
}

void loop(void){
  httpServer.handleClient();
  
  // Target Application starts here
  // set the LED brightness
  analogWrite(ledPin, brightness);

  // increment/decrement the brightness for the next loop
  brightness = brightness + fadeAmount;

  // limit to 10-bit (0-1023)
  if (brightness < 0) brightness = 0;
  if (brightness > 1023) brightness = 1023;

  // reverse the direction of the fading at each end
  if (brightness == 0 || brightness == 1023) {
    fadeAmount = -fadeAmount;
  }

  // pause so you can see each brightness level
  delay(delayMillis);
}
