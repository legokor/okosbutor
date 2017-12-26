//source is the 5fb709 commit.
#ifndef LED_H
#define LED_H

bool targetReached=0;

bool colorBlack = false;

#define redPin  3  //r
#define grnPin  5  //g
#define bluPin  6  //b

#define chargeGreen 17		//A3
#define chargeRed 	18		//A4

#define curR OCR2B
#define curG OCR0B
#define curB OCR0A

byte finR=0;
byte finG=0;
byte finB=0;

signed char dR = 0;//step
signed char dB = 0;
signed char dG = 0;

byte x=0;
byte increaseRate=1;
int colorPalette=0;

byte rgb[3][7][3] = {{{ 0, 0, 255 },       	{ 255, 255, 255 }, 	{ 255, 0, 0 },     { 0, 255, 0 },		{ 0, 0, 255 },		{ 255, 255, 0 },	{ 80, 80, 80 }    },
                     {{0x8d, 0x6e, 0x63},	{0xff, 0x57, 0x22},	{0xff, 0x6f, 0x00},{0xcd, 0xdc, 0x39},	{0x4c, 0xaf, 0x50},	{0x00, 0x96, 0x88},	{0x7e, 0x57, 0xc2}},
                     {{0, 125, 0},     		{0, 125, 65},     	{125, 125, 0},     {125, 65, 0},     	{0, 65, 0},       	{0, 65, 125},    	{0, 0, 125}       }
                    };

typedef enum  {Off=0, Normal, Automatic, Manual,Blinking} ledStateMachine;
ledStateMachine ledstrip=Off;
int colorNum=0;

bool bBlinkingModeActive=false;
bool bColorSettled=false;

#define iLedStepTime (TIMEOUT_100ms*2)
#define iFinalColorStepTime (TIMEOUT_5s)


int iColorSettledAt=0;
int iColorSteppedAt=0;
int iBlinkPeriod=0;
int iBlinkPeriodStart=0;
int iBlinkDuration=0;
int iBlinkStart=0;
double iBlinkFill=0.5;

typedef enum  {IndicatorsOff=0, GreenIsOn,GreenBlink,RedBlinking,BothAreOn} indicatorLedStateMachine;
indicatorLedStateMachine twoleds=IndicatorsOff;

#endif
