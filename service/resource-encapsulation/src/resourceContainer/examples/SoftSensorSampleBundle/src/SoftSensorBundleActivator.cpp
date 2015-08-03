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

#include "SoftSensorBundleActivator.h"
#include "DiscomfortIndexSensorResource.h"
#include <algorithm>

SoftSensorBundleActivator *bundle;

SoftSensorBundleActivator::SoftSensorBundleActivator()
{
    m_pResourceContainer = nullptr;
}

SoftSensorBundleActivator::~SoftSensorBundleActivator()
{
    m_pResourceContainer = nullptr;
}

void SoftSensorBundleActivator::activateBundle(ResourceContainerBundleAPI *resourceContainer,
        std::string bundleId)
{
    m_pResourceContainer = resourceContainer;
    m_bundleId = bundleId;

    std::vector<resourceInfo> resourceConfig;

    resourceContainer->getResourceConfiguration(m_bundleId, &resourceConfig);

    for (vector<resourceInfo>::iterator itor = resourceConfig.begin();
         itor != resourceConfig.end(); itor++)
    {
        createResource(*itor);
    }
}

void SoftSensorBundleActivator::deactivateBundle()
{
    std::vector<BundleResource *>::iterator itor;
    for (itor = m_vecResources.begin(); itor != m_vecResources.end();)
    {
        destroyResource(*itor);
    }
}

void SoftSensorBundleActivator::createResource(resourceInfo resourceInfo)
{
    if (resourceInfo.resourceType == "oic.softsensor")
    {
        static int discomfortIndexSensorCount = 1;

        // create DISensor resource
        DiscomfortIndexSensorResource *newResource = new DiscomfortIndexSensorResource();

        newResource->m_bundleId = m_bundleId;
        newResource->m_uri = "/softsensor/discomfortIndex/" + std::to_string(
                                 discomfortIndexSensorCount++);
        newResource->m_resourceType = resourceInfo.resourceType;
        newResource->m_mapResourceProperty = resourceInfo.resourceProperty;

        newResource->initAttributes();

        m_pResourceContainer->registerResource(newResource);
        m_vecResources.push_back(newResource);
    }
}

void SoftSensorBundleActivator::destroyResource(BundleResource *resource)
{
    std::vector <BundleResource *>::iterator itor;

    itor = std::find(m_vecResources.begin(), m_vecResources.end(), resource);

    if (itor != m_vecResources.end())
    {
        m_pResourceContainer->unregisterResource(resource);
        m_vecResources.erase(itor);
    }
}

extern "C" void externalActivateBundle(ResourceContainerBundleAPI *resourceContainer,
                                       std::string bundleId)
{
    bundle = new SoftSensorBundleActivator();
    bundle->activateBundle(resourceContainer, bundleId);
}

extern "C" void externalDeactivateBundle()
{
    bundle->deactivateBundle();
    delete bundle;
}

extern "C" void externalCreateResource(resourceInfo resourceInfo)
{
    bundle->createResource(resourceInfo);
}

extern "C" void externalDestroyResource(BundleResource *pBundleResource)
{
    bundle->destroyResource(pBundleResource);
}
