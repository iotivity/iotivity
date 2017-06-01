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
#include <EntityHandlerWrapper.h>
#include <EntityHandlerContext.h>
#include <SHBaseResource.h>
#include <CommonApi.h>
#include <PropertyBundle.h>
#include <ResourceQuery.h>
#include <SHBaseResource_Impl.h>
#include "logger.h"

#define TAG "SH_SERVER_ENTITY_WRAPPER"

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            OCEntityHandlerResult EntityHandlerWrapper::ResourceEntityHandler(
                                                    OCEntityHandlerFlag flag,
                                                    OCEntityHandlerRequest *entityHandlerRequest,
                                                    void* callbackParam)
            {
                if (NULL == entityHandlerRequest)
                {
                    OIC_LOG(ERROR, TAG, "Entity handler request is NULL");
                    return OC_EH_ERROR;
                }

                if (NULL == callbackParam)
                {
                    OIC_LOG(ERROR, TAG, "callbackParam is NULL");
                    return OC_EH_ERROR;
                }

                // Get Resource object passed when OCCreateResource is called.
                EntityHandlerContext* ctx = static_cast<EntityHandlerContext*>(callbackParam);
                if (!ctx)
                {
                    OIC_LOG(ERROR, TAG, "EntityHandlerContext is NULL");
                    return OC_EH_ERROR;
                }

                // Get resource from EntityHandlerContext.
                const SHBaseResource_Impl* resource = ctx->getResource();
                if (!resource)
                {
                    OIC_LOG(ERROR, TAG, "SHBaseResource_Impl is NULL");
                    return OC_EH_ERROR;
                }

                // Check request flag.
                ResultCode ret = FAIL;
                if (flag & OC_REQUEST_FLAG)
                {
                    OIC_LOG(DEBUG, TAG, "Request message received");

                    ResourceQuery query;
                    PropertyBundle bundle;

                    if (entityHandlerRequest)
                    {
                        // Get query from request.
                        if (entityHandlerRequest->query)
                        {
                            query.setQuery(entityHandlerRequest->query);
                        }
                    }

                    if (OC_REST_GET == entityHandlerRequest->method)
                    {
                        ret = resource->handleGetRequest(entityHandlerRequest->requestHandle,
                                                         query);
                    }
                    else if (OC_REST_POST == entityHandlerRequest->method)
                    {
                        // TODO: Set payload from request.
                        ret = resource->handleSetRequest(entityHandlerRequest->requestHandle,
                                                         bundle, query);
                    }
                    else
                    {
                        OIC_LOG(ERROR, TAG, "Not supported request method.");
                    }
                }
                return (ret == SUCCESS) ? OC_EH_OK : OC_EH_ERROR;
            }
        }
    }
}
