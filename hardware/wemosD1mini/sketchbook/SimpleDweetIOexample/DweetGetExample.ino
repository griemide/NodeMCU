/*
 * M. Gries, 2016-06-05
 * 
 * Github source used: https://github.com/gamo256/dweet-esp/
 * 
 * modified Libraries: dweetESP8266.cpp
 * 
 */

#include "dweetESP8266.h"
#define THIG_NAME  "af104-test"  // Put here your thing name
#define key  "date_Time"  // Put here your requested key
#define WIFISSID "HP LaserJet P1505n"
#define PASSWORD "vQ1tASKOUu"

dweet client;

void setup() {
    Serial.begin(74880);
    delay(10);
    client.wifiConnection(WIFISSID, PASSWORD);
}

void loop() {
    // specifies the args of type "String"
    String value = client.getDweet(THIG_NAME, key);
    Serial.print(key); Serial.print("=");
    Serial.println(value);
}
