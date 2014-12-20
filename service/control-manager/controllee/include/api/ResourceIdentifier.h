//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

#if !defined(_RESOURCEIDENTIFIER_H)
#define _RESOURCEIDENTIFIER_H

#include "string"

namespace OC
{
    namespace Cm
    {
        namespace Connector
        {
            /**
             * @class       ResourceIdentifier
             * @brief       This class holds the information to address a resource
             *
             * @since       1.0
             *
             * This class holds the information to address a resource
             *
             */
            class ResourceIdentifier
            {
                public:

                    /**
                     * ResourceIdentifier- Default constructor of this class
                     */
                    ResourceIdentifier();

                    /**
                     * ~ResourceIdentifier - Default destructor of this class
                     */
                    ~ResourceIdentifier();

                    /**
                     * getResourcePath - returns resource resource path
                     *
                     * @return type:string& resource path
                     */
                    const std::string &getResourcePath() const;

                    /**
                     * setResourcePath sets resource path
                     *
                     * @param resourcePath resource path
                     * @return none
                     */
                    void setResourcePath(const std::string &resourcePath);

                private:

                    std::string m_resourcePath;/**< Resource path */
            };
        }
    }
}

#endif  //_RESOURCEIDENTIFIER_H
