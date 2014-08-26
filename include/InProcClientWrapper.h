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

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <OCApi.h>
#include <ocstack.h>
#include <IClientWrapper.h>
#include <InitializeException.h>
#include <ResourceInitException.h>

namespace OC
{
    class InProcClientWrapper : public IClientWrapper
    {
    public:
        InProcClientWrapper(std::weak_ptr<std::mutex> csdkLock, PlatformConfig cfg);
        virtual ~InProcClientWrapper();

        virtual OCStackResult ListenForResource(const std::string& serviceUrl, const std::string& resourceType, 
            std::function<void(std::shared_ptr<OCResource>)>& callback);
        
        virtual OCStackResult GetResourceAttributes(const std::string& host, const std::string& uri, const QueryParamsMap& queryParams, 
            std::function<void(const OCRepresentation, const int)>& callback);
        
        virtual OCStackResult SetResourceAttributes(const std::string& host, const std::string& uri, const OCRepresentation& attributes, 
            const QueryParamsMap& queryParams, std::function<void(const OCRepresentation, const int)>& callback);
        
        virtual OCStackResult ObserveResource(ObserveType observeType, OCDoHandle* handle, 
            const std::string& host, const std::string& uri, const QueryParamsMap& queryParams, 
            std::function<void(const OCRepresentation&, const int&, const int&)>& callback);
        
        virtual OCStackResult CancelObserveResource(OCDoHandle handle, const std::string& host, const std::string& uri);
        
        // Note: this should never be called by anyone but the handler for the listen command.  It is public becuase that needs to be a non-instance callback
        virtual std::shared_ptr<OCResource> parseOCResource(IClientWrapper::Ptr clientWrapper, const std::string& host, const boost::property_tree::ptree resourceNode);
    private:
        void listeningFunc();
        std::string assembleSetResourceUri(std::string uri, const QueryParamsMap& queryParams);
        std::string assembleSetResourcePayload(const OCRepresentation& attributes);
        std::thread m_listeningThread;
        bool m_threadRun;
        std::weak_ptr<std::mutex> m_csdkLock;
        std::vector<std::function<void(OCClientResponse*)>> callbackList;

    private:
        PlatformConfig m_cfg;
    };
}

#endif
