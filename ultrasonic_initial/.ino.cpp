#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2017-12-25 21:55:36

#include "Arduino.h"
#include "button.h"
#include "handlers.h"
#include "led.h"
#include "main.h"
#include "ultra.h"
#include "zone.h"
void timingISR(void) ;
void buttonRead() ;
void led(void);
void ledBlinking()   ;
void sensor()   ;
bool inline zonetrig(int zone_border_cm) ;
void allzonetrigger() ;
inline void initialCalibrate() ;
inline void calibrate() ;
void setup() ;
void loop() ;

#include "ultrasonic_initial.ino"


#endif
