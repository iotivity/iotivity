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

/// @file OCRepresentation.h

/// @brief  This file contains the declaration of classes and its members
///         related to OCRepresentation

#ifndef __OCREPRESENTATION_H
#define __OCREPRESENTATION_H


#include <string>
#include <sstream>
#include <vector>
#include <map>

#include <AttributeValue.h>
#include <StringConstants.h>

#ifdef __ANDROID__
#include "OCAndroid.h"
#endif

#include <OCException.h>

namespace cereal
{
    class access;
}

namespace OC
{

    enum class InterfaceType
    {
        None,
        LinkParent,
        BatchParent,
        DefaultParent,
        LinkChild,
        BatchChild,
        DefaultChild
    };

    // The consumer requires resource info to be printed in 2 different ways, both with the "oc":[]
    // and without.  This enum is used to differentiate between the two situations.  When the
    // serialize is called with Include OC, we encode OC, otherwise we skip it and return just the
    // contents of the array.
    enum class OCInfoFormat
    {
        IncludeOC,
        ExcludeOC
    };

    class MessageContainer
    {
        public:
            void setJSONRepresentation(const std::string& payload);

            void setJSONRepresentation(const unsigned char* payload);

            std::string getJSONRepresentation(OCInfoFormat f) const;

            const std::vector<OCRepresentation>& representations() const;

            void addRepresentation(const OCRepresentation& rep);

            const OCRepresentation& operator[](int index) const
            {
                return m_reps[index];
            }

            const OCRepresentation& back() const
            {
                return m_reps.back();
            }
        private:
            std::vector<OCRepresentation> m_reps;
    };
    class OCRepresentation
    {
        public:
            OCRepresentation();
            std::string getJSONRepresentation() const;

            void addChild(const OCRepresentation&);

            void clearChildren();

            const std::vector<OCRepresentation>& getChildren() const;

            void setChildren(const std::vector<OCRepresentation>& children);

            void setUri(const std::string& uri);

            std::string getUri() const;

            const std::vector<std::string>& getResourceTypes() const;

            void setResourceTypes(const std::vector<std::string>& resourceTypes);

            const std::vector<std::string>& getResourceInterfaces() const;

            void setResourceInterfaces(const std::vector<std::string>& resourceInterfaces);

            bool empty() const;

            int numberOfAttributes() const;

            bool erase(const std::string& str);

            template <typename T>
            void setValue(const std::string& str, const T& val)
            {
                m_values[str] = val;
            }

            template <typename T>
            bool getValue(const std::string& str, T& val) const
            {
                auto x = m_values.find(str);

                if(x!= m_values.end())
                {
                    val = boost::get<T>(x->second);
                    return true;
                }
                else
                {
                    val = T();
                    return false;
                }
            }

            template <typename T>
            T getValue(const std::string& str) const
            {
                T val = T();
                auto x = m_values.find(str);
                if(x != m_values.end())
                {
                    val = boost::get<T>(x->second);
                }
                return val;
            }

            bool hasAttribute(const std::string& str) const;

            void setNULL(const std::string& str);

            bool isNULL(const std::string& str) const;
        private:
            friend class OCResourceResponse;
            friend class cereal::access;

            // the root node has a slightly different JSON version
            // based on the interface type configured in ResourceResponse.
            // This allows ResourceResponse to set it, so that the save function
            // doesn't serialize things that it isn't supposed to serialize.
            void setInterfaceType(InterfaceType ift)
            {
                m_interfaceType = ift;
            }

            // class used to wrap the 'prop' feature of the save/load
            class Prop
            {
                public:
                    Prop(std::vector<std::string>& resourceTypes,
                            std::vector<std::string>& interfaces)
                    : m_types(resourceTypes), m_interfaces(interfaces)
                    {}

                 /*   Prop(const std::vector<std::string>& resourceTypes,
                            const std::vector<std::string>& interfaces)
                    :m_types(resourceTypes),
                    m_interfaces(interfaces)
                    {}*/
                private:
                    friend class cereal::access;
                    template <class Archive>
                    void save(Archive& ar) const;

                    template<class Archive>
                    void load(Archive& ar);

                    std::vector<std::string>& m_types;
                    std::vector<std::string>& m_interfaces;
            };
            template<class Archive, class Val>
            static void optional_load(Archive& ar, Val&& v);

            template<class Archive>
            void save(Archive& ar) const;

            template<class Archive>
            void load(Archive& ar);

        private:
            std::string m_uri;
            std::vector<OCRepresentation> m_children;
            std::map<std::string, AttributeValue> m_values;
            std::vector<std::string> m_resourceTypes;
            std::vector<std::string> m_interfaces;

            InterfaceType m_interfaceType;
    };
} // namespace OC


#endif //__OCREPRESENTATION_H
