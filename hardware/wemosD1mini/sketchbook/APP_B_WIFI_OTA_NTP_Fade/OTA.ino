const char* host = "192.168.0.43"; // assign ESP to this IP in Router config

void ota(){
  Serial.println("OTA:");
  MDNS.begin(host);
  httpUpdater.setup(&httpServer);
  httpServer.begin();
  MDNS.addService("http", "tcp", 80);
  Serial.println(" .setup:        \tHTTPUpdateServer ready!");
  Serial.printf("Open \nhttp://%s/update \nin browser\n", host);
}

