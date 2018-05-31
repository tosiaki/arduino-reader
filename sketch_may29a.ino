#define HEARTBEATBUFFERSIZE 60
#define AVERAGINGINTERVAL 10

//  Variables
int PulseSensorPurplePin = A0;        // Pulse Sensor PURPLE WIRE connected to ANALOG PIN 0
int LED13 = 13;   //  Red LED
int LED7 = 5;    //  Yellow LED
int LED3 = 3;    //  Green LED
const int GSR = A1;


int Signal;                // holds the incoming raw data. Signal value can range from 0-1024
int Threshold_0 = 30;
int Threshold_1 = 90;
int Threshold_2 = 120;     // Determine which Signal to "count as a beat", and which to ingore.

int numSignals=100;
int currentIndex=1;
int haveAverage=0;
long workingAverage=0;
int averageSignal;
int haveDeviation=0;
long workingDeviation=0;
int deviationSignal;

const int detectingRise=0;
long riseTime;
const int detectingFall=1;
const int deviationThreshold=20;
int detectingState=detectingRise;

long heartbeatTime[HEARTBEATBUFFERSIZE];
int heartbeatIndex=0;
int heartbeatsRecorded=0;

long firstBeatTime;
long calculationTime;
long lastBPMCalculationTime;
long *lastBPMCalculationTimePointer=&lastBPMCalculationTime;
int heartbeatCount;

int beatsPerMinute;

char stressLevel;

// The SetUp Function:
void setup() {
  pinMode(LED13,OUTPUT);         // pin that will blink to your heartbeat!
  pinMode(LED7,OUTPUT);
  pinMode(LED3,OUTPUT);
   
  Serial.begin(9600);         // Set's up Serial Communication at certain speed.
}

// The Main Loop Function
void loop() {
  while (Serial.available()) {
    stressLevel = (char)Serial.read();
  }

  Signal = analogRead(PulseSensorPurplePin);  // Read the PulseSensor's value.
                                              // Assign this value to the "Signal" variable.
  workingAverage+=Signal;
  if(haveAverage) {
    workingDeviation+=(Signal-averageSignal)*(Signal-averageSignal);
  }
  currentIndex++;
  if(currentIndex>numSignals) {
    currentIndex=1;
    haveAverage=1;
    averageSignal=workingAverage/numSignals;
    workingAverage=0;
   deviationSignal=sqrt(workingDeviation/(numSignals-1));
   workingDeviation=0;
   if(deviationSignal) {
    haveDeviation=1;
   }
   //Serial.println(averageSignal);
  }
   //Serial.print(deviationSignal*100);
   //Serial.print(",");
   Serial.print(analogRead(A0));                    // Send the Signal value to Serial Plotter.
   //Serial.println(millis());
   Serial.print(',');
   Serial.println(analogRead(A1));
   //Serial.print(',');
   //Serial.println(stressLevel);

  if(haveDeviation) {
    if(detectingState==detectingRise) {
      if(Signal-averageSignal>deviationSignal && deviationSignal>deviationThreshold) {
        detectingState=detectingFall;
        riseTime=millis();
      }
    }
    else {
      if(Signal-averageSignal<deviationSignal*0.5) {
        if(millis()<riseTime+500) {
          if(!firstBeatTime) {
            firstBeatTime=millis();
          }
          heartbeatTime[heartbeatIndex++]=millis();
          heartbeatsRecorded++;
          if(heartbeatIndex>HEARTBEATBUFFERSIZE-1) {
            heartbeatIndex=0;
          }
  
          beatsPerMinute=beatsPerMinuteF(firstBeatTime,heartbeatsRecorded,heartbeatTime,lastBPMCalculationTimePointer);
        }
        detectingState=detectingRise;
      }
    }
  }
  if(millis()>*lastBPMCalculationTimePointer+2000 && heartbeatsRecorded) {
     beatsPerMinute=beatsPerMinuteF(firstBeatTime,heartbeatsRecorded,heartbeatTime,lastBPMCalculationTimePointer);
  }


  //beatsPerMinute=beatsPerMinuteF(firstBeatTime,heartbeatsRecorded,heartbeatTime,lastBPMCalculationTimePointer);

//   if(beatsPerMinute > Threshold_2){                          // If the signal is above "550", then "turn-on" Arduino's on-Board LED.
//     digitalWrite(LED13,HIGH);
//     digitalWrite(LED7,LOW);
//     digitalWrite(LED3,LOW);
//   } else if(beatsPerMinute > Threshold_1) {
//     digitalWrite(LED13,LOW);
//     digitalWrite(LED7,HIGH);
//     digitalWrite(LED3,LOW);                 //  Else, the sigal must be below "550", so "turn-off" this LED.
//   } else if(beatsPerMinute > Threshold_0) {
//     digitalWrite(LED13,LOW);
//     digitalWrite(LED7,LOW);
//     digitalWrite(LED3,HIGH);
//   } else {
//     digitalWrite(LED13,LOW);
//     digitalWrite(LED7,LOW);
//     digitalWrite(LED3,LOW);
//   }
   if(stressLevel == 'b'){                          // If the signal is above "550", then "turn-on" Arduino's on-Board LED.
     digitalWrite(LED13,HIGH);
     digitalWrite(LED7,LOW);
     digitalWrite(LED3,LOW);
   } else if(stressLevel == 'c') {
     digitalWrite(LED13,LOW);
     digitalWrite(LED7,HIGH);
     digitalWrite(LED3,LOW);                 //  Else, the sigal must be below "550", so "turn-off" this LED.
   } else if(stressLevel == 'd') {
     digitalWrite(LED13,LOW);
     digitalWrite(LED7,LOW);
     digitalWrite(LED3,HIGH);
   } else {
     digitalWrite(LED13,LOW);
     digitalWrite(LED7,LOW);
     digitalWrite(LED3,LOW);
   }
   
   //Serial.print(',');
   //Serial.println(beatsPerMinute);
}

int beatsPerMinuteF(int firstBeatTime,int heartbeatsRecorded,long heartbeatTime[],long* BPMtimePointer) {
  int heartbeatCount=0;
  long calculationTime=millis();
  if(calculationTime<=AVERAGINGINTERVAL*1000+firstBeatTime) return 0;
  for (int i=0;i<HEARTBEATBUFFERSIZE && i<heartbeatsRecorded;i++) {
    if (heartbeatTime[i]>calculationTime-AVERAGINGINTERVAL*1000) {
      heartbeatCount++;
    }
  }
  
  beatsPerMinute=heartbeatCount*60/AVERAGINGINTERVAL;
  *BPMtimePointer=millis();
        
  return beatsPerMinute;
}


