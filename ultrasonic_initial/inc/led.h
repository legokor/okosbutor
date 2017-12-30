//source is the 5fb709 commit.

#include "global_time.h"
#include "Arduino.h"

#ifndef LED_H
#define LED_H

bool targetReached=0;

bool colorBlack = false;

#define onboardLed  13  // 13

#define redPin  5  //r
#define grnPin  6  //g
#define bluPin  3	 //b
#define posztamensLed 2 //NOT ABLE TO PWM

//3 sensor version:
#define chargeGreen 17		//A3 - bal also = fekete
#define chargeRed 	18		//A4 - bal felso = barna

#define curB OCR2B
#define curR OCR0B
#define curG OCR0A

unsigned char finR=0;
unsigned char finG=0;
unsigned char finB=0;

signed char dR = 0;//step
signed char dB = 0;
signed char dG = 0;

unsigned char x=0;
unsigned char colorPalette=0;


#define u8ColorPalettesCount 8
unsigned char u8LedSpeed = 1;
bool bContinousLight = false;
int colorNum=0;

unsigned char rgb[u8ColorPalettesCount][7][3] =
						{{{ 0, 0, 255 },       	{ 255, 255, 255 }, 	{ 255, 0, 0 },     { 0, 255, 0 },		{ 0, 0, 255 },		{ 255, 255, 0 },	{ 80, 80, 80 }    },
                     	 {{0x8d, 0x6e, 0x63},	{0xff, 0x57, 0x22},	{0xff, 0x6f, 0x00},{0xcd, 0xdc, 0x39},	{0x4c, 0xaf, 0x50},	{0x00, 0x96, 0x88},	{0x7e, 0x57, 0xc2}},
                     	 {{0, 125, 0},     		{0, 125, 65},     	{125, 125, 0},     {125, 65, 0},     	{0, 65, 0},       	{0, 65, 125},    	{0, 0, 125}       },
						 {{ 0, 0, 255 },       	{ 255, 40, 255 }, 	{ 255, 0, 0 },     { 150, 40, 50 },		{ 20, 0, 255 },		{ 255, 100, 20 },	{ 100, 100, 100 } },
						 {{ 215, 100, 0 },       	{ 215, 167, 36 }, 	{ 120, 120, 120 },     { 50, 70,  255 },		{ 0, 160, 255 },		{ 60, 40, 130 },	{ 120, 80, 40 }  },


		/* 5 */			 {{ 0, 0, 200 },       	{ 200, 200, 200 }, 	{ 200, 0, 0 },     { 0, 200, 0 },		{ 0, 0, 200 },		{ 200, 200, 0 },	{ 50, 50, 50 }    },
		/* 6 */			 {{ 0, 0, 100},       	{ 100, 100, 100 }, 	{ 100, 0, 0 },     { 0, 100, 0 },		{ 0, 0, 100 },		{ 100, 100, 0 },	{ 50, 50, 50 }    },
		/* 7 */			 {{ 0, 0, 255 },       	{ 255, 255, 255 }, 	{ 255, 0, 0 },     { 0, 255, 0 },		{ 0, 0, 255 },		{ 255, 255, 0 },	{ 80, 80, 80 }    }

						};

bool bBlinkingModeActive=false;
bool bColorSettled=false;

#define iLedStepTime (TIMEOUT_100ms)
#define iFinalColorStepTime (TIMEOUT_5s)


int iColorSettledAt=0;
int iColorSteppedAt=0;
int iBlinkPeriod=0;
int iBlinkPeriodStart=0;
int iBlinkDuration=0;
long int iBlinkStart=0;
double iBlinkFill=0.5;


typedef enum  { Off=0, Normal, Automatic, Manual, BlinkingStart, BlinkingPeriodOn, BlinkingPeriodOff } ledStateMachine;
ledStateMachine ledstrip;

typedef enum  {IndicatorOff=0, ToBlink, Waiting,IsOn,IsOff} indicatorLedStateMachine;

typedef struct
{
	indicatorLedStateMachine state;
	long int WaitStartedAt;
	long int LitUpAt;
	long int TurnedOffAt;
	unsigned char blinkCount;
	int waitTime=TIMEOUT_500ms;
	int onTime=TIMEOUT_100ms;
	int offTime=TIMEOUT_2s;
	/*
	int onTime=TIMEOUT_200ms;
	int offTime=TIMEOUT_500ms;
	*/
	//void blinkIndicator();

}indicatorLed;

indicatorLed indicatorGreen;
indicatorLed indicatorRed;

#endif
