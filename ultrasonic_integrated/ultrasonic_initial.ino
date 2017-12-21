#include "button.h"
#include "handlers.h"
#include "led.h"
#include "main.h"
#include "ultra.h"
#include "zone.h"


//sensor
Ultrasonic sensor1(2, 4, 50000UL); Ultrasonic sensor2(trig2, echo2, 50000UL); Ultrasonic sensor3(trig3, echo3, 50000UL); Ultrasonic sensor4(trig4, echo4, 50000UL);
int cm1=0,cm2=0,cm3=0,cm4=0;
//mp3
DFRobotDFPlayerMini myDFPlayer;
SoftwareSerial mySoftwareSerial(10, 11);


void timingISR(void)
{
  iISR++;
  k++;				//very long count
  usensor = true;
  uzone=true;
  uled = true;

  if (!(iISR % 10))		//500ms
  {
    usend = true;
  }

  if (!(iISR % 100))
  {
	ubutton = true;
    //usound = true;
  }
  if (!(iISR % 2048))
  {
	  //ritka dolgok, pl calibration
  }
}

/*
 * 1st section:
 * 		BUTTON HANDLING
 */
void buttonRead()
{
	buttonVal = analogRead(buttonAnalogPin); 	//updating
	bButtonPushed=(buttonVal>1000)?(true):(false);

	switch (nyomogomb)
	{
	case NotPushed:
		buttonCounter=k;
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
		else if(k>(pushStartedAt+BUTTON_TIME_1s))
		{
			nyomogomb=ShortPush;
			Serial.println("now pushed 1s long");
		}
		break;
	case ShortPush:
		//next sound

		if(!bButtonPushed)
		{
			nyomogomb=Pushed;
		}
		break;
	case PushedAgain:
		//egyik funkcio

	case LongPush:
		//wait 10s and calibrate




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
    curR=(curR+dR);  //kérdés(ha számolás és túlcsordulás iránya különbözõ)akkor(lépj)ellenben(lassan közelít)
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
      //valami skalafaktor lehet zona fuggvenyeben>>>> a zona valt ColorPalette-t
    }
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
  	//cm2=sensor2.distanceRead();
  	if(cm2==0)
  		{
  			cm2=401;
  		}
  	//delay(20);
  	//cm3=sensor3.distanceRead();
  	if(cm3==0)
  		{
  			cm3=401;
  		}
  	//delay(20);
  	//digitalWrite(signPin,0);
  	//cm4=sensor4.distanceRead();
  	if(cm4==0)
  		{
  			cm4=401;
  		}
  	//digitalWrite(signPin,1);

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
		//Serial.println("sensor 1 triggers");
		return true;
	}
	/*
	if(cm2<iSensor2OffsetValue && cm2<zone_border_cm)
	{
		return true;
	}

	if(cm3<iSensor3OffsetValue && cm3<zone_border_cm)
	{
		return true;
	}

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
		if(k>iZone1TimeoutStart+TIMEOUT_1s)
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

}


/*
 * 5th section:
 * 		CALIBRATION OF SENSORS
 *
 */
inline void initialCalibrate()
{
	delay(10000);	//menekulj!!

	iSensor1OffsetValue=sensor1.distanceRead();
	iSensor2OffsetValue=sensor2.distanceRead();
	iSensor3OffsetValue=sensor3.distanceRead();
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

  /*
   * calibration of sensor viewranges
   *
   */
  initialCalibrate();


  Timer1.initialize(period);
  delay(100);
  Timer1.attachInterrupt(timingISR);

  pinMode(signPin,OUTPUT);
//  digitalWrite(signPin,1);
}

void loop()
{
  if (usensor)
  {
   usensor = false;
   sensor();
  }

  if (usend)
  {
    usend = false;
	Serial.print("zone1: ");
	Serial.print(zone1);Serial.print("\t");
	Serial.print(k);Serial.print("\t");
	Serial.print("measured: ");
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
