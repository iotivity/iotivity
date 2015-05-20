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

/**
 * @file
 *
 * This file contains the declaration of classes and its members related to
 * ThingsDiagnostics.
 */

#ifndef __OC_THINGSDIAGNOSTICS__
#define __OC_THINGSDIAGNOSTICS__

#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include "OCPlatform.h"
#include "OCApi.h"
#include "GroupManager.h"

using namespace OC;
namespace OIC
{
    /// Declearation of Diagnostics Callback funtion type
    typedef std::function<
            void(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode)
            > DiagnosticsCallback;

    /**
     *  @brief
     *  The following class is used as a item stacking in request queue. The class stores a request
     *  and referential information (e.g., a diagnostics name, a target resource object, a callback
     *  function passed from the applications, and a update value). When the function for updating/
     *  getting diagnostics value is called from applications, this class instance is created and
     *  stored in the request queue. The queue is maintained in a std::map structure so if desiring
     *  to find a specific request, you can find it by querying a diagnostics name.
     */
    class DiagnosticsRequestEntry
    {
    public:
        DiagnosticsRequestEntry(std::string ID, DiagnosticsCallback callback,
                std::shared_ptr< OCResource > resource, std::string updateVal);

        // Diagnostics Name (used in key value in std::map structure)
        // e.g., reboot and factoryset
        std::string m_ID;

        // Reference callback pointer
        DiagnosticsCallback m_callback;

        // Reference resource object
        std::shared_ptr< OCResource > m_resource;

        // Update value only used for diagnostics update (always "true")
        std::string m_updateVal;
    };

    /**
     *  @brief
     *  The following class is used to store providing diagnostics name and its relevant information
     *  The relevant information includes a brief description, uri, and attribute key.
     *  Note that a developer only specifies a diagnostics name, not URI nor attribute key, to
     *  update a value to a remote. Thus, using diagnostics name, we convert it to more specific
     *  information (i.e. uri and attribute key) to send a request. This class is reponsible to
     *  storing these information.
     */
    class DiagnosticsUnitInfo
    {
    public:
        std::string m_name;
        std::string m_attribute;
        std::string m_uri;

        DiagnosticsUnitInfo(std::string name, std::string attribute, std::string uri);

        // If a developer wants to know a list of configuration names, gives it in JSON format.
        std::string getJSON();
    };

#define NUMDIAGUNIT 3
    typedef std::string DiagnosticsName;
    typedef std::string DiagnosticsValue;

    class ThingsDiagnostics
    {
    public:
        /**
         * Constructor for ThingsDiagnostics. Constructs a new ThingsDiagnostics
         */
        ThingsDiagnostics(void);

        /**
         * Virtual destructor
         */
        ~ThingsDiagnostics(void);

        static ThingsDiagnostics *thingsDiagnosticsInstance;
        static ThingsDiagnostics* getInstance();
        void deleteInstance();

        void setGroupManager(GroupManager *groupmanager);

        /**
         * API to make things reboot
         * Callback call when a response arrives.
         *
         * @param resource - resource pointer representing the target group
         * @param callback - callback.
         *
         * @return OCStackResult return value of this API. Returns OC_STACK_OK if success.
         *
         * NOTE: OCStackResult is defined in ocstack.h.
         */
        OCStackResult reboot(std::shared_ptr< OCResource > resource, DiagnosticsCallback callback);

        /**
         * API for factory reset on device
         * Callback call when a response arrives.
         *
         * @param resource - resource pointer representing the target group
         * @param callback - callback.
         *
         * @return OCStackResult return value of this API. Returns OC_STACK_OK if success.
         *
         * NOTE: OCStackResult is defined in ocstack.h.
         */

        OCStackResult factoryReset(std::shared_ptr< OCResource > resource,
                DiagnosticsCallback callback);

        /**
         * API to show a list of supported diagnostics units
         * Callback call when a response arrives.
         *
         * @return the list in JSON format
         */
        std::string getListOfSupportedDiagnosticsUnits();

    private:

        GroupManager *g_groupmanager;

        std::vector< DiagnosticsUnitInfo > DiagnosticsUnitTable;

        void onExecuteForGroupAction(const HeaderOptions& headerOptions,
                const OCRepresentation& rep, const int eCode, std::string conf);
        void onDeleteGroupAction(const HeaderOptions& headerOptions,
                const OCRepresentation& rep, const int eCode, std::string conf);
        void onGetChildInfoForUpdate(const HeaderOptions& headerOptions,
                const OCRepresentation& rep, const int eCode, std::string conf);
        void onCreateActionSet(const HeaderOptions& headerOptions, const OCRepresentation& rep,
                const int eCode, std::string conf);
        void onGet(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode,
                std::string conf);
        void onPut(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode,
                std::string conf);

        std::shared_ptr< OCResource > getResource(std::string conf);
        DiagnosticsCallback getCallback(std::string conf);
        std::string getUpdateVal(std::string conf);
        std::string getAttributeByDiagnosticsName(DiagnosticsName name);
        std::string getUriByDiagnosticsName(DiagnosticsName name);

        std::string getHostFromURI(std::string oldUri);

        bool isSimpleResource(std::shared_ptr< OCResource > resource);

    };
}
#endif  /* __OC_THINGSCONFIGURATION__*/
