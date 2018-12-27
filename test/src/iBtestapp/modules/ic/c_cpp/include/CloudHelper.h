/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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
 *
 ******************************************************************/

#ifndef CloudHelper_H_
#define CloudHelper_H_

#include <memory>
#include <iostream>
#include <stdexcept>
#include <condition_variable>
#include <map>
#include <vector>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <logger.h>
#include "ocstack.h"
#include "ocpayload.h"
#include "rd_client.h"
#include "OCApi.h"
#include "OCPlatform.h"
#include "OCPlatform_impl.h"
#include "RDClient.h"
#include "OCResource.h"

#include "IotivityTest_Logger.h"
#include "CloudResourceHelper.h"
#include "BinarySwitchResourceHelper.h"
#include "CommonUtil.h"
#include "Configuration.h"
#include "CloudCommonUtil.h"
#include "CommonProperties.h"

using namespace OC;
using namespace std;

#define CONTROLEE 0
#define CONTROLLER 1
#define NONE -1
#define DEFAULT_PRESENCE_TTL 60
#define DEFAULT_CONTEXT_VALUE 0x99
#define DEVICE_TYPE_VALUE "oic.d.binarySwitch"
#define RES_DEVICE_NAME "FAC_2016"
#define SWITCH_RES_URI "/power/0"
#define SWITCH_RES_TYPE "oic.r.switch.binary"
#define QUERY_FOR_SWITCH "/oic/res"
#define CONTROLEE_DAT "./resource_server.dat"
#define CONTROLLER_DAT "./resource_controller.dat"
#define DELAY_SHORT 1
#define CALLBACK_TIMEOUT 60
#define CALLBACK_INVOKED 1
#define CALLBACK_NOT_INVOKED 0
#define DEFAULT_MQ_BROKER_URI "/oic/ps"

extern vector< shared_ptr< OC::OCResource > > g_topicList;
extern std::string g_accesstoken;

bool initCloudClient();

void handleLoginoutCB(const HeaderOptions &, const OCRepresentation &rep, const int ecode);
void onRefreshTokenCB(const HeaderOptions &, const OCRepresentation &rep, const int ecode);
void onSearchUser(const HeaderOptions &, const OCRepresentation &rep, const int ecode);
void onDeleteDevice(const HeaderOptions &headerOptions, const int eCode);

void onCreateGroup(const HeaderOptions &, const OCRepresentation &rep, const int ecode);
void onJoinGroup(const HeaderOptions &, const OCRepresentation &rep, const int ecode);
void onGetGroupInfo(const HeaderOptions &, const OCRepresentation &rep, const int ecode);
void onDeleteDeviceFromGroup(const HeaderOptions &headerOptions, const int eCode);
void onDeleteGroup(const HeaderOptions &headerOptions, const int eCode);
void onLeaveGroup(const HeaderOptions &headerOptions, const int eCode);
void onPublish(const OCRepresentation &, const int &eCode);
void foundDevice(shared_ptr< OC::OCResource > resource);
void onObserveDevPresence(const HeaderOptions headerOptions, const OCRepresentation &rep,
        const int &eCode, const int &sequenceNumber);

void onObserveGroup(const HeaderOptions headerOptions, const OCRepresentation &rep,
        const int &eCode, const int &sequenceNumber);
void onObserveInvitation(const HeaderOptions headerOptions, const OCRepresentation &rep,
        const int &eCode, const int &sequenceNumber);
void onSendInvitation(const HeaderOptions &, const OCRepresentation &rep, const int ecode);
void onCancelInvitation(const HeaderOptions &headerOptions, const int eCode);
void onDeleteInvitation(const HeaderOptions &headerOptions, const int eCode);

std::shared_ptr< OC::OCResource > getFoundResource();

FILE *controleeOpen(const char * /*path*/, const char *mode);
FILE *controllerOpen(const char * /*path*/, const char *mode);

// API Helper
bool refreshAccessToken(OCAccountManager::Ptr accountMgr, std::string userUuid,
        const std::string refreshToken, PostCallback cloudConnectHandler);
bool searchUser(OCAccountManager::Ptr accountMgr, QueryParamsMap query,
        GetCallback cloudConnectHandler);
bool deleteDevice(OCAccountManager::Ptr accountMgr, const std::string& deviceId,
        const std::string& accessToken, DeleteCallback cloudConnectHandler);

bool createGroup(OCAccountManager::Ptr accountMgr, QueryParamsMap query,
        PostCallback cloudConnectHandler);
bool deleteGroup(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        DeleteCallback cloudConnectHandler);
bool getGroupInfo(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        GetCallback cloudConnectHandler);

bool deleteDeviceFromGroup(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        const std::vector< std::string >& deviceId, DeleteCallback cloudConnectHandler);

bool publishResourceToRD(std::string host, OCConnectivityType connectivityType, ResourceHandles resourceHandles,
        PublishResourceCallback callback);
bool findResource(std::string host, const std::string& resourceURI,
        OCConnectivityType connectivityType, FindCallback resourceHandler);

bool observeGroup(OCAccountManager::Ptr accountMgr, ObserveCallback cloudConnectHandler);
bool cancelObserveGroup(OCAccountManager::Ptr accountMgr);
bool observeInvitation(OCAccountManager::Ptr accountMgr, ObserveCallback cloudConnectHandler);
bool cancelObserveInvitation(OCAccountManager::Ptr accountMgr);
bool sendInvitation(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        const std::string& userUuid, PostCallback cloudConnectHandler);
bool cancelInvitation(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        const std::string& userUuid, DeleteCallback cloudConnectHandler);
void onObserve(const HeaderOptions headerOptions, const OCRepresentation &rep, const int &eCode,
        const int &sequenceNumber);
void onDelete(const HeaderOptions &headerOptions, const int eCode);
void onPost(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);
void onPut(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);
void onGet(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);
void discoverTopicCB(const int ecode, const string &, shared_ptr< OC::OCResource > topic);
void publishMessageCB(const HeaderOptions &, const OCRepresentation &, const int eCode);
void createTopicCB(const int ecode, const string &originUri, shared_ptr< OC::OCResource > topic);
void subscribeCB(const HeaderOptions &, const OCRepresentation &rep, const int eCode,
        const int sequenceNumber);

bool replyToInvitation(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        const std::string& accept, DeleteCallback cloudConnectHandler);
bool addPropertyValueToGroup(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        const OCRepresentation propertyValue, PostCallback cloudConnectHandler);
bool deletePropertyValueFromGroup(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        const OCRepresentation propertyValue, PostCallback cloudConnectHandler);
bool updatePropertyValueOnGroup(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        const OCRepresentation propertyValue, PostCallback cloudConnectHandler);
bool discoveryMQTopics(OC::OCResource::Ptr g_mqBrokerResource,
        const QueryParamsMap& queryParametersMap, MQTopicCallback attributeHandler,
        QualityOfService qos);
bool publishMQTopic(OC::OCResource::Ptr g_mqBrokerResource, const OCRepresentation& rep,
        const QueryParamsMap& queryParametersMap, PostCallback attributeHandler,
        QualityOfService qos);
bool createMQTopic(OC::OCResource::Ptr g_mqBrokerResource, const OCRepresentation& rep,
        const std::string& topicUri, const QueryParamsMap& queryParametersMap,
        MQTopicCallback attributeHandler, QualityOfService qos);
bool subscribeMQTopic(OC::OCResource::Ptr g_mqBrokerResource, ObserveType observeType,
        const QueryParamsMap& queryParametersMap, ObserveCallback observeHandler,
        QualityOfService qos);
bool unsubscribeMQTopic(OC::OCResource::Ptr g_mqBrokerResource, QualityOfService qos);
bool turnOnOffSwitch(bool bolean);

#endif

