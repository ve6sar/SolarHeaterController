#include <OneWire.h>

// Solar heater fan controller Version 2
//
// Written for Version 1 Control PC boards
// 
// 1-wire Data Hooked to Pin 2
// Fan PWM Pin 11
// Fan MOSFET Pin 4
// Fan Speed Pin 12

OneWire ds(2);  // on Pin 2

void setup() {
  // Start Serial for Monitoring
  Serial.begin(9600);
  Serial.println ("Sean's Amazing Solar Fan Controller");
  Serial.println ("Software Version 2");
  Serial.println ("+++++++++++++++++++++++++++++++++++");
}

void loop() {
  // put your main code here, to run repeatedly:

}

void readTemps () {

}

