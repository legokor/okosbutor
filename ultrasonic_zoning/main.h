/*
 * main.h
 *
 *
 */

#include "Arduino.h"
#include "TimerOne.h"
#include "Ultrasonic.h"

/*
 * To use libraries, must be chosen at Arduino->Add a library....
 * if the desired lib not there, search it at Arduino->Preferences->Library Manager.
 * (under Sloeber, Eclipse IDE)
 *
 */

#ifndef MAIN_H_
#define MAIN_H_

#define period 50000 // In microseconds---- TIMING accurately follows this: changing this wont affect the TIMEOUT_1s's accuracy
#define inEachCycle 1
#define inEverySecondCycle 2
#define inEveryTenthCycle 10

/*
 * Booleans to notify the main() to handle the periodically generated interrupts:
 * (maybe Hun. Not. can be useful)
 *
 */

bool uzone=false;
bool usensor=false;
bool usend=false;

/*
bool uled=false;
bool usound=false;
bool ubutton=false;
*/



/*
 * Handlers for the above
 */

//void buttonRead();
//void led();

void sensorRead();
void send();
bool inline zonetrig(int sensor_cm, int zone_border_cm);


word iISR=0;



///ADC OUTPUTS
bool sensorstate = 0;
bool nextSound=false;
volatile word max_adc;

////////////////////////////////////////////////////////////////

/*                      Zoning Principle                      */

////////////////////////////////////////////////////////////////


///////INPUT VARIABLES//////


#define trig1	9
#define trig2	9
#define trig3	9
#define trig4	9
#define echo1	2
#define echo2	4
#define echo3	7
#define echo4	8
#define signPin	3

#define echo1	4
#define trig1	2

/////////CONSTANTS///////////
#define iZone1Radius 50 // [cm] -  dont get confused with Sharp values!


//////////ITERATORS/////////
int lastread = 0;
int avg_i = 0;

//// Internal variables: state machine /////

typedef enum  {idle=0, triggered,timeouting} 	zoneStateMachine;
//typedef enum  { tuzel, golyoRepul, out} 											shootingProcess;			//2 szegmens a golyo utja
//typedef enum  {miss, DED} 															duckState; 	//a lovesnel ezeket vizsgaljuk


zoneStateMachine zone1=idle;

//shootingProcess puska;
//duckState preda=miss;

//// Output variables /////
bool prevsensorstate = 0;
bool button=0;
bool prevButton=0;

bool bZone1toBeTriggered=false;

///////////// TIMEOUT VARIABLES /////

int iZone1TimeoutStart=0;
int iZone2TimeoutStart=0;
int iZone3TimeoutStart=0;




//for color/music changes
int buttonTimer = 0;

//////////////////////////////////////////////////////////
/*					For calibrating:			 		*/
//////////////////////////////////////////////////////////
#define SamplesToCalibrate 20


int iSensor1ValueArray[SamplesToCalibrate];
int iSensor2ValueArray[SamplesToCalibrate];
int iSensor3ValueArray[SamplesToCalibrate];

int iSensor1OffsetValue=400;
int iSensor2OffsetValue=400;
int iSensor3OffsetValue=400;

int iSumToCalibrate=0;







/////////////////////////////////////////////////////////////////////////////////////////////////////

/*                      Sound                      */

/////////////////////////////////////////////////////////////////////////////////////////////////////

#define mutePin 13

#ifdef HALK
	#define SOUND_MAX_VOL 5
	#define SOUND_OFFSET_VOL 2
#else
	#define SOUND_MAX_VOL 13
	#define SOUND_OFFSET_VOL 5
#endif

//unsigned long currenttime;    //  while(1) loopban
//int timeoutCounter = 0;
//int counterK = 0;
//int timeoutCounterLong = 0;
//int setpointReachedSince=0;
//
//int timeoutMillis = 4000;
//int timeStopMillis = 10000;

// 	k+=	1000 increment	= 20s
// 	k+=	   1 incr		= 20ms

//period=50.000 => 1s 20inc
//1s 1.000.000us -> 1.000.000/period=1s
#define TIMEOUT_1s 			1000000/period
#define TIMEOUT_5s 			5*TIMEOUT_1s
#define TIMEOUT_10s 		10*TIMEOUT_1s
#define TIMEOUT_20s 		20*TIMEOUT_1s



int finVol=0;
int curVol=0;
#endif
