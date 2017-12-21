#include "main.h"
#include "ultra.h"

long int k=0; //lasts as long as 29h
long int j=0; //mirror

//sensor variables
Ultrasonic sensor1(2, 4, 50000UL); Ultrasonic sensor2(trig2, echo2, 50000UL); Ultrasonic sensor3(trig3, echo3, 50000UL); Ultrasonic sensor4(trig4, echo4, 50000UL);
int cm1=0,cm2=0,cm3=0,cm4=0;


void timingISR(void)
{
  iISR++;
  k++;				//very long count
  usensor = true;
  uzone=true;

  if (!(iISR % 10))		//500ms
  {
    usend = true;
  }
  if (!(iISR % 2048))
  {
	  //ritka dolgok, pl calibration
  }
}

/*
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


inline void calibrate()
{
	// for each sensor in sensors  - python <3
	int i;
	iSumToCalibrate=0;
	for(i=0; i<SamplesToCalibrate; i++)
	{
		iSumToCalibrate+=iSensor1ValueArray[i];
	}
	iSensor1OffsetValue=iSumToCalibrate/SamplesToCalibrate;
}

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
void send()
{
	j=k;

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


  Timer1.initialize(50000);
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
