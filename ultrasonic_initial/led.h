//source is the 5fb709 commit.
#ifndef LED_H
#define LED_H

bool targetReached=0;

bool colorBlack = false;

#define redPin  5  //r
#define grnPin  6  //g
#define bluPin  3	 //b

#define chargeGreen 17		//A3
#define chargeRed 	18		//A4

#define curB OCR2B
#define curR OCR0B
#define curG OCR0A

byte finR=0;
byte finG=0;
byte finB=0;

signed char dR = 0;//step
signed char dB = 0;
signed char dG = 0;

byte x=0;
byte increaseRate=1;
byte colorPalette=0;
#define u8ColorPalettesCount 5
byte u8LedSpeed = 1;
bool bContinousLight = false;

byte rgb[u8ColorPalettesCount][7][3] =
						{{{ 0, 0, 255 },       	{ 255, 255, 255 }, 	{ 255, 0, 0 },     { 0, 255, 0 },		{ 0, 0, 255 },		{ 255, 255, 0 },	{ 80, 80, 80 }    },
                     	 {{0x8d, 0x6e, 0x63},	{0xff, 0x57, 0x22},	{0xff, 0x6f, 0x00},{0xcd, 0xdc, 0x39},	{0x4c, 0xaf, 0x50},	{0x00, 0x96, 0x88},	{0x7e, 0x57, 0xc2}},
                     	 {{0, 125, 0},     		{0, 125, 65},     	{125, 125, 0},     {125, 65, 0},     	{0, 65, 0},       	{0, 65, 125},    	{0, 0, 125}       },
						 {{ 0, 0, 255 },       	{ 255, 40, 255 }, 	{ 255, 0, 0 },     { 150, 40, 50 },		{ 20, 0, 255 },		{ 255, 100, 20 },	{ 100, 100, 100 } },
						 {{ 215, 100, 0 },       	{ 215, 167, 36 }, 	{ 255, 0, 0 },     { 150, 40, 50 },		{ 20, 0, 255 },		{ 255, 100, 20 },	{ 100, 100, 100 }  }
                    };

typedef enum  {Off=0, Normal, Automatic, Manual,Blinking} ledStateMachine;
ledStateMachine ledstrip=Off;
int colorNum=0;

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

typedef enum  {IndicatorsOff=0, GreenIsOn,GreenBlink,RedBlinking,BothAreOn} indicatorLedStateMachine;
indicatorLedStateMachine twoleds=IndicatorsOff;

#endif
