//******************************************************************
//
// Copyright 2015 Hyunwoo Lee (Seoul National University) All Rights Reserved.
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


#include "ACNESensorBundleActivator.h"

#include <algorithm>
#include <sstream>
#include "ACNESensorResource.h"

ACNESensorBundleActivator *g_bundleACNE;

ACNESensorBundleActivator::ACNESensorBundleActivator()
{
    m_pResourceContainer = nullptr;
}

ACNESensorBundleActivator::~ACNESensorBundleActivator()
{
    m_pResourceContainer = nullptr;
}

void ACNESensorBundleActivator::activateBundle(ResourceContainerBundleAPI *resourceContainer, std::string bundleId)
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

void ACNESensorBundleActivator::deactivateBundle()
{
    std::vector< BundleResource::Ptr >::iterator itor;
    for (itor = m_vecResources.begin(); itor != m_vecResources.end();)
    {
        destroyResource(*itor);
    }
}

void ACNESensorBundleActivator::createResource(resourceInfo resourceInfo)
{
    if (resourceInfo.resourceType == "oic.r.sensor")
    {
        static int ACNESensorCount = 1;

        BundleResource::Ptr newResource = std::make_shared< ACNESensorResource >();

        newResource->m_bundleId = m_bundleId;
        std::string indexCount;
        stringstream convert;
        convert << ACNESensorCount++;

        indexCount = convert.str();

        newResource->m_uri = "/softsensor/ACNESensor/" + indexCount;
        newResource->m_resourceType = resourceInfo.resourceType;
        newResource->m_mapResourceProperty = resourceInfo.resourceProperty;

        newResource->initAttributes();

        m_pResourceContainer->registerResource(newResource);
        m_vecResources.push_back(newResource);
    }
    else if (resourceInfo.resourceType == "oic.r.sensor.moist")
    {
        cout << "Moist Resource" << endl;
    }
    else if (resourceInfo.resourceType == "oic.r.sensor.temp")
    {
        cout << "Temp Resource" << endl;
    }
    else if (resourceInfo.resourceType == "oic.r.sensor.oil")
    {
        cout << "Oil Resource" << endl;
    }
}

void ACNESensorBundleActivator::destroyResource(BundleResource::Ptr resource)
{
    std::vector< BundleResource::Ptr >::iterator itor;

    itor = std::find(m_vecResources.begin(), m_vecResources.end(), resource);

    if (itor != m_vecResources.end())
    {
        m_pResourceContainer->unregisterResource(resource);
        m_vecResources.erase(itor);
    }
}

extern "C" void ACNESensor_externalActivateBundle(ResourceContainerBundleAPI *resourceContainer, std::string bundleId)
{
    g_bundleACNE = new ACNESensorBundleActivator();
    g_bundleACNE->activateBundle(resourceContainer, bundleId);
}

extern "C" void ACNESensor_externalDeactivateBundle()
{
    g_bundleACNE->deactivateBundle();
    delete g_bundleACNE;
}

extern "C" void ACNESensor_externalCreateResource(resourceInfo resourceInfo)
{
    g_bundleACNE->createResource(resourceInfo);
}

extern "C" void ACNESensor_externalDestroyResource(BundleResource::Ptr pBundleResource)
{
    g_bundleACNE->destroyResource(pBundleResource);
}     
