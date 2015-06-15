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

#ifndef BUNDLEINFOINTERNAL_H_
#define BUNDLEINFOINTERNAL_H_

#include <string>
#include "BundleInfo.h"
#include "ResourceContainerBundleAPI.h"

using namespace std;
using namespace RC;

namespace RC
{
    typedef void activator_t(ResourceContainerBundleAPI *, std::string bundleId);
    typedef void deactivator_t(void);

    class BundleInfoInternal: public BundleInfo
    {
        public:
            BundleInfoInternal();
            BundleInfoInternal(BundleInfo *info);
            virtual ~BundleInfoInternal();
            void setID(string id);
            string getID();
            void setPath(string path);
            string getPath();
            void setVersion(string version);
            string getVersion();

            void setLoaded(bool loaded);
            bool isLoaded();
            void setActivated(bool activated);
            bool isActivated();
            int getId();
            void setId(int id);

            void setBundleActivator(activator_t *);
            activator_t *getBundleActivator();

            void setBundleDeactivator(deactivator_t *);
            deactivator_t *getBundleDeactivator();

            void setBundleHandle(void *);
            void *getBundleHandle();

        private:
            bool m_loaded, m_activated;
            int m_id;
            activator_t *m_activator;
            deactivator_t *m_deactivator;
            void *m_bundleHandle;
    };
}

#endif /* BUNDLEINFOINTERNAL_H_ */
