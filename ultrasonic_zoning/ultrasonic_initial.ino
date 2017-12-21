#include "main.h"
#include "ultra.h"

long int k=0; //lasts as long as 29h
long int j=0; //mirror

Ultrasonic sensor1(trig1, echo1, 50000UL);
Ultrasonic sensor2(trig2, echo2, 50000UL);
Ultrasonic sensor3(trig3, echo3, 50000UL);
Ultrasonic sensor4(trig4, echo4, 50000UL);

int cm1=0;
int cm2=0;
int cm3=0;
int cm4=0;



void timingISR(void)
{
  iISR++;
  k++;				//very long count
  //usensor = true;

  if (!(iISR % inEachCycle))
  {
	  //    usensor = true;
  }

  if (!(iISR % 10))		//500ms
  {
    usensor = true;
  }
  if (!(iISR % 2048))
  {
	  //ritka dolgok
  }
}

/*
 * Eldonti, kell-e triggerelni. Vegigmegy minden szenzoron, figyelembe veve azok egyedi
 * latokoret. (Pl ott egy akadaly).
 * Zonat triggerel, aminek a hatosugarat meg kell adni.
 *
 * Lehet, hogy a sensorNumber elhagyhato.
 */
bool inline zonetrig(int sensorNumber, int zone_border_cm)
{

	switch (sensorNumber)
		{
		case 1 :
			if(cm1<iSensor1OffsetValue && cm1<zone_border_cm)
			{
				return true;
			}
			break;
/*
		case 2:
			if(cm2<iSensor2OffsetValue && cm2<zone_border_cm)
			{
				return true;
			}
			break;

		case 3:
			if(cm3<iSensor3OffsetValue && cm3<zone_border_cm)
			{
				return true;
			}
			break;
		case 4:
			if(cm4<iSensor4OffsetValue && cm4<zone_border_cm)
			{
				return true;
			}
			break;*/
	}//end of switch

	return false;
}

void allzonetrigger()
{
switch (zone1)
	{
	case idle :
		if(zonetrig(1, iZone1Radius))
		{
			zone1=triggered;
		}
		break;

	case triggered:
		digitalWrite(1,13);//led
		if(!zonetrig(1, iZone1Radius))
			//TODO: cm1>iSensor1OffsetValue elvben nem lehetseges
			zone1=timeouting;
			iZone1TimeoutStart=k;
		break;

	case timeouting :
		//ido eltelt:

		zone1=idle;

		if(zonetrig(1,iZone1Radius))
		{
			zone1=triggered;
		}
		break;
		}//end of switch
}


void calibrate()
{
	// for each sensor in sensors  - python <3
	int i;
	iSumToCalibrate=0;
	for(i=0; i<SamplesToCalibrate; i++)
	{
		iSumToCalibrate+=iSensor1ValueArray;
	}
	iSensor1OffsetValue=iSumToCalibrate/SamplesToCalibrate;
}

void sensor()
{
//kb. 50-100ms-kent kellene meghivni
//atlagolni nem kell, vagy csak keveset, mert nincs zavarjel
	cm1=sensor1.distanceRead();
	cm2=sensor2.distanceRead();

	//delay(20);
	cm3=sensor3.distanceRead();
	//delay(20);
	//digitalWrite(signPin,0);
	cm4=sensor4.distanceRead();
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
   *	-
   *
   */
  Serial.begin(9600);
  Serial.println("serial enabled");


  Timer1.initialize(50000);
  delay(100);
  Timer1.attachInterrupt(timingISR);

  pinMode(signPin,OUTPUT);
  digitalWrite(signPin,1);
}

void loop()
{
  //handle leds
  if (usensor)
  {
   usensor = false;
   sensor();
   //k+=1;

   	   	   	   	   	 Serial.print("measured: ");
                     Serial.print(cm1);Serial.print("\t");
                     Serial.print(cm2);Serial.print("\t");
                     Serial.print(cm3);Serial.print("\t");
                     Serial.println(cm4);
  }

  //handle sound
  if (usend)
  {
    usend = false;
  }

  if(uzone)
  {
	  uzone=false;
	  allzonetrigger();
  }
}
