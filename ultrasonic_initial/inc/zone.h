/*
 * zone.h
 *
 *  Created on: 2017. dec. 21.
 *      Author: László
 */

#include "global_time.h"

#ifndef ZONE_H_
#define ZONE_H_



/*
//period=50.000 => 1s 20inc
//1s 1.000.000us -> 1.000.000/period=1s
#define TIMEOUT_1s 			1000000/period
#define TIMEOUT_5s 			5000000/period
#define TIMEOUT_10s 		10000000/period
#define TIMEOUT_20s 		20000000/period
*/


#define iZone1Radius 70 // [cm] -  dont get confused with Sharp values!
#define iZone2Radius 110


///////////// TIMEOUT VARIABLES /////

long int iZone1TimeoutStart=0;
long int iZone2TimeoutStart=0;
int iZone3TimeoutStart=0;


typedef enum  {idle=0, triggered, leaved, smallTimeout, bigTimeout} 	zoneStateMachine;
zoneStateMachine zone1=idle;
zoneStateMachine zone2=idle;
zoneStateMachine zone3=idle;


bool bZoneTriggered=false;
int iMinZone=400;			//at zoning

int iMinZone1=400;
int iMinZone2=400;

#endif /* ZONE_H_ */
