//******************************************************************
//
// Copyright 2015 Jake Martin Choi All Rights Reserved.
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

#include "ActivitySensorBundleActivator.h"

#include <algorithm>
#include <sstream>
#include "ActivitySensorResource.h"


ActivitySensorBundleActivator *g_bundleActivity;

ActivitySensorBundleActivator::ActivitySensorBundleActivator()
{
    m_pResourceContainer = nullptr;
}

ActivitySensorBundleActivator::~ActivitySensorBundleActivator()
{
    m_pResourceContainer = nullptr;
}

void ActivitySensorBundleActivator::activateBundle(ResourceContainerBundleAPI *resourceContainer,
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

void ActivitySensorBundleActivator::deactivateBundle()
{
    std::vector< BundleResource::Ptr >::iterator itor;
    for (itor = m_vecResources.begin(); itor != m_vecResources.end();)
    {
        destroyResource(*itor);
    }
}

void ActivitySensorBundleActivator::createResource(resourceInfo resourceInfo)
{
    if (resourceInfo.resourceType == "oic.r.sensor")
    {
        static int ActivitySensorCount = 1;

        BundleResource::Ptr newResource = std::make_shared< ActivitySensorResource >();

        newResource->m_bundleId = m_bundleId;
        std::string indexCount;//string which will contain the indexCount
        stringstream convert; // stringstream used for the conversion
        convert << ActivitySensorCount++;//add the value of Number to the characters in the stream
        indexCount = convert.str();//set indexCount to the content of the stream

        newResource->m_uri = "/softsensor/activitysensor/" + indexCount;
        newResource->m_resourceType = resourceInfo.resourceType;
        newResource->m_mapResourceProperty = resourceInfo.resourceProperty;

        newResource->initAttributes();

        m_pResourceContainer->registerResource(newResource);
        m_vecResources.push_back(newResource);
    }
}

void ActivitySensorBundleActivator::destroyResource(BundleResource::Ptr resource)
{
    std::vector< BundleResource::Ptr >::iterator itor;

    itor = std::find(m_vecResources.begin(), m_vecResources.end(), resource);

    if (itor != m_vecResources.end())
    {
        m_pResourceContainer->unregisterResource(resource);
        m_vecResources.erase(itor);
    }
}

extern "C" void activitysensor_externalActivateBundle(ResourceContainerBundleAPI *resourceContainer,
        std::string bundleId)
{
    g_bundleActivity = new ActivitySensorBundleActivator();
    g_bundleActivity->activateBundle(resourceContainer, bundleId);
}

extern "C" void activitysensor_externalDeactivateBundle()
{
    g_bundleActivity->deactivateBundle();
    delete g_bundleActivity;
}

extern "C" void activitysensor_externalCreateResource(resourceInfo resourceInfo)
{
    g_bundleActivity->createResource(resourceInfo);
}

extern "C" void activitysensor_externalDestroyResource(BundleResource::Ptr pBundleResource)
{
    g_bundleActivity->destroyResource(pBundleResource);
}
