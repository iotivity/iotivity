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
#include "CMConfig.h"

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
            class CMAPI ResourceIdentifier
            {
                public:

                    /**
                     * Default constructor of this class
                     */
                    ResourceIdentifier() ;

                    /**
                     * Default destructor of this class
                     */
                    ~ResourceIdentifier() ;

                    /**
                     * returns resource resource path
                     *
                     * @return resource path
                     */
                    const std::string &getResourcePath() const;

                    /**
                     * sets resource path
                     *
                     * @param resourcePath resource path
                     */
                    void setResourcePath(const std::string &resourcePath);

                private:

                    std::string m_resourcePath;
            };
        }
    }
}

#endif  //_RESOURCEIDENTIFIER_H
