/******************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
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

#include "NSCppHelper.h"

NSCppHelper* NSCppHelper::s_pNSHelperInstance = NULL;
std::mutex NSCppHelper::s_mutex;
string NSCppHelper::s_providerID = "";
string NSCppHelper::s_consumerID = "";
shared_ptr< OIC::Service::NSProvider > NSCppHelper::s_provider;
shared_ptr< OIC::Service::NSConsumer > NSCppHelper::s_consumer;
OIC::Service::NSMessage NSCppHelper::s_message;
OIC::Service::NSSyncInfo NSCppHelper::s_providerSyncInfo;
OIC::Service::NSSyncInfo NSCppHelper::s_consumerSyncInfo;
OIC::Service::NSProviderState NSCppHelper::s_providerState;

void NSCppHelper::waitForConsumer(int timeout)
{
    int waitTime = 0;
    while (waitTime < timeout)
    {
        if (s_consumerID == "")
        {
            CommonUtil::waitInSecond(WAIT_TIME_MIN);
            waitTime++;
        }
        else
        {
            break;
        }
    }
}

void NSCppHelper::syncCallback(OIC::Service::NSSyncInfo sync)
{
    IOTIVITYTEST_LOG(INFO, "NSProviderSyncInfoCallback() called !!");
    s_providerSyncInfo = sync;
    NSCppUtility::printSyncInfo(s_providerSyncInfo);
}

void NSCppHelper::syncSyncronizationCallback(OIC::Service::NSSyncInfo sync)
{
    IOTIVITYTEST_LOG(INFO, "NSConsumerSyncInfoCallback() called !!");
    s_consumerSyncInfo = sync;
    NSCppUtility::printSyncInfo(s_consumerSyncInfo);
}

void NSCppHelper::subscribeRequestCallback(shared_ptr< OIC::Service::NSConsumer > consumer)
{
    IOTIVITYTEST_LOG(INFO, "subscribeRequestCallback() called !!");

    s_consumerID = consumer->getConsumerId();
    s_consumer = consumer;
    IOTIVITYTEST_LOG(INFO, "Consumer ID: %s", s_consumerID.c_str());
}

void NSCppHelper::onProviderDiscovered(shared_ptr< OIC::Service::NSProvider > provider)
{
    IOTIVITYTEST_LOG(INFO, "onProviderDiscovered called");

    s_providerID = provider->getProviderId();
    s_provider = provider;
    s_provider->setListener(onProviderStateReceived, onMessageReceived, syncSyncronizationCallback);
    IOTIVITYTEST_LOG(INFO, "Provider ID: %s", s_providerID.c_str());
}

void NSCppHelper::onConsumerDiscovered(shared_ptr< OIC::Service::NSConsumer > consumer)
{
    IOTIVITYTEST_LOG(INFO, "onProviderDiscovered called");

    s_providerID = consumer->getConsumerId();
    s_consumer = consumer;
    IOTIVITYTEST_LOG(INFO, "Provider ID: %s", s_providerID.c_str());
}

void NSCppHelper::onProviderStateReceived(OIC::Service::NSProviderState state)
{
    IOTIVITYTEST_LOG(INFO, "onProviderStateReceived Listener Called...");

    s_providerState = state;
}

void NSCppHelper::onMessageReceived(OIC::Service::NSMessage message)
{
    IOTIVITYTEST_LOG(INFO, "onMessageReceived Listener Called...");

    s_message = message;

    NSCppUtility::printMessage(s_message);
}

NSCppHelper::NSCppHelper()
{
    OC::PlatformConfig cfg
    { OC::ServiceType::InProc, OC::ModeType::Both, CT_DEFAULT, CT_DEFAULT,
            OC::QualityOfService::HighQos };
    OC::OCPlatform::Configure(cfg);
    IOTIVITYTEST_LOG(INFO, "Platform Configuration Done!!");

    try
    {
        IOTIVITYTEST_LOG(INFO, "%s 2" , __func__);
        OC::OCPlatform::stopPresence();
        IOTIVITYTEST_LOG(INFO, "Successfully stopPresense() called !!");
    }
    catch (exception &e)
    {
        IOTIVITYTEST_LOG(WARNING, "Can't stop presense..!!!");
    }

    preConfigure();

}

NSCppHelper* NSCppHelper::getInstance()
{
    if (s_pNSHelperInstance == NULL)
    {
        s_mutex.lock();
        if (s_pNSHelperInstance == NULL)
        {
            s_pNSHelperInstance = new NSCppHelper();
        }
        s_mutex.unlock();
    }

    return s_pNSHelperInstance;
}

void NSCppHelper::preConfigure()
{
    m_pProviderService = nullptr;
    m_pConsumerService = nullptr;
    m_isConsumerStarted = false;
    m_isProviderStarted = false;
    s_providerState = OIC::Service::NSProviderState::STOPPED;

}

shared_ptr< OIC::Service::NSConsumer > NSCppHelper::getConsumer(bool subcontrollability)
{
    CommonUtil::launchApp(CONSUMER_SIMULATOR_OPTION, false);
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    m_pProviderService = NSProviderService::getInstance();

    NSProviderService::ProviderConfig providerCfg;
    providerCfg.m_subscribeRequestCb = subscribeRequestCallback;
    providerCfg.m_syncInfoCb = syncCallback;

    if (subcontrollability == true)
    {
        providerCfg.subControllability = true;
    }
    else
    {
        providerCfg.subControllability = false;
    }

    m_pProviderService->start(providerCfg);
    waitForConsumer(WAIT_TIME_MAX);

    shared_ptr< OIC::Service::NSConsumer > nsConsumer = m_pProviderService->getConsumer(
            s_consumerID);

    return nsConsumer;
}

shared_ptr< OIC::Service::NSProvider > NSCppHelper::getProvider(bool subcontrollability)
{
    if (subcontrollability == true)
    {
        CommonUtil::launchApp(PROVIDER_SIMULATOR_ALLOW, false);
    }
    else
    {
        CommonUtil::launchApp(PROVIDER_SIMULATOR_NOT_ALLOW, false);
    }
    CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

    OIC::Service::NSConsumerService* nsConsumerService = NSConsumerService::getInstance();
    nsConsumerService->start(onProviderDiscovered);
    waitForProvider(WAIT_TIME_MAX);

    shared_ptr< OIC::Service::NSProvider > nsProvider = nsConsumerService->getProvider(
            s_providerID);

    return nsProvider;
}

void NSCppHelper::waitForProvider(int timeout)
{
    int waitTime = 0;
    while (waitTime < timeout)
    {
        if (s_providerID == "")
        {
            CommonUtil::waitInSecond(WAIT_TIME_MIN);
            waitTime++;
        }
        else
        {
            break;
        }
    }
}

shared_ptr< OIC::Service::NSProvider > NSCppHelper::getLocalProvider(bool subControllability)
{

    m_pProviderService = OIC::Service::NSProviderService::getInstance();
    m_pConsumerService = OIC::Service::NSConsumerService::getInstance();

    try
    {
        OIC::Service::NSProviderService::ProviderConfig providerConfig;
        providerConfig.m_subscribeRequestCb = subscribeRequestCallback;
        providerConfig.m_syncInfoCb = syncCallback;

        if (subControllability == true)
        {
            providerConfig.subControllability = true;
        }
        else
        {
            providerConfig.subControllability = false;
        }

        m_pProviderService->start(providerConfig);
        m_pProviderService->registerTopic(TEST_TOPIC_1);
        m_isProviderStarted = true;
        CommonUtil::waitInSecond(WAIT_TIME_MIN);

        m_pConsumerService->start(onProviderDiscovered);
        m_isConsumerStarted = true;
        m_pConsumerService->rescanProvider();
    }
    catch (NSException &e)
    {
        IOTIVITYTEST_LOG(ERROR, "Can't Start. Exception is %s", e.what());
    }

    waitForService(WAIT_TIME_MAX, true);

    return s_provider;
}

bool NSCppHelper::waitForService(int timeOut, bool isProvider)
{
    int waitTime = 0;

    while (waitTime < timeOut)
    {
        if (isProvider)
            IOTIVITYTEST_LOG(INFO, "Waiting for Provider %d...", waitTime);
        else
            IOTIVITYTEST_LOG(INFO, "Waiting for Consumer %d...", waitTime);

        if (isProvider)
        {
            if (s_provider->getProviderId() != "")
            {
                IOTIVITYTEST_LOG(INFO, "Provider found with ID: %s", s_provider->getProviderId().c_str());
                return true;
            }
        }
        else
        {
            if (s_consumer->getConsumerId() != "")
            {
                IOTIVITYTEST_LOG(INFO, "Consumer found with ID: %s", s_consumer->getConsumerId().c_str());
                return true;
            }
        }

        CommonUtil::waitInSecond(WAIT_TIME_MIN);
        waitTime++;
    }

    return false;
}

shared_ptr< OIC::Service::NSConsumer > NSCppHelper::getLocalConsumer(bool subControllability,
        bool isTopicRegister)
{

    stopServices();
    m_pProviderService = getProviderService();
    m_pConsumerService = getConsumerService();

    try
    {
        OIC::Service::NSProviderService::ProviderConfig providerConfig;
        providerConfig.m_subscribeRequestCb = subscribeRequestCallback;
        providerConfig.m_syncInfoCb = syncCallback;
        providerConfig.subControllability = subControllability;

        m_pProviderService->start(providerConfig);
        m_pProviderService->registerTopic(TEST_TOPIC_1);
        m_isProviderStarted = true;
        CommonUtil::waitInSecond(WAIT_TIME_MIN);

        m_pConsumerService->start(onProviderDiscovered);
        m_isConsumerStarted = true;
        IOTIVITYTEST_LOG(INFO, "provider service started");

        if (isTopicRegister)
        {
            m_pProviderService->registerTopic(TEST_TOPIC_1);
        }
        IOTIVITYTEST_LOG(INFO, "topic registered");

        CommonUtil::waitInSecond(WAIT_TIME_DEFAULT);

        m_pConsumerService->rescanProvider();
        CommonUtil::waitInSecond(WAIT_TIME_MIN + WAIT_TIME_MIN);
    }
    catch (NSException &e)
    {
        IOTIVITYTEST_LOG(ERROR, "Can't Start Consumer Service");
        return nullptr;
    }

    if (subControllability)
    {
        bool isFound = waitForService(WAIT_TIME_MAX, false);
        IOTIVITYTEST_LOG(INFO, "waiting for provider with provider control");

        if (!isFound)
        {
            IOTIVITYTEST_LOG(WARNING, "Can't find provider");
        }
    }
    else
    {
        bool isFound = waitForService(WAIT_TIME_MAX, true);
        IOTIVITYTEST_LOG(INFO, "waiting for provider with consumer control");
        if (isFound)
        {
            try
            {
                s_provider->subscribe();
            }
            catch (NSException &e)
            {
                IOTIVITYTEST_LOG(ERROR, "Can't Subscribe to provider");
            }
        }
        else
        {
            IOTIVITYTEST_LOG(WARNING, "Can't find provider");
        }

        isFound = waitForService(WAIT_TIME_MAX, false);

        if (isFound)
        {
            IOTIVITYTEST_LOG(INFO, "Consumer Found Successfully");
        }
        else
        {
            IOTIVITYTEST_LOG(WARNING, "Can't find any Consumer");
        }
    }

    return s_consumer;
}

bool NSCppHelper::sendNotification()
{
    if (m_pProviderService == nullptr || !s_consumer)
    {
        IOTIVITYTEST_LOG(ERROR, "ProviderService or Consumer is null");
        return false;
    }

    try
    {
        m_pProviderService->registerTopic(TEST_TOPIC_1);
        s_consumer->setTopic(TEST_TOPIC_1);
    }
    catch (NSException &e)
    {
        IOTIVITYTEST_LOG(ERROR, "Exception occurred while registering Topic, reason: %s ",
                e.what());
        return false;
    }

    OIC::Service::NSMessage msg;

    try
    {
        msg = m_pProviderService->createMessage();
        msg.setTitle(TITLE_TO_SET);
        msg.setContentText(CONTENTTEXT_TO_SET);
        msg.setSourceName(SOURCE_NAME);
        msg.setTopic(TEST_TOPIC_1);

        OIC::Service::NSMediaContents* mediaContents = new OIC::Service::NSMediaContents(
                ICONIMAGE_TO_SET);

        msg.setMediaContents(mediaContents);
    }
    catch (NSException &e)
    {
        IOTIVITYTEST_LOG(ERROR, "Exception occurred while creating message, reason: %s ", e.what());
        return false;
    }

    try
    {
        OIC::Service::NSResult result = m_pProviderService->sendMessage(msg);

        if (result != OIC::Service::NSResult::OK)
        {
            IOTIVITYTEST_LOG(INFO, "SendMessage is failed.");
            return false;
        }
    }
    catch (NSException &e)
    {
        IOTIVITYTEST_LOG(ERROR, "Exception occurred while sending message, reason: %s ", e.what());
        return false;
    }

    IOTIVITYTEST_LOG(INFO, "Notification Sent");

    return true;
}

OIC::Service::NSMessage NSCppHelper::getNotification()
{
    return s_message;
}

OIC::Service::NSProviderService* NSCppHelper::getProviderService()
{
    if (m_pProviderService == nullptr)
    {
        m_pProviderService = OIC::Service::NSProviderService::getInstance();
    }
    return m_pProviderService;
}

OIC::Service::NSConsumerService* NSCppHelper::getConsumerService()
{
    if (m_pConsumerService == nullptr)
    {
        m_pConsumerService = OIC::Service::NSConsumerService::getInstance();
    }
    return m_pConsumerService;
}

void NSCppHelper::setProviderStartStatus(bool isStarted)
{
    m_isProviderStarted = isStarted;
}

void NSCppHelper::setConsumerStartStatus(bool isStarted)
{
    m_isConsumerStarted = isStarted;
}

void NSCppHelper::stopServices()
{

    if (m_isConsumerStarted && (m_pConsumerService != nullptr))
    {
        try
        {
            m_pConsumerService->stop();
            m_isConsumerStarted = false;
            IOTIVITYTEST_LOG(INFO, "Consumer Service Stopped Successfully");
        }
        catch (NSException &e)
        {
            IOTIVITYTEST_LOG(ERROR, "Failed to stop Consumer Service %s", e.what());
        }

        CommonUtil::waitInSecond(WAIT_TIME_MIN + WAIT_TIME_MIN);
    }
    m_pConsumerService = nullptr;

    if (m_isProviderStarted && (m_pProviderService != nullptr))
    {
        try
        {
            if (OIC::Service::NSResult::OK == m_pProviderService->stop())
            {
                m_isProviderStarted = false;
                IOTIVITYTEST_LOG(INFO, "Provider Service Stopped Successfully");
            }

        }
        catch (NSException &e)
        {
            IOTIVITYTEST_LOG(ERROR, "Failed to stop Consumer Service %s", e.what());
        }

        CommonUtil::waitInSecond(WAIT_TIME_MIN);
    }
    m_pProviderService = nullptr;
}

OIC::Service::NSSyncInfo NSCppHelper::getSyncInfo(bool isProvider)
{
    if (isProvider)
        return s_providerSyncInfo;
    else
        return s_consumerSyncInfo;
}

OIC::Service::NSProviderState NSCppHelper::getProviderState()
{
    return s_providerState;
}

bool NSCppHelper::sendSyncInfo(bool isProvider, OIC::Service::NSSyncInfo::NSSyncType type)
{
    if (s_message.getProviderId().size() == 0)
    {
        IOTIVITYTEST_LOG(ERROR, "Notification is null");
        return false;
    }

    if (isProvider)
    {
        if (m_pProviderService == nullptr)
        {
            IOTIVITYTEST_LOG(ERROR, "Provider service is null");
            return false;
        }
    }
    else
    {
        if (!s_provider)
        {
            IOTIVITYTEST_LOG(ERROR, "Provider is null");
            return false;
        }
    }

    try
    {
        if (isProvider)
            m_pProviderService->sendSyncInfo(s_message.getMessageId(), type);
        else
            s_provider->sendSyncInfo(s_message.getMessageId(), type);
    }
    catch (NSException &e)
    {
        IOTIVITYTEST_LOG(ERROR, "FAiled t send Sync Info: %s", e.what());
        return false;
    }

    return true;
}

bool NSCppHelper::stopProviderService()
{
    if (!m_pProviderService)
    {
        return false;
    }

    try
    {
        OIC::Service::NSResult result = m_pProviderService->stop();

        if (result != OIC::Service::NSResult::OK)
        {
            IOTIVITYTEST_LOG(ERROR, "Can't stop provider service");
            return false;
        }
    }
    catch (OIC::Service::NSException &e)
    {
        IOTIVITYTEST_LOG(ERROR, "Exception Occured during stopping provider: %s", e.what());
        return false;
    }

    m_isProviderStarted = false;
    m_pProviderService = nullptr;
    return true;
}
