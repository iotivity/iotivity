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

#include "SoftSensorResource.h"
#include <algorithm>

using namespace OIC::Service;
namespace OIC
{
    namespace Service
    {
        SoftSensorResource::SoftSensorResource()
        {

        }

        SoftSensorResource::~SoftSensorResource()
        {

        }

        void SoftSensorResource::initAttributes()
        {
            std::vector< std::map< std::string, std::string > >::iterator itor;

            // initialize input attributes
            for (itor = m_mapResourceProperty["input"].begin(); itor != m_mapResourceProperty["input"].end();
                 itor++)
            {
                m_inputList.push_back((*itor)["name"]);
                BundleResource::setAttribute((*itor)["name"], nullptr);
            }

            // initialize output attributes
            for (itor = m_mapResourceProperty["output"].begin(); itor != m_mapResourceProperty["output"].end();
                 itor++)
                BundleResource::setAttribute((*itor)["name"], nullptr);
        }

        RCSResourceAttributes &SoftSensorResource::getAttributes()
        {
            return BundleResource::getAttributes();
        }

        void SoftSensorResource::setAttribute(std::string key, RCSResourceAttributes::Value &&value)
        {
            BundleResource::setAttribute(key, value.toString());

            if (std::find(m_inputList.begin(), m_inputList.end(), key) != m_inputList.end())
                executeLogic();
        }

        RCSResourceAttributes::Value SoftSensorResource::getAttribute(const std::string &key)
        {
            return BundleResource::getAttribute(key);
        }
    }
}

