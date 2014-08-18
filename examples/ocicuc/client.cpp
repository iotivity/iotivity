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

#include <map>
#include <string>
#include <memory>
#include <utility>
#include <ostream>
#include <sstream>
#include <iostream>

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
 using std::vector;

 namespace po = boost::program_options;

 po::options_description desc("Client options");

 desc.add_options()
    ("nres",        po::value<unsigned long>()->default_value(1),           "number of resources to use for testing")
    ("host_ip",     po::value<string>()->default_value("134.134.161.33"),   "IP of host")
    ("host_port",   po::value<uint16_t>()->default_value(5683),             "port of host")
    ("interface",   po::value<string>()->default_value("eth0"),             "network interface name")
    ("uri",         po::value<vector<string>>(),                            "remote resource URI")
    ;

 return desc;
}

// Prettyprinter for AttributeMaps:
std::ostream& operator<<(std::ostream& os, const OC::AttributeMap& attrs)
{
 for(const auto& attr : attrs)
  {
        os << "Attribute \"" << attr.first << "\": ";

        for(const auto& val : attr.second)
         os << val << "; ";
  }

 return os;
}

int observe_count()
{
 static unsigned long long oc = 0;
 return ++oc;
}

/* Helpers for measuring request times: */
typedef std::pair< 
                    std::chrono::time_point<std::chrono::high_resolution_clock>, 
                    std::chrono::time_point<std::chrono::high_resolution_clock> 
                 > clock_interval;

struct call_times
{
 static map<string, clock_interval> timings;

 void reset(const std::string& entry) { timings[entry] = make_pair(chrono::high_resolution_clock::now(), chrono::high_resolution_clock::time_point()); }

 void mark(const std::string& name)
 {
    auto e = timings.find(name);

    if(timings.end() == e)
     {
        reset(name);
        return;
     }

    auto& tp = (*e).second;

    if(tp.first > tp.second)
     timings[name].second = chrono::high_resolution_clock::now();
  }

  void report()
  {
    cout << "Time marks:\n";

    for_each(begin(timings), end(timings), [](const std::pair<std::string, clock_interval>& tm) -> void {
        using namespace std::chrono;

    const std::string& name     { tm.first };

    const std::chrono::time_point<std::chrono::high_resolution_clock>& request_time    { tm.second.first };
    const std::chrono::time_point<std::chrono::high_resolution_clock>& response_time   { tm.second.second };

    cout << '\t' << name << ": ";

    if(request_time > response_time)
     {
        cout << "<waiting>\n";
        return;
     }

    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(response_time - request_time).count();
    cout << elapsed_ms << "ms (";

    auto elapsed_us = std::chrono::duration_cast<std::chrono::microseconds>(response_time - request_time).count();
    cout << elapsed_us << "us)\n";
   });
  }

  void report_and_reset(const string& name)
  {
       mark(name), report(), reset(name);
  }

} call_timer;

map<string, clock_interval> call_times::timings;

class resource_handler;

class resource_handle
{
 friend class resource_handler;
 
 private:
 const std::string                     URI;
 std::shared_ptr<OC::OCResource>       resource;

 public:
 resource_handle(const std::string& URI_, std::shared_ptr<OC::OCResource> resource_)
  : URI(URI_),
    resource(resource_)
 {}

 resource_handle(const std::string& URI_)
  : resource_handle(URI_, nullptr)
 {}

 // Callbacks (note that the signature after binding will match exactly:
 private:
 void onFoundResource(std::shared_ptr<OC::OCResource> in_resource);
 void onResourceGet(OC::AttributeMap attr_map, const int error_code);
 void onResourcePut(OC::AttributeMap attribute_map, const int error_code);
 void onObserve(const OC::AttributeMap attribute_map, const int error_code, const int& sequence_number);
};

class resource_handler
{
 OC::OCPlatform& platform;

 static std::vector<std::shared_ptr<resource_handle>> resources;    // URI -> Maybe resource

 public:
 resource_handler(OC::OCPlatform& platform_, const std::vector<std::string>& resource_URIs_);
 resource_handler(OC::OCPlatform& platform_);

 public:
 bool has(const std::string& URI)
 {
    for(const auto& r : resources)
     {
      if(URI == r->URI)
       return true;
     }

    return false;
 }

 void add(const std::string& URI)
 {
    if(!has(URI))
     resources.emplace_back(std::make_shared<resource_handle>(URI));
 }

 void find_resources()
 {
        for(const auto& resource : resources)
         {
                std::cout << "* Finding resources \"" << resource->URI << "\".\n";

                call_timer.mark("find_resources");

                platform.findResource("", resource->URI, 
                                      std::bind(&resource_handle::onFoundResource, resource, std::placeholders::_1));
         } 
 }
};

std::vector<std::shared_ptr<resource_handle>> resource_handler::resources;

resource_handler::resource_handler(OC::OCPlatform& platform_, const std::vector<std::string>& resource_URIs)
 : platform(platform_)
{
 for(const auto& URI : resource_URIs)
  add(URI);
}

resource_handler::resource_handler(OC::OCPlatform& platform_)
  : resource_handler::resource_handler(platform_, {})
{}

int exec(const boost::program_options::variables_map& vm)
{
 using namespace std;

 OC::OCPlatform platform({ 
                          OC::ServiceType::InProc,              // in-process server
                          OC::ModeType::Client,                 // client mode
                          vm["host_ip"].as<string>(),           // host
                          vm["host_port"].as<uint16_t>()        // port
                        });

 vector<string> resource_URIs;

 if(0 == vm.count("uri"))
  {
    std::cout << "No URI specified, looking for everything in \"core\".\n";

    // Find all resources:
    resource_URIs.push_back("coap://224.0.1.187/oc/core");

    /* Example of finding specific resources: 
    const auto& nprops = vm["nres"].as<unsigned long>();

    for(unsigned long instance_number = 1;
        instance_number <= nprops;
        instance_number++)
     {
        ostringstream uri;


        uri << "coap://" << vm["host_ip"].as<string>() << "/oc/core?rt=core.light" << '_' << instance_number;
        resource_URIs.push_back(uri.str()); // ie. "coap://224.0.1.187/oc/core?rt=core.light_1");
     }
    */
  }
 else
  {
    const vector<string>& input_URIs = vm["uri"].as< vector<string> >();
    copy(begin(input_URIs), end(input_URIs), back_inserter(resource_URIs));
  }

 std::cout << "Requesting " << resource_URIs.size() << " URIs:\n";

 for(const auto& resource_URI : resource_URIs)
  cout << resource_URI << '\n';

 resource_handler resources(platform, resource_URIs);

 // Register callbacks and wait for resources:
 resources.find_resources();

 // Allow the client to receive events from the server:
 for(;;)
  ;

 return 0;
}

void resource_handle::onFoundResource(std::shared_ptr<OC::OCResource> in_resource)
{
 using std::cout;

 cout << "* onFoundResource():\n";

 try
  {
       if(nullptr == in_resource)
        throw OC::OCException("invalid resource passed to client callback");

       call_timer.report_and_reset("find_resources");

       // Now, fixup our own representation:
       resource = in_resource;

       /* Note: You can combine the host and URI to get a unique identifier, for
       example to filter out events you aren't interested in. Here, we just report the
       data: */
       cout << "resource URI: " << resource->uri() << "; "
            << "host address: " << resource->host() << '\n';

       call_timer.mark("get_resource");

       resource->get(std::bind(&resource_handle::onResourceGet, this, 
                               std::placeholders::_1, std::placeholders::_2));
  }
 catch(OC::OCException& e)
  {
        std::cerr << "onFoundResource(): exception " << e.reason() << ": " << e.what() << '\n';
  }
 catch(std::exception& e)
  {
        std::cerr << "onFoundResource(): exception: " << e.what() << '\n';
  }
}

void resource_handle::onResourceGet(const OC::AttributeMap attr_map, const int error_code)
{
 using std::cout;

 cout << "onResourceGet():\n";

 call_timer.report_and_reset("get_resource");

 if(error_code)
  {
        std::cerr << "onResourceGet(): error: " << error_code << '\n';
        return;
  }

 if(nullptr == resource)
  {
        std::cerr << "onResourceGet(): empty resource pointer.\n";
        return;
  }

 std::cout << attr_map << '\n';

 // Now, make a change to the light representation (replacing, rather than parsing): 
 OC::AttributeMap attrs {
                         { "state", { "true" } },
                         { "power", { "10" } }
                        };

 call_timer.mark("put_resource");

 resource->put(attrs, OC::QueryParamsMap(), 
               std::bind(&resource_handle::onResourcePut, this, std::placeholders::_1, std::placeholders::_2));
}

void resource_handle::onResourcePut(const OC::AttributeMap attribute_map, const int error_code)
{
 std::cout << "onResourcePut():\n";

 call_timer.report_and_reset("put_resource");

 if(0 != error_code)
  {
        std::ostringstream os;

        os << "onResourcePut(): error code " << error_code << " from server response.";

        throw OC::OCException(os.str());
  }

 std::cout << attribute_map << '\n';

 call_timer.mark("observe_resource");

 // Start an observer:
 resource->observe(OC::ObserveType::Observe,
                    std::bind(&resource_handle::onObserve, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void resource_handle::onObserve(const OC::AttributeMap attribute_map, const int error_code, const int& sequence_number)
{
 if(0 != error_code)
  {
    std::ostringstream os;
    os << "onObserve(): error " << error_code << " from callback.\n";
    throw OC::OCException(os.str());
  }

 std::cout << "onObserve(): sequence number: " << sequence_number << ":\n";

 call_timer.report_and_reset("observe_resource");

 std::cout << attribute_map << '\n';

 const auto oc = observe_count();

 std::cout << "onObserve(): observation count is: " << oc << '\n';

 // We don't want to be observed forever for purposes of this demo:
 if(10 <= oc)
  {
    std::cout << "onObserve(): cancelling observation.\n";

    const auto result = resource->cancelObserve();

    std::cout << "onObserve(): result of cancellation: " << result << ".\n";
    
    this_thread::sleep_for(chrono::seconds(10));
  }
}

}} // namespace Intel::OCDemo

