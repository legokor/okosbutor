/*
 * ultra.h
 *
 *  Created on: 2017. dec. 8.
 *      Author: László
 */
#ifndef ULTRA_H_
#define ULTRA_H_

/* Pins: */

#define trig1	9
#define trig2	9
#define trig3	9
#define trig4	9
#define echo1	2
#define echo2	4
#define echo3	7
#define echo4	8
#define signPin	3

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
