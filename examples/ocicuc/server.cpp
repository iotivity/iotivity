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

#include <map>
#include <atomic>
#include <thread>
#include <string>
#include <ostream>
#include <sstream>
#include <iostream>
#include <functional>

#include <boost/program_options.hpp>

#include "OCApi.h"
#include "OCResource.h"
#include "OCPlatform.h"

#include "exec.hpp"

namespace Intel { namespace OCDemo {

auto make_description()
    -> boost::program_options::options_description 
{
 using std::string;

 namespace po = boost::program_options;

 po::options_description desc("Server options");

 desc.add_options()
    ("nres",        po::value<unsigned long>()->default_value(1),         "number of resources to use for testing")
    ("host_ip",     po::value<string>()->default_value("134.134.161.33"), "IP of host")
    ("host_port",   po::value<uint16_t>()->default_value(56832),          "port of host")
    ("interface",   po::value<string>()->default_value("eth0"),           "network interface name") 
    ("uri",     po::value<vector<string>>(),                              "resource URI")
    ;

 return desc;
}

}} // namespace Intel::OCDemo

namespace Intel { namespace OCDemo {

using namespace OC;
using namespace std;

/// This class represents a single resource named 'lightResource'. This resource has
/// two simple properties named 'state' and 'power'
class LightResource
{
 public:
    bool m_state;       // off or on?
    int m_power;        // power level

    private:
    atomic<bool> m_observation; // are we under observation?

    private:
    static atomic<bool> shutdown_flag;
    static thread observe_thread;

    private:
    OCResourceHandle m_resourceHandle;

    public:
    LightResource()
     : m_state(false), 
       m_power(0),
       m_observation(false)
    {}

    ~LightResource()
    {
        shutdown_flag = true;

        if(observe_thread.joinable())
         observe_thread.join();
    }

    private:
    std::string make_URI(const unsigned int resource_number) const
    {
        return std::string("/a/light") + "_" + std::to_string(resource_number); 
    }

    public:
    // This function internally calls registerResource API.
    void createResource(OC::OCPlatform& platform, const unsigned int resource_number);

    OCResourceHandle getHandle() const { return m_resourceHandle; }

    void setRepresentation(AttributeMap& attributeMap);
    void getRepresentation(AttributeMap& attributeMap) const;

    void addType(const OC::OCPlatform& platform, const std::string& type) const;
    void addInterface(const OC::OCPlatform& platform, const std::string& interface) const;

    private:
    void entityHandler(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response);

    private:
    void observe_function();

    // Request handlers:
    private:
    void dispatch_request(const std::string& request_type, std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response);
     void handle_get_request(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response);
     void handle_put_request(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response);
     void handle_post_request(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response);
     void handle_delete_request(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response);
     void handle_observe_event(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response);
};

atomic<bool> LightResource::shutdown_flag(false);
thread LightResource::observe_thread;

void LightResource::setRepresentation(AttributeMap& attributeMap)
{
 cout << "\t\t\t" << "Received representation: " << endl;
 cout << "\t\t\t\t" << "power: " << attributeMap["power"][0] << endl;
 cout << "\t\t\t\t" << "state: " << attributeMap["state"][0] << endl;

 m_state = attributeMap["state"][0].compare("true") == 0;
 m_power = std::stoi(attributeMap["power"][0]);
}

void LightResource::getRepresentation(AttributeMap& attributeMap) const
{
 attributeMap["state"] = { (m_state ? "true" : "false") };
 attributeMap["power"] = { to_string(m_power) };
}

void LightResource::addType(const OC::OCPlatform& platform, const std::string& type) const
{
 OCStackResult result = platform.bindTypeToResource(m_resourceHandle, type);
 
 if(OC_STACK_OK != result)
  cout << "Binding TypeName to Resource was unsuccessful, result was " << result << '\n';
}

void LightResource::addInterface(const OC::OCPlatform& platform, const std::string& interface) const
{
 OCStackResult result = platform.bindInterfaceToResource(m_resourceHandle, interface);

 if(OC_STACK_OK != result)
  cout << "Binding TypeName to Resource was unsuccessful, result was " << result << '\n';
}

void LightResource::createResource(OC::OCPlatform& platform, const unsigned int resource_number)
{
 string resourceURI      { make_URI(resource_number) };
 string resourceTypeName { "core.light" };

 cout << "registering resource: " << resourceURI << '\n';
 cout << "registering type name \"" << resourceTypeName << "\".\n";

 // This will internally create and register the resource, binding the current instance's method as a callback:
 OCStackResult result = platform.registerResource(
                                            m_resourceHandle, resourceURI, resourceTypeName,
                                            DEFAULT_INTERFACE, 
                                            std::bind(&LightResource::entityHandler, this, std::placeholders::_1, std::placeholders::_2), 
                                            OC_DISCOVERABLE | OC_OBSERVABLE);

  if (OC_STACK_OK != result)
   std::cout << "Resource creation failed.\n";
}

void LightResource::observe_function()
{
 cerr << "Observation thread is spinning up.\n";

 while(!shutdown_flag)
  {
    std::this_thread::sleep_for(std::chrono::seconds(2));

    if(!m_observation)
     continue;

    m_power += 10;

    const auto result = OCPlatform::notifyObservers(getHandle());

    // Stop notifications when there are no more observers:
    if(OC_STACK_NO_OBSERVERS == result)
     {
        m_observation = 0;
     }
  }

 cerr << "Observation thread is shutting down.\n";
}

// This is just a sample implementation of entity handler.
// Entity handler can be implemented in several ways by the manufacturer
void LightResource::entityHandler(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response)
{
    if(!request)
     {
        cerr << "entityHandler(): Received invalid request object.\n";
        return;
     }

    if(!response)
     {
        cerr << "entityHandler(): Received invalid response object.\n";
        return;
     }

    switch(request->getRequestHandlerFlag())   
    {
        default:
                cerr << "entityHandler(): invalid request flag\n";
                break;

        case RequestHandlerFlag::InitFlag:
                cerr << "entityHandler(): Initialization requested.\n";
                break;

        case RequestHandlerFlag::RequestFlag:
                dispatch_request(request->getRequestType(), request, response);
                break;

        case RequestHandlerFlag::ObserverFlag:
                handle_observe_event(request, response);
                break;
    }
}

void LightResource::dispatch_request(const std::string& request_type, std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response) 
{
 std::cout << "dispatch_request(): " << request_type << '\n';

 if("GET" == request_type)
  return handle_get_request(request, response);

 if("PUT" == request_type)
  return handle_put_request(request, response);

 if("POST" == request_type)
  return handle_post_request(request, response);

 if("DELETE" == request_type)
  return handle_delete_request(request, response);

 cerr << "entityHandler(): Invalid request type \"" << request_type << "\".\n";
}

void LightResource::handle_get_request(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response) 
{
 cout << "handle_get_request():\n";

 const auto query_params_map = request->getQueryParameters();

 // ...do any processing of the query here...

 // Get a representation of the resource and send it back as a response:
 AttributeMap attribute_map;

 getRepresentation(attribute_map);

 response->setErrorCode(200);
 response->setResourceRepresentation(attribute_map);
}

void LightResource::handle_put_request(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response) 
{
 const auto query_params_map = request->getQueryParameters();

 // ...do something with the query parameters...

 auto attribute_map = request->getResourceRepresentation();

 setRepresentation(attribute_map);

 getRepresentation(attribute_map);

 response->setErrorCode(200);
 response->setResourceRepresentation(attribute_map); 
}

void LightResource::handle_post_request(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response)
{
 // ...demo-code...
 response->setErrorCode(200);

 auto attribute_map = request->getResourceRepresentation();
 getRepresentation(attribute_map);
 response->setResourceRepresentation(attribute_map);
}

void LightResource::handle_delete_request(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response) 
{
 // ...demo-code...
 response->setErrorCode(200);

 auto attribute_map = request->getResourceRepresentation();
 getRepresentation(attribute_map);
 response->setResourceRepresentation(attribute_map);
}

// Set up observation in a separate thread:
void LightResource::handle_observe_event(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response) 
{
 if(observe_thread.joinable())
  return; 

 observe_thread = thread(bind(&LightResource::observe_function, this));
 observe_thread.detach();
}

int exec(const boost::program_options::variables_map& vm)
{
 using namespace std;

 std::cout << "Starting platform: " << std::flush;

 OC::OCPlatform platform({ 
                          OC::ServiceType::InProc,              // in-process server
                          OC::ModeType::Both,                   // run as server and as client
                          vm["host_ip"].as<string>(),           // host
                          vm["host_port"].as<uint16_t>()        // port
                        });

 std::cout << "Ok." << std::endl;

 vector<string> resource_URIs;

 vector<shared_ptr<LightResource>> lights;

 const unsigned long& nresources = vm["nres"].as<unsigned long>();

 lights.resize(nresources);

 for(unsigned int resource_number = 1; nresources >= resource_number; resource_number++)
  {
        cout << "Registering resource " << resource_number << ": " << std::flush;

        auto lr = make_shared<LightResource>();

        lr->createResource(platform, resource_number);
        lr->addType(platform, std::string("core.brightlight"));
        lr->addInterface(platform, std::string("oc.mi.ll"));

        lights.push_back(lr);

        cout << "Ok." << std::endl;
  }

 // Perform app tasks
 while(true)
 {
    std::this_thread::sleep_for(std::chrono::seconds(5));
 }

 return 1;
}

}} // namespace Intel::OCDemo

