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

int portTelnet = 23;

 // declare telnet server (do NOT put in setup())
WiFiServer telnetServer(portTelnet);
WiFiClient serverClient;

void printSerToTelnet(char *LogMessage) {
  if (serverClient && serverClient.connected()) {  // send data to Client
      serverClient.println(LogMessage);
  }
}

void RunTelnetSession() {
  telnetServer.begin();
  telnetServer.setNoDelay(true);
  sprintf(logMessageBuffer, "Telnet server startet on port: %d", portTelnet);
  SerialLog(__func__, logMessageBuffer);
}


void runTelnetAliveHB()
{
  // look for Client connect trial
  if (telnetServer.hasClient()) {
    if (!serverClient || !serverClient.connected()) {
      if (serverClient) {
        serverClient.stop();
        sprintf(logMessageBuffer, "Telnet Client Stop: %d ms", millis() );
        SerialLog(__func__, logMessageBuffer);
      }
      serverClient = telnetServer.available();
      sprintf(logMessageBuffer, "New Telnet client: %d ms", millis() );
      SerialLog(__func__, logMessageBuffer);
      serverClient.flush();  // clear input buffer, else you get strange characters 
    }
  }

  // read from telnet client and copy to Serial
  while(serverClient.available()) {  // get data from Client
    Serial.write(serverClient.read());
  }
 
  // print keep alive message periodically
  sprintf(logMessageBuffer, "Telnet keep alive message: %d ms", millis() );
  SerialLog(__func__, logMessageBuffer);
}

