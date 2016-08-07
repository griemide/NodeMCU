/*
 * SKETCH:
 *  PWSM.ino (Personal Weather Station Messaging)
 * 
 *  (c) 2016, Michael Gries
 *  Creation: 2016-07-30 (Personal Weather Station PWS for weatherstation.wunderground.com)
 *  Modified: 2016-07-31 (feasability study - basis tests by using wiki.wundergrund.com example)
 *  Modified: 2016-08-03 (outdoor temperature added)
 *  Modified: 2016-08-08 (outdoor temperature simulation added)
 * 
 * PREREQUISITES:
 *   uses predefined local WiFi network
 *  
 * LINKS:
 *   see http://wiki.wunderground.com/index.php/PWS_-_Upload_Protocol
 *   see https://www.wunderground.com/personal-weather-station/dashboard?ID=IBADHERS8
 *   
 * EXAMPLE:
 *   https://weatherstation.wunderground.com/weatherstation/updateweatherstation.php?ID=IBADHERS8&PASSWORD=1tqx3ixw&dateutc=2016-07-30+21%3A19%3A59&winddir=230&windspeedmph=12&windgustmph=12&tempf=70&rainin=0&baromin=29.1&dewptf=68.2&humidity=90&weather=&clouds=&softwaretype=vws%20versionxx&action=updateraw
 * 
 * ISSUES:
 *   none
 *   
 */
 
#include <ESP8266WiFi.h>
#include <TimeLib.h>      // by Paul Stoffregen, not included in the Arduino IDE !!!

#define  WU_SERVER   "weatherstation.wunderground.com"  // see example above
#define  PWS_ID      "IBADHERS8"                        // check your WU login profile
#define  PWS_PWD     "1tqx3ixw"                         // check your WU login profile
#define  PWS_VERSION "16.8.8"                           // udtae according modified date of this file
#define  PWS_ACTION  "updateraw"                        // sending raw data (default)

////  DECLARATIONS
const char* WUhost = WU_SERVER;
const float pi = 3.14;
const float TempOutdoorDefault = 15.0;
const int SecondsPerDay = 24 * 3600;
void  TempToString();
float Tc;
float Tf;
char  sTc[12];
char  sTf[12];


void PWSMessageUpdate(){
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(WUhost, httpPort)) {
    Serial.print(__func__); Serial.println(": TCP connection failed (port 80)");
    return;
  }
  Serial.print(__func__); Serial.print(": TCP: port 80 connected, HTTP (sent data to): "); Serial.println(WUhost);

  // IMPORTANT all fields must be url escaped (reference http://www.w3schools.com/tags/ref_urlencode.asp)
  // e.g. 2000-11-22 33:44:55  becomes  2000-11-22+33%3A44%3A55
  // also time stamp need to be UTC coded
  char TimeStamp[24]; // check size if sprinf format-string changed
  sprintf(TimeStamp, "%04d-%02d-%02d+%02d%s%02d%s%02d", year(), month(), day(), hour()-2, "%3A", minute(), "%3A", second() );
  // Serial.print(__func__); Serial.print(": TimeStamp (debug info): "); Serial.println(TimeStamp); // for debug purpose only 
  // debug test result: 2016-07-31+19%3A17%3A27

  //float HMS = hour()+minute()/60.0;                 // https://de.wikipedia.org/wiki/Industrieminute
  //float TcDelta = HMS * 15.0;                       // i.e. 24 hour == 360°  (for sine cycle below)
  int iSec = second()+minute()*60+hour()*3600;
  Tc = TempOutdoorDefault + sin(2*pi*iSec/SecondsPerDay + 3*pi/2) * 3;  // i.e. one periodic cycle per day (variies around giTempOutdoor for test purposes)
  Tf = Tc * (9.0/5.0) + 32.0;                         // https://de.wikipedia.org/wiki/Grad_Fahrenheit

  TempToString();  

  gfTempOutdoor = Tc;
  
  String PWStemp(sTf);                // i.e. one periodic cycle per day (variies around giTempOutdoor for test purposes)
  


  char PWStempDebug[40]; 
  sprintf(PWStempDebug, "iSec: %d Tc: %s Tf: %s", iSec, sTc, sTf );
  Serial.print(__func__); Serial.print(": PWStempDebug (debug info): "); Serial.println(PWStempDebug); // for debug purpose only 
  Serial.print(__func__); Serial.print(": PWStemp (debug info): "); Serial.println(PWStemp); // for debug purpose only 

  String httpPayload;
  httpPayload = String("GET /weatherstation/updateweatherstation.php?") 
                      + "ID=" + PWS_ID
                      + "&" 
                      + "PASSWORD=" + PWS_PWD 
                      + "&" 
                      + "dateutc=" + TimeStamp
                      + "&" 
                      + "tempf=" + PWStemp
                      + "&" 
                      + "softwaretype=" + PWS_VERSION
                      + "&" 
                      + "action=" + PWS_ACTION
                      + " "
                      + "HTTP/1.1\r\n" 
                      + "Host: " + WUhost + "\r\n" 
                      + "Connection: close\r\n\r\n"
                      ;

  //Serial.println(httpPayload); // for debug purposes only 
  client.print(httpPayload);
  delay(10);
  // Read all the lines of the reply from server and print them to Serial
  String httpStatusLine;
  bool   onlyfirstLine = true;
  while(client.available()){
    String line = client.readStringUntil('\r');
    if (onlyfirstLine) {
      httpStatusLine = line;
      onlyfirstLine = false;
    }
    // line.trim(); // get rid of whitespaces (CR, lF, etc.)
    // Serial.print(__func__); Serial.print(": HTTP (debug info): ");Serial.println(line); // only for debug purposes
  }
  Serial.print(__func__); Serial.print(": HTTP (server response): "); Serial.println(httpStatusLine);
}


void TempToString() {
  signed char minStringWidthIncDecimalPoint = 4;
  unsigned char numVarsAfterDecimal = 1;
  dtostrf(Tc, minStringWidthIncDecimalPoint, numVarsAfterDecimal, sTc);
  dtostrf(Tf, minStringWidthIncDecimalPoint, numVarsAfterDecimal, sTf);
}
  
