#include "main.h"


void timingISR(void)
{
	iISR++;
	uled=true;
		
	if(!(iISR%2))
	{
		uadc=true;
	}
	
	if(!(iISR%8))
	{
		usound=true;
	}
}

void ADCread()
//
//		2017.10.27. 23:58: ready to be tested.
//
//		fcn-s copied from program before.
//		changelog: using map function.
//
//  INPUT variables:
//      - three Analog channel
//      - button pin
//
//  OUTPUT variables:
//      - bool sensorState
//      - bool nextSound
//      - max_adc
{
//////////////////////////////BUTTON HANDLING/////////////////////////////////////////
  button=digitalRead(buttonPin);
  if(prevButton<button) 
      {
        buttonTimer=millis();                     // rise edge start buttonTimer
      }
  if(prevButton>button && ((millis()-buttonTimer)<2500))
      {
        nextSound=true;                        // @rise_edge if pos_level shorter than 2sec
      }
  else if(prevButton>button){                                         // @fall_edge if pos_level longer than 2sec
        colorPalette = 0; //((1==colorPalette) ? (0) : (colorPalette+1));  // predefined color sets
      }
  prevButton=button;


  /////////////////////////////////////////////////////////////////////////////////////
  //AVERAGING 4 values, using map fcn
  int lastread = 0;
  lastread=(3==lastread)?(0):(lastread+1);
  
  t_s1_adc[lastread] = analogRead(distPin1);
  t_s2_adc[lastread] = analogRead(distPin2);
  t_s3_long_adc[lastread] = analogRead(distLong); //special sensor
  s1_adc=0;
  s2_adc=0;
  s3_long_adc=0;
  
  int avg_i;
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
              /*Serial.print(s1_adc);
              //Serial.print("\t");
              Serial.print(s2_adc);
              //Serial.print("\t");
              Serial.println(s3_long_adc); */
           }
}//END OF ADCread



void setup()
{
  Timer1.initialize(50000);
  delay(100);
  Timer1.attachInterrupt(timingISR);
}

void loop()
{
    //handle leds
	if(uled)
	{
		//TODO: write led refresh
	}
	
	//handle adc
	if(uadc)
	{
		//TODO: write adc read
	}
	
	//handle sound
	if(usound)
	{
		//TODO: write sound fade
	}
	
	//handle button pushes? with adc.
  
}
