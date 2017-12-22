#include "button.h"
#include "handlers.h"
#include "led.h"
#include "main.h"
#include "ultra.h"
#include "zone.h"


//sensor
Ultrasonic sensor1(trig1, echo1, 50000UL); Ultrasonic sensor2(trig2, echo2, 50000UL); Ultrasonic sensor3(trig3, echo3, 50000UL); Ultrasonic sensor4(trig4, echo4, 50000UL);
int cm1=0,cm2=0,cm3=0,cm4=0;
//mp3
DFRobotDFPlayerMini myDFPlayer;
							//  rx tx
SoftwareSerial mySoftwareSerial(7, 4);

void timingISR(void)
{
  iISR++;
  k++;				//very long count
  usensor = true;
  uzone=true;
  uled = true;

  if (!(iISR % 2)) //100ms
    {
  	ubutton = true;
    }

  if (!(iISR % 20))		//500ms
  {
    usend = true;
  }
  if (!(iISR % 2048))
  {
	  //ritka dolgok, pl calibration
  }
}

/*
void timingISR(void)
{
  iISR++;
  k++;				//very long count
  usensor = true;
  uzone=true;
  //uled = true;

  if (!(iISR % 10))
    {
  	usend=true;
  	Serial.println("vazze");

    }
  if (!(iISR % 40))		//500ms
    {
  	  usend=true;
  		Serial.println("vazze2mp");

    }

  if (!(iISR % BUTTON_TIME_100ms))
  {
	ubutton = true;
  }
  if (!(iISR % TIMEOUT_20s))
  {
		Serial.println("20s elapsed");
  }
}
*/


/*
 * 1st section:
 * 		BUTTON HANDLING
 */
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
			Serial.println("now pushed");
		}
		break;
	case Pushed:
		//timer ticking from pushStartedAt
		if(!bButtonPushed)
		{
			pushReleasedAt=k;
			nyomogomb=Released;
			Serial.println("now released");

		}
		else if(k>(pushStartedAt+BUTTON_TIME_5s))
		{
			nyomogomb=LongPush;
			Serial.println("now pushed 5s long");
		}
		break;
	case Released:
		//timer ticking from pushReleasedAt
		if(bButtonPushed)
		{
			pushedAgainAt=k;
			nyomogomb=PushedAgain;
			Serial.println("pushed again");
		}
		else if(k>(pushReleasedAt+BUTTON_TIME_1s))
		{
			nyomogomb=ShortPush;
			Serial.println("now pushed 1s long");
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
		//egyik funkcio
		if(!bButtonPushed)
		{
			nyomogomb=NotPushed;
		}
		break;


	case LongPush:
		// calibrate
		ledSetBlinking(TIMEOUT_5s, BUTTON_TIME_1s, 0.5);
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
 */
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


  void led(void){               //may increase an int to perform other tasks
	  if(ledstrip!=Blinking)
	  {
		  dR=(0<(finR-curR))?(1):(-1);  // fol vagy le fele kell valtozni
		  if(0 == (finR-curR)){dR=0;}
			  //dR=(0==(finR-curR))?(0):(dR); // elerte a celt

		  dG=(0<(finG-curG))?(1):(-1);
		  if(0 == (finG-curG)){dG=0;}
				  //dG=(0==(finG-curG))?(0):(dG);

		  dB=(0<(finB-curB))?(1):(-1);
		  if(0 == (finB-curB)){dB=0;}
				  //dB=(0==(finB-curB))?(0):(dB);


		  if(dR || dG || dB){         //ha valaminek változnia kell
			curR=(curR+dR);  //kérdés(ha számolás és túlcsordulás iránya különbözõ)akkor(lépj)ellenben(lassan közelít)
			curG=(curG+dG);  //ha increaseRate=1 akkor a lassu lépés nem csordulhat túl, mert a dB érték fentebb 0-ba áll
			curB=(curB+dB);
		  }
		  else{ //ha nem kell valtozni akkor johet az uj cel
			  if(colorBlack){                 //ha nincs senki kozelben sotetbe valt
				  finR = 0;
				  finG = 0;
				  finB = 0;
			  }
			  else{                          	//ha vannak a kozelben
				  x=(x<6)?(x+1):(0);				//x ertek cirkulalas 0->6
				  finR = rgb[colorPalette][x][0];	//a megfelelo palettából az x-edik szín r,g,b azaz 0,1,2 byte-ok
				  finG = rgb[colorPalette][x][1];
				  finB = rgb[colorPalette][x][2];
				  //valami skalafaktor lehet zona fuggvenyeben>>>> a zona valt ColorPalette-t
			  }
		  }
	  }
	  else
	  {
		  ledBlinking();
	  }
}

  void ledSetBlinking(int duration_k_increments, int period_k_increments, int fill=0.5)
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
  //kb. 50-100ms-kent kellene meghivni
  //atlagolni nem kell, vagy csak keveset, mert nincs zavarjel

  	cm1=sensor1.distanceRead();
  	if(cm1==0)
  	{
  		cm1=401;
  	}
  	delay(1000);
  	cm2=sensor2.distanceRead();
  	if(cm2==0)
  		{
  			cm2=401;
  		}
  	delay(1000);

  	cm3=sensor3.distanceRead();
  	if(cm3==0)
  		{
  			cm3=401;
  		}
  	delay(1000);

 	cm4=sensor4.distanceRead();
  	if(cm4==0)
  		{
  			cm4=401;
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
bool inline zonetrig(int zone_border_cm)
{
	if(cm1<iSensor1OffsetValue && cm1<zone_border_cm)
	{
		//Serial.print("sensor 1 triggers z1 with cm of ");Serial.println(cm1);
		return true;
	}

	if(cm2<iSensor2OffsetValue && cm2<zone_border_cm)
	{
		//Serial.print("sensor 2 triggers z1:");Serial.println(cm2);
		return true;
	}

	if(cm3<iSensor3OffsetValue && cm3<zone_border_cm)
	{
		return true;
	}
/*
	if(cm4<iSensor4OffsetValue && cm4<zone_border_cm)
	{
		return true;
	}

	//TODO: cm1>iSensor1OffsetValue elvben nem lehetseges
	 */

	return false;

}

void allzonetrigger()
{
	switch (zone1)
	{
	case idle :
		if(zonetrig(iZone1Radius))
		{
			zone1=triggered;
			//Serial.println("now triggered from idle"); - meghivodik ez is
		}
		break;

	case triggered:
		digitalWrite(1,13);//led
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
			digitalWrite(0,13);//led
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
				Serial.println("z2 trig");//
			}
			break;

		case triggered:
			digitalWrite(1,13);//led
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
				digitalWrite(0,13);//led
				zone2=idle;
			}
			else if(zonetrig(iZone2Radius))
			{
				zone2=triggered;
			}
			break;
		}//end of switch

	colorBlack=(zone1==idle); //NOT(all idle)

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
	delay(10000);	//menekulj!! not working

	iSensor1OffsetValue=sensor1.distanceRead();
  	delay(1000);

	iSensor2OffsetValue=sensor2.distanceRead();
  	delay(1000);

	iSensor3OffsetValue=sensor3.distanceRead();
  	delay(1000);

	iSensor4OffsetValue=sensor4.distanceRead();
}

inline void calibrate()
{
	// in very 5 min, recalculating.
	// for each sensor in sensors  - python <3
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
}


//////////////////////////////////   SETUP   //////////////////////////////////////////////
void setup()
{
  /*optionally paste here:
   *
   * 	- pin mode setup
   * 	- register magics
   */
  Serial.begin(9600);
  Serial.println("serial enabled");

  	pinMode(redPin, OUTPUT);
  	pinMode(grnPin, OUTPUT);
	pinMode(bluPin, OUTPUT);
	//pinMode(chargeGreen, OUTPUT);
	//pinMode(chargeRed, OUTPUT);
	pinMode(mutePin, OUTPUT);

    digitalWrite(mutePin, 1);
    //digitalWrite(chargeGreen, 1);
    //digitalWrite(chargeRed, 1);

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


	ledSetBlinking(TIMEOUT_5s, BUTTON_TIME_1s, 0.5);
	//initialCalibrate();
	Serial.println("Calibrated with:");
	Serial.print(iSensor1OffsetValue);Serial.print("\t");
	Serial.print(iSensor2OffsetValue);Serial.print("\t");
	Serial.print(iSensor3OffsetValue);Serial.print("\t");
	Serial.println(iSensor4OffsetValue);
	ledSetBlinking(TIMEOUT_5s, BUTTON_TIME_2s, 0.1);



}

void loop()
{
	if (usensor)
	  {
	   usensor = false;
	   sensor();
	  }
	if (ubutton)
	  {
	   ubutton = false;
	   buttonRead();
	  }

  if (usend)
  {
    usend = false;
	Serial.print("zone1: ");Serial.print(zone1);
	Serial.print("\t zone2: ");Serial.print(zone2);
	Serial.print("\t colorNo: ");Serial.print(x);
	Serial.print("\t \t measured: ");
	Serial.print(cm1);Serial.print("\t");
	Serial.print(cm2);Serial.print("\t");
	Serial.print(cm3);Serial.print("\t");
	Serial.println(cm4);
  }

  if(uzone)
  {
	  uzone=false;
	  allzonetrigger();
  }
}
