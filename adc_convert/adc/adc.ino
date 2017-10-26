#include "constants.h"

void ADChandler()
{ //read sensors and compute sensorstate
  //read four values in an array
  lastread=(3==lastread)?(0):(lastread+1);
  
  t_s1_adc[lastread] = analogRead(distPin1);
  t_s2_adc[lastread] = analogRead(distPin2);
  t_s3_adc[lastread] = analogRead(distPin3);
  s1_adc=0;
  s2_adc=0;
  s3_adc=0;
  for(avg_i=0;avg_i<4;avg_i++)
  {
    s1_adc+=t_s1_adc[avg_i];
    s2_adc+=t_s2_adc[avg_i];
    s3_adc+=t_s3_adc[avg_i];
  }
  //compute averages
  s1_adc=s1_adc>>2; //divide by 4
  s2_adc=s2_adc>>2; 
  s3_adc=s3_adc>>2;
  //take the maxima of the averaged values
  max_adc=max(s1_adc,max(s2_adc,s3_adc));
  prevsensorstate=sensorstate;
  sensorstate= (sense_radius < max_adc)?(true):(false); //true if people there
  
  if(prevsensorstate<sensorstate)                       // rising edge
  {
          colorValid=true;
  }
  else if (prevsensorstate>sensorstate)                 // falling edge
  {
    timeoutCounter=millis();
  }
  else if (timeoutCounter>timeoutMillis){               // timeout happened
      colorValid=false;
      //
      //Timer1.setPeriod(50000)
      //TODO: if color not valid, compute a new one based on distance
      
  }
  
  
  //timeoutCounter=millis();
  //modifyFading(); -----> modify timer settings instead

  /////////////////////////////////////////////////////////////////
           if(0==lastread && DEBUG)
           {
              Serial.print("adc values:");
              Serial.print(s1_adc);
              Serial.print("\t");
              Serial.print(s2_adc);
              Serial.print("\t");
              Serial.print(s3_adc); 
              Serial.print("\t");
              Serial.print(max_adc);
              Serial.print("\t");
              Serial.println(colorValid); 
              Serial.print("\t");
              Serial.println(sensorstate); 
           }
  //////////////////////////////////////////////////////////////////
  //return;

}//END of ADC handle


void setup() {
  //initialize sensors
  pinMode(distPin1, INPUT);
  pinMode(distPin2, INPUT);
  pinMode(distPin3, INPUT);

  //sensor ADC
  Timer1.initialize(500000);
  Timer1.attachInterrupt(ADChandler);

  //DFRsetup();
  //TODO: init the mp3 play, with library provided at https://github.com/DFRobot/DFRobotDFPlayerMini

  //
    Serial.begin(9600);
}

void loop() {

}
