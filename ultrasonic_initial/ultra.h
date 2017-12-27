/*
 * ultra.h
 *
 *  Created on: 2017. dec. 8.
 *      Author: László
 */
#ifndef ULTRA_H_
#define ULTRA_H_

/* Pins: */

#define trig1	19	//A5
#define trig2	19
#define trig3	19
#define trig4	19
#define echo1	10	//D10
#define echo2	14	//A0
#define echo3	15	//A1
#define echo4	16	//A2

//////////////////////////////////////////////////////////
/*					For calibrating:			 		*/
//////////////////////////////////////////////////////////
#define SamplesToCalibrate 20
#define SensorsToRead 3

byte iSampleIterator=0;
byte iSensorIterator=0;


int iSensor1ValueArray[SamplesToCalibrate];
int iSensor2ValueArray[SamplesToCalibrate];
int iSensor3ValueArray[SamplesToCalibrate];
int iSensor4ValueArray[SamplesToCalibrate];

int iSensor1OffsetValue=85;
int iSensor2OffsetValue=85;
int iSensor3OffsetValue=60;
int iSensor4OffsetValue=95;

int iSumToCalibrate=0;

int iSensor1Previous=399;
int iSensor2Previous=399;
int iSensor3Previous=399;
int iSensor4Previous=399;



#endif
