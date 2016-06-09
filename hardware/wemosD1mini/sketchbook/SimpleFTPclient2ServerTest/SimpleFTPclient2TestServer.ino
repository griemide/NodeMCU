/*
 * M. Gries 2016-05-30
 * modified 2016-05-09: PROGMEN  controls removed
 * File: SimpleFTPclient2TestServer
 * 
 * Online FTP test: https://ftptest.net/
 * 
 * Test Server: speedtest.tele2.net
 * anonymous login (plain FTP)
 * 
 * Successful coplied and tested with Arduino IDE 1.6.7
 * 
 * granted from:
 * File: WiWi_FTP_Client.ino for ESP8266 NodeMCU
 * http://www.rudiswiki.de/wiki9/WiFiFTPServer
 * 
 * 2016-05-30 adapted to Wemos D1 mini (no 2nd build-in LED) TODO
 */ 
 
/*
   Original Arduino: FTP passive client 
   http://playground.arduino.cc/Code/FTP
   Modified 6 June 2015 by SurferTim

   You can pass flash-memory based strings to Serial.print() by wrapping them with F().

   2015-12-09 Rudolf Reuter, adapted to ESP8266 NodeMCU, with the help of Markus.
*/
#include <ESP8266WiFi.h>
#include <FS.h>

// comment out next line to write to SD from FTP server
#define FTPWRITE

// Set these to your desired softAP credentials. They are not configurable at runtime.
const char *ssid = "HP LaserJet P1505n";
const char *password = "vQ1tASKOUu";

//boolean debug = false;  // true = more messages
boolean debug = true;

// LED is needed for failure signalling
const short int BUILTIN_LED2 = 16;  //GPIO16 on NodeMCU (ESP-12)

unsigned long startTime = millis();

// provide text for the WiFi status
const char *str_status[]= {
  "WL_IDLE_STATUS",
  "WL_NO_SSID_AVAIL",
  "WL_SCAN_COMPLETED",
  "WL_CONNECTED",
  "WL_CONNECT_FAILED",
  "WL_CONNECTION_LOST",
  "WL_DISCONNECTED"
};

// provide text for the WiFi mode
const char *str_mode[]= { "WIFI_OFF", "WIFI_STA", "WIFI_AP", "WIFI_AP_STA" };

// change to your server
// IPAddress server( 217, 160, 223, 139 ); // www.gries.name
// IPAddress server( 136, 243, 154, 86  ); // ftptest.net
   IPAddress server( 90, 130, 70, 73  ); // speedtest.tele2.net


WiFiClient client;
WiFiClient dclient;

char outBuf[128];
char outCount;

// change fileName to your file (8.3 format!)
String fileName = "FileUploadTest.txt";
String  path = "/FileUploadTest.txt";

// SPIFFS file handle
File fh;

void signalError() {  // loop endless with LED blinking in case of error
  while(1) {
      digitalWrite(BUILTIN_LED2, LOW);
      delay(300); // ms
      digitalWrite(BUILTIN_LED2, HIGH);
      delay(300); // ms
  }
}

//format bytes
String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}

//----------------------- WiFi handling
void connectWifi() {
  Serial.print("DEBUG=");
  Serial.println(debug);
  Serial.print("Connecting as wifi client to SSID: ");
  Serial.println(ssid);

  // use in case of mode problem
  WiFi.disconnect();
  // switch to Station mode
  if (WiFi.getMode() != WIFI_STA) {
    WiFi.mode(WIFI_STA);
  }

  WiFi.begin ( ssid, password );

  if (debug ) WiFi.printDiag(Serial);

  // ... Give ESP 10 seconds to connect to station.
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  // Check connection
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi connected; IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.print("WiFi connect failed to ssid: ");
    Serial.println(ssid);
    Serial.print("WiFi password <");
    Serial.print(password);
    Serial.println(">");
    Serial.println("Check for wrong typing!");
  }
}  // connectWiFi()

//----------------- FTP fail
void efail() {
  byte thisByte = 0;

  client.println(F("QUIT"));

  while (!client.available()) delay(1);

  while (client.available()) {
    thisByte = client.read();
    Serial.write(thisByte);
  }

  client.stop();
  Serial.println(F("Command disconnected"));
  fh.close();
  Serial.println(F("SD closed"));
}  // efail

//-------------- FTP receive
byte eRcv() {
  byte respCode;
  byte thisByte;

  while (!client.available()) delay(1);

  respCode = client.peek();

  outCount = 0;

  while (client.available()) {
    thisByte = client.read();
    Serial.write(thisByte);

    if (outCount < 127) {
      outBuf[outCount] = thisByte;
      outCount++;
      outBuf[outCount] = 0;
    }
  }

  if (respCode >= '4') {
    efail();
    return 0;
  }
  return 1;
}  // eRcv()

//--------------- FTP handling
byte doFTP(boolean upload) {
  
  if (upload) {
    fh = SPIFFS.open(path, "r");
  } else {
    SPIFFS.remove(path);
    fh = SPIFFS.open(path, "w");
  }

  if (!fh) {
    Serial.println(F("SPIFFS open fail"));
    return 0;
  }

  if (upload) {
    if (!fh.seek((uint32_t)0, SeekSet)) {
      Serial.println(F("Rewind fail"));
      fh.close();
      return 0;
    }
  }

  if (debug) Serial.println(F("SPIFFS opened"));

  if (client.connect(server, 21)) {  // 21 = FTP server
    Serial.println(F("Command connected"));
  } else {
    fh.close();
    Serial.println(F("Command connection failed"));
    return 0;
  }

  if (!eRcv()) return 0;
  if (debug) Serial.println("\nSend USER anonymous");
  client.println("USER anonymous");

  if (!eRcv()) return 0;
  if (debug) Serial.println("Send PASSWORD");
  // client.println(F("PASS "));
  client.println("PASS ");

  if (!eRcv()) {
    if (debug) Serial.println("eRev Error");
    return 0;
  }
  if (debug) Serial.println("Send SYST");
  client.println("SYST");

  if (!eRcv()) return 0;
  if (debug) Serial.println("Send FEAT");
  client.println("FEAT");

  if (!eRcv()) return 0;
  if (debug) Serial.println("Send Type I");
  client.println("Type I");

  if (!eRcv()) return 0;
  if (debug) Serial.println("Send PASV");
  client.println("PASV");

  if (!eRcv()) return 0;

  char *tStr = strtok(outBuf, "(,");
  int array_pasv[6];
  for ( int i = 0; i < 6; i++) {
    tStr = strtok(NULL, "(,");
    array_pasv[i] = atoi(tStr);
    if (tStr == NULL) {
      Serial.println(F("Bad PASV Answer"));
    }
  }
  unsigned int hiPort, loPort;
  hiPort = array_pasv[4] << 8;
  loPort = array_pasv[5] & 255;

  if (debug) Serial.print(F("Data port: "));
  hiPort = hiPort | loPort;
  if (debug) Serial.println(hiPort);

  if (dclient.connect(server, hiPort)) {
    Serial.println(F("Data connected"));
  }
  else {
    Serial.println(F("Data connection failed"));
    client.stop();
    fh.close();
    return 0;
  }

  if (upload) {
    if (debug) Serial.println("Send STOR filename");
    client.print(F("STOR "));
    client.println(fileName);
  } else {
    if (debug) Serial.println("Send RETR filename");
    client.print(F("RETR "));
    client.println(fileName);
  }

  if (!eRcv()) {
    dclient.stop();
    return 0;
  }

  if (upload) {
    if (debug) Serial.println(F("Writing"));
    // for faster upload increase buffer size to 1460
//#define bufSizeFTP 64
#define bufSizeFTP 1460
    uint8_t clientBuf[bufSizeFTP];
    //unsigned int clientCount = 0;
    size_t clientCount = 0;
  
    while (fh.available()) {
      clientBuf[clientCount] = fh.read();
      clientCount++;
      if (clientCount > (bufSizeFTP - 1)) {
        dclient.write((const uint8_t *) &clientBuf[0], bufSizeFTP);
        clientCount = 0;
        delay(1);
      }
    }
    if (clientCount > 0) dclient.write((const uint8_t *) &clientBuf[0], clientCount);

  } else {
    while (dclient.connected()) {
      while (dclient.available()) {
        char c = dclient.read();
        fh.write(c);
        if (debug) Serial.write(c);
      }
    }
  }

  dclient.stop();
  Serial.println(F("Data disconnected"));

  if (!eRcv()) return 0;

  client.println(F("QUIT"));

  if (!eRcv()) return 0;

  client.stop();
  Serial.println(F("Command disconnected"));

  fh.close();
  if (debug) Serial.println(F("SPIFS closed"));
  return 1;
}  // doFTP()


void readSPIFFS() {
  fh = SPIFFS.open(fileName, "r");

  if (!fh) {
    Serial.println(F("SPIFFS open fail"));
    return;
  }

  while (fh.available()) {
    Serial.write(fh.read());
  }

  fh.close();
}  // readSPIFFS()


void setup() {
  delay(1000);
  Serial.begin(74880);
  delay(1000);
  Serial.println("Sync,Sync,Sync,Sync,Sync");
  delay(500);
  Serial.println();
  // signal start
  pinMode(BUILTIN_LED2, OUTPUT);
  digitalWrite(BUILTIN_LED2, LOW);
  delay(100); // ms
  digitalWrite(BUILTIN_LED2, HIGH);
  delay(300); // ms

  Serial.print("Chip ID: 0x");
  Serial.println(ESP.getChipId(), HEX);

  Serial.println ( "Connect to Router requested" );
  connectWifi();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi mode: ");
    Serial.println(str_mode[WiFi.getMode()]);
    Serial.print ( "Status: " );
    Serial.println (str_status[WiFi.status()]);
    // signal WiFi connect
    digitalWrite(BUILTIN_LED2, LOW);
    delay(300); // ms
    digitalWrite(BUILTIN_LED2, HIGH);      
  } else {
    Serial.println("");
    Serial.println("WiFi connect failed, push RESET button.");
    signalError();
  }

  if (!SPIFFS.begin()) {
     Serial.println("SPIFFS failed, needs formatting");
     signalError();
  }

  fh = SPIFFS.open(path, "r");
  if (!fh) {
    Serial.print(__func__);
    Serial.println(F(": SPIFFS open fail"));
    signalError();
  } else fh.close();
  
  Serial.println(F("Ready. Press d, u or r"));
}  // setup()


void loop() {
  byte inChar;

  if (Serial.available() > 0) {
    inChar = Serial.read();
  }

boolean upload = true; // false = download

  if (inChar == 'd') {
    upload = false;
    if (doFTP(upload)) Serial.println(F("FTP OK"));
    else Serial.println(F("FTP FAIL"));
  }
  
  if (inChar == 'u') {
    upload = true;
    if (doFTP(upload)) Serial.println(F("FTP OK"));
    else Serial.println(F("FTP FAIL"));
  }  

  if (inChar == 'r') {
    String fileNameDir;
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      fileNameDir = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("FS File: %s, size: %s\n", fileNameDir.c_str(), formatBytes(fileSize).c_str());
    }
  }
  delay(10);
}

