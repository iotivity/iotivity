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

#if !defined(_CDSERVERLISTENER_H)
#define _CDSERVERLISTENER_H

#include <string>
#include "OCRepresentation.h"
#include "ISerializable.h"


namespace OC
{
    /**
     * @class   CDServerListener
     * @brief   Contract class for the bridge between SHP and OIC
     */
    class CDServerListener
    {
        public:

            /**
             * ~CDServerListener - Default destructor of this class
             */
            virtual ~CDServerListener() {}

            /**
             * handleRequest - listener method which will be invoked when server receives a request
             * @param - ocRequest - The request from lower layers
             * @param - ocRequest - The reply to lower layers (output)
             */

            virtual void handleRequest(const std::shared_ptr<OC::OCResourceRequest> ocRequest,
                                       std::shared_ptr<OC::OCResourceResponse> ocResponse) = 0 ;
    };
}


#endif  //_CDSERVERLISTENER_H
