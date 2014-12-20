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

#include "SubscriptionSerializationFactory.h"
#include "ISerializable.h"
#include "Elements.h"
#include "ResourceTypeEnum.h"

namespace OC
{
    namespace Cm
    {
        namespace Notification
        {

            bool createGetMethodSerializables(int resourceType,
                                              OC::Cm::Serialization::ISerializable **pp_request,
                                              OC::Cm::Serialization::ISerializable **pp_response)
            {
                switch (resourceType)
                {
                    case RT_Subscription:
                        {
                            *pp_request = NULL;
                            *pp_response = new Subscription();
                            return true;
                        }
                    case RT_Subscriptions:
                        {
                            *pp_request = NULL;
                            *pp_response = new Subscriptions();
                            return true;
                        }

                }

                return false;
            }

            bool createPutMethodSerializables(int resourceType,
                                              OC::Cm::Serialization::ISerializable **pp_request,
                                              OC::Cm::Serialization::ISerializable **pp_response)
            {
                switch (resourceType)
                {
                    case RT_Subscription:
                        {
                            *pp_request = new Subscription();
                            *pp_response = NULL;
                            return true;
                        }
                    case RT_Subscriptions:
                        {
                            *pp_request = NULL;
                            *pp_response = NULL;
                            return true;
                        }

                }

                return false;
            }

            bool createPostMethodSerializables(int resourceType,
                                               OC::Cm::Serialization::ISerializable **pp_request,
                                               OC::Cm::Serialization::ISerializable **pp_response)
            {
                switch (resourceType)
                {
                    case RT_Subscription:
                        {
                            *pp_request = new ::Subscription();
                            *pp_response = new ::Subscription();
                            return true;
                        }
                    case RT_Subscriptions:
                        {
                            *pp_request = new ::Subscription();
                            *pp_response = new ::Subscription();
                            return true;
                        }

                }

                return false;
            }

            bool createDeleteMethodSerializables(int resourceType,
                                                 OC::Cm::Serialization::ISerializable **pp_request,
                                                 OC::Cm::Serialization::ISerializable **pp_response)
            {
                switch (resourceType)
                {
                    case RT_Subscription:
                        {
                            *pp_request = NULL;
                            *pp_response = NULL;
                            return true;
                        }
                    case RT_Subscriptions:
                        {
                            *pp_request = NULL;
                            *pp_response = NULL;
                            return true;
                        }

                }

                return false;
            }

            SubscriptionSerializationFactory::SubscriptionSerializationFactory()
            {

            }

            SubscriptionSerializationFactory::~SubscriptionSerializationFactory()
            {

            }

            bool SubscriptionSerializationFactory::createSerializable(
                std::string &elementType,
                OC::Cm::Serialization::ISerializable **pp_output)
            {
                if (0 == elementType.compare("subscription"))
                {
                    *pp_output = new Subscription();
                    return true;
                }
                else if (0 == elementType.compare("subscriptions"))
                {
                    *pp_output = new Subscriptions();
                    return true;
                }

                return false;
            }

            bool SubscriptionSerializationFactory::createMethodSerializables(
                int resourceType, std::string &methodId,
                OC::Cm::Serialization::ISerializable **pp_request,
                OC::Cm::Serialization::ISerializable **pp_response)
            {

                if (methodId.compare("GET") == 0)
                {
                    return createGetMethodSerializables(resourceType, pp_request,
                                                        pp_response);
                }
                else if (methodId.compare("POST") == 0)
                {
                    return createPostMethodSerializables(resourceType, pp_request,
                                                         pp_response);
                }
                else if (methodId.compare("PUT") == 0)
                {
                    return createPutMethodSerializables(resourceType, pp_request,
                                                        pp_response);
                }
                else if (methodId.compare("DELETE") == 0)
                {
                    return createDeleteMethodSerializables(resourceType, pp_request,
                                                           pp_response);
                }

                return false;
            }

            /**
             * Delete request and response object that are identified by specific resource and http method.
             *
             * @param[out] pp_request   Instantiated serializable request object
             * @param[out] pp_response  Instantiated serializable response object
             *
             * @return  @c True If the object is successfully instantiated @n
             *          @c False In case of any error occurred
             */
            bool SubscriptionSerializationFactory::deleteMethodSerializables(
                int responseType, OC::Cm::Serialization::ISerializable *pp_request,
                OC::Cm::Serialization::ISerializable *pp_response)
            {

                switch (responseType)
                {
                    case RT_Subscription:
                    case RT_Subscriptions:
                        if (pp_request != NULL)
                        {
                            delete (pp_request);
                        }
                        if (pp_response != NULL)
                        {
                            delete (pp_response);
                        }
                        return true;
                        break;
                    default:
                        return false;
                }

            }

        }
    }
}
