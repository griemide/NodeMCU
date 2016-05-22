/* M. Gries 2016-05-22
 *  
 * granted by: * 
 * ESP8266 : NTP Time + Timezone
 * Created 07 May 2016 by Ralf Bohnen - www.Arduinoclub.de
 * This example code is in the public domain.
 */

#include <TimeLib.h>  //by Paul Stoffregen, not included in the Arduino IDE !!!
#include <Timezone.h> //by Jack Christensen, not included in the Arduino IDE !!!

time_t compileTime(void);

//Function to return the compile date and time as a time_t unix epoche value
time_t compileTime(void)
{
#define FUDGE 25        // use fudge factor to estimate used compile time (seconds, YMMV)
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
  return t + FUDGE;        //add fudge factor to allow for compile time
}

void build(void)
{
  Serial.print("Complied on (unix epoche): ");
  Serial.println(compileTime());
  Serial.print(__DATE__); Serial.print(" "); Serial.println(__TIME__);
  Serial.println(__FILE__);
  Serial.println(__LINE__);
  Serial.println(__func__);
  Serial.println(__VERSION__);
}

