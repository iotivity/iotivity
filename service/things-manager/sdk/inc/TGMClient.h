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

/// @file   TGMClient.h

/// @brief  This file contains the declaration of classes and its members related to TGMClient.

#ifndef __OC_TGMCLIENT__
#define __OC_TGMCLIENT__

#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;

typedef std::function< void(std::vector< std::shared_ptr< OCResource > >) > CandidateCallback;
typedef std::function< void(std::vector< std::shared_ptr< OCResource > >) > CollectionPresenceCallback;

class TGMClient
{
public:
    /**
     * Constructor for TGMClient. Constructs a new TGMClient
     */
    TGMClient(void);

    /**
     * Virtual destructor
     */
    ~TGMClient(void);

    /**
     * API for candidate resources discovery.
     * Callback only call when all resource types found.
     *
     * @param resourceTypes - required resource types(called "candidate")
     * @param candidateCallback - callback. OCResource vector.
     *
     * @return OCStackResult return value of this API. Returns OC_STACK_OK if success.
     *
     * NOTE: OCStackResult is defined in ocstack.h.
     */
    OCStackResult findCandidateResources(std::vector< std::string > resourceTypes,
            CandidateCallback callback, int waitsec = -1);

    /**
     * API for Collection member's state subscribe.
     *
     * NOTE: NOT IMPLEMENT YET
     */
    OCStackResult subscribeCollectionPresence(OCResourceHandle&, CollectionPresenceCallback)
    {
        return OC_STACK_NOTIMPL;
    }

private:

    void onFoundResource(std::shared_ptr< OCResource > resource, int waitsec);
    void findPreparedRequest(std::map< std::vector< std::string >, CandidateCallback > &request);
    void lazyCallback(int second);

};

#endif  /* __OC_TGMCLIENT__*/
