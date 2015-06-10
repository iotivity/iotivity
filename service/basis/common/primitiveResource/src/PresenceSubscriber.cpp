//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include <PresenceSubscriber.h>

#include <PrimitiveException.h>

#include <OCPlatform.h>

void subscribePresence(OCDoHandle& handle, const std::string& host,
        OCConnectivityType connectivityType, SubscribeCallback presenceHandler)
{
    OCStackResult result = OC::OCPlatform::subscribePresence(handle, host, connectivityType, presenceHandler);

    if (result != OC_STACK_OK)
    {
        throw PlatformException(result);
    }
}

void subscribePresence(OCDoHandle& handle, const std::string& host,
        const std::string& resourceType, OCConnectivityType connectivityType,
        SubscribeCallback presenceHandler)
{
    OCStackResult result = OC::OCPlatform::subscribePresence(handle, host, resourceType,
            connectivityType, presenceHandler);

    if (result != OC_STACK_OK)
    {
        throw PlatformException(result);
    }
}

void unsubscribePresence(OCDoHandle handle)
{
    OCStackResult result = OC::OCPlatform::unsubscribePresence(handle);

    if (result != OC_STACK_OK)
    {
        throw PlatformException(result);
    }
}


PresenceSubscriber::PresenceSubscriber() :
    m_handle{ nullptr }
{
}

PresenceSubscriber::PresenceSubscriber(const std::string& host,
        OCConnectivityType connectivityType, SubscribeCallback presenceHandler) :
        m_handle{ nullptr }
{
    subscribePresence(m_handle, host, connectivityType, presenceHandler);
}

PresenceSubscriber::PresenceSubscriber(const std::string& host, const std::string& resourceType,
        OCConnectivityType connectivityType, SubscribeCallback presenceHandler) :
        m_handle{ nullptr }
{
    subscribePresence(m_handle, host, resourceType, connectivityType, presenceHandler);
}

PresenceSubscriber::~PresenceSubscriber()
{
    if (m_handle)
    {
        try
        {
            unsubscribe();
        }
        catch (...)
        {
        }
    }
}

void PresenceSubscriber::unsubscribe()
{
    if (m_handle == nullptr) return;

    unsubscribePresence(m_handle);

    m_handle = nullptr;
}

bool PresenceSubscriber::isSubscribing() const
{
    return m_handle != nullptr;
}
