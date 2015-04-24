//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

#ifndef _IN_PROC_CLIENT_WRAPPER_H_
#define _IN_PROC_CLIENT_WRAPPER_H_

#include <thread>
#include <mutex>
#include <sstream>
#include <iostream>

#include <OCApi.h>
#include <ocstack.h>
#include <IClientWrapper.h>
#include <InitializeException.h>
#include <ResourceInitException.h>

namespace OC
{
    namespace ClientCallbackContext
    {
        struct GetContext
        {
            GetCallback callback;
        };

        struct SetContext
        {
            PutCallback callback;
        };

        struct ListenContext
        {
            FindCallback callback;
            std::weak_ptr<IClientWrapper> clientWrapper;
        };

        struct DeviceListenContext
        {
            FindDeviceCallback callback;
            IClientWrapper::Ptr clientWrapper;
        };

        struct SubscribePresenceContext
        {
            SubscribeCallback callback;
        };

        struct DeleteContext
        {
            DeleteCallback callback;
        };

        struct ObserveContext
        {
            ObserveCallback callback;
        };
    }

    class InProcClientWrapper : public IClientWrapper
    {

    public:

        InProcClientWrapper(std::weak_ptr<std::recursive_mutex> csdkLock,
                            PlatformConfig cfg);
        virtual ~InProcClientWrapper();

        virtual OCStackResult ListenForResource(const std::string& serviceUrl,
            const std::string& resourceType, OCConnectivityType connectivityType,
            FindCallback& callback, QualityOfService QoS);

        virtual OCStackResult ListenForDevice(const std::string& serviceUrl,
            const std::string& deviceURI, OCConnectivityType connectivityType,
            FindDeviceCallback& callback, QualityOfService QoS);

        virtual OCStackResult GetResourceRepresentation(const std::string& host,
            const std::string& uri, OCConnectivityType connectivityType,
            const QueryParamsMap& queryParams, const HeaderOptions& headerOptions,
            GetCallback& callback, QualityOfService QoS);

        virtual OCStackResult PutResourceRepresentation(const std::string& host,
            const std::string& uri, OCConnectivityType connectivityType,
            const OCRepresentation& attributes, const QueryParamsMap& queryParams,
            const HeaderOptions& headerOptions, PutCallback& callback, QualityOfService QoS);

        virtual OCStackResult PostResourceRepresentation(const std::string& host,
            const std::string& uri, OCConnectivityType connectivityType,
            const OCRepresentation& attributes, const QueryParamsMap& queryParams,
            const HeaderOptions& headerOptions, PostCallback& callback, QualityOfService QoS);

        virtual OCStackResult DeleteResource(const std::string& host, const std::string& uri,
            OCConnectivityType connectivityType, const HeaderOptions& headerOptions,
            DeleteCallback& callback, QualityOfService QoS);

        virtual OCStackResult ObserveResource(ObserveType observeType, OCDoHandle* handle,
            const std::string& host, const std::string& uri, OCConnectivityType connectivityType,
            const QueryParamsMap& queryParams, const HeaderOptions& headerOptions,
            ObserveCallback& callback, QualityOfService QoS);

        virtual OCStackResult CancelObserveResource(OCDoHandle handle, const std::string& host,
            const std::string& uri, const HeaderOptions& headerOptions, QualityOfService QoS);

        virtual OCStackResult SubscribePresence(OCDoHandle* handle, const std::string& host,
            const std::string& resourceType, OCConnectivityType connectivityType,
            SubscribeCallback& presenceHandler);

        virtual OCStackResult UnsubscribePresence(OCDoHandle handle);
        OCStackResult GetDefaultQos(QualityOfService& QoS);
    private:
        void listeningFunc();
        std::string assembleSetResourceUri(std::string uri, const QueryParamsMap& queryParams);
        std::string assembleSetResourcePayload(const OCRepresentation& attributes);
        OCHeaderOption* assembleHeaderOptions(OCHeaderOption options[],
           const HeaderOptions& headerOptions);
        std::thread m_listeningThread;
        bool m_threadRun;
        std::weak_ptr<std::recursive_mutex> m_csdkLock;

    private:
        PlatformConfig  m_cfg;
    };
}

#endif

