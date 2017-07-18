/* LM35DZ temperature sensor with bluetooth test 
*/

#include <SoftwareSerial.h> // for bluetooth

//LM35DZ pin var
int tempPin = 1;

//Bluetooth intialization TXD D2, RXD D3
SoftwareSerial BTserial(2,3);


void setup(){
  Serial.begin(9600);
  BTserial.begin(9600);
}

void loop(){
  // get voltage reading from the temperature sensor
  int tempVal = analogRead(tempPin);
  float voltage = tempVal* 5.0;
  voltage /= 1024.0;
  //Serial.print(voltage); Serial.println(" V");
  
  // Convert voltage to celsius
  // ??? V - 0.5V * 100 C = tempC
  float tempC = (voltage) * 100;
  Serial.print(tempC); Serial.println(" deg C");

  BTserial.print(tempC); BTserial.println(" C;");
  
  delay(1000);
}


