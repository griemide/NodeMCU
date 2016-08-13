/*
 * SKETCH:
 *  INIT.ino  (INITialize hardware module)
 * 
 *  (c) 2016, Michael Gries
 *  Creation: 2016-06-22 (based on SimpleDeepSleepTest.ino)
 *  Modified: 2016-06-23 (editorial changes)
 *  Modified: 2016-06-25 (sketch name added - see __FILE__ handling in main application)
 *  Modified: 2016-07-03 (DEBUG: Serial.setDebugOutput(true) added)
 * 
 * PREREQUISITES:
 *   uses BUILTIN_LED
 *  
 * LINKS:
 *   see https://github.com/griemide/NodeMCU/tree/master/hardware/wemosD1mini/sketchbook/SimpleDeepSleepModule/INIT.ino  
 *  
 * ISSUES:
 *   none
 *   
 */

#define WIFI_DEBUG false

// DECLARATIONS
const int   SERIAL_BAUDRATE     = 74880; // Wemos D1 mini default

void InitializeModule(char* sketchName) 
{
  pinMode(BUILTIN_LED, OUTPUT);  // initialize onboard LED 
  analogWrite(BUILTIN_LED, LED_BRIGHTNESS_FULL);
  Serial.begin(SERIAL_BAUDRATE);  delay(10);
  if(WIFI_DEBUG) {
    Serial.setDebugOutput(true); // prints additional information of WiFi connection
  }
  Serial.println();
  Serial.print(__func__); Serial.print(": reset by ");
  Serial.println(ESP.getResetReason()); //Deep-Sleep Wake OR  External System
  Serial.print(__func__); Serial.print(": sketch "); Serial.println(sketchName);
  Serial.print(__func__); Serial.print(": used IDE Arduino 1.6.7  \n"); 
  Serial.print(__func__); Serial.print(": Compiled on "); 
  Serial.print(__DATE__); Serial.print(" "); Serial.println(__TIME__);
  Serial.print(__func__); Serial.print(": esptool v"); Serial.println(__VERSION__);
  FileCompiled = __DATE__; FileCompiled.replace(" ","-");
  analogWrite(BUILTIN_LED, LED_BRIGHTNESS_LOW);
}

