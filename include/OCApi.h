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

#include <boost/variant.hpp>

#include "ocstack.h"

namespace OC
{
    class OCResource;
    class OCResourceRequest;
    class OCResourceResponse;
} // namespace OC

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

    enum RequestHandlerFlag
    {
        InitFlag = 1 << 0,
        RequestFlag = 1 << 1,
        ObserverFlag = 1 << 2
    };

    enum class ObserveType
    {
        Observe,
        ObserveAll
    };

    typedef std::map<std::string, std::string> AttributeMap;

    struct AttributeNull {};

    // Forward declaration
    struct AttributeDataValue;

    typedef std::map<std::string, AttributeDataValue> AttributeMapValue;
    typedef std::vector<AttributeDataValue> AttributeValueVector;

    typedef boost::variant<
                AttributeNull,
                int,
                bool,
                std::string,
                AttributeMapValue,
                AttributeValueVector> AttributeValue;

    struct AttributeDataValue
    {
        AttributeValue data;
    };

    class ComposeVisitor : public boost::static_visitor<std::string>
    {
        public:
            std::string operator() (const AttributeNull& nl) const
            {
                // TODO Not Implemented
                return std::string();
            }

            // TODO different int sizes
            std::string operator() (const int i) const
            {
                char i_str[16];
                snprintf(i_str, 16, "%d", i);
                return i_str;
            }

            std::string operator() (const std::string& str) const
            {
                return str;
            }

            std::string operator() (const bool b) const
            {
                if(b)
                {
                    return "true";
                }
                else
                {
                    return "false";
                }
            }

            std::string operator() (const AttributeMapValue& objValue) const
            {
                // TODO Not Implemented
                return std::string();
                std::ostringstream json;
            }

            std::string operator() (const AttributeValueVector& values) const
            {
                // TODO Not Implemented
                return std::string();
                std::ostringstream json;
            }
    };

    class ParseVisitor : public boost::static_visitor<void>
    {
        public:

            ParseVisitor(std::string str): m_str(str)
            {
            }

            void operator() (AttributeNull& nl) const
            {
                // TODO Not Implemented
            }

            void operator() (int& i) const
            {
                i = atoi(m_str.c_str());
            }

            void operator() (std::string& str) const
            {
                str = m_str;
            }

            void operator() (bool& b) const
            {
                b = m_str.compare("true") == 0;
            }

            void operator() (AttributeMapValue& objValue) const
            {
                // TODO Not Implemented
            }

            void operator() (AttributeValueVector& values) const
            {
                // TODO Not Implemented
            }

        private:
            std::string m_str;
    };

    typedef std::map<std::string, AttributeValue> AttributeMap1;

    std::string getJSON(const AttributeValue& v);
    void parseJSON(AttributeValue& v, std::string str);

    // Typedef for query parameter map
    typedef std::map<std::string, std::string> QueryParamsMap;

    // Typedef for list of observation IDs
    typedef std::vector<OCObservationId> ObservationIds;

    enum class ObserveAction
    {
        ObserveRegister,
        ObserveUnregister
    };

    typedef struct
    {
        // Action associated with observation request
        ObserveAction action;
        // Identifier for observation being registered/unregistered
        OCObservationId obsId;
    } ObservationInfo;

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
        int errorCode;

        std::vector<OCRepresentation> m_children;

        public:
        OCRepresentation() {}

        std::string getUri(void) const
        {
            return m_uri;
        }

        template <typename T>
        void setValue(const std::string& str, const T& val)
        {
            m_attributeMap[str] = getJSON(val);
        }

        template <typename T>
        bool getValue(const std::string& str, T& val) const
        {
            auto x = m_attributeMap.find(str);

            if(m_attributeMap.end() != x)
            {
                AttributeValue v = val;
                parseJSON(v, x->second);
                val = boost::get<T>(v);
                return true;
            }
            else
            {
                return false;
            } 
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

        std::weak_ptr<OCResource> getResource() const
        {
            // TODO Needs to be implemented
            std::weak_ptr<OCResource> wp;
            return wp;
        }

        AttributeMap getAttributeMap() const
        {
            return m_attributeMap;
        }

        void setAttributeMap(const AttributeMap map)
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
