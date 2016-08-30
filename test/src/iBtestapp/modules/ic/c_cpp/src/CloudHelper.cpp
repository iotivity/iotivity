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

#include <CloudHelper.h>

int g_isCbInvoked = CALLBACK_NOT_INVOKED;
std::string g_uid = "";
std::string g_accesstoken = "";
std::string g_refreshToken = "";
std::string g_groupID = "";

int waitCallbackRet()
{
    IOTIVITYTEST_LOG(DEBUG, "Waiting for Callback to be invoked");

    for (int i = 0; CALLBACK_TIMEOUT > i; ++i)
    {
        if (CALLBACK_INVOKED == g_isCbInvoked)
        {
            return CALLBACK_INVOKED;
        }

        sleep(DELAY_SHORT);
        printf("Second Elapsed : %d seconds\n", i);

        if (OC_STACK_OK != OCProcess())
        {
            printf("OCStack process error\n");
            return CALLBACK_NOT_INVOKED;
        }
    }

    return CALLBACK_NOT_INVOKED;
}

void printRepresentation(OCRepPayloadValue *value)
{
    while (value)
    {
        std::cout << "Key: " << value->name;
        switch (value->type)
        {
            case OCREP_PROP_NULL:
                std::cout << " Value: None" << std::endl;
                break;
            case OCREP_PROP_INT:
                std::cout << " Value: " << value->i << std::endl;
                break;
            case OCREP_PROP_DOUBLE:
                std::cout << " Value: " << value->d << std::endl;
                break;
            case OCREP_PROP_BOOL:
                std::cout << " Value: " << value->b << std::endl;
                break;
            case OCREP_PROP_STRING:
                std::cout << " Value: " << value->str << std::endl;
                break;
            case OCREP_PROP_BYTE_STRING:
                std::cout << " Value: Byte String" << std::endl;
                break;
            case OCREP_PROP_OBJECT:
                std::cout << " Value: Object" << std::endl;
                break;
            case OCREP_PROP_ARRAY:
                std::cout << " Value: Array" << std::endl;
                break;
        }

        if (strcmp(value->name, "accesstoken") == 0)
        {
            g_accesstoken = value->str;
        }

        if (strcmp(value->name, "uid") == 0)
        {
            g_uid = value->str;
        }

        if (strcmp(value->name, "gid") == 0)
        {
            g_groupID = value->str;
        }

        value = value->next;
    }
}

void handleLoginoutCB(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
{
    IOTIVITYTEST_LOG(DEBUG, "Auth response received code: %d", ecode);
    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep.getPayload()->values);
    }

    g_isCbInvoked = CALLBACK_INVOKED;
}

void onRefreshTokenCB(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
{
    IOTIVITYTEST_LOG(DEBUG, "Refresh Token response received code: %d", ecode);
    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep.getPayload()->values);
    }

    g_isCbInvoked = CALLBACK_INVOKED;
}

void onSearchUser(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
{
    IOTIVITYTEST_LOG(DEBUG, "Search User CB response received code: %d", ecode);
    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep.getPayload()->values);
    }

    g_isCbInvoked = CALLBACK_INVOKED;
}

void onCreateGroup(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
{
    IOTIVITYTEST_LOG(DEBUG, "Create Group CB response received code: %d", ecode);
    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep.getPayload()->values);
    }

    g_isCbInvoked = CALLBACK_INVOKED;
}

void onJoinGroup(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
{
    IOTIVITYTEST_LOG(DEBUG, "Join Group CB response received code: %d", ecode);
    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep.getPayload()->values);
    }

    g_isCbInvoked = CALLBACK_INVOKED;
}

void onGetGroupList(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
{
    IOTIVITYTEST_LOG(DEBUG, "Get Group List CB response received code: %d", ecode);
    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep.getPayload()->values);
    }

    g_isCbInvoked = CALLBACK_INVOKED;
}

void onGetGroupInfo(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
{
    IOTIVITYTEST_LOG(DEBUG, "Get Group Info CB response received code: %d", ecode);
    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep.getPayload()->values);
    }

    g_isCbInvoked = CALLBACK_INVOKED;
}

void onAddDeviceToGroup(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
{
    IOTIVITYTEST_LOG(DEBUG, "Add Device to Group CB response received code: %d", ecode);
    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep.getPayload()->values);
    }

    g_isCbInvoked = CALLBACK_INVOKED;
}

void onLeaveGroup(const HeaderOptions &headerOptions, const int eCode)
{
    IOTIVITYTEST_LOG(DEBUG, "Leave Group CB response received code: %d", eCode);
    g_isCbInvoked = CALLBACK_INVOKED;
}

void onDeleteDevice(const HeaderOptions &headerOptions, const int eCode)
{
    IOTIVITYTEST_LOG(DEBUG, "Delete Device CB response received code: %d", eCode);
    g_isCbInvoked = CALLBACK_INVOKED;
}

void onDeleteDeviceFromGroup(const HeaderOptions &headerOptions, const int eCode)
{
    IOTIVITYTEST_LOG(DEBUG, "Delete Device from Group CB response received code: %d", eCode);
    g_isCbInvoked = CALLBACK_INVOKED;
}

void onDeleteGroup(const HeaderOptions &headerOptions, const int eCode)
{
    IOTIVITYTEST_LOG(DEBUG, "Delete Group CB response received code: %d", eCode);
    g_isCbInvoked = CALLBACK_INVOKED;
}

void onPublish(const OCRepresentation &, const int &eCode)
{
    IOTIVITYTEST_LOG(DEBUG, "Publish resource response received code: %d", eCode);
    g_isCbInvoked = CALLBACK_INVOKED;
}

void foundDevice(shared_ptr< OC::OCResource > resource)
{
    IOTIVITYTEST_LOG(DEBUG, "foundDevice CB Invoked");
    vector< string > rt = resource->getResourceTypes();

    cout << "Device found: " << resource->uri() << endl;
    cout << "DI: " << resource->sid() << endl;

    for (auto it = rt.begin(); it != rt.end(); it++)
    {
        cout << "RT: " << *it << endl;
    }
    g_isCbInvoked = CALLBACK_INVOKED;
}

void onObserveDevPresence(const HeaderOptions headerOptions, const OCRepresentation &rep,
        const int &eCode, const int &sequenceNumber)
{
    IOTIVITYTEST_LOG(DEBUG, "onObserveDevPresence CB response received code: %d", eCode);
    IOTIVITYTEST_LOG(DEBUG, "onObserveDevPresence CB response sequenceNumber: %d", sequenceNumber);

    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep.getPayload()->values);
    }

    g_isCbInvoked = CALLBACK_INVOKED;

}

void onObserve(const HeaderOptions headerOptions, const OCRepresentation &rep, const int &eCode,
        const int &sequenceNumber)
{
    IOTIVITYTEST_LOG(DEBUG, "onObserve CB response received code: %d", eCode);
    IOTIVITYTEST_LOG(DEBUG, "onObserve CB response sequenceNumber: %d", sequenceNumber);

    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep.getPayload()->values);
    }

    g_isCbInvoked = CALLBACK_INVOKED;
}
void onObserveInvitation(const HeaderOptions headerOptions, const OCRepresentation &rep,
        const int &eCode, const int &sequenceNumber)
{
    IOTIVITYTEST_LOG(DEBUG, "onObserveInvitation CB response received code: %d", eCode);
    IOTIVITYTEST_LOG(DEBUG, "onObserveInvitation CB response sequenceNumber: %d", sequenceNumber);

    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep.getPayload()->values);
    }

    g_isCbInvoked = CALLBACK_INVOKED;
}
void onSendInvitation(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
{
    IOTIVITYTEST_LOG(DEBUG, "onSendInvitation CB response received code: %d", ecode);
    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep.getPayload()->values);
    }

    g_isCbInvoked = CALLBACK_INVOKED;
}

void onCancelInvitation(const HeaderOptions &headerOptions, const int eCode)
{
    IOTIVITYTEST_LOG(DEBUG, "onCancelInvitation CB response received code: %d", eCode);
    g_isCbInvoked = CALLBACK_INVOKED;
}

void onDeleteInvitation(const HeaderOptions &headerOptions, const int eCode)
{
    IOTIVITYTEST_LOG(DEBUG, "onDeleteInvitation CB response received code: %d", eCode);
    g_isCbInvoked = CALLBACK_INVOKED;
}

bool signUp(OCAccountManager::Ptr accountMgr, std::string authprovider, std::string authCode,
        PostCallback cloudConnectHandler, std::string &devID, std::string &devAccessToken,
        std::string &devRefreshToken)
{
    IOTIVITYTEST_LOG(DEBUG, "SignUp IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = accountMgr->signUp(authprovider, authCode, cloudConnectHandler);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] signUp returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    devID = g_uid;
    devAccessToken = g_accesstoken;
    //devRefreshToken = g_refreshToken; /* Will be enabled if a cloud Service that returns refresh token is available*/
    IOTIVITYTEST_LOG(DEBUG, "SignUp OUT");
    return true;

}

bool signIn(OCAccountManager::Ptr accountMgr, const std::string& userUuid,
        const std::string& accessToken, PostCallback cloudConnectHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "signIn IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->signIn(g_uid, g_accesstoken, cloudConnectHandler);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] signIn returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "signIn OUT");
    return true;
}

bool signOut(OCAccountManager::Ptr accountMgr, PostCallback cloudConnectHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "signOut IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->signOut(cloudConnectHandler);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] signIn returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "signOut OUT");
    return true;
}

bool refreshAccessToken(OCAccountManager::Ptr accountMgr, std::string userUuid,
        const std::string refreshToken, PostCallback cloudConnectHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "refreshAccessToken IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->refreshAccessToken(userUuid, refreshToken,
            cloudConnectHandler);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] refreshAccessToken returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }
    IOTIVITYTEST_LOG(DEBUG, "refreshAccessToken OUT");
    return true;
}

bool searchUser(OCAccountManager::Ptr accountMgr, const std::string& userUuid,
        GetCallback cloudConnectHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "searchUser IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->searchUser(userUuid, cloudConnectHandler);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] searchUser returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "searchUser OUT");
    return true;
}

bool deleteDevice(OCAccountManager::Ptr accountMgr, const std::string& deviceId,
        DeleteCallback cloudConnectHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "deleteDevice IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->deleteDevice(deviceId, cloudConnectHandler);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] deleteDevice returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "deleteDevice OUT");
    return true;
}

bool publishResourceToRD(std::string host, OCConnectivityType connectivityType,
        PublishResourceCallback callback)
{
    IOTIVITYTEST_LOG(DEBUG, "publishResourceToRD IN");

    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = OCPlatform::publishResourceToRD(host, connectivityType, callback);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] publishResourceToRD returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "publishResourceToRD OUT");
    return true;
}

bool createGroup(OCAccountManager::Ptr accountMgr, AclGroupType groupType,
        PostCallback cloudConnectHandler, std::string &strGroupID)
{
    IOTIVITYTEST_LOG(DEBUG, "createGroup IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->createGroup(groupType, cloudConnectHandler);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] createGroup returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    strGroupID = g_groupID;

    IOTIVITYTEST_LOG(DEBUG, "createGroup OUT");
    return true;
}

bool joinGroup(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        PostCallback cloudConnectHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "joinGroup IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->joinGroup(groupId, cloudConnectHandler);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] joinGroup returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "joinGroup OUT");
    return true;
}

bool leaveGroup(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        DeleteCallback cloudConnectHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "leaveGroup IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->deleteGroup(groupId, cloudConnectHandler);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] leaveGroup returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "leaveGroup OUT");
    return true;
}

bool deleteGroup(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        DeleteCallback cloudConnectHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "deleteGroup IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->deleteGroup(groupId, cloudConnectHandler);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] deleteGroup returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "deleteGroup OUT");
    return true;
}

bool getGroupInfo(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        GetCallback cloudConnectHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "getGroupInfo IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->getGroupInfo(groupId, cloudConnectHandler);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] getGroupInfo returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "getGroupInfo OUT");
    return true;
}

bool getGroupList(OCAccountManager::Ptr accountMgr, PostCallback cloudConnectHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "getGroupList IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->getGroupList(cloudConnectHandler);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] getGroupList returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "getGroupList OUT");
    return true;
}

bool addDeviceToGroup(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        const std::vector< std::string >& deviceId, PostCallback cloudConnectHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "addDeviceToGroup IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->addDeviceToGroup(groupId, deviceId, cloudConnectHandler);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] addDeviceToGroup returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "addDeviceToGroup OUT");
    return true;
}

bool deleteDeviceFromGroup(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        const std::vector< std::string >& deviceId, DeleteCallback cloudConnectHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "deleteDeviceFromGroup IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->deleteDeviceFromGroup(groupId, deviceId,
            cloudConnectHandler);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] deleteDeviceFromGroup returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "deleteDeviceFromGroup OUT");
    return true;
}

bool findResource(std::string host, const std::string& resourceURI,
        OCConnectivityType connectivityType, FindCallback resourceHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "findResource IN");

    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = OCPlatform::findResource(host, resourceURI, connectivityType,
            resourceHandler);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] findResource returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "findResource OUT");
    return true;
}

bool observeGroup(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        ObserveCallback cloudConnectHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "observeGroup IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->observeGroup(groupId, cloudConnectHandler);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] observeGroup returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "observeGroup OUT");
    return true;
}

bool cancelObserveGroup(OCAccountManager::Ptr accountMgr, const std::string& groupId)
{
    IOTIVITYTEST_LOG(DEBUG, "cancelObserveGroup IN");

    OCStackResult result = accountMgr->cancelObserveGroup(groupId);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] cancelObserveGroup returns %d", result);

    IOTIVITYTEST_LOG(DEBUG, "cancelObserveGroup OUT");
}

bool observeInvitation(OCAccountManager::Ptr accountMgr, ObserveCallback cloudConnectHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "observeInvitation IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->observeInvitation(cloudConnectHandler);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] observeInvitation returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "observeInvitation OUT");
    return true;
}

bool cancelObserveInvitation(OCAccountManager::Ptr accountMgr)
{
    IOTIVITYTEST_LOG(DEBUG, "cancelObserveInvitation IN");

    OCStackResult result = accountMgr->cancelObserveInvitation();
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] cancelObserveInvitation returns %d", result);

    IOTIVITYTEST_LOG(DEBUG, "cancelObserveInvitation OUT");
}

bool sendInvitation(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        const std::string& userUuid, PostCallback cloudConnectHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "sendInvitation IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->sendInvitation(groupId, userUuid, cloudConnectHandler);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] sendInvitation returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "sendInvitation OUT");
    return true;
}

bool cancelInvitation(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        const std::string& userUuid, DeleteCallback cloudConnectHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "cancelInvitation IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->cancelInvitation(groupId, userUuid, cloudConnectHandler);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] cancelInvitation returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "cancelInvitation OUT");
    return true;
}

bool deleteInvitation(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        DeleteCallback cloudConnectHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "deleteInvitation IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->deleteInvitation(groupId, cloudConnectHandler);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] deleteInvitation returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "deleteInvitation OUT");
    return true;
}

FILE *controllerOpen(const char * /*path*/, const char *mode)
{
    return fopen(CONTROLLER_DAT, mode);
}

FILE *controleeOpen(const char * /*path*/, const char *mode)
{
    return fopen(CONTROLEE_DAT, mode);
}
