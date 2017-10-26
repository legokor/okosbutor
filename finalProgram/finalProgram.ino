/////////////////////////////////////////RELEASE NOTES//////////////////////////////////
//
//    Cleanup @ oct 22, but this version does not include the music change for button press.
//
//    check with higher PWM & baud rates necessary
//

// Set initial color
#include "constants.h"
//#include <avr/io.h>
//#include <util/delay.h>

unsigned long currenttime;        //
unsigned long timeoutshit;        //
byte timeouti;                   //
byte timeoutflag=0;               // GÁNYOLÁSHOZ

DFRobotDFPlayerMini myDFPlayer;
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX

//ADC section
void ADChandler()
{ //read sensors and compute sensorstate
  //first handle buttons
  soundcycle=(5==soundcycle)?(0):(soundcycle+1);
  
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
          myDFPlayer.enableLoopAll();
          //myDFPlayer.volume(10);                  //initial volume visszateres kikapcsbol->ne, a sensorstate-ben beall.
      }
      myDFPlayer.start();
      digitalWrite(mutePin,0);  //low level, no mute
      timeoutCounter = millis();                    // timeout frissites
      Timer1.setPeriod(20001); //frequency slow
      periodSet=20001;

   //   Serial.println("Rising edge!");
  }

  if(sensorstate){                                    //logic high state      
      if(max_adc>350)                             //near
      {
          Timer1.setPeriod(8000);
          if(!soundcycle){myDFPlayer.volume(23);} //just in every 5 cycles      
      }
      else if (max_adc>200)                       //mid range
      {
          Timer1.setPeriod(16001);
          if(!soundcycle){myDFPlayer.volume(19);}
      }
      else if (max_adc>80)
      {
          Timer1.setPeriod(25001);
          if(!soundcycle){myDFPlayer.volume(15);}
      }
      timeoutCounter=millis();  
  }
  else if (prevsensorstate>sensorstate)                 // falling edge
  {
      timeoutCounter=millis();
  }
  else if (millis()>timeoutCounter+timeoutMillis){               // timeout happened
      myDFPlayer.pause();
      digitalWrite(mutePin,1);
      setTargetColor(9);
      Timer1.setPeriod(50000); //very slow to black transition
        //  Serial.println("Timeout: \t Mp3 paused, period 50ms, time:");

  }
  else if (millis()>timeoutCounter+timeStopMillis){
      myDFPlayer.pause();
      digitalWrite(mutePin,1);
      Timer1.setPeriod(5001);
      setTargetColor(9); //black
         //  Serial.println("Big timeout: \t Mp3 stopped, black value set");
  } 
  else if(millis()>timeoutCounter+timeoutMillis-500){     //logic low state 0.5s before timeout to volumedown
      if(!soundcycle){myDFPlayer.volume(1);}//bonus
  }

//////////////////////////////////////////////////////////////GÁNYOLÁS,dont know what xactly happen
  
   if(s1_adc <=30 && s2_adc <=30 && s3_long_adc <=30){
      if(timeoutflag==0){
        timeoutflag = 1;
        timeoutshit = millis();
        timeouti = 0;
        myDFPlayer.volumeDown();
        //ha mar kicsi az ertek de meg 0 a flag, minden ciklusban csökkentünk
      }
      else{
        if(timeouti>=7){
          myDFPlayer.volumeDown();
          timeouti = 0;
        }
        else timeouti++;
      }
  }
  else timeoutflag = 0;
  
  
////////////////////////////////////////////////////////////////GÁNYOLÁS VÉGE





////////////////////////SERIAL - JUST THE MOST IMPORTANT/////////////////////

           if(0==lastread && DEBUG)
           {
          //    Serial.print("adc values:");
              Serial.print(s1_adc);
              Serial.print("\t");
              Serial.print(s2_adc);
              Serial.print("\t");
              Serial.println(s3_long_adc); 
              //Serial.print("\t max_adc|long_adc: ");
              //Serial.print(max_adc);Serial.print("  ");
              //Serial.print("\t long_adc ");
              //Serial.println(s3_long_adc);
            //  Serial.print("\n \t \t senstate|  ");
           //   Serial.println(sensorstate); 
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

/////////////////////DEMONSTRATION FUNCTIONALITY///////////////////////////////

void circularColor()
{//JUST FOR TEST PURPOSE, simple FadeTest.
  if( targetReached )// && (millis()<(setpointReachedSince+setpointWait)))
  {
  int nextColor=(6 == colorNum)?(0):(colorNum+1);
        //Serial.print("Circular color, next:\t");
        //Serial.println(nextColor);
  setTargetColor(nextColor);
  }
}

void fadeTest()
{//just working properly in library1, ith the color names
    //TCCR2B = TCCR2B & 0b11111001; //set divider to 1 on pin3, not working at first glance
    
    Serial.println("set the target color to red");
    setTargetColor(red);
    Timer1.setPeriod(25000);
    delay(10000);

    Serial.println("set the target color to green, fast transition");
    Timer1.setPeriod(1000);
    setTargetColor(green);
    delay(3000);
    Serial.println("set the target color to black");
   
    Timer1.setPeriod(5000);
    //setTargetColor(black);
    delay(100);

    Serial.println("set the target color to blue");
    setTargetColor(blue);
    delay(3000);
    Serial.println("set the target color to yellow, slow transition");

    Timer1.setPeriod(5000);
    setTargetColor(yellow);
    delay(6000);
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
 *    myDFPlayer.loop(1);
 *    myDFPlayer.volume(16);
 *    myDFPlayer.pause();
 * 
 * 
 */
/////////////////////////////////////////////////////////////////////////////////

void setup() {
  pinMode(redPin, OUTPUT);
  //TCCR2A = B00110011;   //_BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  //TCCR2B = B00000001;   //_BV(WGM22) | _BV(CS20);
  
  pinMode(grnPin, OUTPUT);   
  pinMode(bluPin, OUTPUT);
  pinMode(mutePin, OUTPUT);
  digitalWrite(mutePin,1);
  
  Serial.begin(9600);
  Serial.println("ena mute");
   mySoftwareSerial.begin(9600); // maybe higher baud leads to less noise?!
   Serial.println(F("Initializing, DFPlayer"));
   //Serial.println("enable mute");
   //digitalWrite(mutePin,1);
  // MUTE HERE!!
   if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to connect"));
    //Serial.println(F("1.Please recheck the connection!"));
    //Serial.println(F("2.Please insert the SD card!"));
    while(true);
   }
   // DELAY 2 SEC & UNMUTE HERE:
   delay(3000);
   Serial.println("disable mute");
   digitalWrite(mutePin,0);
   Serial.println(F("DFPlayer Mini online."));

   
   myDFPlayer.volume(15);  //Just only one time
   myDFPlayer.enableLoopAll();  //Play the first mp3 and loop all
   Timer1.initialize(20000);
   
  
   delay(100);
   Timer1.attachInterrupt(rgbIncrement);
   Timer1.setPeriod(10001);
   
   currenttime=millis();
}

void loop() {
    //fadeTest(); //test purpose
  
    
    if(millis() - currenttime >100){  // hülye név..
        ADChandler();
        currenttime=millis();
    }
    
}
