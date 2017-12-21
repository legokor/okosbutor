/*
 * main.h
 *
 *
 */


#include "Arduino.h"
#include "TimerOne.h"

/*
 * To use libraries, must be chosen at Arduino->Add a library....
 * if the desired lib not there, search it at Arduino->Preferences->Library Manager.
 * (under Sloeber, Eclipse IDE)
 *
 */

#ifndef MAIN_H_
#define MAIN_H_

#define period 50000 //in microseconds
#define inEachCycle 1
#define inEverySecondCycle 2
#define inEveryTenthCycle 10

/*
 * Booleans to notify the main() to handle the periodically generated interrupts:
 * (maybe Hun. Not. can be useful)
 *
 */


bool uled=false;
bool uadc=false;
bool usound=false;
bool ubutton=false;


/*
 * Handlers for the above
 */

void buttonRead();
void ADCread();

void led();
void sound();


word iISR=0;

#endif /* MAIN_H_ */
