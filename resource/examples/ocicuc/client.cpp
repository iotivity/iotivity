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

// This contains the Boost MPL defines required for the boost_variant
// serialization, so it must go before the boost/program_options
#include "OCApi.h"

#include <map>
#include <string>
#include <memory>
#include <utility>
#include <ostream>
#include <sstream>
#include <iostream>

#include <boost/program_options.hpp>

#include "OCResource.h"
#include "OCPlatform.h"

#include "exec.hpp"
#include "utility.hpp"

#include "demo_client.hpp"

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
    ("host_ip",     po::value<string>()->default_value("0.0.0.0"),   "IP of host")
    ("host_port",   po::value<uint16_t>()->default_value(0),             "port of host")
    ("interface",   po::value<string>()->default_value("eth0"),             "network interface name")
    ("uri",         po::value<vector<string>>(),                            "remote resource URI")
    ;

 return desc;
}

int exec(const boost::program_options::variables_map& vm)
{
 using namespace std;

 OC::OCPlatform::Configure({
                          OC::ServiceType::InProc,              // in-process server
                          OC::ModeType::Client,                 // client mode
                          vm["host_ip"].as<string>(),           // host
                          vm["host_port"].as<uint16_t>(),       // port
                          OC::QualityOfService::LowQos
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

 Intel::OCDemo::client::resource_handler resources(resource_URIs);

 // Register callbacks and wait for resources:
 resources.find_resources();

 // Allow the client to receive events from the server:
 for(;;)
  ;

 return 0;
}

}} // namespace Intel::OCDemo

