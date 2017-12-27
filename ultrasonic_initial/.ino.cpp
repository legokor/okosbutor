#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2017-12-27 18:33:03

#include "Arduino.h"
#include "battery.h"
#include "button.h"
#include "handlers.h"
#include "led.h"
#include "main.h"
#include "ultra.h"
#include "zone.h"
void timingISR(void) ;
void batteryMonitor() ;
void buttonRead() ;
inline void calcColorDifference() ;
void led(void);
void ledSetBlinking(int duration_k_increments, int period_k_increments, double fill)   ;
void ledBlinking()   ;
void sensor() ;
void sensorIter()   ;
void sound() ;
bool zonetrig(int zone_border_cm) ;
void allzonetrigger() ;
inline void initialCalibrate() ;
inline void calibrate() ;
void calibrateAtBeginning() ;
void inline sensorValueAveraging() ;
void setup() ;
void loop() ;

#include "ultrasonic_initial.ino"


#endif
