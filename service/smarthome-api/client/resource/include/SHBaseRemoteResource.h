/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

/**
 * This file contains the declaration of classes and its members related to
 * SHBaseRemoteResource.
 */

#ifndef SMARTHOME_API_CLIENT_SHBASEREMOTERESOURCE_H_
#define SMARTHOME_API_CLIENT_SHBASEREMOTERESOURCE_H_

#include <list>
#include <CommonApi.h>
#include <octypes.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            class SHBaseRemoteResource_Impl;
            class PropertyBundle;
            class ResourceQuery;

            /**
             * This class contains a set of base functions for callback of
             * SHBaseRemoteResource class.
             */
            class SHBaseRemoteResourceDelegate
            {
            public:
                /**
                 * Virtual destructor
                 */
                virtual ~SHBaseRemoteResourceDelegate() {}

                /**
                 * API to receive and handle a response for GET request
                 *
                 * @param[in] bundle PropertyBundle which has property keys and values
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onGet(PropertyBundle bundle, ResultCode ret) = 0;

                /**
                 * API to receive and handle a response for POST request
                 *
                 * @param[in] bundle PropertyBundle which has property keys and values
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onSet(PropertyBundle bundle, ResultCode ret) = 0;

                /**
                 * API to receive and handle a response for OBSERVE request
                 *
                 * @param[in] bundle   PropertyBundle which has property keys and values
                 * @param[in] obsType  Type of response for observe.
                 * @param[in] ret      ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onObserve(PropertyBundle bundle, const ObserveResponse obsType,
                        ResultCode ret) = 0;
            };

            /**
             * This class contains a set of functions to get/set of remote resource.
             *
             * Also, some API can support to get informations about resource.
             */
            class SHBaseRemoteResource
            {
            friend class SHBaseRemoteResourceBuilder;
            public:
                /**
                 * Virtual destructor
                 */
                virtual ~SHBaseRemoteResource();

                /**
                 * API to get URI of this resource.
                 *
                 * @return std::string  resource URI.
                 */
                std::string getUri() const;

                /**
                 * API to get host address of this resource.
                 *
                 * @return std::string  resource host address.
                 */
                std::string getHost() const;

                /**
                 * API to get the endpoints information of this resource.
                 *
                 * @return endpoints information
                 */
                std::list<std::string> getHosts() const;

                /**
                 * API to set specific host information into resource.
                 *
                 * @param[in] host  specific host address.
                 * @resurn std::string  New host address.
                 */
                std::string setHost(const std::string &host);

                /**
                 * API to get types of this resource.
                 *
                 * @return std::list<std::string> resource types.
                 */
                std::list<std::string> getResourceTypes() const;

                /**
                 * API to get interfaces of this resource.
                 *
                 * @return std::list<std::string> resource interfaces.
                 */
                std::list<std::string> getInterfaces() const;

                bool hasResourceType(const std::string resourceType) const;

                /**
                 * API to start observation on the resource.
                 *
                 * @return true if request success.
                 */
                bool startObserve();

                /**
                 * API to start observation on the resource.
                 *
                 * @param[in] query  ResourceQuery to specify request.
                 * @return true if request success.
                 */
                bool startObserve(const ResourceQuery &query);

                /**
                 * API to stop observation on the resource.
                 *
                 * @return true if request success.
                 */
                bool stopObserve();

                /**
                 * API to get known resource without discovering.
                 *
                 * @param host a string containing a resolvable "coap(s)", "coap(s)+protocol"
                 *        uri scheme of the server holding the resource.
                 *        Currently this should be in the format coap(s)://address:port or
                 *        coap(s)+protocol://address:port, though in the future, we expect this to
                 *        change to //address:port
                 *
                 * @param uri the rest of the resource's URI that will permit messages to be
                 *           properly routed.  Example: /a/light
                 *
                 * @param connectivityType ::OCConnectivityType type of connectivity indicating the
                 *                           transport method and IP address scope.
                 *                           Example: CT_DEFAULT, CT_ADAPTER_IP, CT_ADAPTER_TCP.
                 *                           if you want to use a specific Flag like IPv4,
                 *                           you should apply OR operation for the flag in here.
                 *                           Example: static_cast<OCConnectivityType>(CT_ADAPTER_TCP
                 *                                                                   | OC_IP_USE_V4)
                 *
                 * @param isObservable a boolean containing whether the resource
                 *                     supports observation
                 *
                 * @param resourceTypes a collection of resource types implemented by the resource
                 *
                 * @param interfaces a collection of interfaces that the
                 *                   resource supports/implements
                 *
                 * @param endpoints List of endpoints information
                 *
                 * @resurn SHBaseRemoteResource*  Pointer to the new resource object.
                 */
                static SHBaseRemoteResource* constructResourceObject(
                                                const std::string& host,
                                                const std::string& uri,
                                                OCConnectivityType connectivityType,
                                                bool isObservable,
                                                const std::list<std::string>& resourceTypes,
                                                const std::list<std::string>& interfaces,
                                                const std::list<std::string>& endpoints);

            protected:
                SHBaseRemoteResource();

                /**
                 * API to set delegate of this resource.
                 *
                 * @param[in] delegate  Instance that inherit SHBaseRemoteResourceDelegate class
                 */
                void setDelegate(SHBaseRemoteResourceDelegate *delegate);

                /**
                 * API to get the properties of a resource(server-side)
                 * (use GET method)
                 *
                 * @return SHRequestHandle request handle.
                 */
                SHRequestHandle getPropertyBundle();

                /**
                 * API to get the properties of a resource(server-side)
                 * (use GET method)
                 *
                 * @param[in] query  ResourceQuery to specify request.
                 * @return SHRequestHandle request handle.
                 */
                SHRequestHandle getPropertyBundle(const ResourceQuery &query);

                /**
                 * API to set the properties to a resource(server-side)
                 * (use POST method)
                 *
                 * @param[in] bundle Properties to set a resource.
                 * @return SHRequestHandle request handle.
                 */
                SHRequestHandle setPropertyBundle(const PropertyBundle &bundle);

                /**
                 * API to set the properties to a resource(server-side)
                 * (use POST method)
                 *
                 * @param[in] bundle Properties to set a resource.
                 * @param[in] query  ResourceQuery to specify request.
                 * @return SHRequestHandle request handle.
                 */
                SHRequestHandle setPropertyBundle(const PropertyBundle &bundle,
                        const ResourceQuery &query);

            private:
                SHBaseRemoteResource_Impl *m_remoteResourceImpl;
            };
        }
    }
}

#endif /* SMARTHOME_API_CLIENT_SHBASEREMOTERESOURCE_H_ */
