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

/// @file   GroupManager.h

/// @brief  This file contains the declaration of classes and its members related to GroupManager

#ifndef __OC_GROUPMANAGER__
#define __OC_GROUPMANAGER__

#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;

namespace OIC
{
typedef std::function< void(std::vector< std::shared_ptr< OCResource > >) > CandidateCallback;
typedef std::function< void(std::string, OCStackResult) > CollectionPresenceCallback;

typedef std::function< void(const HeaderOptions&, const OCRepresentation&, const int) > GetCallback;
typedef std::function< void(const HeaderOptions&, const OCRepresentation&, const int) > PostCallback;
typedef std::function< void(const HeaderOptions&, const OCRepresentation&, const int) > PutCallback;

class Capability
{
public:
    std::string capability;
    std::string status;
};

class Action
{
public:
    Action() :
            target("")
    {
    }
    ~Action()
    {
        listOfCapability.clear();
    }
    std::string target;

    std::vector< Capability* > listOfCapability;
};

class ActionSet
{
public:
    ActionSet() :
            actionsetName("")
    {
    }
    ~ActionSet()
    {
        listOfAction.clear();
    }
    std::string actionsetName;

    std::vector< Action* > listOfAction;
};

class GroupManager
{
public:
    /**
     * Constructor for GroupManager. Constructs a new GroupManager
     */
    GroupManager(void);

    /**
     * Virtual destructor
     */
    ~GroupManager(void);

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
    OCStackResult subscribeCollectionPresence(std::shared_ptr< OCResource > resource,
            CollectionPresenceCallback);

private:

    void onFoundResource(std::shared_ptr< OCResource > resource, int waitsec);
    void findPreparedRequest(std::map< std::vector< std::string >, CandidateCallback > &request);
    void lazyCallback(int second);

    void onGetForPresence(const HeaderOptions& headerOptions, const OCRepresentation& rep,
            const int eCode, CollectionPresenceCallback callback);
    void checkCollectionRepresentation(const OCRepresentation& rep,
            CollectionPresenceCallback callback);
    void collectionPresenceHandler(OCStackResult result, const unsigned int nonce,
            const std::string& hostAddress, std::string host, std::string uri);

    /**
     *   API for Collection(Group) action.
     */

public:
    std::string getStringFromActionSet(const ActionSet *newActionSet);
    ActionSet* getActionSetfromString(std::string desc);

    OCStackResult addActionSet(std::shared_ptr< OCResource > resource,
            const ActionSet* newActionSet, PutCallback cb);
    OCStackResult executeActionSet(std::shared_ptr< OCResource > resource,
            std::string actionsetName, PostCallback cb);
    OCStackResult getActionSet(std::shared_ptr< OCResource > resource, std::string actionsetName,
            PostCallback cb);
    OCStackResult deleteActionSet(std::shared_ptr< OCResource > resource, std::string actionsetName,
            PostCallback);
};
}
#endif  /* __OC_GROUPMANAGER__*/
