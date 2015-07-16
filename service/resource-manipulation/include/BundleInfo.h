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

#ifndef BUNDLEINFO_H_
#define BUNDLEINFO_H_

#include <string>

using namespace std;

namespace OIC
{
    namespace Service
    {
        /*
         * Describes a bundle with resources, that can be loaded dynamically.
         */
        class BundleInfo
        {
        public:
            BundleInfo();
            virtual ~BundleInfo();
            virtual void setID(string name) = 0;
            virtual string getID() = 0;
            virtual void setPath(string path) = 0;
            virtual string getPath() = 0;
            virtual void setActivatorName(string activator) = 0;
            virtual string getActivatorName() = 0;
            virtual void setLibraryPath(string libpath) = 0;
            virtual string getLibraryPath() = 0;
            virtual void setVersion(string version) = 0;
            virtual string getVersion() = 0;
            static BundleInfo* build();
        protected:
            string m_ID, m_path, m_version;
        };
    }
}

#endif /* BUNDLEINFO_H_ */
