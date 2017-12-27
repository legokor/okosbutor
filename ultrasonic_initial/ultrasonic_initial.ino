#include "battery.h"
#include "button.h"
#include "handlers.h"
#include "led.h"
#include "main.h"
#include "ultra.h"
#include "zone.h"


//sensor
Ultrasonic sensor1(trig1, echo1, 20000UL); Ultrasonic sensor2(trig2, echo2, 15000UL); Ultrasonic sensor3(trig3, echo3, 15000UL); Ultrasonic sensor4(trig4, echo4, 15000UL);
//mp3
DFRobotDFPlayerMini myDFPlayer;
							//  rx tx
SoftwareSerial mySoftwareSerial(7, 4);

void timingISR(void)
{
  iISR++;
  k++;				//very long count

  if(akku!=VoltageCriticalLow)
  {
	  uled = true;
	  if (!(iISR % (TIMEOUT_100ms*2)))
	  {
		  usound = true;
	  }
  }

  if (!(iISR % TIMEOUT_100ms))
  {
	  usensor = true;
	  uzone=true;
	  ubutton = true;

  }
  if (!(iISR % TIMEOUT_500ms))
  {
	  usend = true;
  }


  if (!(iISR % TIMEOUT_10s))
  {
	  sensorValueAveraging();
	  //ritka dolgok, pl calibration
  }
  if (!(iISR % (TIMEOUT_20s*10)))
    {
	  ubattery=true;


	  Serial.println("soft reset");	  //ubattery = false;
	  ubutton=false;
	  uled=false;
	  usend=false;
	  usensor=false;
	  usound=false;
	  uzone=false;
	  ucalibrate=false;

  }

  if(k<TIMEOUT_10s && (nyomogomb==ShortPush))
  {
	  ucalibrate=true;
	  Serial.println("entering calibration");
  }

  if (!(iISR % 4800)) //4 perc
    {
  	  calibrate();
    }
}


/*
 * 1st section:
 * 		BUTTON HANDLING
 */
void batteryMonitor()
{

	batteryAnalog = analogRead(batteryAnalogPin); 	//updating
	batteryVoltage=map(batteryAnalog, 0, 1024, 0, 2190);
	//Mapping: 1500 means 15V. (not using double^^)
	if(batteryVoltage<1100)
	{
		akku=VoltageBelow11V;
	}
	else if(batteryVoltage<1200)
	{
		akku=VoltageBelow12V;
	}
	else if(batteryVoltage>=1200)
	{
		akku=VoltageNormal;
	}
	else
	{
		akku=VoltageCriticalLow;
	}

	switch(akku)
	{
	case VoltageCriticalLow:
	  if (!(iISR % TIMEOUT_500ms))
		{
		  twoleds=RedBlinking;
		  Serial.println("Voltage Critical!");
		}
	  break;
	case VoltageNormal:
	  //if (!(iISR % TIMEOUT_5s))
		{
		  twoleds=GreenBlink;
		  //Serial.print("Voltage Normal\t");  Serial.println(batteryVoltage);
		  break;
		}
	default:
		//dummy:
		Serial.println("Voltage tested");

	}
}

void buttonRead()
{
	buttonVal = analogRead(buttonAnalogPin); 	//updating
	bButtonPushed=(buttonVal>900)?(false):(true);

	switch (nyomogomb)
	{
	case NotPushed:
		if(bButtonPushed)
		{
			pushStartedAt=k;
			nyomogomb=Pushed;
				//Serial.println("now pushed");
		}
		break;
	case Pushed:
		//timer ticking from pushStartedAt
		if(!bButtonPushed)
		{
			pushReleasedAt=k;
			nyomogomb=Released;
				//Serial.println("now released");

		}
		else if(k>(pushStartedAt+BUTTON_TIME_5s))
		{
			u8LedSpeed=(u8LedSpeed==1)?(4):(1);
			nyomogomb=LongPush;
			//Serial.println("now pushed 5s long");
		}
		break;
	case Released:
		//timer ticking from pushReleasedAt
		if(bButtonPushed)
		{
			pushedAgainAt=k;
			colorPalette=(colorPalette==(u8ColorPalettesCount-1))?(0):(colorPalette++);
			nyomogomb=PushedAgain;
			// Serial.println("pushed twice - switch palette");
		}
		else if(k>(pushReleasedAt+BUTTON_TIME_1s))
		{
			bContinousLight=(bContinousLight)?(false):(true);
			nyomogomb=ShortPush;
			// Serial.println("short push - continuous light");
		}
		break;
	case ShortPush:
		//next sound

		if(!bButtonPushed)
		{
			nyomogomb=NotPushed;
		}
		break;
	case PushedAgain:


		myDFPlayer.next();
		if(!bButtonPushed)
		{
			nyomogomb=NotPushed;
		}
		break;


	case LongPush:
		// calibrate
		//ledSetBlinking(TIMEOUT_5s, BUTTON_TIME_1s, 0.5);
		myDFPlayer.randomAll();
		Serial.println("Blinking initiated");
		if(!bButtonPushed)
		{
			nyomogomb=NotPushed;
		}
		break;

		//why
	}//end of switch

}


/*
 * 2nd section:
 * 		LED HANDLING
 *

/////////////////////////////////////    LED FADING    ///////////////////////////////////////
//
//    INPUT variables:  colorBlack - set by sensor reader: most a zona fogja megadni
//                        - shows dark final state
//                      colorPalette
//   GLOBAL constants:  rgb[3][7][3]
//
//   INSIDE variables:  x
//                      finR,finG,finB
//                      curR,curG,curB -> regiszterek
//                        dR,  dG,  dB
//

*/
inline void calcColorDifference()
{
	dR=(0<(finR-curR))?(u8LedSpeed):(-u8LedSpeed);  // fol vagy le fele kell valtozni
	if(0 == (finR-curR)){dR=0;}
	  //dR=(0==(finR-curR))?(0):(dR); // elerte a celt

	dG=(0<(finG-curG))?(u8LedSpeed):(-u8LedSpeed);
	if(0 == (finG-curG)){dG=0;}
		  //dG=(0==(finG-curG))?(0):(dG);

	dB=(0<(finB-curB))?(u8LedSpeed):(-u8LedSpeed);
	if(0 == (finB-curB)){dB=0;}
		  //dB=(0==(finB-curB))?(0):(dB);


	 if(dR || dG || dB){
		 bColorSettled=false;
	 }
	 else
	 {
		 bColorSettled=true;
		 iColorSettledAt=k;
	 }
}

void led(void){               //may increase an int to perform other tasks
  	  switch(ledstrip)
  	  {
  	  case Automatic:
  		calcColorDifference();
  		if(!bColorSettled)
  		{
  			if((k>(iColorSteppedAt+iLedStepTime)))
  			{
				curR=((curR+dR)>255)?(255):(((curR+dR)<0)?(0):(curR+dR));
				curG=((curG+dG)>255)?(255):(((curG+dG)<0)?(0):(curG+dG));
				curB=((curB+dB)>255)?(255):(((curB+dB)<0)?(0):(curB+dB));


				iColorSteppedAt=k;
  			}
  		}
  		else
  		{	//color settled
  			if(colorBlack)
  			  {                 //ha nincs senki kozelben sotetbe valt
  				  finR = 0;
  				  finG = 0;
  				  finB = 0;

  			  }
  			  else //if(k>(iColorSettledAt+iFinalColorStepTime))
  			  {
  				  Serial.println("color chg");
  				x=(x<6)?(x+1):(0);				//x ertek cirkulalas 0->6
  				finR = rgb[colorPalette][x][0];	//a megfelelo palettából az x-edik szín r,g,b azaz 0,1,2 byte-ok
  				finG = rgb[colorPalette][x][1];
				finB = rgb[colorPalette][x][2];
					//a zona valt ColorPalette-t
	  			bColorSettled=false;
  			  }
  		  }
  	  break;
  	  //case automatic

  	 // case Manual:
  		  //TODO: mukodesi mod megadasa
  //		  finR = rgb[colorPalette][x][0];	//a megfelelo palettából az x-edik szín r,g,b azaz 0,1,2 byte-ok
  //		  finG = rgb[colorPalette][x][1];
  //		  finB = rgb[colorPalette][x][2];

  	  case Blinking:
  		  ledBlinking();
  		  break;
  	  default:
  	  {
	  		  //Serial.println("default switch led");
  		  break;
  	  }
  	  case Off:
  		  //colorBlack=true;
  	  		  //Serial.println("off");

  		  break;
  	  }//end switch
  }

void ledSetBlinking(int duration_k_increments, int period_k_increments, double fill)
  {
	  ledstrip=Blinking;
	  iBlinkStart=k;
	  iBlinkPeriodStart=k;
	  iBlinkDuration=duration_k_increments;
	  iBlinkPeriod=period_k_increments;
	  iBlinkFill=fill;

  }
void ledBlinking()
  {
	  if(Blinking==ledstrip)											// villogni fog
	  {
		  if(k<(iBlinkStart+iBlinkDuration))							// meddig?
		  {
			  if(k<(iBlinkPeriodStart+iBlinkPeriod))					// milyen periodusidovel?
			  {
				  if(k<(iBlinkPeriodStart+iBlinkPeriod*iBlinkFill))		// milyen kitoltesi tenyezo?
				  {//red - TODO: to be corrected to any color
					  curR=0;
					  curB=255;
					  curG=255;
				  }//vilagitas vege
				  else
				  {
					  curR=0;
					  curB=255;
					  curG=255;
				  }
			  }// 1 periodus vege
			  iBlinkPeriodStart=k;
		  }// teljes villogasi ido vege
	  ledstrip=Normal; //mar nincs villogasi modban
	  }
  }


/*
 * 3rd section:
 * 		ULTRASONIC
 */


void sensor()
{
	iSensorArrayIterator=(iSensorArrayIterator<sensorValuesToAverage-1)?(iSensorArrayIterator+1):(0);
	delayMicroseconds(8000);// seems no effect, max 1.8m..2.1m

	cm2[iSensorArrayIterator]=sensor2.distanceRead();
	if(cm2[iSensorArrayIterator]==0)
	{
		cm2[iSensorArrayIterator]=401;
	}

	delayMicroseconds(15000);	//ensure 3.2m viewrange

	cm1[iSensorArrayIterator]=sensor1.distanceRead();
	if(cm1[iSensorArrayIterator]==0)
	{
		cm1[iSensorArrayIterator]=401;
	}
		//Serial.print("a");

	delayMicroseconds(14000); // seems no effect, max 1.8m

	cm3[iSensorArrayIterator]=sensor3.distanceRead();
	if(cm3[iSensorArrayIterator]==0)
	{
		cm3[iSensorArrayIterator]=401;
	}


	cm4[iSensorArrayIterator]=sensor4.distanceRead();
	if(cm4[iSensorArrayIterator]==0)
	{
		cm4[iSensorArrayIterator]=401;
	}

	//tobbsegi szavazas, a kozepsot tartjuk meg:
}

void sensor_mid()
{

	if(
			((cm1[0]<=cm1[1])&&(cm1[1]<=cm1[2]))	||	// ( 1.) <= ( 2.) <= ( 3.) ----> 2. is the middle,,, etc.
			((cm1[2]<=cm1[1])&&(cm1[1]<=cm1[0]))	)
	{
			sensor1Mid=cm1[1];
	}
	else if(
			((cm1[0]<=cm1[2])&&(cm1[2]<=cm1[1]))	||
			((cm1[1]<=cm1[2])&&(cm1[2]<=cm1[0]))	)
	{
			sensor1Mid=cm1[2];
	}
	else
	{
			sensor1Mid=cm1[0];
	}

}

/*
void sensorIter()
  {
  //kb. 50-100ms-kent kellene meghivni
  //atlagolni nem kell, vagy csak keveset, mert nincs zavarjel
	long int kdel=k;
	iSensor2Previous=cm2;
	iSensor3Previous=cm3;
	iSensor4Previous=cm4;

	iSensorIterator=(iSensorIterator==SensorsToRead)?(1):(iSensorIterator+1);


	if(iSensorIterator==1)
	{
		iSensor1Previous=cm1;
		cm1=sensor1.distanceRead();
		if(cm1==0)
		{
			//cm1=401;
			Serial.println("!1");

		}
		else
		{
			cm1=iSensor1Previous-0.9*(cm1-iSensor1Previous);
			//cm1=(cm1+iSensor1Previous)/2;
		}

	  	//exp atlagolassal: (bambi 34.dia)
	  	//cm1=iSensor1Previous-0.9*(cm1-iSensor1Previous);
	}


	if(iSensorIterator==2)
	{
		iSensor2Previous=cm2;
		cm2=sensor2.distanceRead();
		if(cm2==0)
		{
			Serial.println("!2");
		}
		else
		{
			cm2=(cm2+iSensor2Previous)/2;
		  			//exp atlagolassal: (bambi 34.dia)
		  	//cm2=iSensor2Previous-0.9*(cm2-iSensor2Previous);
		}


	}
	//sensor3
	if(iSensorIterator==3)
	{
		iSensor3Previous=cm3;
		cm1=sensor3.distanceRead();
		if(cm1==0)
		{
			//cm1=401;
			Serial.println("!3");

		}
		else
		{
			cm3=iSensor3Previous-0.9*(cm3-iSensor3Previous);
			//cm3=(cm3+iSensor3Previous)/2;
		}

	  	//exp atlagolassal: (bambi 34.dia)
	  	//cm1=iSensor1Previous-0.9*(cm1-iSensor1Previous);
	}
	//sensor4
	if(iSensorIterator==4)
	{
		iSensor4Previous=cm4;
		cm4=sensor4.distanceRead();
		if(cm4==0)
		{
			//cm4=401;
			Serial.println("!4");
		}
		else
		{
			//cm4=iSensor4Previous-0.9*(cm4-iSensor4Previous);
			cm4=(cm4+iSensor4Previous)/2;
		}

	  	//exp atlagolassal: (bambi 34.dia)
	  	//cm1=iSensor1Previous-0.9*(cm1-iSensor1Previous);
	}
  }

  */
void sound()
{

  //felkapcsolas a zone triggernel
  //itt csak hangero

	switch (zone1)
	{
	case triggered:
		// iMinCm biztosan kisebb mint iZoneRad, hisz triggered.
		finVol = SOUND_MAX_VOL_ZONE1;

				//map(iMinCm, iZone1Radius, 1, 0, SOUND_MAX_VOL_ZONE1) + SOUND_OFFSET_VOL_ZONE1;
		break;

	case timeouting:
		break;

	case idle:
		// if zone2 triggered
		switch (zone2)
		{
		case triggered:
					//finVol =SOUND_MAX_VOL_ZONE2;
			finVol = (finVol<(SOUND_MAX_VOL_ZONE2+ SOUND_OFFSET_VOL_ZONE2))?(finVol+1):(SOUND_MAX_VOL_ZONE2+ SOUND_OFFSET_VOL_ZONE2);
					//finVol =map(iMinCm, iZone2Radius, 1, 0, SOUND_MAX_VOL_ZONE2) + SOUND_OFFSET_VOL_ZONE2;
			break;
		case timeouting:
			break;
		case idle:
			finVol=0;
			//Serial.println("zone2 inactive");
			break;
		}
	break;
	}


  //SOUND FADING PART

	if 		(curVol < finVol)		// hangosabb lesz
	{
		curVol = ((curVol + 4) > finVol) ? (finVol) : (curVol + 4);	//4-es lepeskoz, szaturalassal
		myDFPlayer.volume(curVol);
		digitalWrite(mutePin, 0);

			//Serial.println("vol++");
	}
	else if	(curVol > finVol)		// halkabb lesz
	{
		curVol = ((curVol - 4) < finVol) ? (finVol) : (curVol - 4);	//4-es lepeskoz, szaturalassal
		myDFPlayer.volume(curVol);
		digitalWrite(mutePin, 0);

			//Serial.println("vol--");
	}

	if (0 == curVol)
	{			//ha 0 a hangero, akkor mute-ol
		digitalWrite(mutePin, 1);
	}
}

/*
 * 4th section:
 * 		ZONE MANAGEMENT
 * Eldonti, kell-e triggerelni. Vegigmegy minden szenzoron, figyelembe veve azok egyedi
 * latokoret. (Pl ott egy akadaly).
 * Zonat triggerel, aminek a hatosugarat meg kell adni.
 *
 * Lehet, hogy a sensorNumber elhagyhato.
 */

 /*
  * Zonetrig: border alapjan triggerel egy hatosugarat.
  * 	bool-t ad vissza hogy a hatosugar triggerelve van-e.
  * 	iMinCm-t allitja: minimalis triggerelt tavolsag
  */
bool zonetrig(int zone_border_cm)
{
	iMinCm=400;			//visszaad egy tavolsagot is a legkozelebbivel

	if(cm1<iSensor1OffsetValue && cm1<zone_border_cm)
	{
		iMinCm=(cm1<iMinCm)?(cm1):(iMinCm);
		return true;
	}

	if(cm2<iSensor2OffsetValue && cm2<zone_border_cm)
	{
		iMinCm=(cm2<iMinCm)?(cm2):(iMinCm);
		return true;

	}

	if(cm3<iSensor3OffsetValue && cm3<zone_border_cm)
	{
		iMinCm=(cm3<iMinCm)?(cm3):(iMinCm);
		return true;

	}

	if( cm4<iSensor4OffsetValue && cm4<zone_border_cm)
	{
		iMinCm=(cm4<iMinCm)?(cm4):(iMinCm);

		return true;
	}

	//TODO: cm1>iSensor1OffsetValue elvben nem lehetseges


	return false;
}

void allzonetrigger()
{
	switch (zone1)
	{
	case idle :
		if(zonetrig(iZone1Radius))
		{
			ledstrip=Automatic;
			colorPalette=0;
			digitalWrite(13,1);//led

			myDFPlayer.randomAll();

			//myDFPlayer.enableLoopAll();


			zone1=triggered;
			//Serial.println("now triggered from idle"); - meghivodik ez is
		}
		break;

	case triggered:
		if(!zonetrig(iZone1Radius))
		{
			zone1=timeouting;
			iZone1TimeoutStart=k;
			//Serial.println("now timeout started"); - eljut ide

		}
		break;

	case timeouting :
		//ido eltelt:
		if(k>iZone1TimeoutStart+TIMEOUT_5s)
		{
			digitalWrite(13,0);//led

			zone1=idle;
		}
		else if(zonetrig(iZone1Radius))
		{
			zone1=triggered;
		}
		break;
	}//end of switch

	switch (zone2)
		{
		case idle :
			if(zonetrig(iZone2Radius))
			{
				zone2=triggered;
				//myDFPlayer.enableLoopAll();
				//Serial.println("z2 trig");//
			}
			break;

		case triggered:
			ledstrip=Automatic;
			digitalWrite(mutePin,0);
			//digitalWrite(13,1);//led
			colorPalette=2;
			if(!zonetrig(iZone2Radius))
			{
				zone2=timeouting;
				iZone2TimeoutStart=k;
				//Serial.println("now timeout started"); - eljut ide

			}
			break;

		case timeouting :
			//ido eltelt:
			if(k>iZone2TimeoutStart+TIMEOUT_10s)
			{
				//digitalWrite(13,1);//led
				myDFPlayer.stop();	// zone 2 goes idle, music stop
				zone2=idle;
			}
			else if(zonetrig(iZone2Radius))
			{
				zone2=triggered;
			}
			break;
		}//end of switch

	colorBlack=!(zone1!=idle || bContinousLight); //NOT(all idle)

	//if zone1 triggered ---> intensity big
	//if zone2 triggered ---> int med

}


/*
 * 5th section:
 * 		CALIBRATION OF SENSORS
 *
 */
inline void initialCalibrate()
{
	int k_wait=k;
	Serial.println("start 5s wait:");

	do
	{
	    digitalWrite(13, 1);
	}
	while(k_wait<TIMEOUT_5s);
    digitalWrite(13, 0);

	Serial.println("end 5s wait:");

	//delay(10000);	//menekulj!! not working

	iSensor1OffsetValue=sensor1.distanceRead();
	/*
	while(k_wait<(TIMEOUT_5s+TIMEOUT_100ms)) {	}

	iSensor2OffsetValue=sensor2.distanceRead();
	while(k_wait<(TIMEOUT_5s+2*TIMEOUT_100ms)) { }

	iSensor3OffsetValue=sensor3.distanceRead();
  	delay(1000);

	iSensor4OffsetValue=sensor4.distanceRead();
	*/
	Serial.println("end calibration");

}
inline void calibrate()
{
	// in every ~5 min, recalculating.
	// for each sensor in sensors  //- python <3
	iSumToCalibrate=0;
	for(int i=0; i<SamplesToCalibrate; i++)
	{
		iSumToCalibrate+=iSensor1ValueArray[i];
	}
	iSensor1OffsetValue=iSumToCalibrate/SamplesToCalibrate;

	iSumToCalibrate=0;
	for(int i=0; i<SamplesToCalibrate; i++)
	{
		iSumToCalibrate+=iSensor2ValueArray[i];
	}
	iSensor2OffsetValue=iSumToCalibrate/SamplesToCalibrate;

	iSumToCalibrate=0;
	for(int i=0; i<SamplesToCalibrate; i++)
	{
		iSumToCalibrate+=iSensor3ValueArray[i];
	}
	iSensor3OffsetValue=iSumToCalibrate/SamplesToCalibrate;

	iSumToCalibrate=0;
	for(int i=0; i<SamplesToCalibrate; i++)
	{
		iSumToCalibrate+=iSensor4ValueArray[i];
	}
	iSensor4OffsetValue=iSumToCalibrate/SamplesToCalibrate;

	Serial.println("Calibrated with:");
		Serial.print(iSensor1OffsetValue);Serial.print("\t");
		Serial.print(iSensor2OffsetValue);Serial.print("\t");
		Serial.print(iSensor3OffsetValue);Serial.print("\t");
		Serial.print(iSensor4OffsetValue);Serial.print("\t");


}

void calibrateAtBeginning()
{
	ledSetBlinking(TIMEOUT_1s*2, BUTTON_TIME_1s, 0.5); //won't work in setup
	initialCalibrate();
	Serial.println("Calibrated with:");
	Serial.print(iSensor1OffsetValue);Serial.print("\t");
	Serial.print(iSensor2OffsetValue);Serial.print("\t");
	Serial.print(iSensor3OffsetValue);Serial.print("\t");
	Serial.println(iSensor4OffsetValue);
	ledSetBlinking(TIMEOUT_5s, BUTTON_TIME_2s, 0.1);
}

void inline sensorValueAveraging()
{
	//filling up the array
	iSampleIterator=(iSampleIterator==SamplesToCalibrate)?(0):(iSampleIterator+1);
	iSensor1ValueArray[iSampleIterator]=cm1;
	iSensor2ValueArray[iSampleIterator]=cm2;
	iSensor3ValueArray[iSampleIterator]=cm3;
	iSensor4ValueArray[iSampleIterator]=cm4;
}



//////////////////////////////////   SETUP   //////////////////////////////////////////////
void setup()
{
  /*optionally paste here:
   *
   * 	- pin mode setup
   * 	- register magics
   */
  Serial.begin(115200);
  Serial.println("serial enabled");

  	pinMode(redPin, OUTPUT);
  	pinMode(grnPin, OUTPUT);
	pinMode(bluPin, OUTPUT);
	pinMode(chargeGreen, OUTPUT);
	pinMode(chargeRed, OUTPUT);
	pinMode(mutePin, OUTPUT);

    digitalWrite(mutePin, 1);
    digitalWrite(chargeGreen, 1);
    digitalWrite(chargeRed, 1);

    //regisztermagic
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
  /*
   * calibration of sensor viewranges
   *
   */
	Timer1.initialize(period);
	delay(100);
	Timer1.attachInterrupt(timingISR);

	Serial.println("Start Blink");

	Serial.println("ena mute");
	mySoftwareSerial.begin(9600); // maybe higher baud leads to less noise?!
	Serial.println(F("Initializing DFPlayer."));
	while (!myDFPlayer.begin(mySoftwareSerial))
	{
		Serial.println(F("Unable to connect"));
		delay(1000);
	}
	Serial.println(F("done."));

    digitalWrite(chargeGreen, 1);
    digitalWrite(chargeRed, 0);

}

void loop()
{
	if (usensor)
	{
		usensor = false;
		sensor();
	}
	else if (uled)
	{
		uled = false;
		led();
	}

	else if (usound)
	{
		usound = false;
		sound();
	}
	else if (uzone)
	{
		uzone=false;
		allzonetrigger();
	}
	else if (ubutton)
	{
		ubutton = false;
		buttonRead();
	}
	else if (ucalibrate)
	{
		ucalibrate=false;
		calibrateAtBeginning();
	}
	else if (ubattery)
	{
		ubattery = false;
		batteryMonitor();
	}
	else if (usend)

	{
    usend = false;
	Serial.print("z1: ");Serial.print(zone1);
	Serial.print("\t z2: ");Serial.print(zone2);
	/*
	Serial.print("\t cNo: ");Serial.print(x);

	*/
	//Serial.print("\t set: ");Serial.print(bColorSettled);
	/*Serial.print("\t z1tr?: ");Serial.print(zonetrig(iZone1Radius));
	Serial.print("\t z2tr?: ");Serial.print(zonetrig(iZone2Radius));*/


/*
 * working
	Serial.print("\t r: ");Serial.print(finR);
	Serial.print(": ");Serial.print(curR);
	Serial.print("\t g: ");Serial.print(finG);
	Serial.print(": ");Serial.print(curG);
	Serial.print("\t b: ");Serial.print(finB);
	Serial.print(": ");Serial.print(curB);
*/

	//Serial.print("\t fV: ");Serial.print(finVol);
	//Serial.print("\t sIter: ");Serial.print(iSensorIterator);


	Serial.print("\t \t");
	Serial.print(cm1);Serial.print("\t\t");
	Serial.print(cm2);Serial.print("\t\t");
	Serial.println(cm3);
	/*Serial.print("\t");
	Serial.println(cm4);
	*/
  }
}
