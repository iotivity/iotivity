/*
 * proximity.cpp
 *
 *  Created on: 2014. 11. 13.
 *      Author: eunseok
 */

#include "Arduino.h"
#include "proximity.h"
#include <math.h>

// Proximity code start
float CalculateExponentialAverage(int numberOfSamples, int* array, int startindex, int flag){
	float numerator = 0;
	float denominator = 0;

	float average = 0.0;

	if (flag < arraysize / RSSI_EA)   // first loop buffer full
	{
		for (int i = 0; i < startindex; i++)
		{
			average += array[i];
		}
		if(startindex == 0){}
		else{
			average = average / startindex;
		}

		Serial.print("average1 : ");
		Serial.println(average);
	}
	else
	{
		for (int i = 0; i < arraysize; i++)
		{
			average += array[i];
		}

		for (int i = startindex; i < startindex + RSSI_EA; i++)
		{
			average -= array[i];
		}
		average = average / (arraysize - numberOfSamples);

		Serial.print("average2 : ");
		Serial.println(average);
	}
	//exponential moving average
	int i = 0;
	//CHANGE THIS FOR DIFFERENT SMOOTHING EFFECT
	float beta=0.8f;
	for (i = startindex + numberOfSamples - 1; i >= startindex; i--)
	{
		numerator += array[i]* pow(beta, startindex + numberOfSamples - i - 1);
		denominator += pow(beta, startindex + numberOfSamples - i - 1);
	}

	int offset = 3;
	if(average != 0.0){
	  numerator += average * pow(beta, offset + numberOfSamples);
	  denominator += pow(beta, offset + numberOfSamples);
	}
	return numerator / denominator;
}


float calculateDistance(float avgRSSI, float txPower)
{
	if (avgRSSI == 0)
	{
		return -1.0;
	}

	float ratio = avgRSSI*1.0/txPower;
	if (ratio < 1.0)
	{
		return pow(ratio,10);
	}
	else
	{
		float distance =  (0.7)*pow(ratio,10) + 0.024;
		return distance;
	}
}

// proximity code end

