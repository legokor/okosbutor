#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include "TimerOne.h"

//State variables
bool sensorstate = 0;
bool prevsensorstate = 0;
bool longsensorstate = 0;
bool prevlongsensorstate = 0;
bool sleepMode=true;
int periodSet=0;
bool targetReached=true;
bool button=false;
bool prevButton=false;


int DEBUG = 1;      // DEBUG counter; if set to 1, will write values back via serial


					///////////////////////  Input  ///////////////////////
          // Analog inputs
#define distLong 5
#define distPin1 6
#define distPin2 7
          //Digital variables
#define buttonPin 12
					//ADC variables
int lastread = 0;
int soundcycle=0;
int avg_i = 0;
int vol_i = 0;
volatile int curVol=5;
volatile int finVol=5;


volatile word s1_adc = 0;
volatile word s2_adc = 0;
volatile word s3_long_adc = 0;
volatile word t_s1_adc[8] = { 0,0,0,0,0,0,0,0 };
volatile word t_s2_adc[8] = { 0,0,0,0,0,0,0,0 };
volatile word t_s3_long_adc[8] = { 0,0,0,0 };
volatile word max_adc;

//Distance constants, limits in adc value
int sense_radius = 40;
int dist_far_trig = 80;
int dist_mid_trig = 200;
int dist_near_trig = 300;
int dist_long_trig = 40;


/////////////////////////////OUTPUTS///////////////////////////////
#define redPin  3  //r
#define grnPin  5  //g
#define bluPin  6  //b
#define mutePin 13  // 8 9 free


// Color arrays
/*byte black[3] = { 0, 0, 0 };
byte white[3] = { 255, 255, 255 };
byte red[3] = { 255, 0, 0 };
byte green[3] = { 0, 255, 0 };
byte blue[3] = { 0, 0, 255 }; 
byte yellow[3] = { 255, 255, 0 }; // wtf not valid { 40, 95, 0 };
byte dimWhite[3] = { 80, 80, 80 };*/

enum color{                black,      white,            red,          green,         blue,          yellow,          dimwhite};
byte rgb[3][7][3] = { {{ 0, 0, 0 }, { 255, 255, 255 },{ 255, 0, 0 },{ 0, 255, 0 }, { 0, 0, 255 }, { 255, 255, 0 }, { 80, 80, 80 }},
                      {{ 0x8d, 0x6e, 0x63 }, { 0xff, 0x57, 0x22 },{ 0xff, 0x6f, 0x00 },{ 0xcd, 0xdc, 0x39 }, { 0x4c, 0xaf, 0x50 }, { 0x00, 0x96, 0x88 }, { 0x7e, 0x57, 0xc2 }},
                      {{255,255,0}, {255,255,0},{250,255,0},{250,127,0},{250,0,0},{250,0,0},{255,0,0}} };
enum color2{                brown,                 orange,                gold,                 lime,             green2,                teal,               deeppurple};

int colorPalette = 2;
  
// etc.
int colorNum=0;

bool reachedSetpoint = false;


//////////////////////////TIMING CONSTANTS//////////////////////////

int timeoutCounter = 0;
int timeoutCounterLong = 0;
int setpointReachedSince=0;

int timeoutMillis = 4000;
int timeStopMillis = 10000;
int timeoutTimerPeriod(1000);
int setpointWait = 1500;



int increaseRate = 1;
