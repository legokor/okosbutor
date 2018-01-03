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
/*
#ifdef HALK
	#define SOUND_MAX_VOL_ZONE1 7
	#define SOUND_OFFSET_VOL_ZONE1 3
	#define SOUND_MAX_VOL_ZONE2 4
	#define SOUND_OFFSET_VOL_ZONE2 1
#else
	#define SOUND_MAX_VOL_ZONE1 20
	#define SOUND_MID_VOL_ZONE1 15
	#define SOUND_LOW_VOL_ZONE1 10
	#define SOUND_OFFSET_VOL_ZONE1 0
	#define SOUND_MAX_VOL_ZONE2 12
	#define SOUND_MID_VOL_ZONE2 7
	#define SOUND_LOW_VOL_ZONE2 3
	#define SOUND_OFFSET_VOL_ZONE2 0
#endif
*/

//SHOWROOM
#define SOUND_MAX_VOL_ZONE1 28
#define SOUND_MID_VOL_ZONE1 23
#define SOUND_LOW_VOL_ZONE1 18
#define SOUND_OFFSET_VOL_ZONE1 0
#define SOUND_MAX_VOL_ZONE2 16
#define SOUND_MID_VOL_ZONE2 14
#define SOUND_LOW_VOL_ZONE2 9
#define SOUND_OFFSET_VOL_ZONE2 0



int finVol=0;
int curVol=0;
bool bSoundButtonMuted=false;

#endif
