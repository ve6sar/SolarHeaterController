// Solar Heater Fan Controller with 20 x 4 LCD Sketch
// Description of Operation
//
// Using two 1-wire temperature sensors one on the inlet of the solar screen heater 
// and one in inside the heater in front of the oulet fan. The arduino compares the 
// temperatures and if there's a differnece of more then 5C a relay is closed turning
// on power to a PC fan that is PWM speed controlled.
//
// Once the fan is turned on the speed is automaticly adjusted based on the temperature 
// difference. If the temp difference drops below 5C the relay contacts are opened and 
// the fan shuts off.
// 
// The PWM output on Pin 9 must be adjusted to 31kHz by a function making a direct call 
// to the micro controller

// Required Libraries
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>


// Data wire is plugged into pin 3 on the Arduino
#define ONE_WIRE_BUS 3

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// Assign the addresses of your 1-Wire temp sensors.
// See the tutorial on how to obtain these addresses:
// http://www.hacktronics.com/Tutorials/arduino-1-wire-address-finder.html

DeviceAddress insideThermometer = { 0x28, 0xFF, 0xB4, 0x8A, 0x65, 0x14, 0x01, 0xC3 };
DeviceAddress outsideThermometer = { 0x28, 0xFF, 0xDF, 0x3C, 0x65, 0x14, 0x01, 0x3A };
//DeviceAddress soilThermometer = { 0x28, 0xFF, 0xF2, 0x8D, 0x65, 0x14, 0x01, 0x30 };


// Lets Define our variables
int fanPin = 11;  // Sets our fan speed controll pin
int fanSpeed = 0; // Sets the initial speed for the fan
int fanRelay = 4; // The fan relay control pin
int fanState = 1;  // The fan state
int diff = 5; //The temperature difference required to turn the fan on
int tempDiff;
int calFactor = 11; // The multiplier for setting the fan speed must be adjusted for desired temp diff to be maintained.
float fanSpeedDisp;

// Functions

// See http://playground.arduino.cc/Code/PwmFrequency for details
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}

void setup() {
  pinMode(fanPin, OUTPUT); //Set the fan relay control pin as an output
 
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

 
  //Set Pin 9 PWM to 31.250 KHz
  setPwmFrequency(9, 1);

 
  // Start up the 1-wire library
  sensors.begin();
  // set the resolution to 10 bit 0.125C increments
  sensors.setResolution(insideThermometer, 10);
  sensors.setResolution(outsideThermometer, 10);
//  sensors.setResolution(soilThermometer, 10);
  
  // Set Fan relay pin mode
  pinMode(fanRelay, OUTPUT); 
 
 }
 

void loop() {   
  delay(1000); // 10 second delay in the loop
  Serial.print("Getting temperatures...\n\r");
  sensors.requestTemperatures();
  
  float inC=sensors.getTempC(insideThermometer);
  float outC=sensors.getTempC(outsideThermometer);
  //float soilC=sensors.getTempC(soilThermometer);
  
  //Determin if the fan is needed
  float diffTemp = inC - outC; // Get the temperature difference
  int diffTempD = (int)diffTemp; // change diffTemp to an int
  
  if(diffTemp >= diff) { //Turn our fan on
    digitalWrite(fanRelay, HIGH);
    fanState = 1; 
  } else {  // Turn it off if there's not enough temp difference
    digitalWrite (fanRelay, LOW);
    fanState = 0;
    fanSpeed = 0;
  }
 
  //Set fan speed 
  if(fanState == 1){
     fanSpeed = (diffTemp - 4) * calFactor; // This determines our fan speed we take 4c off to compensate for the turn on difference
     fanSpeed = (int) fanSpeed;
     if(fanSpeed > 255) {
       fanSpeed = 255;
     }
     analogWrite(fanPin, fanSpeed);
     Serial.print("Fan Speed: ");
     Serial.print(fanSpeed);
     Serial.print("\r\n");
     float fanSpeedDispF = fanSpeed / 255.0 *100; 
     fanSpeedDisp = (int)fanSpeedDispF;
         
     Serial.print("Fan Speed %: ");
     Serial.print(fanSpeedDisp);
     Serial.print("\r\n");
  }
  
  
  //Clear the serial screen only works with putty etc.
 // Serial.write(27);       // ESC command
 // Serial.print("[2J");    // clear screen command
 // Serial.write(27);
 // Serial.print("[H");     // cursor to home command
  
  //Write our values to the Serial Port
  Serial.print("Solar Heater Version 1 \n\r\n\r");
  Serial.print("Outside Temp: ");
  Serial.print(outC);
  Serial.print(" C \n\r");
  Serial.print("Inside Temp: ");
  Serial.print(inC);
  Serial.print(" C \n\r");
  Serial.print("Temp Difference: ");
  Serial.print(diffTempD);
  Serial.print(" C \n\r");
  Serial.print("Fan Speed: ");
  Serial.print(fanSpeedDisp);
  Serial.print("\n\r\n\r");
//  Serial.print("External Temp: ");
//  Serial.print(soilC);
//  Serial.print(" C \n\r\n\r");
   
} 
