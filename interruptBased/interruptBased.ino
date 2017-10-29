#include "main.h"
#include "led.h"

DFRobotDFPlayerMini myDFPlayer;
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX

void timingISR(void)
{
  iISR++;
  if (!(iISR % 1))
  {
    uled = true;
  }
  if (!(iISR % 2))
  {
    uadc = true;
  }

  if (!(iISR % 32))
  {
    usound = true;
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
  button = digitalRead(buttonPin);
  if (prevButton < button)
  {
    buttonTimer = millis();                   // rise edge start buttonTimer
  }
  if (prevButton > button && ((millis() - buttonTimer) < 2500))
  {
    nextSound = true;                      // @rise_edge if pos_level shorter than 2sec
  }
  else if (prevButton > button) {                                     // @fall_edge if pos_level longer than 2sec
    colorPalette = 0; //((1==colorPalette) ? (0) : (colorPalette+1));  // predefined color sets
  }
  prevButton = button;


  ////////////////////////////  ADC COUNTING   /////////////////////////////////////////

  //AVERAGING 4 values, using map fcn
  lastread = (3 == lastread) ? (0) : (lastread + 1);

  t_s1_adc[lastread] = analogRead(distPin1);
  t_s2_adc[lastread] = analogRead(distPin2);
  t_s3_long_adc[lastread] = analogRead(distLong); //special sensor
  s1_adc = 0;
  s2_adc = 0;
  s3_long_adc = 0;

  int avg_i;
  for (avg_i = 0; avg_i < 4; avg_i++)
  {
    s1_adc += map(t_s1_adc[lastread], 0, 1023, 0, 255);
    s2_adc += map(t_s2_adc[lastread], 0, 1023, 0, 255);
    s3_long_adc += map(t_s3_long_adc[lastread], 0, 1023, 0, 127); //different mapping, since high voltage means bigger distance
  }

  //MAXIMUM calculation, sensorstate
  max_adc = max(s1_adc, max(s2_adc, s3_long_adc));
  //prevsensorstate=sensorstate;
  sensorstate = (sense_radius < max_adc) ? (true) : (false); //true if people there

  //increaseRate = map(max_adc, 0, 1023, 0, 20);

  ///////////////////////FOR DEBUG AND INFORMATION////////////////////////////

  if (usound) {

    /*
                  Serial.print(s1_adc);
                  Serial.print("\t");
                  Serial.print(s2_adc);
                  Serial.print("\t");
                  Serial.println(s3_long_adc); */
  }
}//END OF ADCread
///////////////////////////////////////  SOUND CALC   /////////////////////////////////////////////

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




void inline sound(void) {
  if (nextSound) {                                       //change to next music
    myDFPlayer.next();
  }
  if (prevsensorstate < sensorstate) {                   // rising edge
    digitalWrite(mutePin, 0); //unmute
    if (millis() > timeoutCounter + timeStopMillis)      //if there was a big low sensorstate
    { //stop timeout volt --> elorol kezd
      myDFPlayer.enableLoopAll();
    }
    else
    {
      myDFPlayer.start(); //must be in else branch!
      //started=true;
    }
  }

  if (sensorstate) {
    finVol = map(max_adc, 0, 511, 0, 15) + 10;
    timeoutCounter = millis();
  }
  else if (prevsensorstate > sensorstate)               // falling edge
  {
    timeoutCounter = millis(); //redundant...
  }
  else if (millis() > timeoutCounter + timeoutMillis) {          // timeout happened
    finVol = 0;
  }


  /////////////////////////////   SOUND FADING   ////////////////////////////////

  if (curVol < finVol) {
    myDFPlayer.volume(curVol);
    curVol = ((curVol + 4) > finVol) ? (finVol) : (curVol + 4);
    //Serial.println("vol++");
  }
  else if (curVol > finVol) {
    myDFPlayer.volume(curVol);
    curVol = ((curVol - 4) < finVol) ? (finVol) : (curVol - 4);
    //Serial.println("vol--");
  }

  if (0 == curVol) {
    myDFPlayer.pause();
    digitalWrite(mutePin, 1);
    //paused=true;
  }
  //to be handled here:
  prevsensorstate = sensorstate;
}

////////////////////////////////// LED FADING //////////////////////////////////
/*
int black = 9;

int curR = rgb[0][black][0];//now
int curB = rgb[0][black][1];
int curG = rgb[0][black][2];

byte finR = rgb[0][black][0];//next
byte finB = rgb[0][black][1];
byte finG = rgb[0][black][2];

//double dR = 0;//step
//double dB = 0;
//double dG = 0;

void applyRGB()
{
  analogWrite(redPin, curR);
  analogWrite(grnPin, curG);
  analogWrite(bluPin, curB);

  //OCR2B=100;
}

byte nextVal(word curX, double dX)
{
  curX = curX + dX * increaseRate;
  //Serial.println(curX);

  curX = (curX > 255) ? (255) : (curX);
  curX = (curX < 0) ? (0) : (curX);
  return curX;
}

void rgbIncrement()
{ //not too long interrupt routine?!
  //may increase an int to perform other tasks
  dR = (0 < (finR - curR)) ? (1) : (-1);
  dR = (0 == (finR - curR)) ? (0) : (dR); //elerte a celt

  dG = (0 < (finG - curG)) ? (1) : (-1);
  dG = (0 == (finG - curG)) ? (0) : (dG);

  dB = (0 < (finB - curB)) ? (1) : (-1);
  dB = (0 == (finB - curB)) ? (0) : (dB);

  if (!dR && !dG && !dB)
  {
    targetReached = true;
  }

  curR = nextVal(curR, dR);
  curG = nextVal(curG, dG);
  curB = nextVal(curB, dB);

  applyRGB();
}

void setTargetColor(int x)
{ //WORKING
  if (9 == x) {
    finR = 0;
    finG = 0;
    finB = 0;
  }
  else
  {
    colorNum = x;
    targetReached = (curR == finR && curG == finG && curB == finB) ? (1) : (0);
    finR = rgb[colorPalette][x][0];
    finG = rgb[colorPalette][x][1];
    finB = rgb[colorPalette][x][2];
  }
}

/////////////////////Led color change///////////////////////////////
void circularColor()
{
  if ( targetReached ) // && (millis()<(setpointReachedSince+setpointWait)))
  {
    int nextColor = (6 == colorNum) ? (0) : (colorNum + 1);
    //Serial.print("Circular color, next:\t");
    //Serial.println(nextColor);
    setTargetColor(nextColor);
  }
}*/

#define curR OCR2B
#define curG OCR0B
#define curB OCR0A
byte finR=0;
byte finG=0;
byte finB=0;

  void led(void){
  //may increase an int to perform other tasks
  dR=(0<(finR-curR))?(1):(-1);
  dR=(0==(finR-curR))?(0):(dR); //elerte a celt

  dG=(0<(finG-curG))?(1):(-1);
  dG=(0==(finG-curG))?(0):(dG);

  dB=(0<(finB-curB))?(1):(-1);
  dB=(0==(finB-curB))?(0):(dB);

  if(!dR && !dG && !dB){
    x=(x<6)?(x+1):(0);
    finR = rgb[colorPalette][x][0];
    finG = rgb[colorPalette][x][1];
    finB = rgb[colorPalette][x][2];
  }else{
    curR=((dR>0)&&(curR>curR+dR*increaseRate))?(curR+dR):(curR+dR*increaseRate);
    curG=((dG>0)&&(curG>curG+dG*increaseRate))?(curG+dG):(curG+dG*increaseRate);
    curB=((dB>0)&&(curB>curB+dB*increaseRate))?(curB+dB):(curB+dB*increaseRate);
  }
  
  }
//////////////////////////////////   SETUP   //////////////////////////////////////////////
void setup()
{
  digitalWrite(mutePin, 1);

  pinMode(redPin, OUTPUT);
  pinMode(grnPin, OUTPUT);
  pinMode(bluPin, OUTPUT);
  pinMode(mutePin, OUTPUT);

    TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(WGM00);
    //WGM0[2:0]=001b PWM, Phase Correct TOP=0xFF update of OCR0x at TOP and TOV flag Set on BOTTOM(BOTTOM=0x00)
    //COM0A[1:0]=10b Clear OC0A on Compare Match when up-counting. Set OC0A on Compare Match when down-counting.
    //com0B[1:0]=10b Clear OC0B on Compare Match when up-counting. Set OC0B on Compare Match when down-counting.
    TCCR0B = _BV(CS00);
    //CS0[2:0]=001b clk i/o / 4^cs0=
    TCCR2A = _BV(COM2B1) | _BV(WGM00);
    TCCR2B = _BV(CS20);
    // OCR2B = 125; //R
    //OCR0A = 50;  //B   //compare values
    //OCR0B = 100; //G
  
  Serial.begin(9600);
  Serial.println("ena mute");
  mySoftwareSerial.begin(9600); // maybe higher baud leads to less noise?!
  Serial.println(F("Initializing, DFPlayer"));
  while (!myDFPlayer.begin(mySoftwareSerial)) { //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to connect"));
    delay(1000);
  }
  // DELAY 3 SEC & UNMUTE HERE:
  delay(3000);
  digitalWrite(mutePin, 0);
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
  if (uled)
  {
    uled = false;
   // rgbIncrement();
   led();
   Serial.print(curR);Serial.print("\t");
    Serial.print(curG);Serial.print("\t");
    Serial.println(curB); 
    
    //TODO: write led refresh
  }

  //handle adc
  if (uadc)
  {
    //Serial.println("adc");
    uadc = false;
    ADCread();
  }

  //handle sound
  if (usound)
  {
    //Serial.println("sound");
    usound = false;
    sound();
    //circularColor();
       

    Serial.print(finR);Serial.print("\t");
    Serial.print(finG);Serial.print("\t");
    Serial.println(finB);

  }

  //handle button pushes? with adc.

}
