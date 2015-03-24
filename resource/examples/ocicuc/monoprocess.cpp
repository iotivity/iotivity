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
#include <thread>
#include <atomic>
#include <string>
#include <memory>
#include <utility>
#include <ostream>
#include <sstream>
#include <iostream>

#include "OCApi.h"
#include "OCResource.h"
#include "OCPlatform.h"

#include "exec.hpp"
#include "utility.hpp"

#include "demo_client.hpp"
#include "light_resource.hpp"

namespace Intel { namespace OCDemo {

auto make_description()
    -> boost::program_options::options_description
{
 using std::string;

 namespace po = boost::program_options;

 po::options_description desc("Monoprocess Client/Server options");

 desc.add_options()
    ("nres",        po::value<unsigned long>()->default_value(1),  "number of resources to use for testing")
    ("host_ip",     po::value<string>()->default_value("0.0.0.0"), "IP of host")
    ("host_port",   po::value<uint16_t>()->default_value(0),       "port of host")
    ("interface",   po::value<string>()->default_value("eth0"),    "network interface name")
    ("uri",         po::value<vector<string>>(),                   "resource URI")
    ;

 return desc;
}

/* Unfortunately, our target compiler may not support std::async correctly, so this
leverages RAII to save us from having to use the double-join-check pattern: */
struct simple_join_guard
{
 thread t;

 template <class FnT>
 simple_join_guard(FnT&& fn)
  : t(fn)
 {}

 ~simple_join_guard()
 {
    if(t.joinable())
     t.join();
 }
};

struct client_t
{
 const boost::program_options::variables_map        m_vm;

 atomic<bool>&                                      quit_flag;

 vector<string>                                     resource_URIs;

 public:
 client_t(const boost::program_options::variables_map vm,
          atomic<bool>& quit_flag_)
  : m_vm(vm),
    quit_flag(quit_flag_)
 {}

 public:
 void init()
 {
    if(0 != m_vm.count("uri"))
     {
        const vector<string>& input_URIs = m_vm["uri"].as< vector<string> >();
        copy(begin(input_URIs), end(input_URIs), back_inserter(resource_URIs));
     }
    else
     resource_URIs.push_back("coap://224.0.1.187/oc/core");
  }

 void operator()()
 {
    std::cout << "Requesting " << resource_URIs.size() << " URIs:\n";

    for(const auto& resource_URI : resource_URIs)
     cout << resource_URI << '\n';

    Intel::OCDemo::client::resource_handler resources(resource_URIs);

    // Register callbacks and wait for resources:
    resources.find_resources();

    // Allow the client to receive events from the server:
    while(!quit_flag)
     {
        std::this_thread::sleep_for(std::chrono::seconds(1));
     }
 }
};

struct server_t
{
 const boost::program_options::variables_map        m_vm;

 atomic<bool>&                                      quit_flag;

 unsigned long                                      m_nresources;

 vector<string>                                     resource_URIs;

 vector<shared_ptr<Intel::OCDemo::LightResource>>   lights;

 public:
 server_t(const boost::program_options::variables_map& vm,
          atomic<bool>& quit_flag_)
  : m_vm(vm),
    quit_flag(quit_flag_)
 {
    m_nresources = vm["nres"].as<unsigned long>();
 }

 public:
 void init();

 void operator()()
 {
    while(!quit_flag)
     {
        std::this_thread::sleep_for(std::chrono::seconds(5));
     }
 }
};

void server_t::init()
{
 lights.resize(m_nresources);

 for(unsigned int resource_number = 1; m_nresources >= resource_number; resource_number++)
  {
    cout << "Registering resource " << resource_number << ": " << std::flush;

    auto lr = make_shared<Intel::OCDemo::LightResource>();

    lr->createResource(resource_number);
    lr->addType("core.brightlight");
    lr->addInterface("oc.mi.ll");

    lights.push_back(lr);

    cout << "Ok." << std::endl;
  }
}

int exec(const boost::program_options::variables_map& vm)
{
 using namespace std;

 std::cout << "Starting platform: " << std::flush;

 OC::OCPlatform::Configure(OC::PlatformConfig {
                          OC::ServiceType::InProc,
                          OC::ModeType::Both,                   // run in client/server mode
                          vm["host_ip"].as<string>(),           // host
                          vm["host_port"].as<uint16_t>(),       // port
                          OC::QualityOfService::LowQos
                        });
 std::cout << "Ok." << std::endl;

 std::atomic<bool> quit_flag;

 server_t server(vm, quit_flag);
 client_t client(vm, quit_flag);

std::cout << "JFW: TODO: don't need to separate these any more\n";
 server.init();
 client.init();

 // Run the server and client:
 {
    simple_join_guard server_guard(server);
    simple_join_guard client_guard(client);

    // Note that at present nothing makes this true:
    while(!quit_flag)
     {
        // Perform app tasks
        std::this_thread::sleep_for(std::chrono::seconds(2));
     }
  }

 return 1;
}

}} // namespace Intel::OCDemo

