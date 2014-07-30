//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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

/// @file OCResource.h 

/// @brief  This file contains the declaration of classes and its members related to 
///         Resource.

#ifndef __OCRESOURCE_H
#define __OCRESOURCE_H

#include <memory>
#include <random>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <OCApi.h>
#include <ResourceInitException.h>

namespace OC
{
    /**
    *   @brief  OCResource represents an OC resource. A resource could be a light controller, 
    *           temperature sensor, smoke detector, etc. A resource comes with a well-defined 
    *           contract or interface onto which you can perform different operations, such as 
    *           turning on the light, getting the current temperature or subscribing for event 
    *           notifications from the smoke detector. A resource can be composed of one or
    *           more resources. 
    */
    class OCResource
    {
    public:
        typedef std::shared_ptr<OCResource> Ptr;

        // TODO: This constructor needs to be removed when we give the final version
        // TODO: See if we can do const prop tree
        OCResource(const std::string host, boost::property_tree::ptree& resourceNode);

        virtual ~OCResource(void);
        
        /**
        * Function to get the attributes of a resource. 
        * TODO : Need to have query params? Add implementation in .cpp
        * @param attributeHandler handles callback
        *        The callback function will be invoked with a map of attribute name and values. 
        *        The callback function will also have the result from this Get operation 
        *        This will have HTTP error codes
        */
        void get(std::function<void(const AttributeMap&, const int&)> attributeHandler) { return; }

        /**
        * Function to set the attributes of a resource (via PUT)
        * TODO Add implementation in .cpp
        * @param AttributeMap Map which can either have all the attribute names and values 
                 (which will represent entire state of the resource) or a 
        *        set of attribute names and values which needs to be modified 
        *        (which will represent part of the resoruce state) via PUT
        * @param queryParametersMap a map which will have the query parameters (if any)
        * NOTE: For the current release, only '&' semantics will be provided
        * @param attributeHandler handles callback
        *        The callback function will be invoked with a map of attribute name and values. 
        *        The callback function will also have the result from this Put operation 
        *        This will have HTTP error codes
        */
        void put(const AttributeMap& attributeMap, const QueryParamsMap& queryParametersMap, std::function<void(const int&)> attributeHandler) { return; } 

        /**
        * Function to set observation on the resource 
        * TODO Add implementation in .cpp
        * @param observeHandler handles callback
        *        The callback function will be invoked with a map of attribute name and values. 
        *        The callback function will also have the result from this observe operation 
        *        This will have HTTP error codes
        */
        void observe(std::function<void(const AttributeMap&, const int&)> observeHandler) { return; }

        /**
        * Function to cancel the observation on the resource
        * TODO Add implementation in .cpp
        * @param observeCancellationHandler handles callback
        *        The callback function will also have the result from this operation 
        *        This will have HTTP error codes
        */
        void cancelObserve(std::function<void(const int&)> observeCancellationHandler) { return; }

        /**
        * Function to get the host address of this resource
        * @return std::string host address
        * NOTE: This might or might not be exposed in future due to security concerns
        */
        std::string host() const {return m_host;}
        
        /**
        * Function to get the URI for this resource
        * @return std::string resource URI 
        */
        std::string uri() const {return m_uri;}

        /**
        * Function to provide ability to check if this resource is observable or not
        * @return bool true indicates resource is observable; false indicates resource is
        *         not observable.
        */
        bool isObservable() const {return m_isObservable;}

        /*
        * Allows the server to call notifyObserver
        * @return bool true indicates this operation was success; false indicates this 
        * operation failed
        */
        //static bool notifyObservers(const std::string& resourceURI); 

        // bool whether this is a collection type, and will have children that can be queried
        //bool isCollection() const {return m_isCollection;}
        // a collection of the resource-type names
        //const std::vector<std::string> resourceTypes() const {return m_resourceTypes;}
        // a collection of the interfaces supported by this resource
        //const std::vector<std::string> interfaces() const { return m_interfaces;}
        // a collection of property objects that allow calling of properties on this Resource
        //const std::vector<std::string> properties() const { return m_properties;}
        // a collection of the names of the children of this resource, assuming it supports a collection interface
        //const std::vector<std::string> children() const {return m_children;}

        /*void post(const AttributeMap&, std::function<void(const int&)> attributeHandler);

        NOTE: dont expose the host ip .. so some kind of handle is required 
        static OCResource::Ptr getResource(const std::string& host, const std::string& resourceURI, const std::string& resourceName, 
            const std::string interfaceName, bool observerable);*/

    
    private:    
        std::string m_uri;
        std::string m_host;
        bool m_isObservable;
        bool m_isCollection;
        std::vector<std::string> m_resourceTypes;
        std::vector<std::string> m_interfaces;
        std::vector<std::string> m_children;
        AttributeMap m_attributeMap;
    };

} // namespace OC

#endif //__OCRESOURCE_H
