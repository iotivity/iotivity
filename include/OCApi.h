//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

#ifndef __INTEL_OCAPI_H_2014_07_10
#define __INTEL_OCAPI_H_2014_07_10

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <memory>
#include "ocstack.h"

namespace OC
{
    class OCResource;
    class OCResourceRequest;
    class OCResourceResponse;
} // namespace OC

namespace OC { namespace OCReflect {
    struct entity;
}} // namespace OC::OCReflect

namespace OC
{
    enum class OCPlatformStatus
    {
        PlatformUp,
        PlatformDown
    };

    enum class OCAdvertisementStatus
    {
        None
    };

    typedef std::string URI;

    enum class ServiceType
    {
        InProc,
        OutOfProc
    };

    enum class ModeType
    {
        Server,
        Client,
        Both
    };

    enum class QualityOfService : uint8_t
    {
        Confirmable     = OC_CONFIRMABLE,
        NonConfirmable  = OC_NON_CONFIRMABLE
    };

    struct PlatformConfig
    {
        ServiceType                serviceType;   // This will indicate whether it is InProc or OutOfProc
        ModeType                   mode;          // This will indicate whether we want to do server, client or both
        std::string                ipAddress;     // This is the ipAddress of the server to connect to
        uint16_t                   port;          // Port of the server

        QualityOfService           QoS;

        public:
            PlatformConfig(const ServiceType serviceType_,
            const ModeType mode_,
            const std::string& ipAddress_,
            const uint16_t port_,
            const QualityOfService QoS_)
                : serviceType(serviceType_),
                mode(mode_),
                ipAddress(ipAddress_),
                port(port_),
                QoS(QoS_)
        {}
    };

    enum class RequestHandlerFlag
    {
        InitFlag,
        RequestFlag,
        ObserverFlag
    };

    enum class ObserveType
    {
        Observe,
        ObserveAll
    };

    // TODO: To find the complete JSon data structure and modify map value type
    // Typedef for attribute values and attribute map.
    typedef std::vector<std::string> AttributeValues;
    typedef std::map<std::string, AttributeValues> AttributeMap;

    // Typedef for query parameter map
    typedef std::map<std::string, std::string> QueryParamsMap;

    // const strings for different interfaces

    // Default interface
    const std::string DEFAULT_INTERFACE = "oc.mi.def";

    // Used in discovering (GET) links to other resources of a collection.
    const std::string LINK_INTERFACE = "oc.mi.ll";

    // Used in GET, PUT, POST, DELETE methods on links to other resources of a collection.
    const std::string BATCH_INTERFACE = "oc.mi.b";

    // Helper function to escape character in a string.
    std::string escapeString(const std::string& value);

    class OCRepresentation
    {

        private:
        std::string m_uri;
        AttributeMap m_attributeMap;
        std::vector<std::string> m_resourceTypes;
        std::vector<std::string> m_resourceInterfaces;
        bool m_observable; // TODO : do we need this here???
        int errorCode;

        std::vector<OCRepresentation> m_children;

        public:
        OCRepresentation() {}

        std::string getUri(void) const
        {
            return m_uri;
        }

        void setUri(std::string uri)
        {
            m_uri = uri;
        }

        std::vector<OCRepresentation> getChildren(void) const
        {
            return m_children;
        }

        void setChildren(std::vector<OCRepresentation> children)
        {
        m_children = children;
        }

        OCResource* getResource() const
        {
            // TODO Needs to be implemented
            OCResource* res = NULL;

            return res;
        }

        AttributeMap getAttributeMap() const
        {
            return m_attributeMap;
        }

        void setAttributeMap(AttributeMap map)
        {
            m_attributeMap = map;
        }

        std::vector<std::string> getResourceTypes() const
        {
            return m_resourceTypes;
        }

        void setResourceTypes(std::vector<std::string> resourceTypes)
        {
            m_resourceTypes = resourceTypes;
        }

        std::vector<std::string> getResourceInterfaces(void) const
        {
            return m_resourceInterfaces;
        }

        void setResourceInterfaces(std::vector<std::string> resourceInterfaces)
        {
            m_resourceInterfaces = resourceInterfaces;
        }
    };

    typedef std::function<void(std::shared_ptr<OCResource>)> FindCallback;
    typedef std::function<void(const std::shared_ptr<OCResourceRequest>, const std::shared_ptr<OCResourceResponse>)> RegisterCallback;
    typedef std::function<void(OCStackResult, const unsigned int)> SubscribeCallback;
    typedef std::function<void(const OCRepresentation&, const int)> GetCallback;
    typedef std::function<void(const OCRepresentation&, const int)> PutCallback;
    typedef std::function<void(const OCRepresentation&, const int, const int)> ObserveCallback;
} // namespace OC

#endif
