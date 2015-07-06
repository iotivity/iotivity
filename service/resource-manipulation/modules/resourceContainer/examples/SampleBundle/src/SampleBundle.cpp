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

    vector<resourceInfo> resourceConfig;

    resourceContainer->getResourceConfiguration(m_bundleId, &resourceConfig);

    for (vector<resourceInfo>::iterator itor = resourceConfig.begin();
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

void SampleBundle::createResource(resourceInfo resourceInfo)
{
    std::cout << "SampleBundle::createResource called" << std::endl;

    static int discomfortIndexSensorCount = 0;

    std::vector< std::map< std::string, std::string > >::iterator itor_vec;
    std::map< std::string, std::string >::iterator itor_map;
    std::vector <std::string> inputs;

    for (itor_vec = resourceInfo.resourceProperty["input"].begin();
         itor_vec != resourceInfo.resourceProperty["input"].end(); itor_vec++)
    {
        for (itor_map = (*itor_vec).begin(); itor_map != (*itor_vec).end(); itor_map++)
        {
            inputs.push_back(itor_map->second);
        }
    }
    std::cout << "SampleBundle::creating new discomfort index sensor " << std::endl;
    // create DISensor resource
    DiscomfortIndexSensorResource *newResource = new DiscomfortIndexSensorResource(inputs);

    newResource->m_uri = "/sampleBundle/discomfortIndex/" + std::to_string(
                             discomfortIndexSensorCount++);
    newResource->m_resourceType = resourceInfo.resourceType;
    newResource->m_mapResourceProperty = resourceInfo.resourceProperty;

    // setting input Attributes count
    newResource->inputCount = newResource->m_mapResourceProperty["input"].size();

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
