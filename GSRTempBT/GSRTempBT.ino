/* ===temp===
 *  LM35DZ temperature sensor with bluetooth test 
 *  Read analog pin, this gives a value from 0 - 1024
 *  Scale reading using the 5V input. 0- 1024 * 5 / 1024 = Voltage reading 
 *  Voltage reading * 100 = temperature in deg C
 *  We assume linearity, it should be good enough.
 *  
 *  LM35 -> ARD
 *  GND -> GND
 *  +Vs -> 5V
 *  Vout -> A1
 *  
 * ===GSR===
 * TP4 pin is not used
 * 
 * GSR -> ARD
 * GND -> GND
 * VCC -> 5V
 * SIG -> A2
 * 
 * GSR measures changes in conductivity in skin due to sweat production.
 * Strong emotions can cause the sympathetic nervous system to react
 * by producing different amount of sweat
 */


#include <SoftwareSerial.h> // for bluetooth

// Signal Pins
const int tempPin = A1;
const int gsrPin = A2;

//Bluetooth intialization TXD D2, RXD D3
SoftwareSerial BTserial(2,3);

void setup(){
  Serial.begin(9600);
  BTserial.begin(9600);
  // give arduino some time to adjust
  delay(1000);
}

void loop(){
  // Temperature data
  int tempVal = analogRead(tempPin);
  float voltage = tempVal* 5.0 / 1024.0;
  float tempC = (voltage) * 100;

  // GSR data
  int gsrValue = analogRead(gsrPin);

  // Serial output
  Serial.print(tempC); Serial.println(" C");
  BTserial.print(tempC); BTserial.println(" C;");

  Serial.print(gsrValue); Serial.println(" G");
  BTserial.print(gsrValue); BTserial.println(" G;");
  
  delay(500);
}


