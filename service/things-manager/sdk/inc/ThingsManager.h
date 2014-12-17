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

/// @file   ThingsManager.h

/// @brief  This file contains the declaration of classes and its members related to TGMClient.

#ifndef __OC_THINGSMANAGER__
#define __OC_THINGSMANAGER__

#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include "OCPlatform.h"
#include "OCApi.h"
#include "GroupManager.h"

using namespace OC;


class ThingsManager
{
public:
    /**
     * Constructor for TGMClient. Constructs a new TGMClient
     */
    ThingsManager(void);

    /**
     * Virtual destructor
     */
    ~ThingsManager(void);

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
            std::function< void(std::vector< std::shared_ptr< OCResource > >) >  callback, int waitsec = -1);

    /**
     * API for Collection member's state subscribe.
     *
     *
     */
    OCStackResult subscribeCollectionPresence(std::shared_ptr< OCResource >, std::function< void(std::string, OCStackResult) > );

	// Group Synchronization
	OCStackResult findGroup (std::vector <std::string> collectionResourceTypes, FindCallback resourceHandler);
	OCStackResult createGroup (std::string collectionResourceType);
	OCStackResult joinGroup (std::string collectionResourceType, OCResourceHandle resourceHandle);
	OCStackResult joinGroup (const std::shared_ptr<OCResource> resource, OCResourceHandle resourceHandle);
	OCStackResult leaveGroup (std::string collectionResourceType, OCResourceHandle resourceHandle);
	void deleteGroup (std::string collectionResourceType);
	std::map<std::string, OCResourceHandle> getGroupList ();

    // Things Configuration
    OCStackResult updateConfigurations(std::shared_ptr< OCResource > resource, std::map<std::string, std::string> configurations,
                std::function< void(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode) > callback);
    OCStackResult getConfigurations(std::shared_ptr< OCResource > resource, std::vector<std::string> configurations,
                std::function< void(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode) > callback);
    std::string getListOfSupportedConfigurationUnits();
    OCStackResult doBootstrap(
                std::function< void(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode) > callback);

    // Things Diagnostics
    OCStackResult reboot(std::shared_ptr< OCResource > resource,
                std::function< void(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode) > callback);
    OCStackResult factoryReset(std::shared_ptr< OCResource > resource,
                std::function< void(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode) > callback);




    // Group Action.
    std::string getStringFromActionSet(const ActionSet *newActionSet);
    ActionSet* getActionSetfromString(std::string desc);

    OCStackResult addActionSet(std::shared_ptr< OCResource > resource, const ActionSet* newActionSet, PutCallback cb);
    OCStackResult executeActionSet(std::shared_ptr< OCResource > resource, std::string actionsetName, PostCallback cb);
    OCStackResult getActionSet(std::shared_ptr< OCResource > resource, std::string actionsetName, GetCallback cb);
    OCStackResult deleteActionSet(std::shared_ptr< OCResource > resource, std::string actionsetName, PostCallback);

};

#endif  /* __OC_THINGSMANAGER__*/
