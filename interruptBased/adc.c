#include "adc.h"

void ADCread()
//
//		2017.10.27. 23:58: ready to be tested.
//
//		fcn-s copied from program before.
//		changelog: using map function.
{
//////////////////////////////BUTTON HANDLING/////////////////////////////////////////
  button=digitalRead(buttonPin);
  if(prevButton<button) 
      {
        buttonTimer=millis();                     // rise edge start buttonTimer
      }
  if(prevButton>button && ((millis()-buttonTimer)<2500))
      {
        myDFPlayer.next();                        // @rise_edge if pos_level shorter than 2sec
      }
  else if(prevButton>button){                                         // @fall_edge if pos_level longer than 2sec
        colorPalette = 0; //((1==colorPalette) ? (0) : (colorPalette+1));  // predefined color sets
      }
  prevButton=button;


  /////////////////////////////////////////////////////////////////////////////////////
  //AVERAGING 4 values, using map fcn
  lastread=(3==lastread)?(0):(lastread+1);
  
  t_s1_adc[lastread] = analogRead(distPin1);
  t_s2_adc[lastread] = analogRead(distPin2);
  t_s3_long_adc[lastread] = analogRead(distLong); //special sensor
  s1_adc=0;
  s2_adc=0;
  s3_long_adc=0;
  
  
  for(avg_i=0;avg_i<4;avg_i++)
  {
    s1_adc+=map(t_s2_adc,0,1023,0,63);
    s2_adc+=map(t_s2_adc,0,1023,0,63);
    s3_long_adc+=map(t_s2_adc,0,1023,0,127); //different mapping, since high voltage means bigger distance
  }
	
  //MAXIMUM calculation, sensorstate
  max_adc=max(s1_adc,max(s2_adc,s3_long_adc));
  prevsensorstate=sensorstate;
  sensorstate= (sense_radius < max_adc)?(true):(false); //true if people there

 ///////////////////////FOR DEBUG AND INFORMATION//////////////////////////// 
  
  if(0==lastread)
           {
              Serial.print(s1_adc);
              Serial.print("\t");
              Serial.print(s2_adc);
              Serial.print("\t");
              Serial.println(s3_long_adc); 
           }


}