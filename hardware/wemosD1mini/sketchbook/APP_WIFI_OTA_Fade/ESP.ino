/*
 * 
 * APIs related to deep sleep and watchdog timer are available in the ESP object, only available in Alpha version.
 * 
 * ESP.deepSleep(microseconds, mode) will put the chip into deep sleep. 
 * mode is one of WAKE_RF_DEFAULT, WAKE_RFCAL, WAKE_NO_RFCAL, WAKE_RF_DISABLED. 
 * (GPIO16 needs to be tied to RST to wake from deepSleep.)
 * 
 * ESP.restart() restarts the CPU.
 * ESP.getResetReason() returns String containing the last reset resaon in human readable format.
 * ESP.getFreeHeap() returns the free heap size.
 * ESP.getChipId() returns the ESP8266 chip ID as a 32-bit integer.
 * 
 * Several APIs may be used to get flash chip info:
 * ESP.getFlashChipId() returns the flash chip ID as a 32-bit integer.
 * ESP.getFlashChipSize() returns the flash chip size, in bytes, as seen by the SDK (may be less than actual size).
 * ESP.getFlashChipSpeed(void) returns the flash chip frequency, in Hz.
 * ESP.getCycleCount() returns the cpu instruction cycle count since start as an unsigned 32-bit. 
 * This is useful for accurate timing of very short actions like bit banging.
 * 
 * ESP.getVcc() may be used to measure supply voltage. 
 * ESP needs to reconfigure the ADC at startup in order for this feature to be available. 
 * Add the following line to the top of your sketch to use getVcc:
 * ADC_MODE(ADC_VCC); TOUT pin has to be disconnected in this mode.
 * Note that by default ADC is configured to read from TOUT pin using analogRead(A0), and ESP.getVCC() is not available.
 *
 */

void esp(){
  Serial.println("ESP:");
//ADC_MODE(ADC_VCC); //not supported
//Serial.print(" .getVCC:            \t"); Serial.println(ESP.getVCC()); // not supported
  Serial.print(" .getRestReason:     \t"); Serial.println(ESP.getResetReason());
  Serial.print(" .getFreeHeap:       \t"); Serial.println(ESP.getFreeHeap());
  String sESPchipID;  sESPchipID = ESP.getChipId();  
  char buf[12]; sESPchipID.toCharArray(buf, 10);
  Serial.printf(" .getChipId:        \t%s\n", buf );
  Serial.print(" .getFlashChipId:    \t"); Serial.println(ESP.getFlashChipId());
  Serial.print(" .getFlashChipSize:  \t"); Serial.println(ESP.getFlashChipSize());
  Serial.print(" .getFlashChipSpeed: \t"); Serial.println(ESP.getFlashChipSpeed());
  Serial.print(" .getCycleCount:     \t"); Serial.println(ESP.getCycleCount());
}

