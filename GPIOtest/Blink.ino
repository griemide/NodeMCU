/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
  
  adaptation 6 Matrch 2016 for NodeMCU V1
  by Michael Gries 
  
 */
 
#define LEDblue 2  // NodeMCU blue LED (pin D0)
#define LEDred 16  // NodeMCU red  LED (pin D4)

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(LEDblue, OUTPUT);
  pinMode(LEDred,  OUTPUT);
  Serial.begin(115200);
  Serial.print("\n\rBlink example for NodeMCU V1.0 loaded and executed ...");
}

// the loop function runs over and over again forever
void loop() {
  Serial.print(".");             // keep alive message on Serial Monitor
  digitalWrite(LEDblue, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(LEDred,  HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                   // wait for a second
  digitalWrite(LEDblue, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(LEDred,  LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                   // wait for a second
}
