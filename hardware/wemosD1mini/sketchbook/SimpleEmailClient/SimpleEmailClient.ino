#include <ESP8266WiFi.h>
const char* SSID = "HP LaserJet P1505n";
const char* PASS = "vQ1tASKOUu";
char server[] = "smtp.1und1.de";

WiFiClient client;
byte sendEmail();
byte eRcv();

void setup()
{
  Serial.begin(74880);
  delay(10);
  Serial.println("");
  Serial.println("");
  Serial.print("Connecting To ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  byte ret = sendEmail();
}

void loop()
{
}

byte sendEmail()
{
  Serial.print(__func__);  
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
  client.println(F("From: esp8266@gries.name"));
  client.println(F("Subject: ESP8266 email test\r\n"));
  client.println(F("Device: Wemos D1 mini"));
  client.print(F("IP: "));
  client.println(WiFi.localIP());
  client.print(F("RSSI: "));
  client.print(WiFi.RSSI());
  client.println(F(" dBm"));
  client.println(F("Sketch: SimpleEmailClient"));
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

// - See more at: http://www.esp8266.com/viewtopic.php?f=32&t=6139&start=8#sthash.YN7XmreR.dpuf
// - See also https://de.wikipedia.org/wiki/Simple_Mail_Transfer_Protocol
