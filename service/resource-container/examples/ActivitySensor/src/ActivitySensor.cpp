//******************************************************************
//
// Copyright 2015 Jake Martin Choi All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/**
 * This file contains the exported symbol.
 */
#include "ActivitySensor.h"

#include <iostream>
#include <stdlib.h>
#include "SysTimer.h"

#ifdef __ANDROID__
#include "OCAndroid.h"
#endif

using namespace ActivitySensorName;


ActivitySensor::ActivitySensor()
{
    heartrate = "";
    activityresult = "";	
    gps = "";
    acceleration = "";
    /*m_weight = "";
    m_height = "";
    m_BMIResult = "";*/
    time(&m_timepstampW);
    time(&m_timepstampH);
}

ActivitySensor::~ActivitySensor()
{

}

int ActivitySensor::executeActivitySensorLogic(std::map<std::string, std::string> *pInputData, std::string *pOutput)
{
	ActivityResult result;

	if(pInputData->find("heartrate") != pInputData->end())
	{
		heartrate = pInputData->at("heartrate");
		time(&m_timepstampW);
	}
	
	if(pInputData->find("acceleration") != pInputData->end())
		heartrate = pInputData->at("acceleration");
	if(pInputData->find("gps") != pInputData->end())
		heartrate = pInputData->at("gps");



	if ((result = makeActivity()) != SUCCESS)
		return -1;

	(*pOutput) = activityresult;

	return 0;
}

ActivityResult ActivitySensor::makeActivity(void)
{
	double heart, timediffsecond;
	double accel, points[3];

	int ActivityResult;

	if(!heartrate.empty() && !acceleration.empty() && !gps.empty())
	{
		heart = std::stod(heartrate);	
		accel = std::stod(acceleration);
		
		timediffsecond = abs(difftime(m_timepstampW, m_timepstampH));

		if(timediffsecond > DIFFTIME)
		{
			ActivityResult = -1;
			std::cout << "[ActivitySensor] : Activity Result Wrong: " << ActivityResult << std::endl;
		}
		else if ((heart > 100) && (accel > 10))
		{
			ActivityResult = (int)EXERCISING; 
			std::cout << "[ActivitySensor] : Activityresult:" << ActivityResult << " EXERCISING" << std::endl;
		}
		else if ((heart < 50) || ((heart > 100) && (accel < 10))) {
			ActivityResult = (int)ABNORMAL;
			
			std::cout << "[ActivitySensor] : Activityresult:" << ActivityResult << " ABNORMAL" << std::endl;
		}
		else
		{
			ActivityResult = (int)NORMAL;
			std::cout << "[ActivitySensor] : Activityresult:" << ActivityResult << " NORMAL" << std::endl;
		}
			

    std::cout << std::endl;

	activityresult  = std::to_string(ActivityResult);

        return SUCCESS;
    }

        else
        {
		heart = -1;
		ActivityResult = -1;
            	std::cout << "[ActivitySensor] :   UNKNOWN: " << ActivityResult << std::endl;
        }

    
    return ERROR;

}



