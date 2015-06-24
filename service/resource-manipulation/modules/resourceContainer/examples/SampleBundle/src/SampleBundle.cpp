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
#include "DiscomfortIndexSensorResource.h"

using namespace OIC::Service;

SampleBundle *bundle;

SampleBundle::SampleBundle()
{
}

SampleBundle::~SampleBundle()
{
}

void SampleBundle::activateBundle(ResourceContainerBundleAPI *resourceContainer,
                                  std::string bundleId)
{
    std::cout << "SampleBundle::activateBundle called" << std::endl;

    m_pResourceContainer = resourceContainer;
    m_bundleId = bundleId;

    vector<Configuration::resourceInfo> resourceConfig;

    resourceContainer->getResourceConfiguration(m_bundleId, &resourceConfig);

    for (vector<Configuration::resourceInfo>::iterator itor = resourceConfig.begin();
         itor != resourceConfig.end(); itor++)
    {
        createResource(*itor);
    }
}

void SampleBundle::deactivateBundle()
{
    std::cout << "SampleBundle::deactivateBundle called" << std::endl;

    for (std::vector<BundleResource *>::iterator itor = m_vecResources.begin();
         itor != m_vecResources.end(); itor++)
    {
        destroyResource(*itor);
    }
}

// TODO : has to be updated for setting the info
void SampleBundle::createResource(Configuration::resourceInfo resourceInfo)
{
    std::cout << "SampleBundle::createResource called" << std::endl;
    static int discomfortIndexSensorCount = 0;
    DiscomfortIndexSensorResource *newResource = new DiscomfortIndexSensorResource();
    if(!resourceInfo.uri.empty()){
        newResource->m_uri = resourceInfo.uri;
    }
    else{
        newResource->m_uri = "sampleBundle/discomfortIndex/" + std::to_string(discomfortIndexSensorCount++);
    }
    newResource->m_resourceType = resourceInfo.resourceType;

    m_pResourceContainer->registerResource(newResource);

    m_vecResources.push_back(newResource);
}

void SampleBundle::destroyResource(BundleResource *resource)
{
    std::cout << "SampleBundle::destroyResource called" << std::endl;

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
    bundle = new SampleBundle();
    bundle->activateBundle(resourceContainer, bundleId);
}

extern "C" void externalDeactivateBundle()
{
    if (!bundle)
    {
        bundle->deactivateBundle();
    }
}
