#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include "TimerOne.h"

#ifndef MAIN_H
#define MAIN_H

//#define HALK
//if not defined, volumes higher

bool uled=false;
bool uadc=false;
bool usound=false;
bool ubutton=false;


word iISR=0;


///ADC OUTPUTS
bool sensorstate = 0;
bool nextSound=false;
volatile word max_adc;
//colorpalette

/////////////////////////////////////////////////////////////////////////////////////////////////////

/*                      ADC FUNCTION HEADER                      */

/////////////////////////////////////////////////////////////////////////////////////////////////////


///////INPUT VARIABLES//////
// panel rev2 !!
#define distLong 3
#define distPin1 4
#define distPin2 5
#define buttonPin 12
#define buttonAnalogPin 7

/////////CONSTANTS///////////
const byte sense_radius = 80;


//////////ITERATORS/////////
int lastread = 0;
int avg_i = 0;

/////INTERNAL VARIABLES/////

int sum=0;
volatile int s1_adc = 0;
volatile int s2_adc = 0;
volatile int s3_adc = 0;
volatile int t_s1_adc[8] = { 0,0,0,0,0,0,0,0 };
volatile int t_s2_adc[8] = { 0,0,0,0,0,0,0,0 };
volatile int t_s3_adc[8] = { 0,0,0,0,0,0,0,0 };


///////OUTPUT VARIABLES/////
bool prevsensorstate = 0;
bool button=0;
bool prevButton=0;

//for color/music changes
int buttonTimer = 0;


/////////////////////////////////////////////////////////////////////////////////////////////////////

/*                      Sound                      */

/////////////////////////////////////////////////////////////////////////////////////////////////////

#define mutePin 13

#ifdef HALK
	#define SOUND_MAX_VOL 5
	#define SOUND_OFFSET_VOL 2
#else
	#define SOUND_MAX_VOL 15
	#define SOUND_OFFSET_VOL 5
#endif

unsigned long currenttime;    //  while(1) loopban
int timeoutCounter = 0;
int counterK = 0;
int timeoutCounterLong = 0;
int setpointReachedSince=0;

int timeoutMillis = 4000;
int timeStopMillis = 10000;

// 	k+=	1000 increment	= 20s
// 	k+=	   1 incr		= 20ms
#define TIMEOUT_10s 		500



int finVol=0;
int curVol=0;
#endif
