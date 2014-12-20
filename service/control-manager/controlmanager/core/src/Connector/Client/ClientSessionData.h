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



#ifndef CLIENTSESSIONDATA_H_
#define CLIENTSESSIONDATA_H_

#include "CMSerialization.h"
#include "CMClientConnector.h"

namespace OC
{
    namespace Cm
    {
        namespace Connector
        {
            namespace Client
            {

                class ClientSessionData
                {

                    public:

                        ClientSessionData();

                        virtual ~ClientSessionData();

                        /**
                         * This method returns response handlers of this session
                         *
                         * @return response handler of this session
                         */
                        OC::Cm::Client::Resource::IResourceResponseListener *getResponseHandler() const;

                        /**
                         * Configures response handler
                         *
                         * @param[in] responseHandler handler of this session
                         */
                        void setResponseHandler(OC::Cm::Client::Resource::IResourceResponseListener *responseHandler);

                        OC::Cm::Serialization::ISerializable    *m_pRequestPayload ;

                        OC::Cm::Serialization::ISerializable    *m_pResponsePayload ;

                        bool m_isControlRequest;

                        int m_serverType;

                    private:

                        OC::Cm::Client::Resource::IResourceResponseListener *m_pResponseHandler ;
                };

            } /* namespace Client */
        }
    } /* namespace Cm */
} /* namespace OC */
#endif /* CLIENTSESSIONDATA_H_ */
