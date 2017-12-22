/*
 * ultra.h
 *
 *  Created on: 2017. dec. 8.
 *      Author: László
 */
#ifndef ULTRA_H_
#define ULTRA_H_

/* Pins: */

#define trig1	17	//A3
#define trig2	17
#define trig3	17
#define trig4	17
#define echo1	10	//D10
#define echo2	14	//A0
#define echo3	15	//A1
#define echo4	16	//A2

//////////////////////////////////////////////////////////
/*					For calibrating:			 		*/
//////////////////////////////////////////////////////////
#define SamplesToCalibrate 20


int iSensor1ValueArray[SamplesToCalibrate];
int iSensor2ValueArray[SamplesToCalibrate];
int iSensor3ValueArray[SamplesToCalibrate];
int iSensor4ValueArray[SamplesToCalibrate];

int iSensor1OffsetValue=400;
int iSensor2OffsetValue=400;
int iSensor3OffsetValue=400;
int iSensor4OffsetValue=400;

int iSumToCalibrate=0;



#endif
