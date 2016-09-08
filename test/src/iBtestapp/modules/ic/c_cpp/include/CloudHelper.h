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
 *      LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
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

#include "IotivityTest_Logger.h"
#include "CloudResourceHelper.h"
#include "BinarySwitchResourceHelper.h"

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

void handleLoginoutCB(const HeaderOptions &, const OCRepresentation &rep, const int ecode);
void onRefreshTokenCB(const HeaderOptions &, const OCRepresentation &rep, const int ecode);
void onSearchUser(const HeaderOptions &, const OCRepresentation &rep, const int ecode);
void onDeleteDevice(const HeaderOptions &headerOptions, const int eCode);

void onCreateGroup(const HeaderOptions &, const OCRepresentation &rep, const int ecode);
void onJoinGroup(const HeaderOptions &, const OCRepresentation &rep, const int ecode);
void onGetGroupList(const HeaderOptions &, const OCRepresentation &rep, const int ecode);
void onGetGroupInfo(const HeaderOptions &, const OCRepresentation &rep, const int ecode);
void onAddDeviceToGroup(const HeaderOptions &, const OCRepresentation &rep, const int ecode);
void onDeleteDeviceFromGroup(const HeaderOptions &headerOptions, const int eCode);
void onDeleteGroup(const HeaderOptions &headerOptions, const int eCode);
void onLeaveGroup(const HeaderOptions &headerOptions, const int eCode);
void onPublish(const OCRepresentation &, const int &eCode);
void foundDevice(shared_ptr< OC::OCResource > resource);
void onObserveDevPresence(const HeaderOptions headerOptions, const OCRepresentation &rep,
        const int &eCode, const int &sequenceNumber);

void onObserveGroup(const HeaderOptions headerOptions, const OCRepresentation &rep, const int &eCode,
        const int &sequenceNumber);
void onObserveInvitation(const HeaderOptions headerOptions, const OCRepresentation &rep, const int &eCode,
        const int &sequenceNumber);
void onSendInvitation(const HeaderOptions &, const OCRepresentation &rep, const int ecode);
void onCancelInvitation(const HeaderOptions &headerOptions, const int eCode);
void onDeleteInvitation(const HeaderOptions &headerOptions, const int eCode);

std::shared_ptr<OC::OCResource>  getFoundResource();

FILE *controleeOpen(const char * /*path*/, const char *mode);
FILE *controllerOpen(const char * /*path*/, const char *mode);

// API Helper
bool signUp(OCAccountManager::Ptr accountMgr, std::string authprovider, std::string authCode,
        PostCallback cloudConnectHandler, std::string &devID, std::string &devAccessToken,
        std::string &devRefreshToken);
bool signIn(OCAccountManager::Ptr accountMgr, const std::string& userUuid,
        const std::string& accessToken, PostCallback cloudConnectHandler);
bool signOut(OCAccountManager::Ptr accountMgr, PostCallback cloudConnectHandler);
bool refreshAccessToken(OCAccountManager::Ptr accountMgr, std::string userUuid,
        const std::string refreshToken, PostCallback cloudConnectHandler);
bool searchUser(OCAccountManager::Ptr accountMgr, const std::string& userUuid,
        GetCallback cloudConnectHandler);
bool deleteDevice(OCAccountManager::Ptr accountMgr, const std::string& deviceId,
        DeleteCallback cloudConnectHandler);


bool createGroup(OCAccountManager::Ptr accountMgr, AclGroupType groupType,
        PostCallback cloudConnectHandler, std::string &strGroupID);
bool joinGroup(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        PostCallback cloudConnectHandler);
bool leaveGroup(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        DeleteCallback cloudConnectHandler);
bool deleteGroup(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        DeleteCallback cloudConnectHandler);
bool getGroupInfo(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        GetCallback cloudConnectHandler);
bool getGroupList(OCAccountManager::Ptr accountMgr, PostCallback cloudConnectHandler);

bool addDeviceToGroup(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        const std::vector< std::string >& deviceId, PostCallback cloudConnectHandler);
bool deleteDeviceFromGroup(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        const std::vector< std::string >& deviceId, DeleteCallback cloudConnectHandler);

bool publishResourceToRD(std::string host, OCConnectivityType connectivityType,
        PublishResourceCallback callback);
bool findResource(std::string host, const std::string& resourceURI,
        OCConnectivityType connectivityType, FindCallback resourceHandler);


bool observeGroup(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        ObserveCallback cloudConnectHandler);
bool cancelObserveGroup(OCAccountManager::Ptr accountMgr, const std::string& groupId);
bool observeInvitation(OCAccountManager::Ptr accountMgr, ObserveCallback cloudConnectHandler);
bool cancelObserveInvitation(OCAccountManager::Ptr accountMgr);
bool sendInvitation(OCAccountManager::Ptr accountMgr,const std::string& groupId,
        const std::string& userUuid,
        PostCallback cloudConnectHandler);
bool cancelInvitation(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        const std::string& userUuid,
        DeleteCallback cloudConnectHandler);
bool deleteInvitation(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        DeleteCallback cloudConnectHandler);
void onObserve(const HeaderOptions headerOptions, const OCRepresentation &rep, const int &eCode,
        const int &sequenceNumber);
void onDelete(const HeaderOptions &headerOptions, const int eCode);
void onPost(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);
void onPut(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);
void onGet(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);


#endif

