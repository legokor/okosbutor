/*
 * battery.h
 *
 *  Created on: 2017. dec. 23.
 *      Author: László
 */

#ifndef BATTERY_H_
#define BATTERY_H_


#define batteryAnalogPin 7

int batteryVoltage;
int batteryAnalog;
//int batteryLastCheckedAt=0;
int batteryGreenBlinkedAt=0;
int batteryRedBlinkedAt=0;

typedef enum  {VoltageNormal=0, VoltageBelow12V, VoltageBelow11V, VoltageCriticalLow} batteryStateMachine;
batteryStateMachine akku=VoltageNormal;


#endif /* BATTERY_H_ */
