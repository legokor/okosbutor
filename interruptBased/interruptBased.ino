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
////////////////////////////////////    ADC HANDLER   ////////////////////////////////////////
//
//    2017.10.27. 23:58: ready to be tested.
//
//    fcn-s copied from program before.
//    changelog: using map function.
//
//  INPUT variables:
//      - three Analog channel
//      - button pin
//
//  OUTPUT variables:
//      - bool sensorState
//      - bool nextSound
//      - max_adc
void ADCread()

{
  //////////////////////////////    BUTTON HANDLING   /////////////////////////////////////////
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
    colorPalette = ((1==colorPalette) ? (0) : (colorPalette+1));  // predefined color sets
  }
  prevButton = button;


/////////////////////////////////////   ADC COUNTING   //////////////////////////////////////////////

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

  increaseRate = map(max_adc, 0, 1024, 0, 8);

///////////////////////////////   FOR DEBUG AND INFORMATION   ////////////////////////////////////

  if (usound) {

    /*
                  Serial.print(s1_adc);
                  Serial.print("\t");
                  Serial.print(s2_adc);
                  Serial.print("\t");
                  Serial.println(s3_long_adc); */
  }
}//END OF ADCread
///////////////////////////////////////   SOUND CALC    ///////////////////////////////////////////
//
//    INPUT variables:  (working based on the values of)
//                    nextSound
//                    sensorstate
//                    max_adc
//
//   OUTPUT variables:  (set by this function)
//                    music
//                    prevsensorstate
//                    timeoutCounter
//                    started
//                    colorBlack
//
//   INTERNAL variables:
//                    finVol (target to reach)
//
//   GLOBAL constants:timeStopMillis
//                    mutePin
//                    timeoutMillis
//                    
//   physical output: 
//                    curVol (on SoftwareSerial ports: digital 10-11)
//
//


void inline sound(void) {
  if (nextSound) {                                       //change to next music
    myDFPlayer.next();
  }
  if (prevsensorstate < sensorstate) {                   // rising edge
    digitalWrite(mutePin, 0); //unmute
    colorBlack = false;
    if (millis() > timeoutCounter + timeStopMillis)      //if there was a big low sensorstate
    {                                                    //stop timeout volt --> elorol kezd
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
    colorBlack = true;
  }
  
  //SOUND FADING PART
 
  if (curVol < finVol) {        //ha felfele kell lépni  
    curVol = ((curVol + 4) > finVol) ? (finVol) : (curVol + 4); //4-et lépünk  kivéve, 
    myDFPlayer.volume(curVol);                                  //ha túlllépünk akkor egyből a célba lép
    //Serial.println("vol++");
  }
  else if (curVol > finVol) {   //ha lefele kell lépni
    curVol = ((curVol - 4) < finVol) ? (finVol) : (curVol - 4);
    myDFPlayer.volume(curVol);
    //Serial.println("vol--");
  }

  if (0 == curVol) {            //ha 0 a hangerő, akkor zene leáll és mute-ol is 
    myDFPlayer.pause();
    digitalWrite(mutePin, 1);
    //paused=true;
  }
  //to be handled here:
  prevsensorstate = sensorstate;
}

/////////////////////////////////////    LED FADING    ///////////////////////////////////////
//
//    INPUT variables:  increaseRate
//                        - adc alapjan gyorsabban novel, a regi conceptbeli timert valtja ki
//                      colorBlack 
//                        - shows dark final state
//                      colorPalette
//   GLOBAL constants:  rgb[3][7][3]
//
//   INSIDE variables:  x
//                      finR,finG,finB
//                      curR,curG,curB -> regiszterek
//                        dR,  dG,  dB
//

  void led(void){               //may increase an int to perform other tasks
  dR=(0<(finR-curR))?(1):(-1);  // föl vagy le fele kelll változni
  dR=(0==(finR-curR))?(0):(dR); // elerte a celt

  dG=(0<(finG-curG))?(1):(-1);
  dG=(0==(finG-curG))?(0):(dG);

  dB=(0<(finB-curB))?(1):(-1);
  dB=(0==(finB-curB))?(0):(dB);


  if(dR || dG || dB){         //ha valaminek változnia kell
    curR=((dR>0)^(curR>curR+dR*increaseRate))?(curR+dR*increaseRate):(curR+dR);  //kérdés(ha számolás és túlcsordulás iránya különböző)akkor(lépj)ellenben(lassan közelít)
    curG=((dG>0)^(curG>curG+dG*increaseRate))?(curG+dG*increaseRate):(curG+dG);  //ha increaseRate=1 akkor a lassu lépés nem csordulhat túl, mert a dB érték fentebb 0-ba áll
    curB=((dB>0)^(curB>curB+dB*increaseRate))?(curB+dB*increaseRate):(curB+dB);  
 
  }else{                      //ha nem kell változni akkor jöhet az uj cél
    if(colorBlack){                 //ha nincs senki közelben sötétbe vált
      finR = 0;
      finG = 0;
      finB = 0;
    }else{                          //ha vannak a közelben
      x=(x<6)?(x+1):(0);                  //x érték cirkulálása 0->6
      finR = rgb[colorPalette][x][0];     //a megfelelő palettából az x-edik szín r,g,b azaz 0,1,2 byte-ok
      finG = rgb[colorPalette][x][1];
      finB = rgb[colorPalette][x][2];
    }
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
   led();
  }

  //handle adc
  if (uadc)
  {
    uadc = false;
    ADCread();
  }

  //handle sound
  if (usound)
  {
    usound = false;
    sound();
      
                  Serial.print(s1_adc);
                  Serial.print("\t");
                  Serial.println(s2_adc);
                  //Serial.print("\t");
                  //Serial.println(s3_long_adc); */
  }

  //handle button pushes? with adc.

}
