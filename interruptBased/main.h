#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include "TimerOne.h"



bool uled=false;
bool uadc=false;
bool usound=false;

byte iISR=0;


///ADC OUTPUTS
bool sensorstate = 0;
bool nextSound=false;
volatile word max_adc;
int colorPalette=0;

/////////////////////////////////////////////////////////////////////////////////////////////////////

/*                      ADC FUNCTION HEADER                      */

/////////////////////////////////////////////////////////////////////////////////////////////////////


///////INPUT VARIABLES//////
#define distLong 5
#define distPin1 6
#define distPin2 7
#define buttonPin 12

/////////CONSTANTS///////////
const byte sense_radius = 40;


//////////ITERATORS/////////
int lastread = 0;
int avg_i = 0;

/////INTERNAL VARIABLES/////

volatile word s1_adc = 0;
volatile word s2_adc = 0;
volatile word s3_long_adc = 0;
volatile word t_s1_adc[8] = { 0,0,0,0,0,0,0,0 };
volatile word t_s2_adc[8] = { 0,0,0,0,0,0,0,0 };
volatile word t_s3_long_adc[8] = { 0,0,0,0 };


///////OUTPUT VARIABLES/////
bool prevsensorstate = 0;
bool button=0;
bool prevButton=0;

//for color/music changes
int buttonTimer = 0;


/////////////////////////////////////////////////////////////////////////////////////////////////////

/*                      Sound                      */

/////////////////////////////////////////////////////////////////////////////////////////////////////

#define mutePin 13

unsigned long currenttime;    //  while(1) loopban
int timeoutCounter = 0;
int timeoutCounterLong = 0;
int setpointReachedSince=0;

int timeoutMillis = 4000;
int timeStopMillis = 10000;

int finVol=0;
int curVol=0;
