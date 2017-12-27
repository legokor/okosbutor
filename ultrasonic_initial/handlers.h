/*
 * handlers.h
 *
 *  Created on: 2017. dec. 21.
 *      Author: László
 */

#ifndef HANDLERS_H_
#define HANDLERS_H_

word iISR=0;
long int k=0; //lasts as long as 29h

#define inEachCycle 1
#define inEverySecondCycle 2
#define inEveryTenthCycle 10

bool ubattery = false;
bool ubutton=false;
bool uled=false;
bool usend=false;
bool usensor=false;
bool usensormid=false;
bool usound=false;
bool uzone=false;
bool ucalibrate=false;




/*
 * Handlers for the above
 */
void batteryMonitor();
void buttonRead();

void sensor();
void led();
void ledSetBlinking(int duration_k_increments, int period_k_increments, int fill=0.5);
void ledBlinking();


void allzonetrigger();
bool inline zonetrig(int sensor_cm, int zone_border_cm);

//void send();



#endif /* HANDLERS_H_ */
