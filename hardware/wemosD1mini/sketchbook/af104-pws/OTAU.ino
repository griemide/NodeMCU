/*
 * SKETCH:
 *  OTAU.ino  (Over The Air Update)
 * 
 *  (c) 2016, Michael Gries
 *  Creation: 2016-08-15 (based on BasicOTA.ino)
 *  Modified: 2016-08-16 (editorial changes)
 * 
 * PREREQUISITES:
 *   deviceIPstring defined in main sketch
 *  
 * LINKS:
 *   see http://192.168.0.171/update
 *   see file://C:/Users/griemide/AppData/Local/Temp
 *   see file://C:/Users/griemide/AppData/Local/Temp/build8cd6792565aebefbc5f65d65d5e446e2.tmp
 * 
 * ISSUES:
 *   none
 *   
 */

void HandleOTAUpdater(){
  MDNS.begin(host);
  httpUpdater.setup(&httpServer);
  httpServer.begin();
  MDNS.addService("http", "tcp", 80);
  Serial.print(__func__); Serial.print(": OTA: "); Serial.println("HTTPUpdateServer ready!");
  Serial.print(__func__); Serial.print(": OTA: "); Serial.printf("open http://%s/update in browser\n", deviceIPstring);
}

