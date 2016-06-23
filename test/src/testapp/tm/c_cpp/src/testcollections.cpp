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

#include <vector>
#include <iostream>
#include <stdlib.h>

#include "GroupManager.h"
//#include "GroupSynchronization.h"
#include "SampleResource.h"
#include "CommonUtil.h"

#define STARTING_PORT 33343

#define THERMOMETER_URI "/sensor/thermometer"
#define BAROMETER_URI "/sensor/barometer"
#define RESOURCE_TYPE_THERMOMETER "core.thermometer"
#define RESOURCE_TYPE_BAROMETER "core.barometer"
#define COLLECTION_TYPE_ROOM_SMALL "core.room-small"
#define COLLECTION_TYPE_ROOM_MEDIUM "core.room-medium"
#define COLLECTION_TYPE_ROOM_LARGE "core.room-large"
#define DEFAULT_GROUP_TYPE "oic.wk.col"

using namespace OIC;

OCResourceHandle createGroup(string groupType)
{
    OCResourceHandle collectionHandle = nullptr;

    try
    {
        string groupURI = "/core/a/collection/" + groupType.substr(5);
        string resourceInterface = BATCH_INTERFACE;

        // EntityHandler entityHandler = std::bind(, PH::_1, PH::_2);
        OCPlatform::registerResource(collectionHandle, groupURI, groupType, resourceInterface,
        NULL,
        //&entityHandler, // entityHandler
                OC_DISCOVERABLE | OC_OBSERVABLE);

        cout << "Create Group is called." << endl;

//	        requestURI << OC_RSRVD_WELL_KNOWN_URI << "?rt=core.light";
//
//	        OCPlatform::findResource("", requestURI.str(),
//	                                 connectivityType, &foundResource);

        OCPlatform::bindInterfaceToResource(collectionHandle, GROUP_INTERFACE);
        OCPlatform::bindInterfaceToResource(collectionHandle, DEFAULT_INTERFACE);
        OCPlatform::bindTypeToResource(collectionHandle, DEFAULT_GROUP_TYPE);

        cout << "Successfully Created Group with uri = " << groupURI << endl;
    }
    catch (OCException& e)
    {
        oclog() << "Exception in main: " << e.what();
        cout << "Unable to Create Group!!" << endl;
    }
    return collectionHandle;
}

bool joinGroup(OCResourceHandle collectionHandle, OCResourceHandle childHandle)
{
    bool isJoined = false;

    try
    {
        OCStackResult expectedResult = OCPlatform::bindResource(collectionHandle, childHandle);
        if (expectedResult == OC_STACK_OK)
        {
            cout << "Joining to the group completed!!" << endl;
            isJoined = true;
        }
        else
        {
            cout << "Unable to join to group" << endl;
        }
    }
    catch (exception& e)
    {
        cout << "Exception occurred while trying to join to grop, exception is: " << e.what()
                << endl;
    }

    return isJoined;
}

int main()
{
    SampleResource dummySensorThermometer;
    SampleResource dummySensorBarometer;

    OCStackResult result = OC_STACK_OK;

    string serverIp = "0.0.0.0";

    result = SampleResource::constructServer(serverIp, 0);
    result = (OCStackResult) ((int) result
            + (int) dummySensorThermometer.setResourceProperties(THERMOMETER_URI,
            RESOURCE_TYPE_THERMOMETER, DEFAULT_INTERFACE));
    result = (OCStackResult) ((int) result + (int) dummySensorThermometer.startServer());

    result = (OCStackResult) ((int) result
            + (int) dummySensorBarometer.setResourceProperties(BAROMETER_URI,
            RESOURCE_TYPE_BAROMETER, DEFAULT_INTERFACE));
    result = (OCStackResult) ((int) result + (int) dummySensorBarometer.startServer());

    if (result != OC_STACK_OK)
    {
        IOTIVITYTEST_LOG(ERROR, "Unable to start resources");
        return 1;
    }

    OCResourceHandle largeRoomHandle = createGroup(COLLECTION_TYPE_ROOM_LARGE);
    OCResourceHandle mediumRoomHandle = createGroup(COLLECTION_TYPE_ROOM_MEDIUM);
    OCResourceHandle smallRoomHandle = createGroup(COLLECTION_TYPE_ROOM_SMALL);
    if (joinGroup(largeRoomHandle, dummySensorBarometer.getResourceHandle()))
    {
        IOTIVITYTEST_LOG(INFO, "Barometer successfully joined to room-large");
    }
    if (joinGroup(largeRoomHandle, dummySensorThermometer.getResourceHandle()))
    {
        IOTIVITYTEST_LOG(INFO, "Thermometer successfully joined to room-large");
    }

    IOTIVITYTEST_LOG(INFO, "Press any key and the enter to stop the Collections :-)....");
    char n;
    std::cin >> n;

    OCPlatform::unbindResource(largeRoomHandle, dummySensorBarometer.getResourceHandle());
    OCPlatform::unbindResource(largeRoomHandle, dummySensorThermometer.getResourceHandle());

    dummySensorThermometer.stopServer();
    dummySensorBarometer.stopServer();

    OCPlatform::unregisterResource(largeRoomHandle);
    OCPlatform::unregisterResource(mediumRoomHandle);
    OCPlatform::unregisterResource(smallRoomHandle);

    return 0;
}
