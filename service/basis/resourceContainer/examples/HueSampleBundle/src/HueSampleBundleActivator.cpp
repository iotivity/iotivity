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

#include "HueSampleBundleActivator.h"
#include "HueLight.h"

#include <algorithm>
#include <vector>

using namespace OIC::Service;

HueSampleBundleActivator *bundle;

HueSampleBundleActivator::HueSampleBundleActivator()
{
}

HueSampleBundleActivator::~HueSampleBundleActivator()
{
}

void HueSampleBundleActivator::activateBundle(ResourceContainerBundleAPI *resourceContainer,
                                  std::string bundleId)
{
    std::cout << "HueSampleBundle::activateBundle called" << std::endl;

    m_pResourceContainer = resourceContainer;
    m_bundleId = bundleId;
    m_connector = new HueConnector();

    vector<Configuration::resourceInfo> resourceConfig;

    resourceContainer->getResourceConfiguration(m_bundleId, &resourceConfig);

    for (vector<Configuration::resourceInfo>::iterator itor = resourceConfig.begin();
         itor != resourceConfig.end(); itor++)
    {
        createResource(*itor);
    }
}

void HueSampleBundleActivator::deactivateBundle()
{
    std::cout << "HueSampleBundle::deactivateBundle called" << std::endl;

    for (std::vector<BundleResource *>::iterator itor = m_vecResources.begin();
         itor != m_vecResources.end(); itor++)
    {
        destroyResource(*itor);
    }
}


void HueSampleBundleActivator::createResource(Configuration::resourceInfo resourceInfo)
{
    std::cout << "HueSampleBundle::createResource called" << std::endl;

    if(resourceInfo.resourceType == "oic.light.control"){
        static int lightCount = 1;
        HueLight* hueLight = new HueLight(m_connector, resourceInfo.address);
        resourceInfo.uri = "/hue/light/" + std::to_string(lightCount++);
        std::cout << "Registering resource " << resourceInfo.uri << std::endl;
        hueLight->m_uri = resourceInfo.uri;
        hueLight->m_resourceType = resourceInfo.resourceType;
        hueLight->m_name = resourceInfo.name;

        m_pResourceContainer->registerResource(hueLight);
        m_vecResources.push_back(hueLight);
    }
}


void HueSampleBundleActivator::destroyResource(BundleResource *resource)
{
    std::cout << "HueSampleBundle::destroyResource called" << std::endl;

    std::vector <BundleResource *>::iterator itor;

    itor = std::find(m_vecResources.begin(), m_vecResources.end(), resource);

    if (itor != m_vecResources.end())
        m_vecResources.erase(itor);

    // check
    //delete resource;

    m_pResourceContainer->unregisterResource(resource);
}

extern "C" void externalActivateBundle(ResourceContainerBundleAPI *resourceContainer,
                                       std::string bundleId)
{
    bundle = new HueSampleBundleActivator();
    bundle->activateBundle(resourceContainer, bundleId);
}

extern "C" void externalDeactivateBundle()
{
    if (!bundle)
    {
        bundle->deactivateBundle();
    }
}
