#include "main.h"

DFRobotDFPlayerMini myDFPlayer;
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
  
void timingISR(void)
{
	iISR++;
	uled=true;
		
	if(!(iISR%2))
	{
		uadc=true;
	}
	
	if(!(iISR%32))
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
    s1_adc+=map(t_s1_adc[lastread],0,1023,0,255);
    s2_adc+=map(t_s2_adc[lastread],0,1023,0,255);
    s3_long_adc+=map(t_s3_long_adc[lastread],0,1023,0,127); //different mapping, since high voltage means bigger distance
  }
	
  //MAXIMUM calculation, sensorstate
  max_adc=max(s1_adc,max(s2_adc,s3_long_adc));
  //prevsensorstate=sensorstate;
  sensorstate= (sense_radius < max_adc)?(true):(false); //true if people there

 ///////////////////////FOR DEBUG AND INFORMATION//////////////////////////// 
  
if(usound){
  
/*
              Serial.print(s1_adc);
              Serial.print("\t");
              Serial.print(s2_adc);
              Serial.print("\t");
              Serial.println(s3_long_adc); */
 }
}//END OF ADCread
///////////////////////////////////////  SOUND   /////////////////////////////////////////////

//    input variables:nextSound
//                    sensorstate
//                    max_adc
//
//   output variables:music               
//                    prevsensorstate
//                    timeoutCounter
//                    started
//                    finVol
//                    curVol
//                    
//



    
void inline sound(void){
    if(nextSound){                                         //change to next music
       myDFPlayer.next();
    }
    if(prevsensorstate<sensorstate){                       // rising edge
      digitalWrite(mutePin,0);  //unmute
      if(millis()>timeoutCounter+timeStopMillis)           //if there was a big low sensorstate
      { //stop timeout volt --> elorol kezd
            myDFPlayer.enableLoopAll();
      }
      else                                                 
      {
        myDFPlayer.start(); //must be in else branch!
        //started=true;
      }
    }
    
    if(sensorstate){
      finVol=map(max_adc,0,511,0,15)+10;
      timeoutCounter=millis();
    }
    else if (prevsensorstate>sensorstate)                 // falling edge
    {
      timeoutCounter=millis(); //redundant...
    }
    else if (millis()>timeoutCounter+timeoutMillis){               // timeout happened
      finVol=0;
    }

    
/////////////////////////////   SOUND FADING   ////////////////////////////////
  
    if(curVol<finVol){
      myDFPlayer.volume(curVol);
      curVol=((curVol+4)>finVol)?(finVol):(curVol+4);
      //Serial.println("vol++");
    }
    else if(curVol>finVol){
      myDFPlayer.volume(curVol);
      curVol=((curVol-4)<finVol)?(finVol):(curVol-4);
      //Serial.println("vol--");
    }
  
    if(0==curVol){
            myDFPlayer.pause();
            digitalWrite(mutePin,1);
            //paused=true;
    }
    //to be handled here:
    prevsensorstate=sensorstate;
}


void setup()
{
  /*pinMode(redPin, OUTPUT);
  pinMode(grnPin, OUTPUT);   
  pinMode(bluPin, OUTPUT);*/
  pinMode(mutePin, OUTPUT);
  digitalWrite(mutePin,1);
  
  Serial.begin(9600);
  Serial.println("ena mute");
   mySoftwareSerial.begin(9600); // maybe higher baud leads to less noise?!
   Serial.println(F("Initializing, DFPlayer"));
   if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to connect"));
    while(true);
   }
   // DELAY 3 SEC & UNMUTE HERE:
   delay(3000);
   digitalWrite(mutePin,0);
   Serial.println("disable mute");
   Serial.println(F("online."));
   myDFPlayer.volume(1);

   
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
   //Serial.println("adc");
   uadc=false;
		ADCread();
	}
	
	//handle sound
	if(usound)
	{
     //Serial.println("sound");
    usound=false;
		sound();
	}
	
	//handle button pushes? with adc.
  
}
