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

#ifndef BUNDLERESOURCE_H_
#define BUNDLERESOURCE_H_

#include <map>
#include <string>

namespace RC
{
    class BundleResource
    {
        public:
            BundleResource();
            virtual ~BundleResource();

            virtual void onGetRequest() = 0;
            virtual void onSetRequest() = 0;

            std::map <std::string, std::string> m_mapAttributes;
    };

    class SoftSensorResource : public BundleResource
    {
        public:
            SoftSensorResource();
            virtual ~SoftSensorResource();

            virtual void runLogic() = 0;

            std::map <std::string, std::string> m_mapInputs;
    };
}

#endif
