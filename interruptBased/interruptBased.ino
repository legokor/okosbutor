#include "main.h"
#include "adc.c"
#include "led.c"
#include "sound.c"

void timingISR(void)
{
	iISR++;
	uled=true;
		
	if(!(iISR%2))
	{
		uadc=true;
	}
	
	if(!(iISR%8))
	{
		usound=true;
	}
	
}



void main() {
  Timer1.initialize(50000);
  delay(100);
  Timer1.attachInterrupt(timingISR);

  while(1)
  {
    //handle leds
	if(uled)
	{
		//TODO: write led refresh
	}
	
	//handle adc
	if(uadc)
	{
		//TODO: write adc read
	}
	
	//handle sound
	if(usound)
	{
		//TODO: write sound fade
	}
	
	//handle button pushes? with adc.
  }
}
