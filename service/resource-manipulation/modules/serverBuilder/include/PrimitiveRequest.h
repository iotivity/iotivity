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

#ifndef __PRIMITIVEREQUEST_H
#define __PRIMITIVEREQUEST_H

#include <string>

namespace OIC
{
    namespace Service
    {

        class PrimitiveRequest
        {
        public:
            explicit PrimitiveRequest(const std::string& resourceUri) :
                    m_resourceUri(resourceUri)
            {
            }

            PrimitiveRequest& operator=(PrimitiveRequest&) = delete;

            std::string getResourceUri() const
            {
                return m_resourceUri;
            }

        private:
            std::string m_resourceUri;
        };

    }
}

#endif // __PRIMITIVEREQUEST_H
