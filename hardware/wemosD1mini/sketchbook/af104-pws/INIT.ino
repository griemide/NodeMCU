/*
 * SKETCH:
 *  INIT.ino  (INITialize hardware module)
 * 
 *  (c) 2016, Michael Gries
 *  Creation: 2016-06-22 (based on SimpleDeepSleepTest.ino)
 *  Modified: 2016-06-23 (editorial changes)
 *  Modified: 2016-06-25 (sketch name added - see __FILE__ handling in main application)
 *  Modified: 2016-07-03 (DEBUG: Serial.setDebugOutput(true) added)
 *  Modified: 2016-10-23 (Reset reason set to public)
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
void  compileTimeUnix();
void  compileDate();
String resetReason; //SMTP.ino

void InitializeModule(char* sketchName) 
{
  pinMode(BUILTIN_LED, OUTPUT);  // initialize onboard LED 
  analogWrite(BUILTIN_LED, LED_BRIGHTNESS_FULL);
  resetReason = ESP.getResetReason();
  Serial.begin(SERIAL_BAUDRATE);  delay(10);
  if(WIFI_DEBUG) {
    Serial.setDebugOutput(true); // prints additional information of WiFi connection
  }
  Serial.println();
  Serial.print(__func__); Serial.print(": reset by "); Serial.println(resetReason); 
  Serial.print(__func__); Serial.print(": sketch "); Serial.println(sketchName);
  Serial.print(__func__); Serial.print(": used IDE Arduino 1.6.7  \n"); 
  Serial.print(__func__); Serial.print(": Compiled on "); 
  Serial.print(__DATE__); Serial.print(" "); Serial.println(__TIME__);
  //FileCompiled = __DATE__; FileCompiled.replace(" ","-");
  compileTimeUnix();
  Serial.print(__func__); Serial.print(": compileTimeUnix: "); Serial.println(FileCompiledUnix);
  compileDate();
  Serial.print(__func__); Serial.print(": compileDate: "); Serial.println(FileCompiled);
  Serial.print(__func__); Serial.print(": esptool v"); Serial.println(__VERSION__);
  analogWrite(BUILTIN_LED, LED_BRIGHTNESS_LOW);
}

//Function to return the compile date and time as a int value
void compileTimeUnix(void)  // unix epoche
{
#define FUDGE 25        //fudge factor to allow for compile time (seconds, YMMV)
  char *compDate = __DATE__, *compTime = __TIME__, *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
  char chMon[3], *m;
  int d, y;
  tmElements_t tm;
  time_t t;
  strncpy(chMon, compDate, 3);
  chMon[3] = '\0';
  m = strstr(months, chMon);
  tm.Month = ((m - months) / 3 + 1);
  tm.Day = atoi(compDate + 4);
  tm.Year = atoi(compDate + 7) - 1970;
  tm.Hour = atoi(compTime);
  tm.Minute = atoi(compTime + 3);
  tm.Second = atoi(compTime + 6);
  t = makeTime(tm);
  FileCompiledUnix = (int)t + FUDGE;        //add fudge factor to allow for compile time
}

//Function to return the compile date and time as a time_t value
void compileDate(void)  // YY.M.D
{
  char *compDate = __DATE__,  *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
  char chMon[3], *mon;
  int d, m, y;
  strncpy(chMon, compDate, 3);
  chMon[3] = '\0';
  mon = strstr(months, chMon);
  m= ((mon - months) / 3 + 1);
  d = atoi(compDate + 4);
  y = atoi(compDate + 7);
  char DATE_YMD[10];  sprintf(DATE_YMD, "%d.%d.%d", y-2000, m, d);
  FileCompiled = (String)DATE_YMD;        
}


