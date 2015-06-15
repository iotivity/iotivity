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

#include "ResourceContainerImpl.h"
#include "BundleActivator.h"
#include "ResourceContainer.h"
#include "BundleInfoInternal.h"
#include "logger.h"
#include "PrimitiveRequest.h"
#include "PrimitiveResponse.h"
#include "PrimitiveServerResource.h"


#include <dlfcn.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include "oc_logger.hpp"

using OC::oc_log_stream;

/* Annother way to create a context: */
auto info_logger = []() -> boost::iostreams::stream<OC::oc_log_stream> &
{
    static OC::oc_log_stream ols(oc_make_ostream_logger);
    static boost::iostreams::stream<OC::oc_log_stream> os(ols);
    os->set_level(OC_LOG_INFO);
    os->set_module("ResourceContainerImpl");
    return os;
};

/* Annother way to create a context: */
auto error_logger = []() -> boost::iostreams::stream<OC::oc_log_stream> &
{
    static OC::oc_log_stream ols(oc_make_ostream_logger);
    static boost::iostreams::stream<OC::oc_log_stream> os(ols);
    os->set_level(OC_LOG_ERROR);
    os->set_module("ResourceContainerImpl");
    return os;
};

using namespace std;
using namespace RC;

namespace RC
{

    ResourceContainerImpl::ResourceContainerImpl()
    {
        // TODO Auto-generated constructor stub

    }

    ResourceContainerImpl::~ResourceContainerImpl()
    {
        // TODO Auto-generated destructor stub
    }

    void ResourceContainerImpl::init()
    {

    }

    void ResourceContainerImpl::init(string configFile)
    {
        m_config = new Configuration(configFile);
        Configuration::configInfo bundles;
        m_config->getConfiguredBundles(&bundles);
        for (int i = 0; i < bundles.size(); i++)
        {
            BundleInfo *bundleInfo = BundleInfo::createBundleInfo();
            bundleInfo->setPath(bundles[i]["path"]);
            bundleInfo->setVersion(bundles[i]["version"]);
            bundleInfo->setID(bundles[i]["id"]);
            cout << "Init Bundle:" << bundles[i]["id"] << ";" << bundles[i]["path"] << endl;
            registerBundle(bundleInfo);
            activateBundle(bundleInfo);
        }
    }

    // loads the bundle
    void ResourceContainerImpl::registerBundle(BundleInfo *bundleInfo)
    {
        info_logger() << "Registering bundle: " << bundleInfo->getPath() << endl;

        m_bundles.push_back((BundleInfoInternal *) bundleInfo);
        ((BundleInfoInternal *) bundleInfo)->setId(m_bundles.size() - 1);

        char *error;

        activator_t *bundleActivator = NULL;
        deactivator_t *bundleDeactivator = NULL;

        //sstream << bundleInfo.path << std::ends;

        void *bundleHandle = NULL;
        bundleHandle = dlopen(bundleInfo->getPath().c_str(), RTLD_LAZY);

        if (bundleHandle != NULL)
        {
            bundleActivator = (activator_t *) dlsym(bundleHandle, "externalActivateBundle");
            bundleDeactivator = (deactivator_t *) dlsym(bundleHandle, "externalDeactivateBundle");
            if ((error = dlerror()) != NULL)
            {
                error_logger() << error << endl;
            }
            else
            {
                ((BundleInfoInternal *) bundleInfo)->setBundleActivator(bundleActivator);
                ((BundleInfoInternal *) bundleInfo)->setBundleDeactivator(bundleDeactivator);
                ((BundleInfoInternal *) bundleInfo)->setLoaded(true);
                ((BundleInfoInternal *) bundleInfo)->setBundleHandle(bundleHandle);
            }
        }
        else
        {
            if ((error = dlerror()) != NULL)
            {
                error_logger() << error << endl;
            }
        }
    }

    void ResourceContainerImpl::activateBundle(int id)
    {
        activator_t *bundleActivator = m_bundles[id]->getBundleActivator();
        info_logger() << "Activating bundle: " << m_bundles[id]->getID() << ", "
                      << m_bundles[id]->getId() << endl;

        if (bundleActivator != NULL)
        {
            bundleActivator(this, m_bundles[id]->getID());
            m_bundles[id]->setActivated(true);
        }
        else
        {
            //Unload module and return error
            error_logger() << "Activation unsuccessful." << endl;
        }
    }

    void ResourceContainerImpl::activateBundle(BundleInfo *bundleInfo)
    {
        if (((BundleInfoInternal *) bundleInfo)->isLoaded())
        {
            activateBundle(bundleInfo->getId());
        }
    }

    void ResourceContainerImpl::deactivateBundle(BundleInfo *bundleInfo)
    {
        if (((BundleInfoInternal *) bundleInfo)->isActivated())
        {
            deactivateBundle(bundleInfo->getId());
        }
    }

    void ResourceContainerImpl::deactivateBundle(int id)
    {
        deactivator_t *bundleDeactivator = m_bundles[id]->getBundleDeactivator();
        info_logger() << "De-activating bundle: " << m_bundles[id]->getID() << ", "
                      << m_bundles[id]->getId() << endl;

        if (bundleDeactivator != NULL)
        {
            bundleDeactivator();
            m_bundles[id]->setActivated(false);
        }
        else
        {
            //Unload module and return error
            error_logger() << "De-activation unsuccessful." << endl;
        }
    }

    void ResourceContainerImpl::activateBundleByName(string name)
    {

    }

    void ResourceContainerImpl::deactivateBundleByName(string id)
    {

    }

    vector< Resource * > ResourceContainerImpl::listBundleResources(string id)
    {
        vector< Resource * > ret;
        return ret;
    }

    void ResourceContainerImpl::registerResource(BundleResource *resource)
    {
        cout << "Register resource called.\n";
        PrimitiveServerResource::Ptr server =
            PrimitiveServerResource::Builder("/softSensors/discomfortIndex/1",
                                             "core.softSensor",
                                             "DEFAULT_INTERFACE").setObservable(false).setDiscoverable(true).create();

        for (auto i : resource->m_mapAttributes)
        {
            cout << "Setting attribute " << i.first.c_str() << endl;
            server->setAttribute(i.first.c_str(), i.second.c_str());
        }

    }

    void ResourceContainerImpl::unregisterResource(BundleResource *resource)
    {

    }

    void ResourceContainerImpl::unregisterBundle(BundleInfo *bundleInfo)
    {
        if (((BundleInfoInternal *) bundleInfo)->isLoaded()
            && !((BundleInfoInternal *) bundleInfo)->isActivated())
        {
            unregisterBundle(bundleInfo->getId());
        }
    }

    void ResourceContainerImpl::unregisterBundle(int id)
    {
        void *bundleHandle = m_bundles[id]->getBundleHandle();
        info_logger() << "Unregister bundle: " << m_bundles[id]->getID() << ", "
                      << m_bundles[id]->getId() << endl;
        char *error;
        dlclose(bundleHandle);
        if ((error = dlerror()) != NULL)
        {
            error_logger() << error << endl;
        }
    }

    ResourceContainerImpl *ResourceContainerImpl::getImplInstance()
    {
        ResourceContainerImpl *ret = new ResourceContainerImpl();
        return ret;
    }

    void ResourceContainerImpl::getCommonConfiguration(configInfo *configOutput)
    {
        m_config->getCommonConfiguration((Configuration::configInfo *) configOutput);
    }

    void ResourceContainerImpl::getBundleConfiguration(std::string bundleId, configInfo *configOutput)
    {
        m_config->getBundleConfiguration(bundleId, (Configuration::configInfo *) configOutput);
    }

    void ResourceContainerImpl::getResourceConfiguration(std::string bundleId,
            std::vector<resourceInfo> *configOutput)
    {
        m_config->getResourceConfiguration(bundleId, configOutput);
    }
}
