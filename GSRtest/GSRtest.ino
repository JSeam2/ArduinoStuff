/*
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

const int GSRpin = A2;
int GSRthresh = 0;
int GSRvalue;


void setup() {
  // put your setup code here, to run once:
  long sum = 0;
  Serial.begin(9600);
  Serial.println("GSR start!");
  
   // sample the threshold you should be wearing the GSR 
  // This is to give some baseline readings
  // sample 500 times with 5ms delay (2.5s)
  // uncomment the code to use threshold. 
/*  delay(1000);
 *   for(int i = 0; i<500; i++){
    GSRvalue = analogRead(GSRpin);
    sum += GSRvalue;
    delay(5); 
  }
  GSRthresh = sum/500;

  Serial.print("GSR Threshold: "); Serial.println(GSRthresh);
*/ 
}

void loop() {
  int GSRtemp;
  GSRvalue = analogRead(GSRpin);
  Serial.println(GSRvalue); //Serial.println("G");
  delay(500);
}
  

