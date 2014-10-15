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
#include <iterator>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/variant.hpp>

#include "ocstack.h"
#include "OCHeaderOption.h"
#include <OCException.h>

#include "oc_logger.hpp"

namespace OC
{
    class OCPlatform;
    class OCResource;
    class OCResourceRequest;
    class OCResourceResponse;
} // namespace OC

namespace OC
{
    typedef boost::iostreams::stream<OC::oc_log_stream>     log_target_t;

    namespace detail
    {
        /* We'll want to provide some sort of explicit hook for custom logging at some
        point; until then, this should do nicely (note that since these are lambdas,
        later a special target could be captured, allowing much flexibility): */
        auto oclog_target = []() -> log_target_t&
        {
            static OC::oc_log_stream    ols(oc_make_ostream_logger);
            static log_target_t         os(ols);

            return os;
        };
    } // namespace OC::detail

    auto oclog = []() -> boost::iostreams::stream<OC::oc_log_stream>&
    {
        return detail::oclog_target();
    };

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
        LowQos      = OC_LOW_QOS,
        MidQos      = OC_MEDIUM_QOS,
        HighQos     = OC_HIGH_QOS,
        NaQos       = OC_NA_QOS // No Quality is defined, let the stack decide
    };

    /**
    *  Data structure to provide the configuration.
    *  ServiceType: indicate InProc or OutOfProc
    *  ModeType   : indicate whether we want to do server, client or both
    *  ipAddress  : ip address of server.
    *               if you speecifiy 0.0.0.0 : it listens on any interface.
    *  port       : port of server.
    *             : if you specifiy 0 : next available random port is used.
    *             : if you specify 5683 : client discovery can work even if they don't specify port.
    *  QoS        : Quality of Service : CONFIRMABLE or NON CONFIRMABLE.
    */
    struct PlatformConfig
    {
        ServiceType                serviceType;
        ModeType                   mode;
        std::string                ipAddress;
        uint16_t                   port;

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

    // Helper function to escape character in a string.
    std::string escapeString(const std::string& value);

    typedef std::map<std::string, std::string> AttributeMap;

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

        bool erase(const std::string& str)
        {
            return m_attributeMap.erase(str) != 0;
        }

        std::string getUri(void) const
        {
            return m_uri;
        }

        template <typename T>
        void setValue(const std::string& str, const T& val);

        template <typename T>
        bool getValue(const std::string& str, T& val) const;

        template <typename T>
        T getValue(const std::string& str) const;

        bool hasAttribute(const std::string& str) const
        {
            return m_attributeMap.find(str) != m_attributeMap.end();
        }

        void setNULL(const std::string& str)
        {
            m_attributeMap[str] = "null";
        }

        bool isNULL(const std::string& str) const
        {
            auto x = m_attributeMap.find(str);

            if(m_attributeMap.end() != x)
            {
                return x->second.compare("null") == 0;
            }
            else
            {
                std::ostringstream message;
                message << "attribute: " << str << " doesn't exist\n";
                throw OCException(message.str());
            }

            return false;
        }

        int numberOfAttributes() const
        {
            return m_attributeMap.size();
        }

        void setUri(std::string uri)
        {
            m_uri = uri;
        }

        std::vector<OCRepresentation> getChildren(void) const
        {
            return m_children;
        }

        void setChildren(const std::vector<OCRepresentation>& children)
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

        void setAttributeMap(const AttributeMap& map)
        {
            m_attributeMap = map;
        }

        std::string getJSONRepresentation(void) const
        {
            std::ostringstream json;

            json << "{";

            for(auto itr = m_attributeMap.begin(); itr!= m_attributeMap.end(); ++ itr)
            {
                if(itr != m_attributeMap.begin())
                {
                    json << ',';
                }
                json << "\""<<itr->first<<"\":"<< itr->second;
            }
            json << "}";

            return json.str();
        }

        std::vector<std::string> getResourceTypes() const
        {
            return m_resourceTypes;
        }

        void setResourceTypes(const std::vector<std::string>& resourceTypes)
        {
            m_resourceTypes = resourceTypes;
        }

        std::vector<std::string> getResourceInterfaces(void) const
        {
            return m_resourceInterfaces;
        }

        void setResourceInterfaces(const std::vector<std::string>& resourceInterfaces)
        {
            m_resourceInterfaces = resourceInterfaces;
        }
    };

    OCRepresentation parseJSONToRepresentation(const std::string& str);

    inline OCRepresentation parseJSONToRepresentation(const std::string& str)
    {
        OCRepresentation rep;

        AttributeMap attributeMap;

        std::stringstream requestStream;
        requestStream << str;
        boost::property_tree::ptree payload;
        try
        {
            boost::property_tree::read_json(requestStream, payload);
        }
        catch(boost::property_tree::json_parser::json_parser_error &e)
        {
            throw OCException("JSON parse error");
        }

        for(auto& item: payload)
        {
            std::string name = item.first.data();
            std::string value = item.second.data();

            attributeMap[name] = value;
        }

        rep.setAttributeMap(attributeMap);

        return rep;
    }

    typedef boost::variant<
                int,
                double,
                bool,
                OCRepresentation,
                std::string,
                std::vector<int>,
                std::vector<double>,
                std::vector<bool>,
                std::vector<std::string>,
                std::vector<OCRepresentation>
                > AttributeValue;

    template <typename T>
    inline std::string getJSONFromVector(const std::vector<T>& v)
    {
        std::ostringstream json;

        json << "\"[";
        if(v.size() != 0)
        {
            std::copy(v.begin(), v.end() - 1, std::ostream_iterator<T>(json, ","));
            json << v.back();
        }
        json << "]\"";

        return json.str();
    }

    class ComposeVisitor : public boost::static_visitor<std::string>
    {
        public:

            // TODO different int sizes
            std::string operator() (const int i) const
            {
                return std::to_string(i);
            }

            std::string operator() (const double d) const
            {
                return std::to_string(d);
            }

            std::string operator() (const std::string& str) const
            {
                std::ostringstream json;
                json << "\"";
                json << str;
                json << "\"";

                return json.str();
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

            std::string operator() (const std::vector<int>& numbers) const
            {
                return getJSONFromVector(numbers);
            }

            std::string operator() (const std::vector<double>& numbers) const
            {
                return getJSONFromVector(numbers);
            }

            std::string operator() (const std::vector<bool>& bools) const
            {
                std::ostringstream json;
                int first = 1;

                json << "\"[";
                for(auto b: bools)
                {
                    if(first)
                    {
                        b ? json << "true" : json << "false";
                        first = 0;
                    }
                    else
                    {
                        b ? json << ",true" : json << ",false";
                    }
                }
                json << "]\"";

                return json.str();
            }

            std::string operator() (const std::vector<std::string>& strings) const
            {
                return getJSONFromVector(strings);
            }

            std::string operator() (const OCRepresentation& rep) const
            {
                std::ostringstream json;

                json << "\"";

                json << escapeString(rep.getJSONRepresentation());

                json << "\"";

                return json.str();
            }

            std::string operator() (const std::vector<OCRepresentation>& reps) const
            {
                std::ostringstream json;
                int first = 1;

                json << "\"[";
                for(auto rep: reps)
                {
                    if(first)
                    {
                        first = 0;
                        json << escapeString(rep.getJSONRepresentation());
                    }
                    else
                    {
                        json << ",";
                        json << escapeString(rep.getJSONRepresentation());
                    }
                }
                json << "]\"";

                return json.str();
            }


    };

    inline void split(std::string input, char delimiter, std::vector<std::string>& tokens)
    {
        std::stringstream ss(input);
        std::string item;

        while(std::getline(ss, item, delimiter))
        {
            tokens.push_back(item);
        }
    }

    class ParseVisitor : public boost::static_visitor<void>
    {
        public:

            ParseVisitor(std::string str): m_str(str)
            {
            }

            void operator() (int& i) const
            {
                i = std::stoi(m_str);
            }

            void operator() (double& d) const
            {
                d = std::stod(m_str);
            }

            void operator() (std::string& str) const
            {
                str = m_str;
            }

            void operator() (bool& b) const
            {
                b = m_str.compare("true") == 0;
            }

            void operator() (std::vector<int>& numbers) const
            {
                numbers.clear();

                if(m_str.length() >= 2)
                {
                    std::string str = m_str.substr(1, m_str.length()-2);

                    std::vector<std::string> tokens;
                    split(str, ',', tokens);

                    for(auto s: tokens)
                    {
                        numbers.push_back(std::stoi(s));
                    }
                }
                else
                {
                    throw OCException("Array type should have at least []");
                }

            }

            void operator() (std::vector<double>& numbers) const
            {
                numbers.clear();

                if(m_str.length() >= 2)
                {
                    std::string str = m_str.substr(1, m_str.length()-2);
                    std::vector<std::string> tokens;
                    split(str, ',', tokens);

                    for(auto s: tokens)
                    {
                        numbers.push_back(std::stod(s));
                    }
                }
                else
                {
                    throw OCException("Array type should have at least []");
                }
            }

            void operator() (std::vector<bool>& bools) const
            {
                bools.clear();

                if(m_str.length() >= 2)
                {
                    std::string str = m_str.substr(2, m_str.length()-3);

                    std::vector<std::string> tokens;
                    split(str, ',', tokens);

                    for(auto s: tokens)
                    {
                        bools.push_back(s.compare("true") == 0);
                    }
                }
                else
                {
                    throw OCException("Array type should have at least []");
                }

            }

            void operator() (std::vector<std::string>& strings) const
            {
                strings.clear();

                if(m_str.length() >= 2)
                {
                    std::string str = m_str.substr(1, m_str.length()-2);

                    std::vector<std::string> tokens;
                    split(str, ',', tokens);

                    for(auto s: tokens)
                    {
                        strings.push_back(s);
                    }
                }
                else
                {
                    throw OCException("Array type should have at least []");
                }
            }

            void operator() (std::vector<OCRepresentation>& reps) const
            {
                reps.clear();

                if(m_str.length() >= 2)
                {
                    std::string str = m_str.substr(1, m_str.length()-2);

                    std::vector<std::string> tokens;
                    split(str, ',', tokens);

                    for(auto s: tokens)
                    {
                        reps.push_back(parseJSONToRepresentation(s));
                    }
                }
                else
                {
                    throw OCException("Array type should have at least []");
                }
            }

            void operator() (OCRepresentation& rep) const
            {
                rep = parseJSONToRepresentation(m_str);
            }

        private:
            std::string m_str;
    };


    inline std::string getJSON(const AttributeValue& v)
    {
        return boost::apply_visitor(ComposeVisitor(), v);
    }

    inline void parseJSON(AttributeValue& v, std::string str)
    {
        boost::apply_visitor(ParseVisitor(str), v);
    }

    template <typename T>
    void OCRepresentation::setValue(const std::string& str, const T& val)
    {
        m_attributeMap[str] = getJSON(val);
    }

    template <typename T>
    T OCRepresentation::getValue(const std::string& str) const
    {
        T val = T();

        auto x = m_attributeMap.find(str);

        if(m_attributeMap.end() != x)
        {
            AttributeValue v = val;
            parseJSON(v, x->second);
            val = boost::get<T>(v);
        }

        return val;
    }

    template <typename T>
    bool OCRepresentation::getValue(const std::string& str, T& val) const
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

    // Typedef for header option vector
    // OCHeaderOption class is in HeaderOption namespace
    typedef std::vector<HeaderOption::OCHeaderOption> HeaderOptions;

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

    typedef std::function<void(std::shared_ptr<OCResource>)> FindCallback;

    typedef std::function<void(const std::shared_ptr<OCResourceRequest>,
                                const std::shared_ptr<OCResourceResponse>)> EntityHandler;

    typedef std::function<void(OCStackResult, const unsigned int)> SubscribeCallback;

    typedef std::function<void(const HeaderOptions&,
                                const OCRepresentation&, const int)> GetCallback;

    typedef std::function<void(const HeaderOptions&,
                                const OCRepresentation&, const int)> PostCallback;

    typedef std::function<void(const HeaderOptions&,
                                const OCRepresentation&, const int)> PutCallback;

    typedef std::function<void(const HeaderOptions&, const int)> DeleteCallback;

    typedef std::function<void(const HeaderOptions&,
                                const OCRepresentation&, const int, const int)> ObserveCallback;
} // namespace OC

#endif
