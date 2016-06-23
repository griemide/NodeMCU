/*
 * SKETCH:
 *  INIT.ino 
 * 
 *  (c) 2016, Michael Gries
 *  Creation: 2016-06-22 (based on SimpleDeepSleepTest.ino)
 *  Modified: 2016-06-23 (editorial changes)
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

// DECLARATIONS
const int   SERIAL_BAUDRATE     = 74880; // Wemos D1 mini default
const int   LED_BRIGHTNESS      = 512;   // 0..1023 (0=full)

void InitializeModule() 
{
  Serial.begin(SERIAL_BAUDRATE);  delay(10);
  pinMode(BUILTIN_LED, OUTPUT);  // initialize onboard LED 
  analogWrite(BUILTIN_LED, LED_BRIGHTNESS);
  Serial.println();
  Serial.print(__func__); Serial.print(": reset by ");
  Serial.println(ESP.getResetReason()); //Deep-Sleep Wake OR  External System
  Serial.print(__func__); Serial.print(": used IDE Arduino 1.6.7  \n"); 
  Serial.print(__func__); Serial.print(": Compiled on "); 
  Serial.print(__DATE__); Serial.print(" "); Serial.println(__TIME__);
  Serial.print(__func__); Serial.print(": esptool v"); Serial.println(__VERSION__);
}
