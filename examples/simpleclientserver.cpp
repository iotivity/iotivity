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

///
/// This sample provides steps to define an interface for a resource
/// (properties and methods) and host this resource on the server.
/// Additionally, it'll have a client example to discover it as well.
///
#include <memory>
#include <iostream>
#include <condition_variable>
#include <map>
#include <vector>
#include "OCPlatform.h"
#include "OCApi.h"
using namespace OC;

class ClientWorker
{
private:
    void putResourceInfo(const AttributeMap requestedPut, const AttributeMap attrMap, const int eCode)
    {
       std::cout <<"Clientside Put response to get was: "<<std::endl;
       std::cout <<"ErrorCode: "<<eCode <<std::endl;

       if(eCode == 0)
       {
            std::cout<<"Successful Put.  Attributes sent were: "<<std::endl;

            for(const auto& attr : requestedPut)
            {
                std::cout << "\tName: "<< attr.first << " value: ";
                for(const auto& val : attr.second)
                {
                    std::cout << val << "  ";
                }

                std::cout << std::endl;
            }

            std::cout<<"Actual New values are: "<<std::endl;

            for(const auto& attr : attrMap)
            {
                std::cout << "\tName: "<< attr.first << " value: ";
                for(const auto& val : attr.second)
                {
                    std::cout << val << "  ";
                }

                std::cout << std::endl;
            }
            m_cv.notify_all();
       }
    }

    void getResourceInfo(const AttributeMap attrMap, const int eCode)
    {

        std::cout<<"Clientside response to get was: "<<std::endl;
        std::cout<<"Error Code: "<<eCode<<std::endl;

        if(eCode == 0)
        {
            std::cout <<"Successful Get.  Attributes are: "<<std::endl;

            for(const auto& attr : attrMap)
            {
                std::cout << "\tName: "<< attr.first << " value: ";
                for(const auto& val : attr.second)
                {
                    std::cout << val << "  ";
                }

                std::cout << std::endl;
            }

            std::cout << "Doing a put on q/foo" <<std::endl;
            AttributeMap attrMap2(attrMap);
            attrMap2["isFoo"][0] ="false";
            attrMap2["barCount"][0]="211";

            m_resource->put(attrMap2, QueryParamsMap(), std::function<void(const AttributeMap, const int)>(std::bind(&ClientWorker::putResourceInfo, this, attrMap2, std::placeholders::_1, std::placeholders::_2)));
        }
    }

    void foundResource(std::shared_ptr<OCResource> resource)
    {    
        if(resource && resource->uri() == "/q/foo")
        {
            {
                std::lock_guard<std::mutex> lock(m_resourceLock);
                if(m_resource)
                {
                    return;
                }

                m_resource = resource;
            }

            std::cout << "Found Resource: "<<std::endl;
            std::cout << "\tHost: "<< resource->host()<<std::endl;
            std::cout << "\tURI:  "<< resource->uri()<<std::endl;

            std::cout<<"Doing a get on q/foo."<<std::endl;
            resource->get(std::function<void(const AttributeMap, const int)>(std::bind(&ClientWorker::getResourceInfo, this, std::placeholders::_1, std::placeholders::_2)));
        } 
    }

public:
    void start(OCPlatform& platform)
    {
        std::cout<<"Starting Client find:"<<std::endl;
        std::function<void(std::shared_ptr<OCResource>)> f (std::bind(&ClientWorker::foundResource, this, std::placeholders::_1));
        std::cout<<"result:" << platform.findResource("", "coap://224.0.1.187/oc/core?rt=core.foo", f)<< std::endl;
        std::cout<<"Finding Resource..."<<std::endl;

        {
            std::unique_lock<std::mutex> lk(m_mutex);
            m_cv.wait(lk);
        }
    }
private:
    std::mutex m_mutex;
    std::mutex m_resourceLock;
    std::condition_variable m_cv;
    std::shared_ptr<OCResource> m_resource;
};

struct FooResource
{
    bool m_isFoo;
    int m_barCount;
    OCResourceHandle m_resourceHandle;

    FooResource(): m_isFoo(true), m_barCount (0){}

    bool createResource(OCPlatform& platform)
    {
        std::string resourceURI = "/q/foo";
        std::string resourceTypeName = "core.foo";
        std::string resourceInterface = DEFAULT_INTERFACE;

        uint8_t resourceProperty = OC_DISCOVERABLE;

        std::function<void(std::shared_ptr<OCResourceRequest>, std::shared_ptr<OCResourceResponse>)> eh(std::bind(&FooResource::entityHandler, this, std::placeholders::_1, std::placeholders::_2));
        OCStackResult result = platform.registerResource(m_resourceHandle, resourceURI, resourceTypeName,
                                    resourceInterface, 
                                    eh, resourceProperty);
        if(OC_STACK_OK != result)
        {
            std::cout<<"Resource creation unsuccessful"<<std::endl;
            return false;
        }

        return true;
    }

    void getRepresentation(AttributeMap& attributeMap)
    {
        AttributeValues isFooVal;
        isFooVal.push_back(m_isFoo ? "true" : "false");

        AttributeValues barCt;
        barCt.push_back(to_string(m_barCount));

        attributeMap["isFoo"] = isFooVal;
        attributeMap["barCount"] = barCt;
    }

    void setRepresentation(const AttributeMap& attributeMap)
    {
        auto fooVector = attributeMap.at("isFoo");
        m_isFoo = fooVector[0] == "true";
        auto barVector = attributeMap.at("barCount");
        m_barCount = std::stoi(barVector[0]);
    }

    void entityHandler(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response)
    {
        std::cout<<"\tConsumer Entity Handler:"<<std::endl;

        if(request)
        {
            // Note: Most of the handlers are not here, since this is for demoing client/server co-process existence.
            // See simpleserver for a more complete example.
            if(request->getRequestHandlerFlag()  == RequestHandlerFlag::RequestFlag)
            {
                std::cout << "\t\trequestFlag : Request"<<std::endl;

                if(request->getRequestType() == "GET")
                {
                    std::cout<<"\t\t\trequestType : GET"<<std::endl;

                    AttributeMap attrs;
                    getRepresentation(attrs);

                    if(response)
                    {
                        response->setErrorCode(200);
                        response->setResourceRepresentation(attrs);
                    }
                }
                else if (request->getRequestType() == "PUT")
                {
                    std::cout<<"\t\t\trequestType : PUT"<<std::endl;

                    setRepresentation(request->getResourceRepresentation());

                    if(response)
                    {
                        response->setErrorCode(200);
                        AttributeMap attrs;
                        getRepresentation(attrs);
                        response->setResourceRepresentation(attrs);
                    }
                }
                else
                {
                    std::cout<<"\t\t\trequestType : UNSUPPORTED: "<<request->getRequestType()<<std::endl;
                }
            }
            else
            {
                std::cout <<"\t\trequestFlag : UNSUPPORTED: ";
                
                if(request->getRequestHandlerFlag()==RequestHandlerFlag::InitFlag)
                {
                    std::cout<<"InitFlag"<<std::endl;
                }
                else if(request->getRequestHandlerFlag()== RequestHandlerFlag::ObserverFlag)
                {
                    std::cout<<"ObserverFlag"<<std::endl;
                }
            }
        }
        else
        {
            std::cout << "Request Invalid!"<<std::endl;
        }
    }

};
int main()
{
    PlatformConfig cfg;
    cfg.ipAddress = "134.134.161.33";
    cfg.port = 56833;
    cfg.mode = ModeType::Both;
    cfg.serviceType = ServiceType::InProc;

    FooResource fooRes;

    try
    {
        OCPlatform platform(cfg);
        
        if(!fooRes.createResource(platform))
        {
            return -1;
        }

        ClientWorker cw;
        cw.start(platform);
    }
    catch(OCException& e)
    {
        std::cout<< "Exception in main: "<<e.what()<<std::endl;
    }
}
