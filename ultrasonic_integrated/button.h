/*
 * button.h

 *
 *  Created on: 2017. nov. 16.
 *      Author: László
 */
#ifndef BUTTON_H
#define BUTTON_H


#define buttonAnalogPin 7

extern long int k;

int buttonVal=0;
//int buttonCounter=0;
int pushStartedAt=0;
int pushReleasedAt=0;
int pushedAgainAt=0;
int pushDelay=0;

bool bButtonPushed=false;
bool repeatedPush=false;
bool pushInProgress=false;
bool continousPush=true;

// 	k+=	1000 increment	= 20s
// 	k+=	   1 incr		= 20ms


#define BUTTON_TIME_1s 		1000000/period
#define BUTTON_TIME_100ms	BUTTON_TIME_1s/10
#define BUTTON_TIME_2s 		2*BUTTON_TIME_1s
#define BUTTON_TIME_5s 		5*BUTTON_TIME_1s

typedef enum  {NotPushed=0, Pushed, Released, ShortPush, PushedAgain, LongPush} buttonStateMachine;
buttonStateMachine nyomogomb=NotPushed;


#endif
