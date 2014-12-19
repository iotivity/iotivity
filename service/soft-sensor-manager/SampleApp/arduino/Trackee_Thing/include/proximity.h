/*
 * proximity.h
 *
 *  Created on: 2014. 11. 13.
 *      Author: eunseok
 */

#ifndef PROXIMITY_H_
#define PROXIMITY_H_


#define arraysize	6
#define RSSI_EA		3

float CalculateExponentialAverage(int numberOfSamples, int* array, int startindex, int flag);

float calculateDistance(float avgRSSI, float txPower);

#endif /* PROXIMITY_H_ */
