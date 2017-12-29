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


#include "Arduino.h"
#include "global_time.h"

#ifndef MAIN_H_
#define MAIN_H_

bool nextSound=false;

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
	#define SOUND_MID_VOL_ZONE1 10
	#define SOUND_LOW_VOL_ZONE1 6
	#define SOUND_OFFSET_VOL_ZONE1 6
	#define SOUND_MAX_VOL_ZONE2 8
	#define SOUND_MID_VOL_ZONE2 6
	#define SOUND_MIN_VOL_ZONE2 4
	#define SOUND_OFFSET_VOL_ZONE2 3
#endif


int finVol=0;
int curVol=0;
#endif
