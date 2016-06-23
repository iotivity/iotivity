/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#ifndef __SSM_RH_COMMON_H__
#define __SSM_RH_COMMON_H__

#include <string>
#include <cstdlib>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

extern "C" {
    #include "Hosting.h"
}

#include "OCPlatform.h"
#include "OCApi.h"
#include "CommonUtil.h"
#include <mutex>
using namespace OC;

#define 	OBSERVE 		1
#define 	GET     2
#define 	PUT     3
#define 	DELETE  4
#define 	START_ERR_MSG 		"FAILED to start Coordinate"
#define 	STOP_ERR_MSG 		"Coordinator can't be stopped"
#define 	POST_FAIL_MSG 		"Fail resource POST process"
#define 	GET_FAIL_MSG 		"Fail resource GET process"
#define  	FAIL_RES_POST 		"Fail resource post() process"
#define  	FAIL_RES_GET 		"Fail resource get() process"
#define  	FAIL_RES_OBSERVE	"Fail resource observe() process"


#define RUN_HOSTABLE_RESOURCE_ONE() \
do{ \
    CommonUtil::launchApp("./rh_temp_hum_sensor");\
}while(0)

#define RUN_HOSTABLE_RESOURCE_TWO() \
do{ \
    CommonUtil::launchApp("./rh_temp_hum_sensor2");\
}while(0)

#define RUN_NON_HOSTABLE_RESOURCE() \
do{ \
    CommonUtil::launchApp("./rh_non_hostable_provider");\
}while(0)


#define RUN_RESOURCE_HOST() \
do{ \
    CommonUtil::launchApp("./rh_manager");\
}while(0)

#define KILL_RESOURCE_HOST() \
do{ \
    CommonUtil::killApp("rh_manager");\
}while(0)

#define KILL_HOSTABLE_RESOURCE_ONE() \
do{ \
    CommonUtil::killApp("rh_temp_hum_sensor");\
}while(0)

#define KILL_HOSTABLE_RESOURCE_TWO() \
do{ \
    CommonUtil::killApp("rh_temp_hum_sensor2");\
}while(0)

#define KILL_NON_HOSTABLE_RESOURCE() \
do{ \
    CommonUtil::killApp("rh_non_hostable_provider");\
}while(0)

#define KILL_ALL() \
do{ \
    CommonUtil::killApp("rh_temp_hum_sensor");\
    CommonUtil::killApp("rh_temp_hum_sensor2");\
    CommonUtil::killApp("rh_manager");\
    CommonUtil::killApp("rh_non_hostable_provider");\
}while(0)

class RHCommon
{
public:

    static std::shared_ptr< OCResource > m_curResource;
    static std::shared_ptr< OCResource > m_curResource2;
    static std::shared_ptr< OCResource > m_curResourceNonHostable;
    std::vector <std::shared_ptr< OCResource >> resourceVector;
    int SUCCESS_RESPONSE;
    std::string m_errorMessage;
    std::string resourceUriOne;
    std::string resourceUriTwo;
    int resourceOneTemp;
    int resourceOneHum;
    int resourceTwoTemp;
    int resourceTwoHum;


    RHCommon();
    void initialize();
    bool startOCResourceHost();
    bool stopOCResourceHost();
    bool verifySensor(std::shared_ptr< OCResource > resource, std::string uri, std::string &errorMessage);
    bool verifyTempHumSensorOne(std::string &errorMessage);
    bool verifyTempHumSensorTwo(std::string &errorMessage);
    bool verifyObserveDataFromSensorOne(std::string &errorMessage, int expectedTemperature = 100, int expectedHumidity = 100);
    bool verifyObserveDataFromSensorTwo(std::string &errorMessage, int expectedTemperature = 30, int expectedHumidity = 30);



    void onObserve(const HeaderOptions &headerOption , const OCRepresentation &rep , const int &eCode,
                   const int &sequenceNumber);

    void onPost(const HeaderOptions &headerOption, const OCRepresentation &rep, const int eCode);
    void onPut(const HeaderOptions &headerOption, const OCRepresentation &rep, const int eCode);
    void onGet(const HeaderOptions &headerOption , const OCRepresentation &rep , const int eCode);
    void onDelete(const HeaderOptions &headerOption , const int eCode);

    static void foundResource(std::shared_ptr< OCResource > resource);

    static int TestFunction();
};

#endif
