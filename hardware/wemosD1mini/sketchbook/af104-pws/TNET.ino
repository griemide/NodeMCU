/*
 * SKETCH:
 *  TNET.ino  (TELNET support)
 * 
 *  (c) 2016, Michael Gries
 *  Creation: 2016-08-28 (based on TelnetServer.ino)
 *  Modified: 2016-08-29 (feasability study only )
 * 
 * PREREQUISITES:
 *   uses port 23
 *  
 * LINKS:
 *   see 2015-12-07 Rudolf Reuter www.rudiswiki.de/wiki9
 *   see https://en.wikipedia.org/wiki/Telnet
 *   see file://bbmagf1.bbmag.bbraun.com/Home/griemide/Documents/putty_0.63.exe (open PuTTY tool)
 *   
 * ISSUES:
 *   none
 *   
 */

#include <ESP8266WiFi.h>
#include <Arduino.h>

 // declare telnet server (do NOT put in setup())
WiFiServer telnetServer(23);
WiFiClient serverClient;


void RunTelnetSession() {
  telnetServer.begin();
  telnetServer.setNoDelay(true);
  Serial.println("Please connect Telnet Client, exit with ^] and 'quit'");
  if (serverClient && serverClient.connected()) {  // send data to Client
      serverClient.print("Telnet server on port 23 started");
  }
}


void runTelnetAliveHB()
{
  // look for Client connect trial
  if (telnetServer.hasClient()) {
    if (!serverClient || !serverClient.connected()) {
      if (serverClient) {
        serverClient.stop();
        Serial.println("Telnet Client Stop");
      }
      serverClient = telnetServer.available();
      Serial.println("New Telnet client");
      serverClient.flush();  // clear input buffer, else you get strange characters 
    }
  }

  // read from telnet client and copy to Serial
  while(serverClient.available()) {  // get data from Client
    Serial.write(serverClient.read());
  }
 
  // print keep alive messahe periodically
  if (serverClient && serverClient.connected()) {  // send data to Client
      serverClient.print(millis());

      serverClient.println(": Telnet feasability study");
  }
}




