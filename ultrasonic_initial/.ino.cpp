#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2017-12-28 10:36:14

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
void blinkIndicators(bool greenToBlink, bool redToBlink) ;
void buttonRead() ;
inline void calcColorDifference() ;
void led(void);
void ledSetBlinking(int duration_k_increments, int period_k_increments, double fill)   ;
void ledBlinking()   ;
void sensor() ;
inline void sensor_mid() ;
void sound() ;
bool zonetrig(int zone_border_cm, int cm_output_zone) ;
void allzonetrigger() ;
inline void initialCalibrate() ;
inline void calibrate() ;
void calibrateAtBeginning() ;
void inline sensorValueAveraging() ;
void setup() ;
void loop() ;

#include "ultrasonic_initial.ino"


#endif
