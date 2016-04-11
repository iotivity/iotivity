//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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
#include <stdexcept>
#include <condition_variable>
#include <map>
#include <vector>
#include "OCPlatform.h"
#include "OCApi.h"

#include "cloud_connector.h"

#define DEFAULT_CONTEXT_VALUE 0x99
#define DEFAULT_PUBLISH_QUERY "/oic/rd?rt=oic.wk.rdpub"
#define DEFAULT_DISCOVER_QUERY "/oic/res?rt=core.foo"

using namespace OC;

typedef std::map<OCResourceIdentifier, std::shared_ptr<OCResource>> DiscoveredResourceMap;

DiscoveredResourceMap discoveredResources;

class ResourceClient
{
    private:
        void putResourceInfo(const HeaderOptions & /*headerOptions*/,
                             const OCRepresentation rep, const OCRepresentation /*rep2*/, const int eCode)
        {
            std::cout << "In PutResourceInfo" << std::endl;

            std::cout << "Clientside Put response to get was: " << std::endl;
            std::cout << "ErrorCode: " << eCode << std::endl;

            if (eCode == 0)
            {
                std::cout << "Successful Put.  Attributes sent were: " << std::endl;

                rep.getValue("isFoo", m_isFoo);
                rep.getValue("barCount", m_barCount);

                std::cout << "\tisFoo: " << m_isFoo << std::endl;
                std::cout << "\tbarCount: " << m_barCount << std::endl;

                std::cout << "Actual New values are: " << std::endl;

                rep.getValue("isFoo", m_isFoo);
                rep.getValue("barCount", m_barCount);

                std::cout << "\tisFoo: " << m_isFoo << std::endl;
                std::cout << "\tbarCount: " << m_barCount << std::endl;

                m_cv.notify_all();
            }
        }

        void getResourceInfo(const HeaderOptions & /*headerOptions*/, const OCRepresentation rep,
                             const int eCode)
        {
            std::cout << "In getResourceInfo" << std::endl;

            std::cout << "Clientside response to get was: " << std::endl;
            std::cout << "Error Code: " << eCode << std::endl;

            if (eCode == 0)
            {
                std::cout << "Successful Get.  Attributes are: " << std::endl;

                rep.getValue("isFoo", m_isFoo);
                rep.getValue("barCount", m_barCount);

                std::cout << "\tisFoo: " << m_isFoo << std::endl;
                std::cout << "\tbarCount: " << m_barCount << std::endl;

                std::cout << "Doing a put on q/foo" << std::endl;
                OCRepresentation rep2(rep);
                m_isFoo = false;
                m_barCount = 211;

                rep2.setValue("isFoo", m_isFoo);
                rep2.setValue("barCount", m_barCount);

                m_resource->put(rep2, QueryParamsMap(),
                                PutCallback(std::bind(&ResourceClient::putResourceInfo, this, std::placeholders::_1,
                                                      rep2, std::placeholders::_2, std::placeholders::_3)));
            }
        }

        void foundResource(std::shared_ptr<OCResource> resource)
        {
            std::cout << "In foundResource" << std::endl;
            std::lock_guard<std::mutex> lock(m_resourceLock);

            if (discoveredResources.find(resource->uniqueIdentifier()) == discoveredResources.end())
            {
                std::cout << "Found resource " << resource->uniqueIdentifier() <<
                          " for the first time on server with ID: " << resource->sid() << std::endl;
                discoveredResources[resource->uniqueIdentifier()] = resource;
            }
            else
            {
                std::cout << "Found resource " << resource->uniqueIdentifier() << " again!" << std::endl;
            }

            if (resource)
            {
                std::cout << "Found Resource: " << std::endl;
                std::cout << "\tHost: " << resource->host() << std::endl;
                std::cout << "\tURI:  " << resource->uri() << std::endl;

                // Get the resource types
                std::cout << "\tList of resource types: " << std::endl;
                for (auto &resourceTypes : resource->getResourceTypes())
                {
                    std::cout << "\t\t" << resourceTypes << std::endl;
                }

                // Get the resource interfaces
                std::cout << "\tList of resource interfaces: " << std::endl;
                for (auto &resourceInterfaces : resource->getResourceInterfaces())
                {
                    std::cout << "\t\t" << resourceInterfaces << std::endl;
                }

                std::cout << "found resource OUT" << std::endl;

                m_resource = resource;

                std::cout << "Doing a get on q/foo." << std::endl;

                m_resource->get(QueryParamsMap(),
                                GetCallback(std::bind(&ResourceClient::getResourceInfo, this,
                                                      std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
            }
        }

    public:
        ResourceClient()
        {}

        OCStackResult start(std::string hostUri, std::string requestUri)
        {
            FindCallback f(std::bind(&ResourceClient::foundResource, this, std::placeholders::_1));

            return OCPlatform::findResource(hostUri, requestUri, CT_ADAPTER_TCP, f);
        }
    private:
        std::mutex m_mutex;
        std::mutex m_resourceLock;
        std::condition_variable m_cv;
        std::shared_ptr<OCResource> m_resource;
        bool m_isFoo;
        int m_barCount;
};

struct FooResource
{
    bool m_isFoo;
    int m_barCount;
    OCResourceHandle m_resourceHandle;
    OCResourceHandle m_resourceHandle2;
    OCRepresentation m_rep;

    FooResource() : m_isFoo(true), m_barCount(0)
    {
        m_rep.setValue("isFoo", m_isFoo);
        m_rep.setValue("barCount", m_barCount);
    }

    bool createResource(std::string resourceURI)
    {
        std::string resourceTypeName = "core.foo";
        std::string resourceInterface = DEFAULT_INTERFACE;

        m_rep.setUri(resourceURI);

        uint8_t resourceProperty = OC_DISCOVERABLE;

        EntityHandler eh(std::bind(&FooResource::entityHandler,
                                   this, std::placeholders::_1));
        OCStackResult result = OCPlatform::registerResource(m_resourceHandle,
                               resourceURI, resourceTypeName,
                               resourceInterface,
                               eh, resourceProperty);
        if (OC_STACK_OK != result)
        {
            std::cout << "Resource creation unsuccessful" << std::endl;
            return false;
        }

        return true;
    }

    OCResourceHandle getHandle()
    {
        return m_resourceHandle;
    }

    OCResourceHandle getHandle2()
    {
        return m_resourceHandle2;
    }


    OCRepresentation get()
    {
        m_rep.setValue("isFoo", m_isFoo);
        m_rep.setValue("barCount", m_barCount);

        return m_rep;
    }

    void put(OCRepresentation &rep)
    {
        rep.getValue("isFoo", m_isFoo);
        rep.getValue("barCount", m_barCount);
    }

    OCStackResult sendResponse(std::shared_ptr<OCResourceRequest> pRequest)
    {
        auto pResponse = std::make_shared<OC::OCResourceResponse>();
        pResponse->setRequestHandle(pRequest->getRequestHandle());
        pResponse->setResourceHandle(pRequest->getResourceHandle());
        pResponse->setResourceRepresentation(get(), "");
        pResponse->setErrorCode(200);
        pResponse->setResponseResult(OC_EH_OK);

        return OCPlatform::sendResponse(pResponse);
    }

    OCEntityHandlerResult entityHandler(std::shared_ptr<OCResourceRequest> request)
    {
        std::cout << "\tConsumer Entity Handler:" << std::endl;
        OCEntityHandlerResult ehResult = OC_EH_ERROR;

        if (request)
        {
            // Note: Most of the handlers are not here, since this is for
            // demoing client/server co-process existence.
            // See simpleserver for a more complete example.
            if (request->getRequestHandlerFlag() == RequestHandlerFlag::RequestFlag)
            {
                std::cout << "\t\trequestFlag : Request" << std::endl;

                if (request->getRequestType() == "GET")
                {
                    std::cout << "\t\t\trequestType : GET" << std::endl;
                    if (OC_STACK_OK == sendResponse(request))
                    {
                        ehResult = OC_EH_OK;
                    }
                }
                else if (request->getRequestType() == "PUT")
                {
                    std::cout << "\t\t\trequestType : PUT" << std::endl;

                    OCRepresentation rep = request->getResourceRepresentation();
                    put(rep);
                    if (OC_STACK_OK == sendResponse(request))
                    {
                        ehResult = OC_EH_OK;
                    }
                }
                else
                {
                    std::cout << "\t\t\trequestType : UNSUPPORTED: " <<
                              request->getRequestType() << std::endl;
                }
            }
            else
            {
                std::cout << "\t\trequestFlag : UNSUPPORTED: ";

                if (request->getRequestHandlerFlag() == RequestHandlerFlag::ObserverFlag)
                {
                    std::cout << "ObserverFlag" << std::endl;
                }
            }
        }
        else
        {
            std::cout << "Request Invalid!" << std::endl;
        }

        return ehResult;
    }
};

OCStackApplicationResult handlePublishCB(void *ctx,
        OCDoHandle /*handle*/,
        OCClientResponse *clientResponse)
{
    if (ctx != (void *)DEFAULT_CONTEXT_VALUE)
    {
        std::cout << "Invalid Publish callback received" << std::endl;
    }

    std::cout << "Publish resource response received, code: " << clientResponse->result << std::endl;

    return OC_STACK_KEEP_TRANSACTION;
}

OCStackApplicationResult handleLoginoutCB(void *ctx,
        OCDoHandle /*handle*/,
        OCClientResponse *clientResponse)
{
    if (ctx != (void *)DEFAULT_CONTEXT_VALUE)
    {
        std::cout << "Invalid Login callback received" << std::endl;
    }

    std::cout << "Login/out response received code: " << clientResponse->result << std::endl;

    if (clientResponse->payload != NULL &&
        clientResponse->payload->type == PAYLOAD_TYPE_REPRESENTATION)
    {
        std::cout << "PAYLOAD_TYPE_REPRESENTATION received" << std::endl;

        OCRepPayloadValue *val = ((OCRepPayload *)clientResponse->payload)->values;

        while (val)
        {
            std::cout << "Name: " << val->name << " Value: " << val->str << std::endl;
            val = val->next;
        }
    }

    return OC_STACK_KEEP_TRANSACTION;
}

void PrintUsage()
{
    std::cout << std::endl;
    std::cout << "Usage : cloud_device <addr:port> <session>\n";
    std::cout << "<addr:port>: Cloud Address, \"127.0.0.1:5683\"\n";
    std::cout <<
              "<session>: String value, Provided by response of onboarding scenario\n\tor kind of registration portal\n\n";
    std::cout <<
              "If you want to go API test mode include device registration,\n\tleave blank to <session> fields\n";
    std::cout <<
              "sample: \"cloud_device 127.0.0.1:5683\"\n\t-Enter API testmode\n\n";
    std::cout <<
              "sample: \"cloud_device 127.0.0.1:5683 1234567890123456\"\n\t-Enter API testmode using registered session\n\n";
}

void PublishResources(std::string host, std::string additionalQuery)
{
    std::cout << "Running as Server mode" << std::endl;

    FooResource fooRes1, fooRes2;

    if (!fooRes1.createResource("/q/resource_foo1"))
    {
        std::cout << "Unable to create resource" << std::endl;
        return;
    }

    if (!fooRes2.createResource("/q/resource_foo2"))
    {
        std::cout << "Unable to create resource" << std::endl;
        return;
    }

    std::string requestQuery = DEFAULT_PUBLISH_QUERY;
    requestQuery += additionalQuery;

    std::cout << "Publishing resources..." << std::endl;
    std::cout << host.c_str() << requestQuery.c_str() << std::endl;

    if (OCCloudPublish(host.c_str(), requestQuery.c_str(), &handlePublishCB, 2,
                       fooRes1.getHandle(), fooRes2.getHandle()) != OC_STACK_OK)
    {
        std::cout << "Unable to publish resources to cloud" << std::endl;
    }
}

void DiscoverResources(std::string host, std::string additionalQuery)
{
    std::cout << "Running as Client mode" << std::endl;

    ResourceClient client;
    std::string requestQuery = DEFAULT_DISCOVER_QUERY;
    requestQuery += additionalQuery;

    std::cout << "Finding resources..." << std::endl;
    std::cout << host.c_str() << requestQuery.c_str() << std::endl;

    if (client.start(host.c_str(), requestQuery.c_str()) != OC_STACK_OK)
    {
        std::cout << "Unable to find resources from cloud" << std::endl;
    }
}

int main(int argc, char *argv[])
{
    std::string host = "coap+tcp://";

    std::string cmdQuery;
    std::string session;

    PlatformConfig cfg
    {
        OC::ServiceType::InProc,
        OC::ModeType::Both,
        "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
        0,         // Uses randomly available port
        OC::QualityOfService::LowQos
    };

    OCPlatform::Configure(cfg);
    FooResource defaultResource;
    if (!defaultResource.createResource("/q/default"))
    {
        std::cout << "Unable to create default resource" << std::endl;
        return -1;
    }

    switch (argc)
    {
        case 1:
            PrintUsage();
            return 0;
            break;

        case 2:
            std::cout <<
                      "1. Login to cloud using OAuth2 auth code and auth provider name(AuthCode'OAuth2' required)" <<
                      std::endl;
            std::cout << "2. Login to cloud using session(Session required)" << std::endl;
            std::cout << "3. Logout from cloud using session(Session required)" << std::endl;
            std::cout << "s. Running as Resource Server mode" << std::endl;
            std::cout << "c. Running as Resource Client mode" << std::endl;
            std::cout << "exit: q" << std::endl;
            std::cin >> cmdQuery;
            break;

        case 3:
            cmdQuery = "2";
            session = argv[2];
            break;
    }

    host += argv[1];

    std::cout << "Host " << host.c_str() << std::endl;

    std::string authProvider;
    std::string authCode;

    OCStackResult   res = OC_STACK_ERROR;

    while (cmdQuery[0] != 'q')
    {
        switch (cmdQuery[0])
        {
            case '1':
                std::cout << "Put auth provider name(ex: github)" << std::endl;
                std::cin >> authProvider;
                std::cout << "Put auth code(provided by auth provider)" << std::endl;
                std::cin >> authCode;
                std::cout << "Login to cloud using " << authProvider << " " << authCode << std::endl;
                res = OCCloudRegisterLogin(host.c_str(), authProvider.c_str(), authCode.c_str(),
                                           handleLoginoutCB);
                std::cout << "OCCloudRegisterLogin return " << res << std::endl;
                break;

            case '2':
                std::cout << "Put session to login" << std::endl;
                if (session.size() == 0)
                    std::cin >> session;
                else
                    std::cout << session.c_str() << std::endl;
                res = OCCloudLogin(host.c_str(), session.c_str(), handleLoginoutCB);
                std::cout << "OCCloudLogin return " << res << std::endl;
                break;

            case '3':
                std::cout << "Put session to logout" << std::endl;
                if (session.size() == 0)
                    std::cin >> session;
                else
                    std::cout << session.c_str() << std::endl;
                res = OCCloudLogout(host.c_str(), session.c_str(), handleLoginoutCB);
                std::cout << "OCCloudLogout return" << res << std::endl;
                break;

            case 's':
                PublishResources(host, "");
                break;

            case 'c':
                DiscoverResources(host, "");
                break;
        }

        std::cin >> cmdQuery;
    }

    return 0;
}
