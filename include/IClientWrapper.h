//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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

#ifndef _I_CLIENT_WRAPPER_H_
#define _I_CLIENT_WRAPPER_H_

#include <memory>
#include <string>

#include <OCApi.h>
namespace OC
{
    class IClientWrapper : public std::enable_shared_from_this<IClientWrapper>
    {
    public:
        typedef std::shared_ptr<IClientWrapper> Ptr;

        virtual OCStackResult ListenForResource(const std::string& serviceUrl, const std::string& resourceType,
            std::function<void(std::shared_ptr<OCResource>)>& callback) = 0;
        
        virtual OCStackResult GetResourceAttributes(const std::string& host, const std::string& uri, const QueryParamsMap& queryParams, 
            std::function<void(const AttributeMap, const int)>& callback)=0;
        
        virtual OCStackResult SetResourceAttributes(const std::string& host, const std::string& uri, const AttributeMap& attributes, 
            const QueryParamsMap& queryParams, std::function<void(const AttributeMap,const int)>& callback)=0;

        virtual OCStackResult ObserveResource(ObserveType observeType, OCDoHandle* handle, 
            const std::string& host, const std::string& uri, const QueryParamsMap& queryParams, 
            std::function<void(const AttributeMap&, const int&, const int&)>& callback)=0;
        
        virtual OCStackResult CancelObserveResource(OCDoHandle handle, const std::string& host, const std::string& uri)=0;
        
        virtual ~IClientWrapper(){}
       

        // Note: this should never be called by anyone but the handler for the listen command.  It is public becuase that needs to be a non-instance callback
        virtual std::shared_ptr<OCResource> parseOCResource(IClientWrapper::Ptr clientWrapper, const std::string& host, const boost::property_tree::ptree resourceNode)=0;
    private:
    };
}

#endif
