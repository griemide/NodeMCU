/*
 * SKETCH:
 *  OTAU.ino  (Over The Air Update)
 * 
 *  (c) 2016, Michael Gries
 *  Creation: 2016-08-15 (based on BasicOTA.ino)
 *  Modified: 2016-08-19 (editorial changes)
 * 
 * PREREQUISITES:
 *   deviceIPstring defined in main sketch
 *  
 * LINKS:
 *   see http://192.168.0.41/update (ESP_0f791F productive system )
 *   see http://192.168.0.43/update (ESP_E1B43E testing system / Q-system)
 *   see file://C:/Users/griemide/AppData/Local/Temp
 *   see file://C:/Users/griemide/AppData/Local/Temp/build8cd6792565aebefbc5f65d65d5e446e2.tmp
 *   see http://esp8266.github.io/Arduino/versions/2.1.0-rc2/doc/ota_updates/ota_updates.html
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

