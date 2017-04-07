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

#include <CloudHelper.h>

int g_isCbInvoked = CALLBACK_NOT_INVOKED;
std::string g_uid = "";
std::string g_accesstoken = "";
std::string g_refreshToken = "";
std::string g_groupID = "";
std::shared_ptr< OC::OCResource > g_foundFoundResource;
vector< shared_ptr< OC::OCResource > > g_topicList;

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

void printRepresentation(OCRepresentation rep)
{
    for (auto itr = rep.begin(); itr != rep.end(); ++itr)
    {
        cout << "\t" << itr->attrname() << ":\t" << itr->getValueToString() << endl;

        if (itr->attrname().compare("accesstoken") == 0)
        {
            g_accesstoken = itr->getValueToString();
        }

        if (itr->attrname().compare("uid") == 0)
        {
            g_uid = itr->getValueToString();
        }

        if (itr->attrname().compare("gid") == 0)
        {
            g_groupID = itr->getValueToString();
        }

        if (itr->type() == AttributeType::Vector)
        {
            switch (itr->base_type())
            {
                case AttributeType::OCRepresentation:
                    for (auto itr2 : (*itr).getValue< vector< OCRepresentation > >())
                    {
                        printRepresentation(itr2);
                    }
                    break;

                case AttributeType::Integer:
                    for (auto itr2 : (*itr).getValue< vector< int > >())
                    {
                        cout << "\t\t" << itr2 << endl;
                    }
                    break;

                case AttributeType::String:
                    for (auto itr2 : (*itr).getValue< vector< string > >())
                    {
                        cout << "\t\t" << itr2 << endl;
                    }
                    break;

                default:
                    cout << "Unhandled base type " << itr->base_type() << endl;
                    break;
            }
        }
        else if (itr->type() == AttributeType::OCRepresentation)
        {
            printRepresentation((*itr).getValue< OCRepresentation >());
        }
    }
}

void handleLoginoutCB(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
{
    IOTIVITYTEST_LOG(DEBUG, "Auth response received code: %d", ecode);
    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep);
    }

    g_isCbInvoked = CALLBACK_INVOKED;
}

void onRefreshTokenCB(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
{
    IOTIVITYTEST_LOG(DEBUG, "Refresh Token response received code: %d", ecode);
    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep);
    }

    g_isCbInvoked = CALLBACK_INVOKED;
}

void onSearchUser(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
{
    IOTIVITYTEST_LOG(DEBUG, "Search User CB response received code: %d", ecode);
    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep);
    }

    g_isCbInvoked = CALLBACK_INVOKED;
}

void onCreateGroup(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
{
    IOTIVITYTEST_LOG(DEBUG, "Create Group CB response received code: %d", ecode);
    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep);
    }

    g_isCbInvoked = CALLBACK_INVOKED;
}

void onGetGroupInfo(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
{
    IOTIVITYTEST_LOG(DEBUG, "Get Group Info CB response received code: %d", ecode);
    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep);
    }

    g_isCbInvoked = CALLBACK_INVOKED;
}

void onDeleteDevice(const HeaderOptions &headerOptions, const int eCode)
{
    IOTIVITYTEST_LOG(DEBUG, "Delete Device CB response received code: %d", eCode);
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

    cout << "Resource found with URI: " << resource->uri() << endl;
    cout << "Resource DI: " << resource->sid() << endl;
    cout << "Resource Host: " << resource->host() << endl;

    if (resource->uri().compare(SWITCH_RES_URI) == 0)
    {
        IOTIVITYTEST_LOG(DEBUG, "Found desired resource");
        g_foundFoundResource = resource;

        for (auto resourceType : rt)
        {
            cout << "RT of desired resource: " << resourceType << endl;
        }
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
        printRepresentation(rep);
    }

    g_isCbInvoked = CALLBACK_INVOKED;

}

void onObserveGroup(const HeaderOptions headerOptions, const OCRepresentation &rep,
        const int &eCode, const int &sequenceNumber)
{
    IOTIVITYTEST_LOG(DEBUG, "onObserve CB response received code: %d", eCode);
    IOTIVITYTEST_LOG(DEBUG, "onObserve CB response sequenceNumber: %d", sequenceNumber);

    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep);
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
        printRepresentation(rep);
    }

    g_isCbInvoked = CALLBACK_INVOKED;
}
void onSendInvitation(const HeaderOptions &, const OCRepresentation &rep, const int ecode)
{
    IOTIVITYTEST_LOG(DEBUG, "onSendInvitation CB response received code: %d", ecode);
    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep);
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

////////////////////MQ CB start ///////////////////////////
void discoverTopicCB(const int ecode, const string &, shared_ptr< OC::OCResource > topic)
{
    IOTIVITYTEST_LOG(DEBUG, "Topic discovered code: %d", ecode);
    g_topicList.push_back(topic);
    cout << "Topic [" << g_topicList.size() - 1 << "] " << topic->uri() << " discovered" << endl;
    g_isCbInvoked = CALLBACK_INVOKED;
}

void publishMessageCB(const HeaderOptions &, const OCRepresentation &, const int eCode)
{
    IOTIVITYTEST_LOG(DEBUG, "Publish message response received, code: %d", eCode);
    g_isCbInvoked = CALLBACK_INVOKED;
}

void createTopicCB(const int ecode, const string &originUri, shared_ptr< OC::OCResource > topic)
{
    cout << "Create topic response received, code: " << ecode << endl;

    if (ecode == OCStackResult::OC_STACK_RESOURCE_CREATED)
    {
        cout << "Created topic : " << topic->uri() << endl;
    }
    else
    {
        cout << "Topic creation failed : " << originUri << endl;
    }
    g_isCbInvoked = CALLBACK_INVOKED;
}

void subscribeCB(const HeaderOptions &, const OCRepresentation &rep, const int eCode,
        const int sequenceNumber)
{
    try
    {
        if (eCode == OC_STACK_OK && sequenceNumber <= MAX_SEQUENCE_NUMBER)
        {
            if (sequenceNumber == OC_OBSERVE_REGISTER)
            {
                cout << "Observe registration action is successful" << endl;
            }

            cout << "OBSERVE RESULT:" << endl;
            printRepresentation(rep);
        }
        else
        {
            if (eCode == OC_STACK_OK)
            {
                cout << "Observe registration failed or de-registration action failed/succeeded"
                        << endl;
            }
            else
            {
                cout << "onObserve Response error: " << eCode << endl;
                exit(-1);
            }
        }
    }
    catch (exception &e)
    {
        cout << "Exception: " << e.what() << " in onObserve" << endl;
    }
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

bool searchUser(OCAccountManager::Ptr accountMgr, QueryParamsMap query,
        GetCallback cloudConnectHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "searchUser IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->searchUser(query, cloudConnectHandler);
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
        const std::string& accessToken, DeleteCallback cloudConnectHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "deleteDevice IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->deleteDevice(deviceId, accessToken, cloudConnectHandler);
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
        ResourceHandles resourceHandles, PublishResourceCallback callback)
{
    IOTIVITYTEST_LOG(DEBUG, "publishResourceToRD IN");
    //  ResourceHandles resourceHandles; //need to work in this resource handler
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = RDClient::Instance().publishResourceToRD(host, connectivityType,
            resourceHandles, callback);
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

bool createGroup(OCAccountManager::Ptr accountMgr, QueryParamsMap query,
        PostCallback cloudConnectHandler)
{
    OCStackResult result;
    IOTIVITYTEST_LOG(DEBUG, "createGroup IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    if (query.empty())
    {
        result = accountMgr->createGroup(cloudConnectHandler);
    }
    else
    {
        result = accountMgr->createGroup(query, cloudConnectHandler);
    }
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] createGroup returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

//    strGroupID = g_groupID;

    IOTIVITYTEST_LOG(DEBUG, "createGroup OUT");
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
    OCStackResult result;
    IOTIVITYTEST_LOG(DEBUG, "getGroupInfo IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    if (groupId.empty())
    {
        result = accountMgr->getGroupInfoAll(cloudConnectHandler);
    }
    else
    {
        result = accountMgr->getGroupInfo(groupId, cloudConnectHandler);
    }

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

bool turnOnOffSwitch(bool bolean)
{
    IOTIVITYTEST_LOG(DEBUG, "turnOnOffSwitch IN");
    if (g_foundFoundResource == nullptr)
    {
        cout << "Binary switch not found" << endl;
        return false;
    }
    OCRepresentation binarySwitch;
    binarySwitch.setValue("value", bolean);
    QueryParamsMap query;
    g_foundFoundResource->post("oic.r.switch.binary", DEFAULT_INTERFACE, binarySwitch, query,
            &onPost);
}

bool observeGroup(OCAccountManager::Ptr accountMgr, ObserveCallback cloudConnectHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "observeGroup IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->observeGroup(cloudConnectHandler);
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

bool cancelObserveGroup(OCAccountManager::Ptr accountMgr)
{
    IOTIVITYTEST_LOG(DEBUG, "cancelObserveGroup IN");

    OCStackResult result = accountMgr->cancelObserveGroup();
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] cancelObserveGroup returns %d", result);
    if (OC_STACK_OK != result)
    {
        return false;
    }
    IOTIVITYTEST_LOG(DEBUG, "cancelObserveGroup OUT");
    return true;
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
    if (OC_STACK_OK != result)
    {
        return false;
    }
    return true;

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

bool replyToInvitation(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        const std::string& accept, DeleteCallback cloudConnectHandler)
{
    OCStackResult result;
    IOTIVITYTEST_LOG(DEBUG, "deleteInvitation IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    if (accept == "1")
    {
        result = accountMgr->replyToInvitation(groupId, true, cloudConnectHandler);
    }
    else
    {
        result = accountMgr->replyToInvitation(groupId, false, cloudConnectHandler);
    }
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

bool addPropertyValueToGroup(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        const OCRepresentation propertyValue, PostCallback cloudConnectHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "addPropertyValueToGroup IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->addPropertyValueToGroup(groupId, propertyValue,
            cloudConnectHandler);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] addPropertyValueToGroup returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }
    IOTIVITYTEST_LOG(DEBUG, "addPropertyValueToGroup OUT");
    return true;
}

bool deletePropertyValueFromGroup(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        const OCRepresentation propertyValue, PostCallback cloudConnectHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "deletePropertyValueFromGroup IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->deletePropertyValueFromGroup(groupId, propertyValue,
            cloudConnectHandler);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] deletePropertyValueFromGroup returns %d",
            result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }
    IOTIVITYTEST_LOG(DEBUG, "deletePropertyValueFromGroup OUT");
    return true;
}

bool updatePropertyValueOnGroup(OCAccountManager::Ptr accountMgr, const std::string& groupId,
        const OCRepresentation propertyValue, PostCallback cloudConnectHandler)
{
    IOTIVITYTEST_LOG(DEBUG, "deletePropertyValueFromGroup IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->deletePropertyValueFromGroup(groupId, propertyValue,
            cloudConnectHandler);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] deletePropertyValueFromGroup returns %d",
            result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }
    IOTIVITYTEST_LOG(DEBUG, "deletePropertyValueFromGroup OUT");
    return true;
}

bool discoveryMQTopics(OC::OCResource::Ptr g_mqBrokerResource,
        const QueryParamsMap& queryParametersMap, MQTopicCallback attributeHandler,
        QualityOfService qos)
{
    QueryParamsMap query;
    IOTIVITYTEST_LOG(DEBUG, "discoveryMQTopics IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = g_mqBrokerResource->discoveryMQTopics(query, &discoverTopicCB,
            QualityOfService::LowQos);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] discoveryMQTopics returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }
    IOTIVITYTEST_LOG(DEBUG, "discoveryMQTopics OUT");
    return true;
}

bool publishMQTopic(OC::OCResource::Ptr g_mqBrokerResource, const OCRepresentation& rep,
        const QueryParamsMap& queryParametersMap, PostCallback attributeHandler,
        QualityOfService qos)
{
    IOTIVITYTEST_LOG(DEBUG, "publishMQTopic IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = g_mqBrokerResource->publishMQTopic(rep, queryParametersMap,
            attributeHandler, qos);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] publishMQTopic returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }
    IOTIVITYTEST_LOG(DEBUG, "publishMQTopic OUT");
    return true;
}

bool createMQTopic(OC::OCResource::Ptr g_mqBrokerResource, const OCRepresentation& rep,
        const std::string& topicUri, const QueryParamsMap& queryParametersMap,
        MQTopicCallback attributeHandler, QualityOfService qos)
{
    IOTIVITYTEST_LOG(DEBUG, "createMQTopic IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = g_mqBrokerResource->createMQTopic(rep, topicUri, queryParametersMap,
            attributeHandler, qos);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] createMQTopic returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }
    IOTIVITYTEST_LOG(DEBUG, "createMQTopic OUT");
    return true;
}

bool subscribeMQTopic(OC::OCResource::Ptr g_mqBrokerResource, ObserveType observeType,
        const QueryParamsMap& queryParametersMap, ObserveCallback observeHandler,
        QualityOfService qos)
{
    IOTIVITYTEST_LOG(DEBUG, "subscribeMQTopic IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = g_mqBrokerResource->subscribeMQTopic(observeType, queryParametersMap,
            observeHandler, qos);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] subscribeMQTopic returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }
    IOTIVITYTEST_LOG(DEBUG, "subscribeMQTopic OUT");
    return true;
}

bool unsubscribeMQTopic(OC::OCResource::Ptr g_mqBrokerResource, QualityOfService qos)
{
    IOTIVITYTEST_LOG(DEBUG, "unsubscribeMQTopic IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = g_mqBrokerResource->unsubscribeMQTopic(qos);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] unsubscribeMQTopic returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }
    IOTIVITYTEST_LOG(DEBUG, "unsubscribeMQTopic OUT");
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

void onGet(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    if (eCode == 0)
    {
        IOTIVITYTEST_LOG(DEBUG, "Response: GET request was successful");

        vector< string > interfacelist = rep.getResourceInterfaces();

        bool isCollection = false;
        for (auto interface = interfacelist.begin(); interface != interfacelist.end(); interface++)
        {
            if (((*interface).compare(GROUP_INTERFACE) == 0)
                    || ((*interface).compare(BATCH_INTERFACE) == 0))
            {
                isCollection = true;
                break;
            }
        }

        IOTIVITYTEST_LOG(DEBUG, "The GET Response has the following representation:");
        printRepresentation(rep);

    }
    else
    {
        IOTIVITYTEST_LOG(ERROR, "onGET Response error: %d", eCode);
    }
}

// callback handler on PUT request
void onPut(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    if (eCode == 0 || eCode == OC_STACK_OK || eCode == OC_STACK_RESOURCE_CREATED)
    {
        IOTIVITYTEST_LOG(DEBUG, "Response: PUT request was successful");
        IOTIVITYTEST_LOG(DEBUG, "The PUT response has the following representation:");
        printRepresentation(rep);
    }
    else
    {
        IOTIVITYTEST_LOG(ERROR, "onPUT Response error: %d", eCode);
    }
}

// callback handler on POST request
void onPost(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    if (eCode == 0 || eCode == OC_STACK_OK)
    {
        IOTIVITYTEST_LOG(DEBUG, "Response: POST request was successful");
        IOTIVITYTEST_LOG(DEBUG, "The POST Response has the following representation:");
        printRepresentation(rep);
    }
    else
    {
        IOTIVITYTEST_LOG(ERROR, "onPOST Response error: %d", eCode);
    }

}

// callback handler on DELETE request
void onDelete(const HeaderOptions &headerOptions, const int eCode)
{
    if (eCode == 0)
    {
        IOTIVITYTEST_LOG(DEBUG, "Response: DELETE request was successful");
    }
    else
    {
        IOTIVITYTEST_LOG(ERROR, "onDELETE Response error: ", eCode);
    }
}

void onObserve(const HeaderOptions headerOptions, const OCRepresentation &rep, const int &eCode,
        const int &sequenceNumber)
{
    try
    {
        if (eCode == 0 || eCode == OC_STACK_OK)
        {
            IOTIVITYTEST_LOG(DEBUG, "OBSERVE RESULT:");
            IOTIVITYTEST_LOG(DEBUG, "\tSequenceNumber: %d", sequenceNumber);
            printRepresentation(rep);
        }
        else
        {
            IOTIVITYTEST_LOG(ERROR, "Observe Response/Notification Error: ", eCode);
        }
    }
    catch (exception &e)
    {
        IOTIVITYTEST_LOG(DEBUG, "Exception: %s in onObserve", e.what());
    }

}

std::shared_ptr< OC::OCResource > getFoundResource()
{
    return g_foundFoundResource;

}
