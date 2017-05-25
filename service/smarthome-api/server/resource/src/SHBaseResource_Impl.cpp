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
#include <cstring>
#include <CommonApi.h>
#include <SHBaseResource.h>
#include <SHBaseResource_Impl.h>
#include <EntityHandlerWrapper.h>
#include <EntityHandlerContext.h>
#include <ResourceQuery.h>
#include <PayloadConverter.h>
#include "ocstack.h"
#include "ocpayload.h"
#include "oic_malloc.h"
#include "logger.h"

#define TAG "SH_SERVER_BASE_RESOURCE"

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            // callback for auto response
            // this callback will called by internal delegate
            void sendAutoResponse(ResultCode resultCode,
                                  const OCEntityHandlerRequest* entityHandlerRequest,
                                  const SHBaseResource_Impl* ctx);

            SHBaseResource_Impl::SHBaseResource_Impl(const std::string& uri,
                                                     const std::string& type)
             : m_resourceHandle(NULL),
               m_resourceUri(uri),
               m_context(NULL),
               m_delegate(NULL)
            {
                m_resourceType.push_back(type);
                m_resourceInterface.push_back(INTERFACE::BASELINE);
                registerResource();
            }

            SHBaseResource_Impl::SHBaseResource_Impl(const std::string& uri,
                                                     const std::string& type,
                                                     const std::string& interface)
             : m_resourceHandle(NULL),
               m_resourceUri(uri),
               m_context(NULL),
               m_delegate(NULL)
            {
                m_resourceType.push_back(type);
                m_resourceInterface.push_back(interface);
                registerResource();
            }

            SHBaseResource_Impl::SHBaseResource_Impl(const std::string& uri,
                                                     const std::list<std::string>& types)
             : m_resourceHandle(NULL),
               m_resourceType(types),
               m_resourceUri(uri),
               m_context(NULL),
               m_delegate(NULL)
            {
                m_resourceInterface.push_back(INTERFACE::BASELINE);
                registerResource();
            }

            SHBaseResource_Impl::SHBaseResource_Impl(const std::string& uri,
                                                     const std::list<std::string>& types,
                                                     const std::list<std::string>& interfaces)
             : m_resourceHandle(NULL),
               m_resourceUri(uri),
               m_resourceType(types),
               m_resourceInterface(interfaces),
               m_context(NULL),
               m_delegate(NULL)
            {
                registerResource();
            }

            SHBaseResource_Impl::~SHBaseResource_Impl()
            {
                deregisterResource();
            }

            std::string SHBaseResource_Impl::getUri() const
            {
                return m_resourceUri;
            }

            void SHBaseResource_Impl::setUri(const std::string& uri)
            {
                m_resourceUri = uri;
            }

            std::list<std::string> SHBaseResource_Impl::getTypes() const
            {
                return m_resourceType;
            }

            void SHBaseResource_Impl::setTypes(const std::list<std::string>& types)
            {
                this->bindTypesToResource(types);
                std::list<std::string>::const_iterator iter = types.begin();
                while (iter != types.end())
                {
                    m_resourceType.push_back(*iter);
                    ++iter;
                }
            }

            std::list<std::string> SHBaseResource_Impl::getInterfaces() const
            {
                return m_resourceInterface;
            }

            void SHBaseResource_Impl::setInterfaces(const std::list<std::string>& interfaces)
            {
                this->bindInterfacesToResource(interfaces);
                std::list<std::string>::const_iterator iter = interfaces.begin();
                while (iter != interfaces.end())
                {
                    m_resourceInterface.push_back(*iter);
                    ++iter;
                }
            }

            void SHBaseResource_Impl::setPropertyBundle(const PropertyBundle& bundle)
            {
                OIC_LOG(DEBUG, TAG, "Entered setPropertyBundle");
                m_propertyBundle = bundle;
            }

            const PropertyBundle& SHBaseResource_Impl::getPropertyBundle() const
            {
                OIC_LOG(DEBUG, TAG, "Entered getPropertyBundle");
                return m_propertyBundle;
            }

            bool SHBaseResource_Impl::sendResponse(RequestId requestId,
                                                   const PropertyBundle& bundle)
            {
                OIC_LOG(DEBUG, TAG, "Entered sendResponse");
                return true;
            }

            bool SHBaseResource_Impl::sendErrorResponse(RequestId requestId,
                                                        const PropertyBundle& bundle)
            {
                OIC_LOG(DEBUG, TAG, "Entered sendErrorResponse");
                return true;
            }

            void SHBaseResource_Impl::setDelegate(SHBaseResourceDelegate *delegate)
            {
                OIC_LOG(DEBUG, TAG, "Entered setDelegate");
                m_delegate = delegate;
            }

            void SHBaseResource_Impl::registerResource()
            {
                if (!m_context)
                {
                    m_context = new EntityHandlerContext(this);
                }

                // TODO: we should provide a public api to set resource property.
                m_resourceProperty = OC_DISCOVERABLE;

                // Bind the first element of resource type/interface.
                // Only one resource type/interface can be set at a time.
                std::list<std::string>::iterator typeIter = m_resourceType.begin();
                std::list<std::string>::iterator ifIter = m_resourceInterface.begin();
                OCStackResult result = OCCreateResource(&m_resourceHandle,
                                                        typeIter->c_str(),
                                                        ifIter->c_str(),
                                                        m_resourceUri.c_str(),
                                                        EntityHandlerWrapper::ResourceEntityHandler,
                                                        m_context, m_resourceProperty);
                if (result != OC_STACK_OK)
                {
                    throw CommonException("Exception on OCCreateResource");
                }

                // Skip the first element. It was already bound when OCCreateResource is called.
                typeIter = m_resourceType.begin();
                ++typeIter;
                for (; typeIter != m_resourceType.end(); ++typeIter)
                {
                    result = OCBindResourceInterfaceToResource(m_resourceHandle,
                                                               typeIter->c_str());
                    if (result != OC_STACK_OK)
                    {
                        throw CommonException("Exception on OCBindResourceInterfaceToResource");
                    }
                }

                // Skip the first element. It was already bound when OCCreateResource is called.
                ifIter = m_resourceInterface.begin();
                ++ifIter;
                for (; ifIter != m_resourceInterface.end(); ++ifIter)
                {
                    result = OCBindResourceInterfaceToResource(m_resourceHandle,
                                                               ifIter->c_str());
                    if (result != OC_STACK_OK)
                    {
                        throw CommonException("Exception on OCBindResourceInterfaceToResource");
                    }
                }
            }

            void SHBaseResource_Impl::deregisterResource()
            {
                if (m_context)
                {
                    delete m_context;
                }

                OCStackResult result = OCDeleteResource(m_resourceHandle);
                if (result != OC_STACK_OK)
                {
                    throw CommonException("Exception on OCDeleteResource");
                }
            }

            void SHBaseResource_Impl::bindTypesToResource(std::list<std::string> types)
            {
                std::list<std::string>::const_iterator iter;
                for (iter = types.begin(); iter != types.end(); ++iter)
                {
                    OCStackResult result = OCBindResourceTypeToResource(m_resourceHandle,
                                                                        iter->c_str());
                    if (result != OC_STACK_OK)
                    {
                        throw CommonException("Exception on OCBindResourceTypeToResource");
                    }
                }
            }

            void SHBaseResource_Impl::bindInterfacesToResource(std::list<std::string> interfaces)
            {
                std::list<std::string>::const_iterator iter;
                for (iter = interfaces.begin(); iter != interfaces.end(); ++iter)
                {
                    OCStackResult result = OCBindResourceInterfaceToResource(m_resourceHandle,
                                                                             iter->c_str());
                    if (result != OC_STACK_OK)
                    {
                        throw CommonException("Exception on OCBindResourceInterfaceToResource");
                    }
                }
            }

            ResultCode SHBaseResource_Impl::
                       handleGetRequest(const OCEntityHandlerRequest* entityHandlerRequest,
                                        const SHBaseResource_Impl* resourceContext) const
            {
                OIC_LOG(DEBUG, TAG, "Entered handleGetRequest");

                if (!entityHandlerRequest)
                {
                    OIC_LOG(ERROR, TAG, "entityHandlerRequest is NULL");
                    return FAIL;
                }

                if (!resourceContext)
                {
                    OIC_LOG(ERROR, TAG, "resourceContext is NULL");
                    return FAIL;
                }

                if (!entityHandlerRequest->requestHandle)
                {
                    OIC_LOG(ERROR, TAG, "requestHandle is NULL");
                    return FAIL;
                }

                RequestId requestId = entityHandlerRequest->requestHandle;

                ResourceQuery query;
                if (entityHandlerRequest->query)
                {
                    query.setQuery(entityHandlerRequest->query);
                }

                // Call user onGet delegate callback.
                ResultCode userCbResult = FAIL;
                if (m_delegate)
                {
                    userCbResult = m_delegate->onGet(requestId, query);

                    if (KEEP != userCbResult)
                    {
                        sendAutoResponse(userCbResult, entityHandlerRequest, resourceContext);
                    }
                }

                return userCbResult;
            }

            ResultCode SHBaseResource_Impl::
                       handleSetRequest(const OCEntityHandlerRequest* entityHandlerRequest,
                                        const SHBaseResource_Impl* resourceContext) const
            {
                OIC_LOG(DEBUG, TAG, "Entered handleSetRequest");

                if (!entityHandlerRequest)
                {
                    OIC_LOG(ERROR, TAG, "entityHandlerRequest is NULL");
                    return FAIL;
                }

                if (!resourceContext)
                {
                    OIC_LOG(ERROR, TAG, "resourceContext is NULL");
                    return FAIL;
                }

                RequestId requestId;
                if (entityHandlerRequest->requestHandle)
                {
                    requestId = entityHandlerRequest->requestHandle;
                }
                else
                {
                    OIC_LOG(ERROR, TAG, "requestHandle is NULL");
                    return FAIL;
                }

                ResourceQuery query;
                if (entityHandlerRequest->query)
                {
                    query.setQuery(entityHandlerRequest->query);
                }

                // TODO: Set payload from request.
                PropertyBundle bundle;
                if (Converter::convertPayloadToBundle(entityHandlerRequest->payload, bundle))
                {
                    OIC_LOG(DEBUG, TAG, "Success at convert payload to bundle");
                }
                else
                {
                    OIC_LOG(ERROR, TAG, "Fail at convert payload to bundle");
                    return FAIL;
                }

                // Call user onSet delegate callback.
                ResultCode userCbResult = FAIL;
                if (m_delegate)
                {
                    userCbResult = m_delegate->onSet(requestId, bundle, query);

                    if (KEEP != userCbResult)
                    {
                        sendAutoResponse(userCbResult, entityHandlerRequest, resourceContext);
                    }
                }

                return userCbResult;
            }

            // callback for auto response
            // this callback will called by internal delegate
            void sendAutoResponse(ResultCode resultCode,
                                  const OCEntityHandlerRequest* entityHandlerRequest,
                                  const SHBaseResource_Impl* ctx)
            {
                OIC_LOG(INFO, TAG, "Entered autoResponse");

                if (!entityHandlerRequest)
                {
                    OIC_LOG(ERROR, TAG, "Entity handler request is NULL");
                    return;
                }

                if (!ctx)
                {
                    OIC_LOG(ERROR, TAG, "ctx is NULL");
                    return;
                }

                // TODO: memset
                OCEntityHandlerResponse response;
                memset(&response, 0, sizeof(OCEntityHandlerResponse));

                response.requestHandle = entityHandlerRequest->requestHandle;
                response.resourceHandle = entityHandlerRequest->resource;

                const PropertyBundle& bundle = ctx->getPropertyBundle();

                OCPayload* payload = Converter::convertBundleToPayload(bundle);

                if (!payload)
                {
                    OIC_LOG(ERROR, TAG, "payload is NULL");
                    return;
                }

                response.payload = payload;

                response.persistentBufferFlag = 0;

                response.numSendVendorSpecificHeaderOptions =
                                           entityHandlerRequest->numRcvdVendorSpecificHeaderOptions;

                OIC_LOG_V(DEBUG, TAG, "Send response success");

                for (int itr = 0; itr < response.numSendVendorSpecificHeaderOptions; itr++)
                {
                    response.sendVendorSpecificHeaderOptions[itr].protocolID =
                              entityHandlerRequest->rcvdVendorSpecificHeaderOptions[itr].protocolID;
                    response.sendVendorSpecificHeaderOptions[itr].optionID =
                                entityHandlerRequest->rcvdVendorSpecificHeaderOptions[itr].optionID;
                    response.sendVendorSpecificHeaderOptions[itr].optionLength =
                            entityHandlerRequest->rcvdVendorSpecificHeaderOptions[itr].optionLength;

                    memcpy(response.sendVendorSpecificHeaderOptions[itr].optionData,
                           entityHandlerRequest->rcvdVendorSpecificHeaderOptions[itr].optionData,
                           sizeof(response.sendVendorSpecificHeaderOptions[itr].optionData));
                }

                OCStackResult result = OC_STACK_ERROR;
                if (SUCCESS == resultCode)
                {
                    // send all data as bundle with success code
                    response.ehResult = OC_EH_OK;
                    result = OCDoResponse(&response);
                }
                else if (FAIL == resultCode)
                {
                    // send all data as bundle with fail code
                    response.ehResult = OC_EH_ERROR;
                    result = OCDoResponse(&response);
                }
                else
                {
                    OIC_LOG(ERROR, TAG, "Invalid value on resultCode");
                    OCPayloadDestroy(response.payload);
                    return;
                }

                if (OC_STACK_OK == result)
                {
                    OIC_LOG(DEBUG, TAG, "Send response success");
                }
                else
                {

                    OIC_LOG_V(ERROR, TAG, "Error at send response result code is %d", result);
                }
                OCPayloadDestroy(response.payload);
            }
        }
    }
}
