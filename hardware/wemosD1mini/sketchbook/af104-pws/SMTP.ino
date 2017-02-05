/*
 * SKETCH:
 *  SMTP.ino  (Simple Mail Transfer Protocol)
 * 
 *  (c) 2016, Michael Gries
 *  Creation: 2016-09-01 (based on SimpleEmail Client.ino)
 *  Modified: 2016-09-02 (feasability study only)
 *  Modified: 2016-10-23 (reboor reason added to email)
 *  Modified: 2016-10-24 (MAC address added to email)
 *  Modified: 2017-02-05 (eRcv function name printing added)
 * 
 * PREREQUISITES:
 *   existing email account - used provider: https://webmailer.1und1.de/ 
 *   for Authentication use credential as Base64 encoded (see LINKS)
  *  
 * LINKS:
 *   see https://de.wikipedia.org/wiki/Simple_Mail_Transfer_Protocol
 *   see https://de.wikipedia.org/wiki/SMTP-Auth
 *   see https://www.base64encode.org/
 *   
 * ISSUES:
 *   none
 *   
 */

#include <ESP8266WiFi.h>

char server[] = "smtp.1und1.de";
char smtp_from[] = "esp8266@gries.name";
char smtp_subject[] = "Device reboot ";
int  portSMTP = 23;

WiFiClient client;
byte sendEmailViaSMTP();
byte eRcv();


byte sendEmailViaSMTP(){
  Serial.print(__func__);  
  //const char *RecvEmailViaSMTP = __func__; // __func__ workaround for eRev routine
  Serial.println(F(": establising connection to server (smtp.1und1.de) via port 587 ..."));
  byte thisByte = 0;
  byte respCode;

  if (client.connect(server, 587) == 1) {
    Serial.print(__func__);  Serial.println(F(": connected"));
  } else {
    Serial.print(__func__);  Serial.println(F(": connection failed"));
    return 0;
  }
  if (!eRcv()) return 0;

  Serial.print(__func__);  Serial.println(F(": Sending EHLO www.gries.name"));  // Extended HELO
  client.println("EHLO www.gries.name");
  if (!eRcv()) return 0;
  // https://de.wikipedia.org/wiki/SMTP-Auth
  Serial.print(__func__);  Serial.println(F(": Sending AUTH LOGIN")); 
  client.println("AUTH LOGIN");
  if (!eRcv()) return 0;
  Serial.print(__func__);  Serial.println(F(": Sending User (base64)"));
  // Change to your base64, ASCII encoded user 
  // e.g. using Online converter (https://www.base64encode.org/)
  client.println("bWljaGFlbEBncmllcy5uYW1l"); //<---------User (michael@gries.name)
  if (!eRcv()) return 0;
  Serial.print(__func__);  Serial.println(F(": Sending Password (base64)"));
  // change to your base64, ASCII encoded password
  client.println("bm9wYXNzd2Qx");//<---------Password (nxxxxxxx1)
  if (!eRcv()) return 0;
  Serial.print(__func__);  Serial.println(F(": Sending MAIL FROM: esp8266@gries.name"));
  // change to your email address (sender)
  client.println(F("MAIL FROM: esp8266@gries.name"));
  if (!eRcv()) return 0;
  // change to recipient address
  Serial.print(__func__);  Serial.println(F(": Sending RCPT TO: michael@gries.name"));
  client.println(F("RCPT TO: michael@gries.name"));
  if (!eRcv()) return 0;
  Serial.print(__func__);  Serial.println(F(": Sending DATA"));
  client.println(F("DATA"));
  if (!eRcv()) return 0;
  Serial.print(__func__);  Serial.println(F(": Sending email"));
  // change to recipient address
  client.println(F("To:  michael@gries.name"));
  // change to your address
  client.print(F("From: "));
  client.println(smtp_from);
  client.print(F("Subject: "));
  client.print(smtp_subject);
  client.print(deviceMACaddress);  //WLAN.ino
  client.println("\r\n");  // required ?
  client.println(F("Device: Wemos D1 mini"));
  client.print(F("Reason: "));
  client.println(resetReason);
  client.print(F("IP: "));
  client.println(WiFi.localIP());
  client.print(F("RSSI: "));
  client.print(WiFi.RSSI());
  client.println(F(" dBm"));
  client.print(F("Sketch: "));
  client.println(sketchName);
  client.print(F("Build: "));
  client.println(__DATE__);

  client.println(F(".")); // required
  if (!eRcv()) return 0;
  Serial.print(__func__);  Serial.println(F(": Sending QUIT"));
  client.println(F("QUIT"));
  if (!eRcv()) return 0;
  client.stop();
  Serial.print(__func__);  Serial.println(F(": disconnected"));
  return 1;
}

byte eRcv()
{
  byte respCode;
  byte thisByte;
  int loopCount = 0;

  while (!client.available()) {
    delay(1);
    loopCount++;
    // if nothing received for 10 seconds, timeout
    if (loopCount > 10000) {
      client.stop();
      Serial.print(__func__);  Serial.println(F(": Timeout"));
      return 0;
    }
  }

  respCode = client.peek();
  Serial.print("RecvEmailViaSMTP");  Serial.print(": ");
  while (client.available())
  {
    thisByte = client.read();
     Serial.write(thisByte);
  }

  if (respCode >= '4')
  {
    return 0;
  }
  return 1;
} 

