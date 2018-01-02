/*
 * button.h

 *
 *  Created on: 2017. nov. 16.
 *      Author: László
 */
#ifndef BUTTON_H
#define BUTTON_H


#define buttonAnalogPin 6

extern long int k;

int buttonVal=0;
//int buttonCounter=0;
long int pushStartedAt=0;
long int pushReleasedAt=0;
long int pushedAgainAt=0;
long int pushDelay=0;

bool bButtonPushed=false;
bool repeatedPush=false;
bool pushInProgress=false;
bool continousPush=true;

// 	k+=	1000 increment	= 20s
// 	k+=	   1 incr		= 20ms

//50ms

#define BUTTON_TIME_1s 		20
#define BUTTON_TIME_100ms	2
#define BUTTON_TIME_2s 		40
#define BUTTON_TIME_5s 		100

typedef enum  {NotPushed=0, Pushed, Released, ShortPush, PushedAgain, LongPush} buttonStateMachine;
buttonStateMachine nyomogomb=NotPushed;


#endif
