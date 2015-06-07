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

#include "SampleBundle.h"
#include "ResourceContainerBundleAPI.h"
#include "Configuration.h"


using namespace RC;

SampleBundle *bundle;

SampleBundle::SampleBundle()
{
}

SampleBundle::~SampleBundle()
{
}

void SampleBundle::activateBundle(ResourceContainerBundleAPI* resourceContainer)
{
    std::cout << "SampleBundle::activateBundle called" << std::endl;

    m_ResourceContainer = resourceContainer;

    // parse configuration, instantiate resource and register resources
    Configuration::configInfo config;
  //  m_ResourceContainer->getConfiguration(ConfigKey::CONFIG_RESOURCES, "oic.bundle.sample", &config);

    std::cout << "Resource Information" << std::endl;
    // for (int i = 0; i < configParam.size(); i++)
    //{
    //     for (map <string, string>::iterator itor = configParam[i].begin(); itor != configParam[i].end();
    //        itor++)
    //
    //       cout << "key : " << itor->first << " | value : " << itor->second << endl;
    // }

    // createResource();
}

void SampleBundle::deactivateBundle()
{
    std::cout << "SampleBundle::deactivateBundle called" << std::endl;
    // unregister resources
}

void SampleBundle::createResource()
{
    //std::cout << "SampleBundle::createResource called" << std::endl;
    //SampleBundleResource *newResource = new SampleBundleResource();
    //m_vecResources.push_back(newResource);
    //std::cout << "resourceContainer.registerResource()" << std::endl;
}

void SampleBundle::destroyResource()
{
    //std::cout << "SampleBundle::destroyResource called" << std::endl;
    //std::cout << "resourceContainer.unregisterResource()" << std::endl;
}

extern "C" void externalActivateBundle(ResourceContainerBundleAPI* resourceContainer)
{
    bundle = new SampleBundle();
    bundle->activateBundle(resourceContainer);
}

extern "C" void externalDeactivateBundle()
{
    if (!bundle)
    {
        bundle->deactivateBundle();
    }
}
