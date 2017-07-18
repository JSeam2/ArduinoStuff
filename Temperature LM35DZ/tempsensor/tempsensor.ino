/* LM35DZ temperature sensor test 
*/

//LM35DZ pin var
int tempPin = 1;

void setup(){
  Serial.begin(115200);
}

void loop(){
  // get voltage reading from the temperature sensor
  int tempVal = analogRead(tempPin);
  float voltage = tempVal* 5.0;
  voltage /= 1024.0;
  Serial.print(voltage); Serial.println(" V");
  
  // Convert voltage to celsius
  // ??? V - 0.5V * 100 C = tempC
  float tempC = (voltage) * 100;
  Serial.print(tempC); Serial.println(" deg C");
  
  delay(1000);
}


