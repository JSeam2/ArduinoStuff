/*
Heartrate detector
*/

// Variables
int HRinput = A0;
int LED = 13; //on-board arduino LED

// Incoming data
int Signal; // for incoming signal from the heart rate sensor, this is a value from 0-1024
int Threshold = 550; // Determine what signal counts as a beat, if it is too low it could be noise. Determine experimentally

// Derived data
volatile int rate[10];


void setup(){
  pinMode(LED, OUTPUT);
  pinMode(HRinput, INPUT);
  Serial.begin(115200);
}

void loop(){
  Signal = analogRead(HRinput); // read heart rate value
  Serial.println(Signal); // print signal value

  // Check if the signal is above threshold
  if (Signal > Threshold){
    Serial.println("!!!!!!!!!Beat!!!!!!!");
    digitalWrite(LED, HIGH);}
  
  else{
   digitalWrite(LED, LOW);
  } 
}

