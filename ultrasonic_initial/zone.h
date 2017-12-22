/*
 * zone.h
 *
 *  Created on: 2017. dec. 21.
 *      Author: L�szl�
 */


#ifndef ZONE_H_
#define ZONE_H_

#include "global_time.h"

/*
//period=50.000 => 1s 20inc
//1s 1.000.000us -> 1.000.000/period=1s
#define TIMEOUT_1s 			1000000/period
#define TIMEOUT_5s 			5000000/period
#define TIMEOUT_10s 		10000000/period
#define TIMEOUT_20s 		20000000/period
*/


#define iZone1Radius 50 // [cm] -  dont get confused with Sharp values!
#define iZone2Radius 125
#define iZone3Radius 200

///////////// TIMEOUT VARIABLES /////

int iZone1TimeoutStart=0;
int iZone2TimeoutStart=0;
int iZone3TimeoutStart=0;


typedef enum  {idle=0, triggered,timeouting} 	zoneStateMachine;
zoneStateMachine zone1=idle;
zoneStateMachine zone2=idle;
zoneStateMachine zone3=idle;


#endif /* ZONE_H_ */
