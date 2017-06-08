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
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#include "ICResourceHelper.h"

ICResourceHelper* ICResourceHelper::s_ICResourceHelper = NULL;
std::mutex ICResourceHelper::s_mutex;

vector<shared_ptr<OC::OCResource> > ICResourceHelper::s_TopicList;
bool ICResourceHelper::s_isCreateTopicSuccess = false;
bool ICResourceHelper::s_isDiscoverTopicSuccess = false;
bool ICResourceHelper::s_isPublishTopicSuccess = false;
bool ICResourceHelper::s_isrequestTopicPublishSuccess = false;
bool ICResourceHelper::s_isSubscribeSuccess = false;
bool ICResourceHelper::s_isUnsubscribeSuccess = false;

ICResourceHelper::ICResourceHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[ICResourceHelper] IN");
}

bool ICResourceHelper::initCloudClient()
{
    __FUNC_IN__

    static OCPersistentStorage pstStr;
    pstStr.read = fread;
    pstStr.write = fwrite;
    pstStr.close = fclose;
    pstStr.unlink = unlink;

    if (OC_STACK_OK != OCRegisterPersistentStorageHandler(&pstStr))
    {
        IOTIVITYTEST_LOG(ERROR, "[ICResourceHelper] OCRegisterPersistentStorageHandler error");
        return false;
    }

    // initialize OC stack and provisioning manager
    if (OC_STACK_OK != OCInit(NULL, 0, OC_CLIENT_SERVER))
    {
        IOTIVITYTEST_LOG(ERROR, "[ICResourceHelper] OCStack init error");
        return false;
    }
    __FUNC_OUT__
    return true;
}

ICResourceHelper* ICResourceHelper::getInstance(void)
{
    if (s_ICResourceHelper == NULL)
    {
        s_mutex.lock();

        s_ICResourceHelper = new ICResourceHelper();

        s_mutex.unlock();
    }
    return s_ICResourceHelper;
}

ICResourceHelper::~ICResourceHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[ICResourceHelper] OUT");
    if (s_ICResourceHelper != NULL)
    {
        delete s_ICResourceHelper;

        s_ICResourceHelper = NULL;
    }
}

void ICResourceHelper::createTopicCB(const int ecode, const string &originUri,
        shared_ptr< OC::OCResource > topic)
{
    IOTIVITYTEST_LOG(INFO, "Create topic response received, code: %d" , ecode);

    if (ecode == OCStackResult::OC_STACK_RESOURCE_CREATED)
    {
        ICResourceHelper::s_isCreateTopicSuccess = true;
        IOTIVITYTEST_LOG(INFO, "Created topic : %s" , topic->uri().c_str());
    }
    else
    {
        ICResourceHelper::s_isCreateTopicSuccess=false;
        IOTIVITYTEST_LOG(INFO, "Topic creation failed : %s" , originUri.c_str());
    }
}

void ICResourceHelper::discoverTopicCB(const int ecode, const string &brokerUri,
                     shared_ptr<OC::OCResource> topic)
{
    ICResourceHelper::s_isDiscoverTopicSuccess= true;
    IOTIVITYTEST_LOG(INFO, "Topic discovered code: %d" , ecode);
    ICResourceHelper::s_TopicList.push_back(topic);
    IOTIVITYTEST_LOG(INFO, "Topic [ %d ] %s discovered" , s_TopicList.size() - 1 , topic->uri().c_str());
}

void ICResourceHelper::publishMessageCB(const HeaderOptions &, const OCRepresentation &, const int eCode)
{
    ICResourceHelper::s_isPublishTopicSuccess=true;

}

void ICResourceHelper::requestMQPublishCB(const HeaderOptions&, const OCRepresentation&, const int)
{
    ICResourceHelper::s_isrequestTopicPublishSuccess=true;
}

void ICResourceHelper::subscribeCB(const HeaderOptions &,
                 const OCRepresentation &rep, const int eCode, const int sequenceNumber)
{
    try
    {
        if (eCode == OC_STACK_OK && sequenceNumber <= MAX_SEQUENCE_NUMBER)
        {
            ICResourceHelper::s_isSubscribeSuccess = true;
            if (sequenceNumber == OC_OBSERVE_REGISTER)
            {
                IOTIVITYTEST_LOG(INFO, "Observe registration action is successful");
            }

            IOTIVITYTEST_LOG(INFO, "OBSERVE RESULT:");
            ICHelper::icPrintRepresentation(rep);
        }
        else
        {
            if (eCode == OC_STACK_OK)
            {
                ICResourceHelper::s_isSubscribeSuccess = true;
                IOTIVITYTEST_LOG(INFO, "Observe registration failed or de-registration action failed/succeeded");
            }
            else
            {
                ICResourceHelper::s_isSubscribeSuccess = false;
                IOTIVITYTEST_LOG(INFO, "onObserve Response error: %d" , eCode);
                exit(-1);
            }
        }
    }
    catch (exception &e)
    {
        ICResourceHelper::s_isSubscribeSuccess = false;
        IOTIVITYTEST_LOG(INFO, "Exception: %d  in onObserve" , e.what());
    }
}

