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

bool nextSound=false;

//bool sensorstate = 0;
//volatile word max_adc;
//

//// Output variables /////
//bool prevsensorstate = 0;
//bool button=0;
//bool prevButton=0;


//for color/music changes
//int buttonTimer = 0;


/////////////////////////////////////////////////////////////////////////////////////////////////////

/*                      Sound                      */

/////////////////////////////////////////////////////////////////////////////////////////////////////

#define mutePin 8

#ifdef HALK
	#define SOUND_MAX_VOL_ZONE1 7
	#define SOUND_OFFSET_VOL_ZONE1 3
	#define SOUND_MAX_VOL_ZONE2 4
	#define SOUND_OFFSET_VOL_ZONE2 1
#else
	#define SOUND_MAX_VOL_ZONE1 14
	#define SOUND_OFFSET_VOL_ZONE1 6
	#define SOUND_MAX_VOL_ZONE2 8
	#define SOUND_OFFSET_VOL_ZONE2 3
#endif


int finVol=0;
int curVol=0;
#endif
