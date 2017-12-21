/*
 * global_time.h
 *
 *  Created on: 2017. dec. 21.
 *      Author: László
 */

#ifndef GLOBAL_TIME_H_
#define GLOBAL_TIME_H_


#define period 50000 // In microseconds---- TIMING accurately follows this: changing this wont affect the TIMEOUT_1s's accuracy

////period=50.000 => 1s 20inc
////1s 1.000.000us -> 1.000.000/period=1s
//#define TIMEOUT_1s 			1000000/period
//#define TIMEOUT_5s 			5*TIMEOUT_1s
//#define TIMEOUT_10s 		10*TIMEOUT_1s
//#define TIMEOUT_20s 		20*TIMEOUT_1s



#endif /* GLOBAL_TIME_H_ */
