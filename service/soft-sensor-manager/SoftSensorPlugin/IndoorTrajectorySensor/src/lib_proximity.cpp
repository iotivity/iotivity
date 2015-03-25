/******************************************************************
*
* Copyright 2014 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#include <stdio.h>
#include "tizen_log.h"
#include "lib_proximity.h"
#include <math.h>

// Proximity code start
float CalculateExponentialAverage(int numberOfSamples, int *array, int startindex, int flag)
{
    float numerator = 0;
    float denominator = 0;

    float average = 0.0;

    if (flag < arraysize / RSSI_EA)   // first loop buffer full
    {

        for (int i = 0; i < startindex; i++)
        {
            average += array[i];
        }

        if (startindex == 0) {}
        else
        {
            average = average / startindex;
        }
        //       DBG("exp: ");
        DBG("average1 : %f", average);
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

        //            DBG("exp: ");
        DBG("average2 : %f", average);
    }

    //exponential moving average
    int i = 0;
    //CHANGE THIS FOR DIFFERENT SMOOTHING EFFECT
    float beta = 0.8f;
    for (i = startindex + numberOfSamples - 1; i >= startindex; i--)
    {
        double temp = pow(beta, startindex + numberOfSamples - i - 1);
        numerator += array[i] * (float)temp;
        denominator += (float)temp;
    }

    int offset = 3;
    if (average != 0.0)
    {
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

    float ratio = avgRSSI * 1.0 / txPower;
    if (ratio < 1.0)
    {
        return pow(ratio, 10);
    }
    else
    {
        float distance =  (0.7) * pow(ratio, 10) + 0.024;
        return distance;
    }
}

// proximity code end

