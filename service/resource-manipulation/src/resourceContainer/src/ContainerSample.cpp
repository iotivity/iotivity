//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include "ResourceContainer.h"
#include "BundleInfo.h"
#include "oc_logger.hpp"
#include <iostream>

using namespace OIC::Service;
using OC::oc_log_stream;

/* Annother way to create a context: */
auto info_logger = []() -> boost::iostreams::stream<OC::oc_log_stream> &
{
    static OC::oc_log_stream ols(oc_make_ostream_logger);
    static boost::iostreams::stream<OC::oc_log_stream> os(ols);

    return os;
};

int main()
{
    info_logger()->set_module("ContainerTest");
    info_logger()->set_level(OC_LOG_INFO);

    info_logger() << "Starting container test." << std::flush;

    ResourceContainer *container = ResourceContainer::getInstance();
    container->startContainer("examples/ResourceContainerConfig.xml");

    std::list<BundleInfo*> bundles = container->listBundles();
    std::list<BundleInfo*>::iterator bundleIt;

    for(bundleIt = bundles.begin(); bundleIt != bundles.end(); bundleIt++){
        BundleInfo* bi = *bundleIt;
        info_logger() << "Available bundle: " << bi->getID() << endl;
    }

    cout << "Press enter to stop all bundles " << endl;
    getchar();

    for(bundleIt = bundles.begin(); bundleIt != bundles.end(); bundleIt++){
        BundleInfo* bi = *bundleIt;
        info_logger() << "Stopping bundle: " << bi->getID() << endl;
        container->stopBundle(bi->getID());
    }

    cout << "Press enter to restart all bundles " << endl;
    getchar();

    for(bundleIt = bundles.begin(); bundleIt != bundles.end(); bundleIt++){
        BundleInfo* bi = *bundleIt;
        info_logger() << "Starting bundle: " << bi->getID() << endl;
        container->startBundle(bi->getID());
    }

    cout << "Press enter to stop container " << endl;
    getchar();
    container->stopContainer();
    cout << "Container stopped. Bye" << endl;
}
