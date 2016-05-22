
const char* ssid = "HP LaserJet P1505n";
const char* password = "vQ1tASKOUu";

void wifi(){
  Serial.println("WiFi:");
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  while(WiFi.waitForConnectResult() != WL_CONNECTED){
    WiFi.begin(ssid, password);
    Serial.println("WiFi failed, retrying.");
  }
  IPAddress localhost = WiFi.localIP();
  Serial.print(" .localIP:          \t");  Serial.println(localhost); 
  Serial.println(" .printDiag: "); 
  Serial.println(WiFi.RSSI()); 
  WiFi.printDiag(Serial); // will print out some diagnostic info
}

