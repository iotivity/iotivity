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

#ifndef RESOURCECONTAINERIMPL_H_
#define RESOURCECONTAINERIMPL_H_

#include "ResourceContainer.h"
#include "ResourceContainerBundleAPI.h"
#include "BundleInfoInternal.h"

#include "PrimitiveRequest.h"
#include "PrimitiveResponse.h"
#include "ResourceObject.h"

#include "jni.h"
#include <map>

using namespace OIC::Service;

namespace OIC
{
    namespace Service
    {

        class ResourceContainerImpl: public ResourceContainer, public ResourceContainerBundleAPI
        {
            public:
                ResourceContainerImpl();
                virtual ~ResourceContainerImpl();

                // methods from ResourceContainer
                void startContainer(string configFile);
                void stopContainer();
                void activateBundle(BundleInfo *bundleInfo);
                void deactivateBundle(BundleInfo *bundleInfo);
                void activateBundle(string bundleId);
                void deactivateBundle(string bundleId);
                void registerBundle(BundleInfo *bundleinfo);
                void unregisterBundle(BundleInfo *bundleinfo);
                void unregisterBundle(string id);

                // methods from ResourceContainerBundleAPI
                void registerResource(BundleResource *resource);
                void unregisterResource(BundleResource *resource);

                void getBundleConfiguration(std::string bundleId, configInfo *configOutput);
                void getResourceConfiguration(std::string bundleId,
                                              std::vector< resourceInfo > *configOutput);

                PrimitiveGetResponse getRequestHandler(const PrimitiveRequest &request,
                                                       const ResourceAttributes &attributes);
                PrimitiveSetResponse setRequestHandler(const PrimitiveRequest &request,
                                                       const ResourceAttributes &attributes);

                void onNotificationReceived(std::string strResourceUri);

                static ResourceContainerImpl *getImplInstance();
                static ResourceObject::Ptr buildResourceObject(string strUri, string strResourceType);

                void startBundle(string bundleId);
                void stopBundle(string bundleId);

                void addBundle(string bundleId, string bundleUri, string bundlePath,
                               std::map<string, string> params);
                void removeBundle(string bundleId);

                std::list<BundleInfo *> listBundles();

                void addResourceConfig(string bundleId, string resourceUri, std::map<string, string> params);
                void removeResourceConfig(string bundleId, string resourceUri);

                std::list<string> listBundleResources(string bundleId);

                JavaVM *getJavaVM(string bundleId);


            private:
                map< std::string, BundleInfoInternal * > m_bundles; // <bundleID, bundleInfo>
                map< std::string, ResourceObject::Ptr > m_mapServers; //<uri, serverPtr>
                map< std::string, BundleResource * > m_mapResources; //<uri, resourcePtr>
                string m_configFile;
                Configuration *m_config = NULL;
                map<string, JavaVM *> m_bundleVM;

                void registerJavaBundle(BundleInfo *bundleInfo);
                void registerSoBundle(BundleInfo *bundleInfo);
                void activateJavaBundle(string bundleId);
                void activateSoBundle(string bundleId);
                void deactivateJavaBundle(string bundleId);
                void deactivateSoBundle(string bundleId);
        };
    }
}
#endif
