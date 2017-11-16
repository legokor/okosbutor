/*
 * button.h
 *
 *  Created on: 2017. nov. 16.
 *      Author: László
 */

#ifndef BUTT_H
#define BUTT_H

int buttonVal=0;
int buttonCounter=0;
int pushStartedAt=0;
int pushedAgainAt=0;
int pushDelay=0;

bool repeatedPush=false;
bool pushInProgress=false;
bool continousPush=true;

// 	k+=	1000 increment	= 20s
// 	k+=	   1 incr		= 20ms

#define BUTTON_TIME_100ms	5
#define BUTTON_TIME_1s 		50
#define BUTTON_TIME_2s 		100
#define BUTTON_TIME_5s 		250

#endif
