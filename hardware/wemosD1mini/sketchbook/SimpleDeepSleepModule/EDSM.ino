/*
 * SKETCH:
 *  EDSM.ino 
 * 
 *  (c) 2016, Michael Gries
 *  Creation: 2016-06-22 (based on SimpleDeepSleepTest.ino)
 *  Modified: 2016-06-23 (editorial changes)
 *  Modified: 2016-06-25 (handling added if period is exceeded)
 * 
 * PREREQUISITES (hardware dependencies for Deep-Sleep Wake mode):
 *   for Wemos D1 mini: connect pin RST with D0 (GPIO16)
 *   for NodeMCU: shortcut both pads of the not mounted resitor R3
 *   (otherwise if GPIO16 can't trigger RST than ESP8266 will respond with ets_main.c)
 *  
 * LINKS:
 *   see https://github.com/griemide/NodeMCU/tree/master/hardware/wemosD1mini/sketchbook/SimpleDeepSleepModule/EDSM.ino  
 *    
 * ISSUES:
 *   see: https://github.com/esp8266/Arduino/issues/1363
 *   
 */

// DECLARATIONS
const int     SLEEP_TIME_SECONDS  = 15;    // * 1E6 (internal µs)
int           sleepTimeDiff;
int           sleepTimeExceeded;
unsigned long timeAfterReset;

void EspDeepSleepMode() 
{
  timeAfterReset = millis(); 
  sleepTimeDiff = SLEEP_TIME_SECONDS * 1000 - timeAfterReset;
  Serial.print(__func__); Serial.print(": setup runs for "); 
  Serial.print(timeAfterReset); Serial.print(" ms after reset \n"); 
  if(sleepTimeDiff < 0){ // check if current period exceeded (neg.)
    sleepTimeExceeded = sleepTimeDiff; // negative value
    sleepTimeDiff = sleepTimeExceeded + (SLEEP_TIME_SECONDS * 1000); // new period
    Serial.print(__func__); Serial.print(": last period of "); 
    Serial.print(SLEEP_TIME_SECONDS); Serial.print(" seconds exceeded ("); 
    Serial.print(sleepTimeExceeded); Serial.println(" ms) - next period skipped");
  }
  Serial.print(__func__); Serial.print(": activating Deep-Sleep Wake mode for "); 
  Serial.print(SLEEP_TIME_SECONDS); Serial.print(" seconds ("); 
  Serial.print(sleepTimeDiff); Serial.println(" ms left)");
  ESP.deepSleep(sleepTimeDiff * 1E3);
}

