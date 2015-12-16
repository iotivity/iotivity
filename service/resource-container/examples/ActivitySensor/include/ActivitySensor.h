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

#ifndef ACTIVITYSENSOR_H_
#define ACTIVITYSENSOR_H_
#include <string>
#include <map>
#include <time.h>


#define DIFFTIME      5     // valid time difference. (seconds)
#define NORMAL	1
#define EXERCISING 2
#define ABNORMAL 3



namespace ActivitySensorName
{
    typedef enum
    {
        SUCCESS = 0, ERROR
    } ActivityResult;

    class ActivitySensor
    {
        public:
            ActivitySensor();
            ~ActivitySensor();  
            int executeActivitySensorLogic(std::map<std::string, std::string> *pInputData, std::string *pOutput);
            ActivityResult makeActivity(void);

        private:
            std::string heartrate, gps, acceleration; 
	    std::string activityresult;

            time_t  m_timepstampW;
            time_t  m_timepstampH;
    };
};

#endif /* BMISENSOR_H_ */
