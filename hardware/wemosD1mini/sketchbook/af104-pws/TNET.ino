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
 *   see https://gist.github.com/atomsfat/1813823
 *   
 * ISSUES:
 *   none
 *   
 */

#include <ESP8266WiFi.h>
#include <Arduino.h>

int portTelnet = 23;

// Other global variables
#define textBuffSize 9 //length of longest command string plus two spaces for CR + LF
char textBuff[textBuffSize]; //someplace to put received text
int charsReceived = 0;

boolean connectFlag = 0; //we'll use a flag separate from client.connected
         //so we can recognize when a new connection has been created
unsigned long timeOfLastActivity; //time in milliseconds of last activity
unsigned long allowedConnectTime = 300000; //five minutes



void getReceivedText();
void checkConnectionTimeout();
void parseReceivedText();
void printErrorMessage();
void printHelpMessage();
void checkCloseConnection();
void closeConnection();

byte sendEmailViaSMTP();


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


void handleTelnetServ() {
  // look for Client connect trial
  if (telnetServer.hasClient()) {
    if (!serverClient || !serverClient.connected()) {
      if (serverClient) {
        serverClient.stop();  telnetActive = false;  
        sprintf(logMessageBuffer, "Telnet Client Stop: %d ms", millis() );
        SerialLog(__func__, logMessageBuffer);
      }
      telnetActive = true;  
      serverClient = telnetServer.available();
      sprintf(logMessageBuffer, "New Telnet client: %d ms", millis() );
      SerialLog(__func__, logMessageBuffer);
      serverClient.flush();  // clear input buffer, else you get strange characters 
    }
  }

  // read from telnet client and copy to Serial
  while(serverClient.available() ) {  // get data from Client
    getReceivedText();
    //Serial.write(serverClient.read() );
  }
}

void runTelnetAliveHB() {
  if (telnetActive) {
    // print keep alive message periodically
    sprintf(logMessageBuffer, "Telnet keep alive message: %d ms", millis() );  SerialLog(__func__, logMessageBuffer);
  }
  // check to see if connection has timed out
  if(connectFlag) checkConnectionTimeout();

}


void printPrompt()
{
  timeOfLastActivity = millis();
  serverClient.flush();
  charsReceived = 0; //count of characters received
  serverClient.print("\n>");
}


void checkConnectionTimeout()
{
  if(millis() - timeOfLastActivity > allowedConnectTime) {
    serverClient.println();
    serverClient.println("Timeout disconnect.");
    serverClient.stop();
    connectFlag = 0;
  }
}


void getReceivedText()
{
  char c;
  int charsWaiting;

  // copy waiting characters into textBuff
  //until textBuff full, CR received, or no more characters
  charsWaiting = serverClient.available();
  do {
    c = serverClient.read();
    textBuff[charsReceived] = c;
    charsReceived++;
    charsWaiting--;
  }
  while(charsReceived <= textBuffSize && c != 0x0d && charsWaiting > 0);

  //if CR found go look at received text and execute command
  if(c == 0x0d) {
    parseReceivedText();
    // after completing command, print a new prompt
    printPrompt();
  }

  // if textBuff full without reaching a CR, print an error message
  if(charsReceived >= textBuffSize) {
    serverClient.println();
    printErrorMessage();
    printPrompt();
  }
  // if textBuff not full and no CR, do nothing else;
  // go back to loop until more characters are received

}


void parseReceivedText()
{
  // look at first character and decide what to do
  switch (textBuff[0]) {
    //case 'a' : doAnalogCommand();    break;
    //case 'd' : doDigitalCommand();   break;
    //case 'p' : setPinMode();     break;
    case 'm' : sendEmailViaSMTP();   break;
    case 'c' : checkCloseConnection();   break;
    case '?' : printHelpMessage();   break;
    case 0x0d :         break;  //ignore a carriage return
    default: printErrorMessage();   break;
  }
}

void printErrorMessage()
{
  serverClient.println("Unrecognized command.  ? for help.");
}


void printHelpMessage()
{
  serverClient.println("\nExamples of supported commands:\n");
  serverClient.println("  dr   -digital read:   returns state of digital pins 0 to 9");
  serverClient.println("  dr4 -digital read:   returns state of pin 4 only");
  serverClient.println("  ar   -analog read:    returns all analog inputs");
  serverClient.println("  dw0=hi   -digital write:  turn pin 0 on  valid pins are 0 to 9");
  serverClient.println("  dw0=lo   -digital write:  turn pin 0 off valid pins are 0 to 9");
  serverClient.println("  aw3=222  -analog write:   set digital pin 3 to PWM value 222");
  serverClient.println("          allowed pins are 3,5,6,9");
  serverClient.println("          allowed PWM range 0 to 255");
  serverClient.println("  pm0=in   -pin mode:  set pin 0 to INPUT  valid pins are 0 to 9");
  serverClient.println("  pm0=ou   -pin mode:  set pin 0 to OUTPUT valid pins are 0 to 9");
  serverClient.println("  cl   -close connection");
  serverClient.println("  ?   -print this help message");
}

void checkCloseConnection()
  // if we got here, textBuff[0] = 'c', check the next two
  // characters to make sure the command is valid
{
  if (textBuff[1] == 'l' && textBuff[2] == 0x0d)
    closeConnection();
  else
    printErrorMessage();
}


void closeConnection()
{
  serverClient.println("\nBye.\n");
  serverClient.stop();
  connectFlag = 0;
}




