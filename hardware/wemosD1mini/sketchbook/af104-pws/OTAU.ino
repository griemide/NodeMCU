/*
 * SKETCH:
 *  OTAU.ino  (Over The Air Update)
 * 
 *  (c) 2016, Michael Gries
 *  Creation: 2016-08-15 (based on BasicOTA.ino)
 *  Modified: 2016-08-16 (editorial changes)
 * 
 * PREREQUISITES:
 *   none
 *  
 * LINKS:
 *   see http://192.168.0.171/update
 *   see file://C:/Users/griemide/AppData/Local/Temp
 * 
 * ISSUES:
 *   none
 *   
 */

const char* UpdateHost = "192.168.0.171"; // assign ESP to this IP in Router config

void HandleOTAUpdater(){
  Serial.println("OTA:");
  MDNS.begin(host);
  httpUpdater.setup(&httpServer);
  httpServer.begin();
  MDNS.addService("http", "tcp", 80);
  Serial.println(" .setup:        \tHTTPUpdateServer ready!");
  Serial.printf("Open \nhttp://%s/update \nin browser\n", UpdateHost);
}

