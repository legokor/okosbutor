#include "inc/battery.h"
#include "inc/button.h"
#include "inc/handlers.h"
#include "inc/led.h"
#include "inc/main.h"
#include "inc/ultra.h"
#include "inc/zone.h"


//sensor
Ultrasonic sensor1(trig1, echo1, 20000UL); Ultrasonic sensor2(trig2, echo2,20000UL); Ultrasonic sensor3(trig3, echo3, 20000UL); Ultrasonic sensor4(trig4, echo4, 20000UL);
//mp3
DFRobotDFPlayerMini myDFPlayer;
							//  rx tx
SoftwareSerial mySoftwareSerial(7, 4);

void timingISR(void)
{
  iISR++;
  k++;				//very long count

  usensor = true;
  usensormid = true;


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
	  uzone=true;
	  ubutton = true;
	  uindicator=true;
  }

  if (!(iISR % TIMEOUT_1s))
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


	  Serial.println("soft reset");
	  ubutton=false;
	  uled=false;
	  usend=false;
	  usensor=false;
	  usound=false;
	  uzone=false;
	  ucalibrate=false;

  }

  if(k<TIMEOUT_20s && (nyomogomb==ShortPush) && (Calibrated!=Done))
  {
	  ucalibrate=true;
	  ubattery=true;

	  myDFPlayer.pause();
	  //Serial.println("entering calibration");
  }

  if(k<TIMEOUT_20s && (nyomogomb==PushedAgain))
   {
 	  ubattery=true;
 	 indicatorRed.state=ToBlink;
 	 indicatorRed.blinkCount=5;

 	indicatorGreen.state=ToBlink;
 	indicatorGreen.blinkCount=2;
 	  uindicator=true;
 	  //Serial.println("battery info");
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
	else if (batteryVoltage>=1330)
	{
		akku=VoltageHigh;
	}
	else
	{
		akku=VoltageCriticalLow;
	}

	switch(akku)
	{
	case VoltageCriticalLow:
	{

		  indicatorRed.state=ToBlink;
		  indicatorRed.blinkCount=3;

		  Serial.println("Voltage Critical!");
	  break;
	}
	case VoltageHigh:

	  //				AC / DC
	{
		indicatorGreen.state=ToBlink;
		indicatorGreen.blinkCount=3;


	  Serial.println("Voltage High, blinking");
	  break;
	}
	default:
		//dummy:
		Serial.println("Voltage tested");

	}
}

void blinkIndicators()
{
	switch(indicatorRed.state)
	{
	case ToBlink:
		if(indicatorRed.blinkCount>0)
		{
			indicatorRed.LitUpAt=k;
			indicatorRed.state=IsOn;
			digitalWrite(chargeRed,1);
			indicatorRed.blinkCount--;
		}
		else
		{
			indicatorRed.state=IndicatorOff;
		}

		break;
	case IsOn:
		if(k>(indicatorRed.LitUpAt+(indicatorRed.onTime)))
		{
			indicatorRed.state=IsOff;
			indicatorRed.TurnedOffAt=k;
			digitalWrite(chargeRed,0);
		}
		break;
	case IsOff:
		if(k>(indicatorRed.TurnedOffAt+(indicatorRed.offTime)))
		{
			indicatorRed.state=ToBlink;

		}
		break;
	}//end of switch red led

	switch(indicatorGreen.state)
		{
		case ToBlink:
			if(indicatorGreen.blinkCount>0)
			{
				indicatorGreen.LitUpAt=k;
				indicatorGreen.state=IsOn;
				digitalWrite(chargeGreen,1);
				indicatorGreen.blinkCount--;
			}
			else
			{
				indicatorGreen.state=IndicatorOff;
			}

			break;
		case IsOn:
			if(k>(indicatorGreen.LitUpAt+(indicatorGreen.onTime)))
			{
				indicatorGreen.state=IsOff;
				indicatorGreen.TurnedOffAt=k;
				digitalWrite(chargeGreen,0);
			}
			break;
		case IsOff:
			if(k>(indicatorGreen.TurnedOffAt+(indicatorGreen.offTime)))
			{
				indicatorGreen.state=ToBlink;
			}
			break;
		}//end of switch red led


	//	number of blinks	>>	proportional with voltage. Blink to button event
	//	longPush initiates.
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
		}
		break;
	case Pushed:
				//timer ticking from pushStartedAt
		if(!bButtonPushed)
		{
			pushReleasedAt=k;
			nyomogomb=Released;
		}
		else if(k>(pushStartedAt+BUTTON_TIME_5s))
		{
					//u8LedSpeed=(u8LedSpeed==1)?(4):(1);
			ledSetBlinking(TIMEOUT_5s, TIMEOUT_1s, 0.5);
			//myDFPlayer.randomAll();
			nyomogomb=LongPush;
					//Serial.println("now pushed 5s long");
		}
		break;
	case Released:
				//timer ticking from pushReleasedAt
		if(bButtonPushed)
		{
			pushedAgainAt=k;
			//colorPalette=(colorPalette==(u8ColorPalettesCount-1))?(0):(colorPalette+1);
			uindicator=true;
			myDFPlayer.next();
			nyomogomb=PushedAgain;
					// Serial.println("pushed twice - switch palette");
		}
		else if(k>(pushReleasedAt+BUTTON_TIME_1s))
		{
			//bContinousLight=(bContinousLight)?(false):(true);
			nyomogomb=ShortPush;
					 Serial.println("short push");
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
		if(!bButtonPushed)
		{
			nyomogomb=NotPushed;
		}
		break;


	case LongPush:
		if(!bButtonPushed)
		{
			//Serial.println("Blinking initiated");
			nyomogomb=NotPushed;
		}
		break;
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
  				  digitalWrite(posztamensLed,0);
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
	  			digitalWrite(posztamensLed,1);
  			  }
  		  }
  	  break;
  	  //case automatic

  	 // case Manual:
  		  //TODO: mukodesi mod megadasa
  //		  finR = rgb[colorPalette][x][0];	//a megfelelo palettából az x-edik szín r,g,b azaz 0,1,2 byte-ok
  //		  finG = rgb[colorPalette][x][1];
  //		  finB = rgb[colorPalette][x][2];

  	  case BlinkingStart:
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
	switch(ledstrip)
	{
	case Automatic:
	{
		ledstrip=BlinkingStart;
		iBlinkStart=k;
		iBlinkPeriodStart=k;
		iBlinkDuration=duration_k_increments;
		iBlinkPeriod=period_k_increments;
		iBlinkFill=fill;
		break;
	}
	/*case BlinkingStart:
	{
		if(k>(iBlinkStart+iBlinkDuration))							// meddig?
		{
			//ido letelt
			ledstrip=Automatic;
		}
		else if (k<(iBlinkPeriodStart+iBlinkPeriod*iBlinkFill))
		{
			ledstrip=BlinkingPeriodOn;
		}
		else if (k<(iBlinkPeriodStart+iBlinkPeriod))
		{
			//meg period-on belul de a fill (bekapcsolt) allapoton kivul:
			ledstrip=BlinkingPeriodOff;
		}
		else
		{
			//period ended
			iBlinkPeriod=period_k_increments;
		}

		break;
	}*/
	default:
		//first to be tested this way:
		ledBlinking();
		break;

	}//eof switch

	/*if(ledstrip==BlinkingPeriodOn)
	{
		curR=0;
		curB=255;
		curG=255;
	}
	else
	{
		  curR=0;
		  curB=0;
		  curG=0;
	}*/

  }
void ledBlinking()
  {
	  if(BlinkingStart==ledstrip)											// villogni fog
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
					  curB=0;
					  curG=0;
				  }
			  }// 1 periodus vege
			  iBlinkPeriodStart=k;
		  }
		  // teljes villogasi ido vege
		  else
		  {
		  ledstrip=Automatic; //mar nincs villogasi modban
		  }
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


	cm[1][iSensorArrayIterator]=sensor2.distanceRead();
	if(cm[1][iSensorArrayIterator]==0)
	{
		cm[1][iSensorArrayIterator]=401;
	}

	delayMicroseconds(15000);	//ensure 3.2m viewrange

	cm[0][iSensorArrayIterator]=sensor1.distanceRead();
	if(cm[0][iSensorArrayIterator]==0)
	{
		cm[0][iSensorArrayIterator]=401;
	}
		//Serial.print("a");

	#if sensorValuesToAverage>2
	delayMicroseconds(14000); // seems no effect, max 1.8m

	cm[2][iSensorArrayIterator]=sensor3.distanceRead();
	if(cm[2][iSensorArrayIterator]==0)
	{
		cm[2][iSensorArrayIterator]=401;
	}
	#endif
	#if SensorsToRead==4

	cm[3][iSensorArrayIterator]=sensor4.distanceRead();
	if(cm[3][iSensorArrayIterator]==0)
	{
		cm[3][iSensorArrayIterator]=401;
	}

	#endif
	//tobbsegi szavazas, a kozepsot tartjuk meg: sensor_mid()

}

inline void sensor_mid()
{
	for(int i=0;i<SensorsToRead;i++)
	{
		if(
				((cm[i][0]<=cm[i][1])&&(cm[i][1]<=cm[i][2]))	||	// ( 1.) <= ( 2.) <= ( 3.) ----> 2. is the middle,,, etc.
				((cm[i][2]<=cm[i][1])&&(cm[i][1]<=cm[i][0]))	)
		{
				cmAveraged[i]=cm[i][1];
		}
		else if(
				((cm[i][0]<=cm[i][2])&&(cm[i][2]<=cm[i][1]))	||
				((cm[i][1]<=cm[i][2])&&(cm[i][2]<=cm[i][0]))	)
		{
				cmAveraged[i]=cm[i][2];
		}
		else
		{
				cmAveraged[i]=cm[i][0];
		}
		//Serial.print(i);Serial.print(": ");Serial.print(cm[i][0]);Serial.print("\t");Serial.print(cm[i][1]);Serial.print("\t");Serial.print(cm[i][2]);Serial.print("\t mid:");Serial.println(cmAveraged[i]);
	}
	//Serial.println("-");
}

void sound()
{

  //felkapcsolas a zone triggernel
  //itt csak hangero

	switch (zone1)
	{
	case triggered:
		// iMinZone1 biztosan kisebb mint iZoneRad, hisz triggered.
		if(iMinZone1<20)
		{
			finVol = SOUND_MAX_VOL_ZONE1+SOUND_OFFSET_VOL_ZONE1;
			colorPalette=0;
		}
		else if (iMinZone1<30)
		{
			finVol = SOUND_MID_VOL_ZONE1+SOUND_OFFSET_VOL_ZONE1;
			colorPalette=5;

		}
		else // useless: if(iMinZone1<50)
		{
			finVol = SOUND_MID_VOL_ZONE1+SOUND_OFFSET_VOL_ZONE1;
			colorPalette=5;

		}
				//finVol = SOUND_MAX_VOL_ZONE1;
				//map(iMinZone1, iZone1Radius, 1, 0, SOUND_MAX_VOL_ZONE1) + SOUND_OFFSET_VOL_ZONE1;
		break;

	case timeouting:
		break;

	case idle:
		// if zone2 triggered
		switch (zone2)
		{
		case triggered:
			if(iMinZone2<60)
			{
				finVol = SOUND_MAX_VOL_ZONE2+SOUND_OFFSET_VOL_ZONE2;
				colorPalette=6;

			}
			else if (iMinZone2<80)
			{
				finVol = SOUND_MID_VOL_ZONE2+SOUND_OFFSET_VOL_ZONE2;
				colorPalette=6;

			}
			else // useless: if(iMinZone1<50)
			{
				finVol = SOUND_MID_VOL_ZONE2+SOUND_OFFSET_VOL_ZONE2;
				colorPalette=7;

			}
			//finVol =SOUND_MAX_VOL_ZONE2;
			finVol = (finVol<(SOUND_MAX_VOL_ZONE2+ SOUND_OFFSET_VOL_ZONE2))?(finVol+1):(SOUND_MAX_VOL_ZONE2+ SOUND_OFFSET_VOL_ZONE2);
					//finVol =map(iMinCm, iZone2Radius, 1, 0, SOUND_MAX_VOL_ZONE2) + SOUND_OFFSET_VOL_ZONE2;
			break;
		case timeouting:
			break;
		case idle:
			break;
		}
	break;
	}

  //SOUND FADING PART

	if 		(curVol < finVol)		// hangosabb lesz
	{
		curVol = ((curVol + 1) > finVol) ? (finVol) : (curVol + 1);
		myDFPlayer.volume(curVol);
		digitalWrite(mutePin, 0);

			//Serial.println("vol++");
	}
	else if	(curVol > finVol)		// halkabb lesz
	{
		curVol = ((curVol - 1) < finVol) ? (finVol) : (curVol - 1);
		myDFPlayer.volume(curVol);
		digitalWrite(mutePin, 0);

			//Serial.println("vol--");
	}

	if (0 == curVol)
	{				//ha 0 a hangero, akkor mute-ol
			//Serial.println("to mute");
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
bool zonetrig(int zone_border_cm, int cm_output_zone)
{
	bool retval=false;
	iMinZone=400;			//visszaad egy tavolsagot is a legkozelebbivel

	//iterating all sensor

	for (int sensorsCount=0;sensorsCount<SensorsToRead;sensorsCount++)
	{
		//midvalue:
		if(cmAveraged[sensorsCount]<cmOffsets[sensorsCount] && cmAveraged[sensorsCount]<zone_border_cm)
			{
				iMinZone=(cmAveraged[sensorsCount]<iMinZone)?(cmAveraged[sensorsCount]):(iMinZone);
				retval =  true;
			}
	}
	//TODO: cm1>iSensor1OffsetValue elvben nem lehetseges

	switch(cm_output_zone)
	{
	case 1:
		iMinZone1=iMinZone;
		break;
	case 2:
		iMinZone2=iMinZone;
		break;
	default:
		break;
	}

	return retval;
}

void allzonetrigger()
{
	switch (zone1)
	{
	case idle :
		if(zonetrig(iZone1Radius,1))
		{
			//ha az 1. zonat triggereljuk...
			ledstrip=Automatic;
			colorPalette=0;
			digitalWrite(onboardLed,1);//led, teszthez
			zone1=triggered;
		}
		break;

	case triggered:
		if(!zonetrig(iZone1Radius,1))
		{
			zone1=timeouting;
			iZone1TimeoutStart=k;
		}
		break;

	case timeouting :
		//ido eltelt:
		if(k>iZone1TimeoutStart+TIMEOUT_10s)
		{
			digitalWrite(onboardLed,0);//led
			zone1=idle;
		}
		else if(zonetrig(iZone1Radius,1))
		{
			zone1=triggered;
		}
		break;
	}//end of switch

	//ftn-ul: zone 2 trig

	switch (zone2)
		{
		case idle :
			if(zonetrig(iZone2Radius,2))
			{
				zone2=triggered;
				Serial.println("randomAll - zone2 idle-> T'D");
				digitalWrite(mutePin,0);
				myDFPlayer.next();
				//TODO: next-ekkel bekkeljuk ki
			}
			break;

		case triggered:
			ledstrip=Automatic;
			colorPalette=2;
			if(!zonetrig(iZone2Radius,2))
			{
				zone2=timeouting;
				iZone2TimeoutStart=k;
						//Serial.println("now timeout started");
			}
			break;

		case timeouting :
			if(k>iZone2TimeoutStart+TIMEOUT_20s)
			{
				myDFPlayer.pause();	// zone 2 goes idle, music stop
				Serial.println("pause - zone2 idle-> T'D");
				//maybe pause instead
				zone2=idle;
			}
			if(k>iZone2TimeoutStart+TIMEOUT_10s)
			{
				finVol=0;
			}
			if(zonetrig(iZone2Radius,2))
			{
				zone2=triggered;
				//Serial.println("timeout -> T'D");

			}
			break;
		}//end of switch

	colorBlack=!(zone1!=idle || bContinousLight); //NOT(all idle)
	if(zone2!=idle)
	{
		digitalWrite(posztamensLed, 1);	//on
	}
	else
	{
		digitalWrite(posztamensLed, 0);	//off when zone 2 idle
	}

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
	switch(Calibrated)
	{
	case NotYet:

		Serial.println("Initial calibrate");

		digitalWrite(onboardLed, 1);
		digitalWrite(chargeGreen, 0);
		digitalWrite(chargeRed, 1);
		//Serial.println("start 5s wait:");

		k_start=k;
		Calibrated=Initiated;
		break;

	case Initiated:
		digitalWrite(posztamensLed,1);

		if(k>(k_start+TIMEOUT_5s))
		{
			Serial.println("5s elapsed!");
			Calibrated=InProgress;
		}
		break;

	case InProgress:
	{
		sensor();
		delayMicroseconds(50000);	//50ms
		sensor();
		delayMicroseconds(50000);	//50ms
		sensor();

		//Serial.println("end 5s wait:");

		for(int j=0; j<SensorsToRead; j++)
		{
			cmOffsets[j]=cmAveraged[j]+3;
		}

		Serial.println("Calibrated with:");
		Serial.print(cmOffsets[0]);Serial.print("\t");
		Serial.print(cmOffsets[1]);Serial.print("\t");
		Serial.print(cmOffsets[2]);Serial.print("\t");

		#if SensorsToRead==4
		Serial.print(cmOffsets[3]);Serial.print("\t");
		#endif

		digitalWrite(chargeGreen, 0);	//turn on?
		digitalWrite(chargeRed, 1);	//turn off
		Calibrated=Done;
		digitalWrite(posztamensLed,0);

		colorBlack=false;
		//desired color set!
		break;
	}
	case Done:
		//already calibrated
		ucalibrate=false;
		break;
	}//eof switch
}// eof calibrate

inline void calibrate()
{
	// in every ~5 min, recalculating.
	// for each sensor in sensors  //- python <3
	for(int j=0; j<SensorsToRead; j++)
	{
		iSumToCalibrate=0;
		for(int i=0; i<SamplesToCalibrate; i++)
		{
			iSumToCalibrate+=cmSamples[j][i];
		}
		cmOffsets[j]=iSumToCalibrate/SamplesToCalibrate;
	}

	Serial.println("Calibrated with:");
		Serial.print(cmOffsets[0]);Serial.print("\t");
		Serial.print(cmOffsets[1]);Serial.print("\t");
		Serial.print(cmOffsets[2]);Serial.print("\t");
		#if sensorValuesToAverage==4
		Serial.print(cmOffsets[3]);Serial.print("\t");
		#endif

}

void calibrateAtBeginning()
{
	//ledSetBlinking(TIMEOUT_1s*2, BUTTON_TIME_1s, 0.5); //won't work in setup
	initialCalibrate();
	//ledSetBlinking(TIMEOUT_5s, BUTTON_TIME_2s, 0.1);
}

void inline sensorValueAveraging()
{
	//filling up the array
	iSampleIterator=(iSampleIterator>SamplesToCalibrate-1)?(0):(iSampleIterator+1);
	for(int i=0;i<SensorsToRead;i++)
	{
		cmSamples[i][iSampleIterator]=cmAveraged[i];
	}
}



//////////////////////////////////   SETUP   //////////////////////////////////////////////
void setup()
{
  /*optionally paste here:
   *
   * 	- pin mode setup
   * 	- register magics
   */
  Serial.begin(230400);
  Serial.println("serial enabled");

  	pinMode(redPin, OUTPUT);
  	pinMode(grnPin, OUTPUT);
	pinMode(bluPin, OUTPUT);
	pinMode(chargeGreen, OUTPUT);
	pinMode(chargeRed, OUTPUT);
	pinMode(mutePin, OUTPUT);

    digitalWrite(mutePin, 1);	//muting works with molex set
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

	Serial.println("ena mute");
	mySoftwareSerial.begin(9600); // maybe higher baud leads to less noise?!
	Serial.println(F("Initializing DFPlayer."));
	while (!myDFPlayer.begin(mySoftwareSerial))
	{
		Serial.println(F("Unable to connect"));
		delay(1000);
	}
	Serial.println(F("done."));

    digitalWrite(mutePin, 0);

	myDFPlayer.volume(10);
	myDFPlayer.enableLoopAll();

    digitalWrite(chargeGreen, 1);
    digitalWrite(chargeRed, 0);

}

void loop()
{
	if (ucalibrate)
	{
		// szandekosan nincs itt: majd a kalibralas vegen nullazodik ucalibrate=false;
		calibrateAtBeginning();
	}
	else
	{
		if (usensor)
		{
			usensor = false;
			sensor();
		}
		if (usensormid)
		{
			usensormid = false;
			sensor_mid();
		}

		if (uled)
		{
			uled = false;
			led();
		}
		if (usound)
		{
			usound = false;
			sound();
		}
		if (uzone)
		{
			uzone=false;
			allzonetrigger();
		}
		if (ubutton)
		{
			ubutton = false;
			buttonRead();
		}

		if (ubattery)
		{
			ubattery = false;
			batteryMonitor();
		}

		if (uindicator)
		{
			uindicator=false;
			blinkIndicators();
		}
		if (usend)

		{
		usend = false;
		Serial.print("z1: ");Serial.print(zone1);
		Serial.print("\t z2: ");Serial.print(zone2);
		/*
		Serial.print("\t cNo: ");Serial.print(x);

		*/
		//Serial.print("\t set: ");Serial.print(bColorSettled);
		//Serial.print("\t z1tr?: ");Serial.print(zonetrig(iZone1Radius));
		//Serial.print("\t z2tr?: ");Serial.print(zonetrig(iZone2Radius));

		//Serial.print("\t cvol:\t");Serial.print(curVol);
		//Serial.print("\t fvol:\t");Serial.print(finVol);


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


		Serial.print("\t cm-s:\t");
		Serial.print(cmAveraged[0]);Serial.print("\t\t");
		Serial.print(cmAveraged[1]);Serial.print("\t\t");
		Serial.print(cmAveraged[2]);Serial.print("\t\t");
		Serial.println(cmAveraged[3]);
		/*Serial.print("\t");
		Serial.println(cm4);
		*/
	  }
		}
}
