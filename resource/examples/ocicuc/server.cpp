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

#include "utility.hpp"

#include "exec.hpp"
#include "light_resource.hpp"

namespace Intel { namespace OCDemo {

auto make_description()
    -> boost::program_options::options_description
{
 using std::string;

 namespace po = boost::program_options;

 po::options_description desc("Server options");

 desc.add_options()
    ("nres",        po::value<unsigned long>()->default_value(1),         "number of resources to use for testing")
    ("host_ip",     po::value<string>()->default_value("0.0.0.0"), "IP of host")
    ("host_port",   po::value<uint16_t>()->default_value(0),          "port of host")
    ("interface",   po::value<string>()->default_value("eth0"),           "network interface name")
    ("uri",     po::value<vector<string>>(),                              "resource URI")
    ("runtime", po::value<unsigned int>()->default_value(3600),             "time in seconds to keep the server alive")
    ;

 return desc;
}

int exec(const boost::program_options::variables_map& vm)
{
 using namespace std;

 std::cout << "Starting platform: " << std::flush;

 OC::OCPlatform::Configure({
                          OC::ServiceType::InProc,              // in-process server
                          OC::ModeType::Server,                 // run in server mode
                          vm["host_ip"].as<string>(),           // host
                          vm["host_port"].as<uint16_t>(),       // port
                          OC::QualityOfService::LowQos
                        });

 std::cout << "Ok." << std::endl;

 vector<string> resource_URIs;

 vector<shared_ptr<Intel::OCDemo::LightResource>> lights;

 const unsigned long& nresources = vm["nres"].as<unsigned long>();

 for(unsigned int resource_number = 1; nresources >= resource_number; resource_number++)
  {
        cout << "Registering resource " << resource_number << ": " << std::flush;

        auto lr = make_shared<Intel::OCDemo::LightResource>();

        lr->createResource(resource_number);
        lr->addType(std::string("core.brightlight"));
        lr->addInterface(std::string("oc.mi.ll"));

        lights.push_back(lr);

        cout << "Ok." << std::endl;
  }

 // Perform app tasks
 cout << "Sleeping for "<< vm["runtime"].as<unsigned int>()<<" seconds."<<endl;
 std::this_thread::sleep_for(std::chrono::seconds( vm["runtime"].as<unsigned int>()));

 for(auto light: lights)
 {
    light->unregisterResource();
 }

 return 1;
}

}} // namespace Intel::OCDemo

