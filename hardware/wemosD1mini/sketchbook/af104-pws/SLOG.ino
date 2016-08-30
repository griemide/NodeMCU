/*
 * SKETCH:
 *  SLOG.ino  (Serial LOG support)
 * 
 *  (c) 2016, Michael Gries
 *  Creation: 2016-08-30 (based on TelnetServer.ino)
 *  Modified: 2016-08-309 (feasability study only )
 * 
 * PREREQUISITES:
 *   none
 *  
 * LINKS:
 *   see t.b.d.
 *   
 * ISSUES:
 *   none
 *   
 */

void printSerToTelnet(char *LogMessage); // TNET.ino

void SerialLog(String func, String message) {
  char logMessage[81];  char *cFunc = &func[0];   char *cMsg = &message[0];
  sprintf(logMessage, "%s: %s", cFunc, cMsg);
  Serial.println(logMessage);   // send to COM-port
  printSerToTelnet(logMessage); // mirror message to client via telnet
}

