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

/// @file OCResource.h

/// @brief  This file contains the declaration of classes and its members related to
///         Resource.

#ifndef __OCRESOURCE_H
#define __OCRESOURCE_H

#include <memory>
#include <random>
#include <algorithm>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <OCApi.h>
#include <ResourceInitException.h>
#include <IClientWrapper.h>
#include <InProcClientWrapper.h>
#include <OCRepresentation.h>

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
    friend class OCPlatform;
    friend class InProcClientWrapper;
    public:
        typedef std::shared_ptr<OCResource> Ptr;
        /**
        * Virtual destructor
        */
        virtual ~OCResource(void);

        /**
        * Function to get the attributes of a resource.
        * @param queryParametersMap map which can have the query parameter name and value
        * @param attributeHandler handles callback
        *        The callback function will be invoked with a map of attribute name and values.
        *        The callback function will also have the result from this Get operation
        *        This will have error codes
        * @return OCStackResult return value of this API. Returns OC_STACK_OK if success.
        * NOTE: OCStackResult is defined in ocstack.h.
        */
        OCStackResult get(const QueryParamsMap& queryParametersMap, GetCallback attributeHandler);

        /**
        * Function to get the attributes of a resource.
        *
        * @param resourceType resourceType of the resource operate on
        * @param resourceInterface interface type of the resource to operate on
        * @param queryParametersMap map which can have the query parameter name and value
        * @param attributeHandler handles callback
        *        The callback function will be invoked with a map of attribute name and values.
        *        The callback function will be invoked with a list of URIs if 'get' is invoked on a resource container
        *        (list will be empty if not a container)
        *        The callback function will also have the result from this Get operation. This will have error codes
        * @return OCStackResult return value of this API. Returns OC_STACK_OK if success. <br>
        * NOTE: OCStackResult is defined in ocstack.h.<br>
        * <b>Example:</b><br>
        * Consider resource "a/home" (with link interface and resource type as home) contains links to "a/kitchen" and "a/room".
        * Step 1: get("home", Link_Interface, &onGet)<br>
        * Callback onGet will receive a) Empty attribute map because there are no attributes for a/home b) list with
        * full URI of "a/kitchen" and "a/room" resources and their properties c) error code for GET operation<br>
        * NOTE: A resource may contain single or multiple resource types. Also, a resource may contain single or multiple interfaces.<br>
        * Currently, single GET request is allowed to do operate on single resource type or resource interface. In future, a single GET <br>
        * can operate on multiple resource types and interfaces. <br>
        * NOTE: A client can traverse a tree or graph by doing successive GETs on the returned resources at a node.<br>
        */
        OCStackResult get(const std::string& resourceType, const std::string& resourceInterface, const QueryParamsMap& queryParametersMap, GetCallback attributeHandler);

        /**
        * Function to set the representation of a resource (via PUT)
        * @param representation which can either have all the attribute names and values
                 (which will represent entire state of the resource) or a
        *        set of attribute names and values which needs to be modified
        *        The callback function will be invoked with a map of attribute name and values.
        *        The callback function will also have the result from this Put operation
        *        This will have error codes
        * @param queryParametersMap map which can have the query parameter name and value
        * @param attributeHandler attribute handler
        * @return OCStackResult return value of this API. Returns OC_STACK_OK if success.
        * NOTE: OCStackResult is defined in ocstack.h.
        */
        OCStackResult put(const OCRepresentation& representation,
                const QueryParamsMap& queryParametersMap, PutCallback attributeHandler);

        /**
        * Function to set the attributes of a resource (via PUT)
        * @param resourceType resource type of the resource to operate on
        * @param resourceInterface interface type of the resource to operate on
        * @param representation representation of the resource
        * @param queryParametersMap Map which can have the query parameter name and value
        * @param attributeHandler attribute handler
        *        The callback function will be invoked with a map of attribute name and values.
        *        The callback function will also have the result from this Put operation
        *        This will have error codes.
        *        The Representation parameter maps which can either have all the attribute names
        *        and values
        *        (which will represent entire state of the resource) or a
        *        set of attribute names and values which needs to be modified
        * @return OCStackResult return value of this API. Returns OC_STACK_OK if success. <br>
        * NOTE: OCStackResult is defined in ocstack.h. <br>
        */
        OCStackResult put(const std::string& resourceType, const std::string& resourceInterface,
            const OCRepresentation& representation, const QueryParamsMap& queryParametersMap,
            PutCallback attributeHandler);

        /**
        * Function to post on a resource
        * @param representation which can either have all the attribute names and values
                 (which will represent entire state of the resource) or a
        *        set of attribute names and values which needs to be modified
        *        The callback function will be invoked with a map of attribute name and values.
        *        The callback function will also have the result from this Put operation
        *        This will have error codes
        * @param queryParametersMap map which can have the query parameter name and value
        * @param attributeHandler attribute handler
        * @return OCStackResult return value of this API. Returns OC_STACK_OK if success.
        * NOTE: OCStackResult is defined in ocstack.h.
        */
        OCStackResult post(const OCRepresentation& representation,
                const QueryParamsMap& queryParametersMap, PostCallback attributeHandler);

        /**
        * Function to post on a resource
        * @param resourceType resource type of the resource to operate on
        * @param resourceInterface interface type of the resource to operate on
        * @param representation representation of the resource
        * @param queryParametersMap Map which can have the query parameter name and value
        * @param attributeHandler attribute handler
        *        The callback function will be invoked with a map of attribute name and values.
        *        The callback function will also have the result from this Put operation
        *        This will have error codes.
        *        The Representation parameter maps which can either have all the attribute names
        *        and values
        *        (which will represent entire state of the resource) or a
        *        set of attribute names and values which needs to be modified
        * @return OCStackResult return value of this API. Returns OC_STACK_OK if success. <br>
        * NOTE: OCStackResult is defined in ocstack.h. <br>
        */
        OCStackResult post(const std::string& resourceType, const std::string& resourceInterface,
            const OCRepresentation& representation, const QueryParamsMap& queryParametersMap,
            PostCallback attributeHandler);

        /**
        * Function to set observation on the resource
        * @param observeType allows the client to specify how it wants to observe.
        * @param queryParametersMap map which can have the query parameter name and value
        * @param observeHandler handles callback
        *        The callback function will be invoked with a map of attribute name and values.
        *        The callback function will also have the result from this observe operation
        *        This will have error codes
        * @return OCStackResult return value of this API. Returns OC_STACK_OK if success.
        * NOTE: OCStackResult is defined in ocstack.h.
        */
        OCStackResult observe(ObserveType observeType, const QueryParamsMap& queryParametersMap,
            ObserveCallback observeHandler);

        /**
        * Function to cancel the observation on the resource
        * @return OCStackResult return value of this API. Returns OC_STACK_OK if success.
        * NOTE: OCStackResult is defined in ocstack.h.
        */
        OCStackResult cancelObserve();

        /**
        * Function to get the host address of this resource
        * @return std::string host address
        * NOTE: This might or might not be exposed in future due to security concerns
        */
        std::string host() const;

        /**
        * Function to get the URI for this resource
        * @return std::string resource URI
        */
        std::string uri() const;

        /**
        * Function to provide ability to check if this resource is observable or not
        * @return bool true indicates resource is observable; false indicates resource is
        *         not observable.
        */
        bool isObservable() const;

        /**
        * Function to get the list of resource types
        * @return vector of resource types
        */
        std::vector<std::string> getResourceTypes() const
        {
            return m_resourceTypes;
        }

        /**
        * Function to get the list of resource interfaces
        * @return vector of resource interface
        */
        std::vector<std::string> getResourceInterfaces(void) const
        {
            return m_interfaces;
        }

    private:
        std::weak_ptr<IClientWrapper> m_clientWrapper;
        std::string m_uri;
        std::string m_host;
        bool m_isObservable;
        bool m_isCollection;
        std::vector<std::string> m_resourceTypes;
        std::vector<std::string> m_interfaces;
        std::vector<std::string> m_children;
        OCDoHandle m_observeHandle;

    private:
        OCResource(std::weak_ptr<IClientWrapper> clientWrapper, const std::string& host, const std::string& uri,
            bool observable, const std::vector<std::string>& resourceTypes, const std::vector<std::string>& interfaces);
    };

} // namespace OC

#endif //__OCRESOURCE_H
