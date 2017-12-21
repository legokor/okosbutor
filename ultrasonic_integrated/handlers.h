/*
 * handlers.h
 *
 *  Created on: 2017. dec. 21.
 *      Author: L�szl�
 */

#ifndef HANDLERS_H_
#define HANDLERS_H_

word iISR=0;
long int k=0; //lasts as long as 29h

#define inEachCycle 1
#define inEverySecondCycle 2
#define inEveryTenthCycle 10

bool uzone=false;
bool usensor=false;
bool usend=false;
bool uled=false;
bool usound=false;
bool ubutton=false;


/*
 * Handlers for the above
 */
void allzonetrigger();
bool inline zonetrig(int sensor_cm, int zone_border_cm);
void sensor();
void led();
void buttonRead();

//void send();



#endif /* HANDLERS_H_ */
