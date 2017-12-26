/*
 * global_time.h
 *
 *  Created on: 2017. dec. 21.
 *      Author: László
 */

#ifndef GLOBAL_TIME_H_
#define GLOBAL_TIME_H_


#define period 50000 // In microseconds---- TIMING accurately follows this: changing this wont affect the TIMEOUT_1s's accuracy

//period=50.000 => 1s 20inc
//1s 1.000.000us -> 1.000.000/period=1s
#define TIMEOUT_1s 			20
#define TIMEOUT_5s 			100
//#define TIMEOUT_5s 			(TIMEOUT_1s*5)
#define TIMEOUT_10s 		200
#define TIMEOUT_20s 		400
#define TIMEOUT_100ms 		2
#define TIMEOUT_500ms 		10


#endif /* GLOBAL_TIME_H_ */
