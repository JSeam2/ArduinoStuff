/*  Modified Pulse Sensor adapted from Joel Murphy and Yury Gitman   http: //www.pulsesensor.com
----------------------  Notes ----------------------  ----------------------
This code:
1) Blinks an LED to User's Live Heartbeat   PIN 13
2) Determines BPM
3) Prints BPM to Serial
Read Me:
https: //github.com/WorldFamousElectronics/PulseSensor_Amped_Arduino/blob/master/README.md
 ----------------------       ----------------------  ----------------------
*/

/* ###Todo how the hell do I get ISR to go in the main loop!????


#define PROCESSING_VISUALIZER 1
#define SERIAL_PLOTTER 2

// outputType PROCESSING_VISUALIZER OR SERIAL_PLOTTER
// PROCESSING_VISUALIZER works with Pulse Sensor Processing Visualizer
int outputType = SERIAL_PLOTTER;


// Variables for pins
int pulsePin = 0;
int blinkPin = 13;

/*
Volatile Variables, used in interrupt service routine (ISR)
BPM: Holds raw analog in 0. Updated every 2mS
Signal: Hold incoming unprocessed data
IBI: In-between time interval
Pulse: True if heart beat is detected then False if there is not pulse
QS: Quantitative Self true when arduino finds a beat*/
volatile int BPM;
volatile int Signal;
volatile int IBI = 600;
volatile boolean Pulse = false;
volatile boolean QS = false;




void setup(){
  Serial.begin(115200);
  Serial.println("Code begin");
}

void loop(){
  // If QS finds a heartbeat 
  if (QS == true){
    serialOutput(); // A beat happened, output to serial
    QS = false;     // Reset QS to repeat the loop
  } 
 delay(20);         // Take a short break 
}




/*--------------------------------------------------
                  ALL SERIAL HANDLING
---------------------------------------------------*/
void serialOutput(){   // Decide How To Output Serial.
  switch(outputType){
    case PROCESSING_VISUALIZER:
      // Send data to the Serial
      sendDataToSerial('S', Signal); // send Signal with 'S' prefix
      sendDataToSerial('B',BPM);   // send heart rate with a 'B' prefix
      sendDataToSerial('Q',IBI);   // send time between beats with a 'Q' prefix     
      break;
    
    
    case SERIAL_PLOTTER:  
      // open the Arduino Serial Plotter to visualize these data
      Serial.print(BPM);
      Serial.print(",");
      Serial.print(IBI);
      Serial.print(",");
      Serial.println(Signal);
      break;
    
    
    default:
      break;
  }

}


//  Sends Data to Pulse Sensor Processing App, Native Mac App, or Third-party Serial Readers.
void sendDataToSerial(char symbol, int data ){
    Serial.print(symbol);
    Serial.println(data);
}


/*--------------------------------------------------
                  INTERRUPT
---------------------------------------------------*/
// Volatile variables
volatile int rate[10];                             // array to hold last ten IBI values

volatile unsigned long sampleCounter = 0;          // used to determine pulse timing
volatile unsigned long lastBeatTime = 0;           // used to find IBI

volatile int P =512;                               // used to find Peak in pulse wave, seeded
volatile int T = 512;                              // used to find Trough in pulse wave, seeded

volatile int thresh = 530;                         // used to find instant moment of heart beat, seeded
volatile int amp = 0;                              // used to hold amplitude of pulse waveform, seeded

volatile boolean firstBeat = true;                 // used to seed rate array so we startup with reasonable BPM
volatile boolean secondBeat = false;               // used to seed rate array so we startup with reasonable BPM


// Setup for interrupt ATmega328p, check notes for Timer_Interrupt_Notes for other chipsets
// Initializes Timer 2 to throw an interrupt every 2ms
void interruptSetup(){   
  TCCR2A = 0x02;                                   // DISABLE PWM ON DIGITAL PINS 3 AND 11, AND GO INTO CTC MODE
  TCCR2B = 0x06;                                   // DON'T FORCE COMPARE, 256 PRESCALER
  OCR2A = 0x7C;                                    // SET THE TOP OF THE COUNT TO 124 FOR 500Hz SAMPLE RATE
  TIMSK2 = 0x02;                                   // ENABLE INTERRUPT ON MATCH BETWEEN TIMER2 AND OCR2A
  sei();                                           // MAKE SURE GLOBAL INTERRUPTS ARE ENABLED
}



//___________________________________________________________________________________________________
//===================================|    ISR    |===================================================
//---------------------------------------------------------------------------------------------------


// Timer2 Interrupt Service Routine (ISR), timer2 make sure that a reading is taken every 2 ms
// triggered when Timer2 counts to 124
ISR(TIMER2_COMPA_vect){
//ISR(){   
  cli();                                            // disable interrupts while we do this
  Signal = analogRead(pulsePin);                    // read the Pulse Sensor
  sampleCounter += 2;                               // keep track of the time in mS with this variable
  int N = sampleCounter - lastBeatTime;             // monitor the time since the last beat to avoid noise


  // Find the trough of the pulse wave and avoid dichrotic noise by waiting 3/5 of last IBI
  if(Signal < thresh && N > (IBI/5)*3){      
    if (Signal < T){                                // keep track of Trough/Low point 
      T = Signal;                         
    }
  }

  // Find the peak, include thresh to avoid noise
  if(Signal > thresh && Signal > P){          
    P = Signal;                                     // keep track P is the peak/highest point
  }

  

  // Look for heartbeat which is just a surge in signal
  
  if (N > 250){                                       // avoid high frequency noise
    
    if ( (Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3) ){
      Pulse = true;                                   // set the Pulse flag when we think there is a pulse
      digitalWrite(blinkPin,HIGH);                    // turn on pin 13 LED
      
      IBI = sampleCounter - lastBeatTime;             // measure time between beats in ms
      lastBeatTime = sampleCounter;                   // keep track of time for next pulse



      // Keep track of first and second beat flags
      if(secondBeat == true){                         // if this is the second beat, if secondBeat == true
        secondBeat = false;                           // clear secondBeat flag
        for(int i=0; i<=9; i++){                      // seed the running total to get a realisitic BPM at startup
          rate[i] = IBI;
        }
      }

      if(firstBeat == true){                         // if it's the first time we found a beat, if firstBeat == TRUE
        firstBeat = false;                           // clear firstBeat flag
        secondBeat = true;                           // set the secondBeat flag
        sei();                                       // enable interrupts again
        return;                                      // IBI value is unreliable so discard it
      }



      // keep a running total of the last 10 IBI values
      word runningTotal = 0;                        // clear the runningTotal variable

      for(int i=0; i<=8; i++){                      // shift data in the rate array
        rate[i] = rate[i+1];                        // and drop the oldest IBI value
        runningTotal += rate[i];                    // add up the 9 oldest IBI values
      }

      rate[9] = IBI;                                // add the latest IBI to the rate array
      runningTotal += rate[9];                      // add the latest IBI to runningTotal
      runningTotal /= 10;                           // average the last 10 IBI values
      BPM = 60000/runningTotal;                     // BPM = 1 minute / time of beats
      QS = true;                                    // Set QS flag
    }
  }

  
  
  
  // if the values are going down, the beat is over, we reset values
  // Let threshold be 1/2 the amplitude of Peak and Trough
  // Reset the P and T to threshold
  if (Signal < thresh && Pulse == true){             // when the values are going down, the beat is over
    digitalWrite(blinkPin,LOW);                      // turn off pin 13 LED
    Pulse = false;                                   // reset the Pulse flag so we can do it again
    amp = P - T;                                     // get amplitude of the pulse wave
    thresh = amp/2 + T;                              // set thresh at 50% of the amplitude
    P = thresh;                                      // reset these for next time
    T = thresh;
  }

  if (N > 2500){                                     // if 2.5 seconds go by without a beat
    thresh = 560;                                    // set thresh default
    P = 540;                                         // set P default
    T = 540;                                         // set T default
    lastBeatTime = sampleCounter;                    // bring the lastBeatTime up to date
    firstBeat = true;                                // set these to avoid noise
    secondBeat = false;                              // when we get the heartbeat back
  }

  sei();                                   // enable interrupts as we are done with the cycle
}
