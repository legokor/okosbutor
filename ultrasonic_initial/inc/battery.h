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

typedef enum  {VoltageNormal=0, VoltageBelow12V, VoltageBelow11V, VoltageCriticalLow, VoltageHigh} batteryStateMachine;
batteryStateMachine akku=VoltageNormal;


#endif /* BATTERY_H_ */
