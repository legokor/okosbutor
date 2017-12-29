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
#define SensorsToRead 4
#define sensorValuesToAverage 3

//works with 3^^
int cm[SensorsToRead][sensorValuesToAverage]; //init to 400 with function
int cmAveraged[SensorsToRead];
int cmOffsets[SensorsToRead];
int cmSamples[SensorsToRead][SamplesToCalibrate];

unsigned char iSampleIterator=0;
unsigned char iSensorIterator=0;
//unsigned char iMidValueIterator=0;
unsigned char iSensorArrayIterator=0;

int k_start=0;
int iSumToCalibrate=0;
bool bNotCalibrated=true;

typedef enum  {NotYet=0, Initiated, InProgress, Done} calibrationStateMachine;
calibrationStateMachine Calibrated=NotYet;



#endif
