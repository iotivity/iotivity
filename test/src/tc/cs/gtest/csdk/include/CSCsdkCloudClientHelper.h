/******************************************************************
 *
 * Copyright 20167 Samsung Electronics All Rights Reserved.
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
 *
 ******************************************************************/

#ifndef CSCsdkCloudClientHelper_H_
#define CSCsdkCloudClientHelper_H_

#if defined(__CLOUD__)
#include "CSCsdkUtilityHelper.h"

using namespace OC;
using namespace std;

#define BINARY_SWITCH_KEY "value"
#define BINARY_SWITCH_VALUE true

#define TEMPERATURE_RANGE_KEY "range"
#define TEMPERATURE_UNITS_KEY "units"
#define TEMPERATURE_UNITS_SUPPORTED_VALUE "C"
#define TEMPERATURE_RANGE_START_VALUE 10.01
#define TEMPERATURE_RANGE_END_VALUE 40.01
#define TEMPERATURE_KEY "temperature"
#define TEMPERATURE_VALUE 15.05

#define AIRFLOW_RANGE_KEY "range"
#define AIRFLOW_SPEED_KEY "speed"
#define AIRFLOW_DIRECTION_KEY "direction"
#define AIRFLOW_SPEED_VALUE 20
#define AIRFLOW_DIRECTION_VALUE "up"
#define AIRFLOW_SPEED_RANGE_START_VALUE 0
#define AIRFLOW_SPEED_RANGE_END_VALUE 50

#define TIMER_HOUR_KEY "x.com.vendor.timer.hour"
#define TIMER_MINUTE_KEY "x.com.vendor.timer.minute"
#define TIMER_SECOND_KEY "x.com.vendor.timer.second"
#define TIMER_RESET_KEY "x.com.vendor.timer.reset"
#define TIMER_HOUR_VALUE 10
#define TIMER_MINUTE_VALUE 30
#define TIMER_SECOND_VALUE 30
#define TIMER_RESET_VALUE true

#define CHILD_LOCK_KEY "x.com.vendor.child.lock.value"
#define CHILD_LOCK_VALUE false

#define NEW_RESOURCE_URI "/new-resource/created-light-1"

using namespace OC;
using namespace std;

enum class ResourceSelectionType
{
    VERTICAL_RESOURCE = 0,
    SYSTEM_RESOURCE,
    VERTICAL_OBSERVABLE_RESOURCE,
    VERTICAL_NON_OBSERVABLE_RESOURCE
};

void onObserve(const HeaderOptions headerOptions, const OCRepresentation &rep, const int &eCode,
        const int &sequenceNumber);
void onDelete(const HeaderOptions &headerOptions, const int eCode);
void onPost(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);
void onPut(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);
void onGet(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);
void onDeviceInfoReceived(const OCRepresentation& rep);
void onPlatformInfoReceived(const OCRepresentation& rep);
void onResourceFound(shared_ptr< OCResource > resource);
void onPublish(const OCRepresentation &, const int &eCode);

void handleMenu(void);
OCStackResult initiateServer(void);
void createAirConDevice(bool isSecured = false);
void deleteResource(void);
void findResource(string resourceType, string host = "");
void findAllResources(string host = "", string query = "");
void discoverDevice(bool);
void discoverPlatform(bool isMulticast = true);
void publishResource();
void sendGetRequest(ResourceSelectionType type);
void sendPutRequest(ResourceSelectionType type);
void sendPostRequest(ResourceSelectionType type);
void sendBlockwisePost(void);
void sendDeleteRequest(ResourceSelectionType type);
void waitForCallback(void);
void observeResource(ResourceSelectionType type);
void cancelObserveResource(ResourceSelectionType type);
vector< shared_ptr< OCResource > > getRemoteResourceList(ResourceSelectionType type);
void updateLocalResources(void);
AttributeValue getResourceAttributeValue(string uri, string& key);
string getHost();
FILE* server_fopen(const char*, const char*);
FILE* client_fopen(const char*, const char*);


#endif /*__CLOUD*/
#endif /*CSCsdkCloudHelper_H_*/
