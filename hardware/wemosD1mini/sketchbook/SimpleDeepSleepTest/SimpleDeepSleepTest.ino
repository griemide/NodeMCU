/*
 * SKETCH:
 *  SimpleDeepSleepTest.ino 
 * 
 *  (c) 2016, Michael Gries
 *  Creation: 2016-06-15 (based on Sparkfun tutorial ESP8266_Phant_Sleep)
 *  Modified: 2016-06-17 (BUITIN_LED support added to indicate Active Mode)
 *  Modified: 2016-06-19 (editorial changes)
 * 
 * PREREQUISITES (hardware dependencies for Deep-Sleep Wake mode):
 *   for Wemos D1 mini: connect pin RST with D0 (GPIO16)
 *   for NodeMCU: shortcut both pads of the not mounted resitor R3
 *   (otherwise if GPIO16 can't trigger RST than ESP8266 will respond with ets_main.c)
 *  
 * LINKS:
 *   see https://github.com/griemide/NodeMCU/tree/master/hardware/wemosD1mini/sketchbook/SimpleDeepSleep
 * 
 * ISSUES:
 *   see: https://github.com/esp8266/Arduino/issues/1363
 *   
 */

#include <ESP8266WiFi.h>

// DECLARATIONS
const char* ssid                = "HP LaserJet P1505n";
const char* password            = "vQ1tASKOUu";
const int   SERIAL_BAUDRATE     = 74880; // Wemos D1 mini default
const int   LED_BRIGHTNESS      = 512;   // 0..1023 (0=full)
const int   SLEEP_TIME_SECONDS  = 15;    // * 1E6 (internal µs)

void setup() {
  Serial.begin(SERIAL_BAUDRATE);  delay(10);
  pinMode(BUILTIN_LED, OUTPUT);  // initialize onboard LED 
  analogWrite(BUILTIN_LED, LED_BRIGHTNESS);
  Serial.println();
  Serial.print(__func__); Serial.print(": reset by ");
  Serial.println(ESP.getResetReason()); //Deep-Sleep Wake OR  External System
  
  // Connect to WiFi network
  Serial.print(__func__); Serial.print(": connecting to WiFi ...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);  Serial.print(".");
  }
  Serial.println();
  Serial.print(__func__); Serial.print(": WiFi connected with IP: ");  
  Serial.println(WiFi.localIP());

  // activate Deep-Sleep Wake mode
  Serial.print(__func__); Serial.print(": "); Serial.print(millis()); 
  Serial.print(" ms -> activating Deep-Sleep Wake mode for ");
  Serial.print(SLEEP_TIME_SECONDS); Serial.println(" seconds");
  ESP.deepSleep(SLEEP_TIME_SECONDS * 1E6);
}

void loop() 
{
  // no further actions taken due to ESP.deepSleep mode
}


