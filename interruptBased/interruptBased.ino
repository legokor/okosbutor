#include "main.h"
#include "led.h"
#include "button.h"

long int k=0; //lasts as long as 29h
DFRobotDFPlayerMini myDFPlayer;
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX

void timingISR(void)
{
  iISR++;
  if (!(iISR % 1))
  {
    uled = true;
  }
  if (!(iISR % 100))
  {
    ubutton = true;
  }

  if (!(iISR % 20))
  {
    uadc = true;
  }

  if (!(iISR % 100))
  {
    usound = true;
  }
  if (!(iISR % 2048))
  {
	  //ritka dolgok
  }
}

/////////////////////////////////    BUTTON HANDLING   /////////////////////////////////////////
/* Taszkok leirasa
 * 	(0)
 * Ha nem nyomnak gombot, folyton porgeti a countert.
 *
 * Lekezelt esemenyek:
 *
 * 	(1) Megnyomom a gombot
 * 	(1b)elengedem
 * 	(1c)nem nyulok hozza 2s-ig.-->Timeouted event
 *
 * 	(2) Nyomva tartom a gombot 5s-ig.
 * 	(3) Megnyomom a gombot, es 1s-en belul ujra. 						Below 1s repeated push
 */

/* Az esemenyek kezelese
 *
 * 	(1)
 * 	- elso megnyomas: pushInProgress TRUE lesz
 * 		1s-en belul nem szabad uj ciklusnak kezdodnie
 *
 * 	(2)
 * 	- ameddig nyomva tartom, TRUE lesz a continousPush
 * 		5s folyamatosan, TODO: ciklus vege
 *
 * 	(3)  - not implemented
 * 		ha 1 s-en belul 2 gombnyomas erkezik
 *
 */

void buttonRead()
{
//(0)
	buttonVal = analogRead(buttonAnalogPin); 	//updating
	button=(buttonVal>1000)?(1):(0);
	if(!button && !pushInProgress)
	{
		buttonCounter=k;
	}
//(1)
								//elso megnyomas a ciklusban
	if(button && !pushInProgress)
	{
		pushStartedAt=k;
		pushInProgress=true;		//START OF CYCLE
		continousPush=true;
		Serial.println("pushed!");
	}
//(1b)								//elengedi miutan meg volt nyomva
	if(!button && continousPush && pushInProgress)
	{
		continousPush=false;
		Serial.println("released after push");
	}
//(1c)
	if(!button && !continousPush && pushInProgress)
	{
		if(k > (buttonCounter+BUTTON_TIME_2s))
		{
			Serial.println("timeout after release");
			myDFPlayer.next();
			digitalWrite(mutePin,0);

			pushInProgress=false;	//END OF CYCLE
		}
	}

//(2)
	if(button && continousPush && pushInProgress)
	{
		pushedAgainAt=k;
		pushDelay=pushedAgainAt-pushStartedAt;
		if(pushDelay>BUTTON_TIME_5s)
		{
			//k=0;
			Serial.println("5s continuous push");
			pushInProgress=false;	//END OF CYCLE
			buttonCounter=k; 		//for "reset" button handling
			digitalWrite(mutePin,1);
			myDFPlayer.reset();
			delay(100);
			myDFPlayer.volume(1);


			//setup(); //trash
		}
	}
}

//////////////////////////////////////    ADC HANDLER   ////////////////////////////////////////
/*
//    2017.11.15: tested, corrected, button pushing rewritten and outsourced
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
 * */

void ADCread()
{
  ////////////////////////////////   ADC COUNTING   ///////////////////////////////////////////

  //AVERAGING 4 values, using map fcn
  lastread = (7 == lastread) ? (0) : (lastread + 1);

  t_s1_adc[lastread] = analogRead(distPin1);
  t_s2_adc[lastread] = analogRead(distPin2);
  t_s3_adc[lastread] = analogRead(distLong); //special sensor
  s1_adc = 0;
  s2_adc = 0;
  s3_adc = 0;

  sum=0;
  int avg_i;
  for (avg_i = 0; avg_i < 8; avg_i++)
  {
    s1_adc += t_s1_adc[avg_i];
    s2_adc += t_s2_adc[avg_i];
    s3_adc += t_s3_adc[avg_i];
  }

  s1_adc/=8;
  s2_adc/=8;
  s3_adc/=8;
  
  //MAXIMUM calculation, sensorstate
  max_adc = max(s1_adc, max(s2_adc, s3_adc));
  //prevsensorstate=sensorstate;
  sensorstate = (sense_radius < max_adc) ? (true) : (false); //true if people there

///////////////////////////   FOR DEBUG AND INFORMATION   /////////////////////////////////
  if (usound) {

    /*
                  Serial.print(s1_adc);
                  Serial.print("\t");
                  Serial.print(s2_adc);
                  Serial.print("\t");
                  Serial.println(s3_long_adc); */
  }
}//END OF ADCread
////////////////////////////////////   SOUND CALC    ///////////////////////////////////////
/*
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
*/



void inline sound(void) {
  if (nextSound) {                                       //change to next music
    myDFPlayer.next();
  }
  // rising edge
  else if (prevsensorstate < sensorstate) {
    colorBlack = false;
    if ((0==counterK)||(k>(counterK+TIMEOUT_10s)))
    {//stop timeout volt --> elorol kezd
        digitalWrite(mutePin, 0); //unmute
    	myDFPlayer.enableLoopAll();
    		Serial.println("timeoutbol elorol lejatszok");
    }
    else
    {
        digitalWrite(mutePin, 0); //unmute
    		Serial.print("siman folztatom a lejatszast");
    }
  }
  // continuous high
  else if (sensorstate) {
    finVol = map(max_adc, 0, 511, 0, SOUND_MAX_VOL) + SOUND_OFFSET_VOL;
    counterK=k;
  }
  // sensorstate=false AND timed out
  else if (k>(counterK+300)){
    //1000K=20s     --> after 6 sec decreasing vol
    finVol = 0;
    colorBlack = true;
  }
  
  //SOUND FADING PART
 
  if (curVol < finVol) {        //ha felfele kell lépni  
    curVol = ((curVol + 4) > finVol) ? (finVol) : (curVol + 4); //4-et lépünk  kivéve, 
    myDFPlayer.volume(curVol);                                  //ha túlllépünk akkor egyből a célba lép
    digitalWrite(mutePin, 0);

    //Serial.println("vol++");
  }
  else if (curVol > finVol) {   //ha lefele kell lépni
    curVol = ((curVol - 4) < finVol) ? (finVol) : (curVol - 4);
    myDFPlayer.volume(curVol);
    digitalWrite(mutePin, 0);

    //Serial.println("vol--");
  }

  if (0 == curVol) {            //ha 0 a hangero, akkor mute-ol
    digitalWrite(mutePin, 1);
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
  dR=(0<(finR-curR))?(1):(-1);  // fol vagy le fele kelll valtozni
  if(0 == (finR-curR)){dR=0;}
  	  	  //dR=(0==(finR-curR))?(0):(dR); // elerte a celt

  dG=(0<(finG-curG))?(1):(-1);
  if(0 == (finG-curG)){dG=0;}
  	  	  //dG=(0==(finG-curG))?(0):(dG);

  dB=(0<(finB-curB))?(1):(-1);
  if(0 == (finB-curB)){dB=0;}
  	  	  //dB=(0==(finB-curB))?(0):(dB);


  if(dR || dG || dB){         //ha valaminek változnia kell
    curR=(curR+dR);  //kérdés(ha számolás és túlcsordulás iránya különböző)akkor(lépj)ellenben(lassan közelít)
    curG=(curG+dG);  //ha increaseRate=1 akkor a lassu lépés nem csordulhat túl, mert a dB érték fentebb 0-ba áll
    curB=(curB+dB);  
 
  }else{                      		//ha nem kell valtozni akkor johet az uj cel
    if(colorBlack){                 //ha nincs senki kozelben sotetbe valt
      finR = 0;
      finG = 0;
      finB = 0;
    }else{                          	//ha vannak a kozelben
      x=(x<6)?(x+1):(0);				//x ertek cirkulalas 0->6
      finR = rgb[colorPalette][x][0];	//a megfelelo palettából az x-edik szín r,g,b azaz 0,1,2 byte-ok
      finG = rgb[colorPalette][x][1];
      finB = rgb[colorPalette][x][2];
    }
  } 
}
//////////////////////////////////   SETUP   //////////////////////////////////////////////
void setup()
{

  pinMode(redPin, OUTPUT);
  pinMode(grnPin, OUTPUT);
  pinMode(bluPin, OUTPUT);
  pinMode(mutePin, OUTPUT);

  digitalWrite(mutePin, 1);


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

  Timer1.initialize(20000);
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
   k+=1;
  }
  //button pushes
  if (ubutton)
  {
   ubutton = false;
   buttonRead();
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
    //if(k%30), if(k%125)
                  Serial.print(s1_adc); 
                  Serial.print("\t ");
                  Serial.print(s2_adc); 
                  Serial.print("\t ");
                  Serial.print(s3_adc);
                  Serial.print("\t");
                  Serial.println(k);
                  
  }
}
