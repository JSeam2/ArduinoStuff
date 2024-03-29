/* Project: Digital Pulse Meter
   Description: Arduino processes the output of Easy Pulse Plugin sensor to compute
   the heart beat rate and display it on seven segment LEDs.
   
   Written by: Raj Bhatt (www.Embedded-Lab.com)
   Date: May 5, 2015
   
   This is super inaccurate for the sensor we are using. It could be that the infrared detection
   mechanism is not the most reliable way of detecting heart rate. A better mechanism might 
   require the use of electrodes. The peak detection mechanism does not work. The range somehow
   decreases, it could be an issue with the kind of electrodes being used. 
   
   For the mvp, we might need to use some kind of random generator to simulate the values. This 
   is disappointing. Oh well.
*/

/*
#include "LedControl.h"
// Arduino Pin 7 to DIN, 6 to Clk, 5 to LOAD, no.of devices is 1
LedControl lc=LedControl(7,6,5,1);
*/

#define Sampling_Time 5
#define Num_Samples 700
#define Peak_Threshold_Factor 85
#define Minimum_Range 500
#define Minimum_Peak_Separation 75  // 80*5 = 400 ms
#define DC_Added 10
#define Moving_Average_Num 5

void setup() {
  // Initialize the MAX7219 device
  //  lc.shutdown(0,false);   // Enable display
  //  lc.setIntensity(0,15);  // Set brightness level (0 is min, 15 is max)
  //  lc.clearDisplay(0);     // Clear display register
  Serial.begin(115200);
}

int ADC_Samples[Num_Samples], Index1, Index2, Index3, Peak1, Peak2, Peak3;
long Pulse_Rate, Temp1=1L, Pulse_Time1, Pulse_Time2;
int Peak_Magnitude, Peak_Threshold, Minima, Range;

void loop() {
  
  Read_ADC_Samples();
  Serial.println("Sample Read Finished ");
  Remove_DC();
  Serial.println("DC component subtracted ");
  Scale_Data();
  Serial.println("Data scaled ");
  
  if (Range > Minimum_Range){  // ADC range is > 70, otherwise increase gain
    Filter_Data();
    Serial.println("Data Filtered ");
    Compute_Pulse_Rate();
    // we can comment this out since we are not using an lcd display
    //Serial.println("Pulse rate computed ");
    //Display_PulseRate();

  }
  else{
    Serial.println("ERROR: cannot detect pulse signal");
    //If we have an lcd display 
    //Print_Error_Message();
  }
  
}  // Main Loop
    
// Read ADC samples at a predefined interval
 void Read_ADC_Samples(){
   for (int i = 0; i < Num_Samples; i++){
      ADC_Samples[i] = analogRead(A0);
      delay(Sampling_Time);  
   }  
 }

 void Remove_DC(){
   Find_Minima(0);
   Serial.print("Minima = ");
   Serial.println(Minima);
   // Subtract DC (minima) 
   for (int i = 0; i < Num_Samples; i++){
     ADC_Samples[i] = ADC_Samples[i] - Minima;
   }
   Minima = 0;  // New minima is zero  
 }  // Remove_DC

void Find_Minima(int Num){
  Minima = 1024;
  for (int m = Num; m < Num_Samples-Num; m++){
      if(Minima > ADC_Samples[m]){
         Minima = ADC_Samples[m];
      }
  }
}  // Find_Minima  

void Scale_Data(){
  // Find peak value
  Find_Peak(0);
  Serial.print("Peak = ");
  Serial.println(Peak_Magnitude);
  Serial.print("Minima = ");
  Serial.println(Minima);
  Range = Peak_Magnitude - Minima;
  Serial.print("Range = ");
  Serial.println(Range);
  // Sclae from 1 to 1023 
  for (int i = 0; i < Num_Samples; i++){
     ADC_Samples[i] = map(ADC_Samples[i], 0, Range, 1, 1023);
     
  }
  Find_Peak(0);
  Find_Minima(0);
  Serial.print("Scaled Peak = ");
  Serial.println(Peak_Magnitude);
  Serial.print("Scaled Minima = ");
  Serial.println(Minima);
  
} // Scale_Data

void Find_Peak(int Num){
  Peak_Magnitude = 0;
  for (int m = Num; m < Num_Samples-Num; m++){
      if(Peak_Magnitude < ADC_Samples[m]){
        Peak_Magnitude = ADC_Samples[m];
     }
  }
} // Find_Peak

void Filter_Data(){
  int Num_Points = 2*Moving_Average_Num+1;
  for (int i = Moving_Average_Num; i < Num_Samples-Moving_Average_Num; i++){
    int Sum_Points = 0;
    for(int k =0; k < Num_Points; k++){   
      Sum_Points = Sum_Points + ADC_Samples[i-Moving_Average_Num+k]; 
    }    
    ADC_Samples[i] = Sum_Points/Num_Points; 
  } 
}  // Filter_Date


void Compute_Pulse_Rate(){
  // Detect Peak magnitude and minima
  Find_Peak(Moving_Average_Num);
  Find_Minima(Moving_Average_Num);
  Range = Peak_Magnitude - Minima;
  Peak_Threshold = Peak_Magnitude*Peak_Threshold_Factor;
  Peak_Threshold = Peak_Threshold/100;
 
  // Now detect three peaks 
  Peak1 = 0;
  Peak2 = 0;
  Peak3 = 0;
  Index1 = 0;
  Index2 = 0;
  Index3 = 0;
  // Find first peak
  for (int j = Moving_Average_Num; j < Num_Samples-Moving_Average_Num; j++){
      if(ADC_Samples[j] >= ADC_Samples[j-1] && ADC_Samples[j] > ADC_Samples[j+1] && 
         ADC_Samples[j] > Peak_Threshold && Peak1 == 0){
        Peak1 = ADC_Samples[j];
        Index1 = j; 
      }
      
      // Search for second peak which is at least 10 sample time far
      if(Peak1 > 0 && j > (Index1+Minimum_Peak_Separation) && Peak2 == 0){
         if(ADC_Samples[j] >= ADC_Samples[j-1] && ADC_Samples[j] > ADC_Samples[j+1] && 
         ADC_Samples[j] > Peak_Threshold){
         Peak2 = ADC_Samples[j];
         Index2 = j; 
         } 
      } // Peak1 > 0
      
      // Search for the third peak which is at least 10 sample time far
      if(Peak2 > 0 && j > (Index2+Minimum_Peak_Separation) && Peak3 == 0){
         if(ADC_Samples[j] >= ADC_Samples[j-1] && ADC_Samples[j] > ADC_Samples[j+1] && 
         ADC_Samples[j] > Peak_Threshold){
            Peak3 = ADC_Samples[j];
            Index3 = j; 
         } 
      } // Peak2 > 0
    
  }
  Serial.print("Index1 = ");
  Serial.println(Index1);
  Serial.print("Index2 = ");
  Serial.println(Index2);
  Serial.print("Index3 = ");
  Serial.println(Index3);
  
  
  Pulse_Time1 = (Index2-Index1)*Sampling_Time; // In milliseconds
  Pulse_Time2 = (Index3-Index2)*Sampling_Time;
  
  if(Pulse_Time1 > 0 && Pulse_Time1 > 0){
    Pulse_Rate = 2*60000/(Pulse_Time1+Pulse_Time2); // In BPM
  }
  
  Serial.print("Pulse Rate (BPM) = ");
  Serial.println(Pulse_Rate);
  Serial.println("");
}  // Compute_Pulse_Rate


/* only use this if we have an lcd display
 void Display_PulseRate(){ 
  
  // Display BPM
  lc.clearDisplay(0);     // Clear display register
  lc.setDigit(0,0,Pulse_Rate%10,false);
  lc.setDigit(0,1,(Pulse_Rate/10)%10,false);
  if(Pulse_Rate > 99) 
  lc.setDigit(0,2,Pulse_Rate/100,false);
 
  delay(100);
  
}  // Display_Pulserate

 void Print_Error_Message(){
  lc.setChar(0, 3, 'E', false); 
  lc.setChar(0, 2, 'E', false); 
  lc.setChar(0, 1, 'E', false);
  lc.setChar(0, 0, 'E', false);
 }
*/
