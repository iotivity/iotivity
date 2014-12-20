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



#ifndef SERVERSESSIONDATA_H_
#define SERVERSESSIONDATA_H_

#include "CMSerialization.h"
#include "CMServerResourceHandler.h"

namespace OC
{
    namespace Cm
    {
        namespace Connector
        {
            namespace Server
            {

                class ServerSessionData
                {

                    public:

                        ServerSessionData();

                        virtual ~ServerSessionData();

                        OC::Cm::Serialization::ISerializable    *m_pRequestPayload ;

                        OC::Cm::Serialization::ISerializable    *m_pResponsePayload ;

                        OC::Cm::Server::ResourceHandler::IResourceHandler   *m_pResourceHandler ;
                };

            } /* namespace Server */
        } /* namespace Connector */
    } /* namespace Cm */
} /* namespace OC */
#endif /* SERVERSESSIONDATA_H_ */
