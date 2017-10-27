/////////////////////////////////////////RELEASE NOTES//////////////////////////////////
//
//    Cleanup @ oct 27
//      - color fading is bad
//      - recseges eltunt
//
//    next versions:
//        check with higher PWM & baud rates necessary
//

// Set initial color
#include "constants.h"
//#include <avr/io.h>
//#include <util/delay.h>


DFRobotDFPlayerMini myDFPlayer;
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX

//ADC section
void ADChandler()
{ //read sensors and compute sensorstate
  //first handle buttons
  soundcycle=(8==soundcycle)?(0):(soundcycle+1);

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
        colorPalette = ((2==colorPalette) ? (0) : (colorPalette+1));  // predefined color sets
      }
  prevButton=button;


  //read four values in an array
  lastread=(3==lastread)?(0):(lastread+1);
  
  t_s1_adc[lastread] = analogRead(distPin1);
  t_s2_adc[lastread] = analogRead(distPin2);
  t_s3_long_adc[lastread] = analogRead(distLong); //special sensor
  s1_adc=0;
  s2_adc=0;
  s3_long_adc=0;
  for(avg_i=0;avg_i<4;avg_i++)
  {
    s1_adc+=t_s1_adc[avg_i];
    s2_adc+=t_s2_adc[avg_i];
    s3_long_adc+=t_s3_long_adc[avg_i];
  }
  //compute averages
  s1_adc=s1_adc>>2; //divide by 4
  s2_adc=s2_adc>>2; 
  s3_long_adc=(s3_long_adc>>3)-10;
  //take the maxima of the averaged values
  max_adc=max(s1_adc,max(s2_adc,s3_long_adc));
  //max_adc=max(s1_adc,s2_adc);
  prevsensorstate=sensorstate;
  sensorstate= (sense_radius < max_adc)?(true):(false); //true if people there


  if(prevsensorstate<sensorstate)                       // rising edge
  {//reset volume, amplitude, freq, timeoutCounter
      if(millis()>timeoutCounter+timeStopMillis){ //stop timeout volt --> elorol kezd
          if(paused){
            myDFPlayer.enableLoopAll();
            paused=false;
          }
      }
      myDFPlayer.start();
      digitalWrite(mutePin,0);  //???low level, no mute
      timeoutCounter = millis();                    // timeout frissites
      Timer1.setPeriod(20001); //frequency slow

   //   Serial.println("Rising edge!");
  }

  if(sensorstate){                                    //logic high state      
      if(max_adc>350)                             //near
      {
          Timer1.setPeriod(8000);
          if(!soundcycle&&(17!=finVol)){        //maybe wirte an inline function
            finVol=17;
            Serial.println("finv17");
          } //just in every 5 cycles      
      }
      else if (max_adc>200)                       //mid range
      {
          Timer1.setPeriod(16001);
          if(!soundcycle)if(!soundcycle&&(15!=finVol)){
            finVol=15;
            Serial.println("finv15");
          }
      }
      else if (max_adc>80)
      {
          Timer1.setPeriod(25001);
          if(!soundcycle&&(10!=finVol)){
            finVol=10;
          }
      }
      circularColor();
      timeoutCounter=millis();  
  }
  else if (prevsensorstate>sensorstate)                 // falling edge
  {
      timeoutCounter=millis(); //redundant...
  }
  ////////////////////////////////////// TIMEOUTING ///////////////////////////////////
 //order of evaluation is important -> first is the longest time - else the longer never be evaluated
  else if (millis()>timeoutCounter+timeStopMillis){     
      if(!paused){
          myDFPlayer.pause();
          paused=true;
          finVol=0; //equal with mute
      }
      //mute just if vol==0
      Timer1.setPeriod(5001);
      setTargetColor(9); //black
      Serial.println("Big timeout: \t Mp3 stopped, black value set");
  } 
  else if (millis()>timeoutCounter+timeoutMillis){               // timeout happened
      //NO pause, no mute
      setTargetColor(9);
      Timer1.setPeriod(50000); //very slow to black transition
      if(soundcycle)
      {
        Serial.println("Timeout: \t Mp3 paused, period 50ms, volume");
      }
      

  }
  else if(millis()>timeoutCounter+timeoutMillis-500){     //logic low state 0.5s before timeout to volumedown
      if(!soundcycle&&(1!=finVol)){
            finVol=1;
      }//bonus
  }

/////////////////////////////   SOUND FADING   ////////////////////////////////

  
  //curVol=(curVol<finVol)?(curVol+1):((
  
  if(curVol<finVol && 0==soundcycle){
    myDFPlayer.volume(curVol);
    curVol++;
    Serial.println("vol++");
  }
  else if(curVol>finVol && 0==soundcycle){
    myDFPlayer.volume(curVol);
    curVol--;
    Serial.println("vol--");
  }

  if(!paused && 0==curVol){
          myDFPlayer.pause();
          digitalWrite(mutePin,1);
          paused=true;
  }

////////////////////////SERIAL - JUST THE MOST IMPORTANT/////////////////////

           if(0==lastread)
           {
              Serial.print(s1_adc);
              Serial.print("\t");
              Serial.print(s2_adc);
              Serial.print("\t");
              Serial.println(s3_long_adc); 
           }
           
////////////////////////////////////////////////////////////////////////////
  
}//END of ADC handle


// COLOR variables
byte curR = rgb[0][black][0];//now
byte curB = rgb[0][black][1];
byte curG = rgb[0][black][2];

byte finR = rgb[0][black][0];//next
byte finB = rgb[0][black][1];
byte finG = rgb[0][black][2];

double dR = 0;//step
double dB = 0;
double dG = 0;

void applyRGB()
{
  analogWrite(redPin, curR);
  analogWrite(grnPin, curG);
  analogWrite(bluPin, curB);
  
  //OCR2B=100;  
}

byte nextVal(word curX, double dX)
{
  curX=curX+dX*increaseRate;
  //Serial.println(curX);
  
  curX=(curX>255)?(255):(curX);
  curX=(curX<0)?(0):(curX);
return curX;
}

void rgbIncrement()
{//not too long interrupt routine?!
  //may increase an int to perform other tasks
  dR=(0<(finR-curR))?(1):(-1);
  dR=(0==(finR-curR))?(0):(dR); //elerte a celt
  
  dG=(0<(finG-curG))?(1):(-1);
  dG=(0==(finG-curG))?(0):(dG);
  
  dB=(0<(finB-curB))?(1):(-1);
  dB=(0==(finB-curB))?(0):(dB);

  if(!dR && !dG && !dB)
  {
    targetReached=true;
  }

  curR=nextVal(curR, dR);
  curG=nextVal(curG, dG);
  curB=nextVal(curB, dB);
  
  applyRGB(); 
}

void setTargetColor(int x) 
{ //WORKING
     if(9==x){
     finR = 0;
     finG = 0;
     finB = 0;
     }
     else
     {
     colorNum=x;
     targetReached=(curR==finR&&curG==finG&&curB==finB)?(1):(0); 
     finR = rgb[colorPalette][x][0];
     finG = rgb[colorPalette][x][1];
     finB = rgb[colorPalette][x][2];
     }
}

/////////////////////Led color change///////////////////////////////

void circularColor()
{
  if( targetReached )// && (millis()<(setpointReachedSince+setpointWait)))
  {
  int nextColor=(6 == colorNum)?(0):(colorNum+1);
        //Serial.print("Circular color, next:\t");
        //Serial.println(nextColor);
  setTargetColor(nextColor);
  }
}

/////////////////////////////////////////////////////////////////////////////////
//            NECESSARY FUNCTIONS:
/////////////////////////////////////////////////////////////////////////////////
/*
 *  Lights:
 *      
 *    changing speed:                              Timer1.setPeriod(1000);
 *    target color (from predefined set):          setTargetColor(byte setC[3]);
 * 
 *  Sound:
 * 
 * 
 */
/////////////////////////////////////////////////////////////////////////////////

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(grnPin, OUTPUT);   
  pinMode(bluPin, OUTPUT);
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
   Serial.println(F("DFPlayer Mini online."));

   myDFPlayer.enableLoopAll();  //Play the first mp3 and loop all
   paused=false;
   Timer1.initialize(20000);
   
  
   delay(100);
   Timer1.attachInterrupt(rgbIncrement);
   Timer1.setPeriod(10001);
   
   currenttime=millis();
}

void loop() {
      
    if(millis() - currenttime >100){  // hülye név..
        ADChandler();
        currenttime=millis();
        Serial.print("finv: ");
        Serial.println(finVol);
    }

}
