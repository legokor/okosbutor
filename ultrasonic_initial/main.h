/*
 * main.h
 *
 *
 */

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
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


#include "Arduino.h"
#include "global_time.h"



///ADC OUTPUTS
bool sensorstate = 0;
bool nextSound=false;
volatile word max_adc;


//test
//#define echo1	4
//#define trig1	2


//// Output variables /////
bool prevsensorstate = 0;
bool button=0;
bool prevButton=0;


//for color/music changes
int buttonTimer = 0;


/////////////////////////////////////////////////////////////////////////////////////////////////////

/*                      Sound                      */

/////////////////////////////////////////////////////////////////////////////////////////////////////

#define mutePin 8

#ifdef HALK
	#define SOUND_MAX_VOL 5
	#define SOUND_OFFSET_VOL 2
#else
	#define SOUND_MAX_VOL 13
	#define SOUND_OFFSET_VOL 5
#endif


int finVol=0;
int curVol=0;
#endif
