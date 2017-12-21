#include "main.h"

long int k=0; //lasts as long as 29h
long int j=0; //mirror

void timingISR(void)
{
  iISR++;
  if (!(iISR % inEachCycle))
  {
    uled = true;
  }
  if (!(iISR % 100))
  {
    ubutton = true;
  }

  if (!(iISR % inEverySecondCycle))
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


void buttonRead()
{
	j=k; //does nothing useful
}
void ADCread()
{
	j=k;

}

void led()
{
	j=k;

}
void sound()
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
                  Serial.print("Params ");
                  Serial.print(1);
                  Serial.print("\t ");
                  Serial.print(2);
                  Serial.print("\t");
                  Serial.println(k);
                  //Serial.print("\t");
                  //Serial.println(sensorstate);
  }
}
