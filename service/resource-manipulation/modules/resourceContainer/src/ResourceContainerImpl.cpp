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
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <jni.h>

#include "oc_logger.hpp"

using OC::oc_log_stream;
using namespace OIC::Service;

auto info_logger = []() -> boost::iostreams::stream<OC::oc_log_stream> &
{
    static OC::oc_log_stream ols(oc_make_ostream_logger);
    static boost::iostreams::stream<OC::oc_log_stream> os(ols);
    os->set_level(OC_LOG_INFO);
    os->set_module("ResourceContainerImpl");
    return os;
};

auto error_logger = []() -> boost::iostreams::stream<OC::oc_log_stream> &
{
    static OC::oc_log_stream ols(oc_make_ostream_logger);
    static boost::iostreams::stream<OC::oc_log_stream> os(ols);
    os->set_level(OC_LOG_ERROR);
    os->set_module("ResourceContainerImpl");
    return os;
};

using namespace std;
using namespace OIC::Service;

namespace OIC
{
    namespace Service
    {

        ResourceContainerImpl::ResourceContainerImpl()
        {
            // TODO Auto-generated constructor stub

        }

        ResourceContainerImpl::~ResourceContainerImpl()
        {
            // TODO Auto-generated destructor stub
        }

        bool has_suffix(const std::string &str, const std::string &suffix)
        {
            return str.size() >= suffix.size()
                    && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
        }

        void ResourceContainerImpl::startContainer(string configFile)
        {
            info_logger() << "Starting resource container" << endl;

            m_config = new Configuration(configFile);
            configInfo bundles;
            m_config->getConfiguredBundles(&bundles);

            for (int i = 0; i < bundles.size(); i++)
            {
                BundleInfo *bundleInfo = BundleInfo::createBundleInfo();
                bundleInfo->setPath(bundles[i]["path"]);
                bundleInfo->setVersion(bundles[i]["version"]);
                bundleInfo->setID(bundles[i]["id"]);
                if (!bundles[i]["activator"].empty())
                {
                    string activatorName = bundles[i]["activator"];
                    std::replace(activatorName.begin(), activatorName.end(), '.', '/');
                    ((BundleInfoInternal*) bundleInfo)->setActivatorName(activatorName);

                }
                info_logger() << "Init Bundle:" << bundles[i]["id"] << ";" << bundles[i]["path"]
                        << endl;
                registerBundle(bundleInfo);
                activateBundle(bundleInfo);
            }
        }

        void ResourceContainerImpl::registerJavaBundle(BundleInfo *bundleInfo)
        {
            info_logger() << "Registering Java bundle " << bundleInfo->getID() << endl;
            JavaVM * jvm;
            JNIEnv * env;
            JavaVMInitArgs vm_args;
            JavaVMOption options[3];

            BundleInfoInternal* bundleInfoInternal = (BundleInfoInternal*) bundleInfo;

            options[0].optionString = "-Djava.compiler=NONE";
            char classpath[1000];
            strcpy(classpath, "-Djava.class.path=");
            strcat(classpath, bundleInfo->getPath().c_str());

            options[1].optionString = classpath;

            options[2].optionString = "-Djava.library.path=.";

            vm_args.version = JNI_VERSION_1_4;
            vm_args.options = options;
            vm_args.nOptions = 3;
            vm_args.ignoreUnrecognized = 1;

            JNI_CreateJavaVM(&jvm, (void **) &env, &vm_args);

            m_bundleVM.insert(std::pair< string, JavaVM * >(bundleInfo->getID(), jvm));

            const char *className = bundleInfoInternal->getActivatorName().c_str();

            jclass bundleActivatorClass = env->FindClass(className);

            if (bundleActivatorClass == NULL)
            {
                error_logger() << "Cannot register bundle " << bundleInfoInternal->getID()
                        << " bundle activator(" << bundleInfoInternal->getActivatorName()
                        << ") not found " << endl;
                return;
            }

            jmethodID activateMethod = env->GetMethodID(bundleActivatorClass, "activateBundle",
                    "()V");

            if (activateMethod == NULL)
            {
                error_logger() << "Cannot register bundle " << bundleInfoInternal->getID()
                        << " activate bundle method not found " << endl;
                return;
            }
            bundleInfoInternal->setJavaBundleActivatorMethod(activateMethod);

            jmethodID deactivateMethod = env->GetMethodID(bundleActivatorClass, "activateBundle",
                    "()V");

            if (deactivateMethod == NULL)
            {
                error_logger() << "Cannot register bundle " << bundleInfoInternal->getID()
                        << " deactivate bundle method not found " << endl;
                return;
            }

            bundleInfoInternal->setJavaBundleDeactivatorMethod(deactivateMethod);

            jmethodID constructor;

            constructor = env->GetMethodID(bundleActivatorClass, "<init>", "(Ljava/lang/String;)V");

            jstring bundleID = env->NewStringUTF(bundleInfoInternal->getID().c_str());

            jobject bundleActivator = env->NewObject(bundleActivatorClass, constructor, bundleID);

            bundleInfoInternal->setJavaBundleActivatorObject(bundleActivator);

            bundleInfoInternal->setLoaded(true);
            info_logger() << "Bundle registered" << endl;
        }

        void ResourceContainerImpl::registerSoBundle(BundleInfo *bundleInfo)
        {
            char *error;

            activator_t *bundleActivator = NULL;
            deactivator_t *bundleDeactivator = NULL;

            //sstream << bundleInfo.path << std::ends;

            void *bundleHandle = NULL;
            bundleHandle = dlopen(bundleInfo->getPath().c_str(), RTLD_LAZY);

            if (bundleHandle != NULL)
            {
                bundleActivator = (activator_t *) dlsym(bundleHandle, "externalActivateBundle");
                bundleDeactivator = (deactivator_t *) dlsym(bundleHandle,
                        "externalDeactivateBundle");
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

        // loads the bundle
        void ResourceContainerImpl::registerBundle(BundleInfo *bundleInfo)
        {
            info_logger() << "Registering bundle: " << bundleInfo->getPath() << endl;

            m_bundles[bundleInfo->getID()] = ((BundleInfoInternal *) bundleInfo);

            if (has_suffix(bundleInfo->getPath(), ".jar"))
            {
                ((BundleInfoInternal *) bundleInfo)->setJavaBundle(true);
                registerJavaBundle(bundleInfo);
            }
            else
            {
                ((BundleInfoInternal *) bundleInfo)->setJavaBundle(false);
                registerSoBundle(bundleInfo);
            }
        }

        void ResourceContainerImpl::activateJavaBundle(string bundleId)
        {
            info_logger() << "Activating java bundle" << endl;
            JavaVM* vm = getJavaVM(bundleId);
            BundleInfoInternal* bundleInfoInternal = (BundleInfoInternal*) m_bundles[bundleId];
            JNIEnv * env;
            int envStat = vm->GetEnv((void **) &env, JNI_VERSION_1_4);

            if (envStat == JNI_EDETACHED)
            {
                if (vm->AttachCurrentThread((void**) &env, NULL) != 0)
                {
                    error_logger() << "Failed to attach " << endl;
                }
            }
            else if (envStat == JNI_EVERSION)
            {
                error_logger() << "Env: version not supported " << endl;
            }

            env->CallVoidMethod(bundleInfoInternal->getJavaBundleActivatorObject(),
                    bundleInfoInternal->getJavaBundleActivatorMethod());
        }

        void ResourceContainerImpl::activateSoBundle(string bundleId)
        {
            activator_t *bundleActivator = m_bundles[bundleId]->getBundleActivator();

            if (bundleActivator != NULL)
            {
                bundleActivator(this, m_bundles[bundleId]->getID());
                m_bundles[bundleId]->setActivated(true);
            }
            else
            {
                //Unload module and return error
                error_logger() << "Activation unsuccessful." << endl;
            }

            BundleInfoInternal* bundleInfoInternal = (BundleInfoInternal*) m_bundles[bundleId];
            bundleInfoInternal->setActivated(true);
        }

        void ResourceContainerImpl::activateBundle(string id)
        {

            info_logger() << "Activating bundle: " << m_bundles[id]->getID() << endl;

            if (m_bundles[id]->getJavaBundle())
            {
                activateJavaBundle(id);
            }
            else
            {
                activateSoBundle(id);
            }

            info_logger() << "Bundle activated: " << m_bundles[id]->getID() << endl;

        }

        void ResourceContainerImpl::activateBundle(BundleInfo *bundleInfo)
        {
            if (((BundleInfoInternal *) bundleInfo)->isLoaded())
            {
                activateBundle(bundleInfo->getID());
            }
        }

        void ResourceContainerImpl::deactivateBundle(BundleInfo *bundleInfo)
        {
            if (((BundleInfoInternal *) bundleInfo)->isActivated())
            {
                deactivateBundle(bundleInfo->getID());
            }
        }


        void ResourceContainerImpl::deactivateJavaBundle(string bundleId)
        {
            info_logger() << "Deactivating java bundle" << endl;
            JavaVM* vm = getJavaVM(bundleId);
            BundleInfoInternal* bundleInfoInternal = (BundleInfoInternal*) m_bundles[bundleId];
            JNIEnv * env;
            int envStat = vm->GetEnv((void **) &env, JNI_VERSION_1_4);

            if (envStat == JNI_EDETACHED)
            {
                if (vm->AttachCurrentThread((void**) &env, NULL) != 0)
                {
                    error_logger() << "Failed to attach " << endl;
                }
            }
            else if (envStat == JNI_EVERSION)
            {
                error_logger() << "Env: version not supported " << endl;
            }

            env->CallVoidMethod(bundleInfoInternal->getJavaBundleActivatorObject(),
                    bundleInfoInternal->getJavaBundleDeactivatorMethod());
        }

        void ResourceContainerImpl::deactivateSoBundle(string id)
        {

            deactivator_t *bundleDeactivator = m_bundles[id]->getBundleDeactivator();
            info_logger() << "De-activating bundle: " << m_bundles[id]->getID() << endl;

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

        void ResourceContainerImpl::deactivateBundle(string id)
        {

            if (m_bundles[id]->getJavaBundle())
            {
                deactivateJavaBundle(id);
            }
            else
            {
                deactivateSoBundle(id);
            }
        }

        std::list< string > ResourceContainerImpl::listBundleResources(string id)
        {
            std::list< string > ret;
            return ret;
        }

        void ResourceContainerImpl::registerResource(BundleResource *resource)
        {

            string strUri = resource->m_uri;
            string strResourceType = resource->m_resourceType;

            info_logger() << "Registration of resource " << strUri << "," << strResourceType
                    << endl;

            ResourceObject::Ptr server = ResourceObject::Builder(strUri, strResourceType,
                    "DEFAULT_INTERFACE").setObservable(true).setDiscoverable(true).build();

            m_mapServers[strUri] = server;
            m_mapResources[strUri] = resource;

            resource->registerObserver(this);
            server->setGetRequestHandler(
                    std::bind(&ResourceContainerImpl::getRequestHandler, this,
                            std::placeholders::_1, std::placeholders::_2));

            server->setSetRequestHandler(
                    std::bind(&ResourceContainerImpl::setRequestHandler, this,
                            std::placeholders::_1, std::placeholders::_2));
            info_logger() << "Registration finished " << strUri << "," << strResourceType << endl;
        }

        PrimitiveGetResponse ResourceContainerImpl::getRequestHandler(
                const PrimitiveRequest &request, const ResourceAttributes &attributes)
        {
            ResourceAttributes attr;

            if (m_mapServers.find(request.getResourceUri()) != m_mapServers.end()
                    && m_mapResources.find(request.getResourceUri()) != m_mapResources.end())
            {
                for (string attrName : m_mapResources[request.getResourceUri()]->getAttributeNames())
                {
                    attr[attrName] = m_mapResources[request.getResourceUri()]->getAttribute(
                            attrName);
                }
            }

            return PrimitiveGetResponse::create(attr);
        }

        PrimitiveSetResponse ResourceContainerImpl::setRequestHandler(
                const PrimitiveRequest &request, const ResourceAttributes &attributes)
        {
            ResourceAttributes attr = attributes;

            if (m_mapServers.find(request.getResourceUri()) != m_mapServers.end()
                    && m_mapResources.find(request.getResourceUri()) != m_mapResources.end())
            {
                for (string attrName : m_mapResources[request.getResourceUri()]->getAttributeNames())
                {
                    if (!attr[attrName].toString().empty())
                    {
                        m_mapResources[request.getResourceUri()]->setAttribute(attrName,
                                attr[attrName].toString());
                    }
                }
            }

            return PrimitiveSetResponse::create(attr);
        }

        void ResourceContainerImpl::unregisterResource(BundleResource *resource)
        {
            // To be implemented
        }

        void ResourceContainerImpl::unregisterBundle(BundleInfo *bundleInfo)
        {
            if (((BundleInfoInternal *) bundleInfo)->isLoaded()
                    && !((BundleInfoInternal *) bundleInfo)->isActivated())
            {
                unregisterBundle(bundleInfo->getID());
            }
        }

        void ResourceContainerImpl::unregisterBundle(string id)
        {
            void *bundleHandle = m_bundles[id]->getBundleHandle();
            info_logger() << "Unregister bundle: " << m_bundles[id]->getID() << ", "
                    << m_bundles[id]->getID() << endl;
            char *error;
            dlclose(bundleHandle);
            if ((error = dlerror()) != NULL)
            {
                error_logger() << error << endl;
            }
        }

        ResourceContainerImpl *ResourceContainerImpl::getImplInstance()
        {
            static ResourceContainerImpl m_instance;
            return &m_instance;
        }

        void ResourceContainerImpl::getBundleConfiguration(std::string bundleId,
                configInfo *configOutput)
        {
            m_config->getBundleConfiguration(bundleId, (configInfo *) configOutput);
        }

        void ResourceContainerImpl::getResourceConfiguration(std::string bundleId,
                std::vector< resourceInfo > *configOutput)
        {
            m_config->getResourceConfiguration(bundleId, configOutput);
        }

        void ResourceContainerImpl::onNotificationReceived(std::string strResourceUri)
        {
            info_logger() << "ResourceContainerImpl::onNotificationReceived\n\tnotification from "
                    << strResourceUri << ".\n";

            /*if (m_mapServers.find(strResourceUri) != m_mapServers.end())
             {
             //m_mapServers[strResourceUri]->notify();
             }*/
        }

        void ResourceContainerImpl::addBundle(string bundleId, string bundleUri, string bundlePath,
                std::map< string, string > params)
        {
            // To be implemented
        }

        void ResourceContainerImpl::removeBundle(string bundleId)
        {
            // To be implemented
        }

        std::list< BundleInfo* > ResourceContainerImpl::listBundles()
        {
            std::list< BundleInfo* > ret;
            return ret;
        }

        void ResourceContainerImpl::addResourceConfig(string bundleId, string resourceUri,
                std::map< string, string > params)
        {
            // To be implemented
        }

        void ResourceContainerImpl::removeResourceConfig(string bundleId, string resourceUri)
        {
            // To be implemented
        }

        void ResourceContainerImpl::startBundle(string bundleId)
        {
            activateBundle(m_bundles[bundleId]);
        }

        void ResourceContainerImpl::stopBundle(string bundleId)
        {
            deactivateBundle(m_bundles[bundleId]);
        }

        void ResourceContainerImpl::stopContainer()
        {
            // deactivate all bundles and unload them
        }

        JavaVM *ResourceContainerImpl::getJavaVM(string bundleId)
        {
            return m_bundleVM[bundleId];
        }
    }
}

