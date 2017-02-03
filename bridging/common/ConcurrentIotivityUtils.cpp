//******************************************************************
//
// Copyright 2017 Intel Mobile Communications GmbH All Rights Reserved.
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
//

#include <octypes.h>
#include "ConcurrentIotivityUtils.h"
#include "ocpayload.h"
#include "logger.h"

#define TAG "CONCURRENT_IOTIVITY_UTILS"

using namespace OC::Bridging;

// Static member initializations.
std::unique_ptr<WorkQueue<std::unique_ptr<IotivityWorkItem>>> ConcurrentIotivityUtils::m_queue ;
std::map<std::string, OCResourceHandle> ConcurrentIotivityUtils::m_resourceUriToHandleMap;

// Whole function is called with a mutex locked. Extracted out here only for readability.
void ConcurrentIotivityUtils::processWorkItem(std::unique_ptr<IotivityWorkItem> workItem)
{
    if (workItem->m_type == WorkItemType::CREATE)
    {
        OCResourceHandle handle;

        OCStackResult res = OCCreateResource(&handle, workItem->m_resourceType.c_str(), workItem->m_interface.c_str(),
            workItem->m_uri.c_str(), workItem->m_entityHandler, workItem->m_callbackParam, workItem->m_resourceProperties);

        if (res == OC_STACK_OK)
        {
            m_resourceUriToHandleMap[workItem->m_uri] = handle;
            OIC_LOG_V(INFO, TAG,"Created and saved %s", workItem->m_uri.c_str());
        }
        else
        {
            OIC_LOG_V(ERROR, TAG,"Failed to create %s", workItem->m_uri.c_str());
        }
    }
    else if (workItem->m_type == WorkItemType::RESPOND)
    {
        OCDoResponse((workItem->m_response).get());
        OCPayloadDestroy(workItem->m_response->payload);
    }
    else if (workItem->m_type == WorkItemType::NOTIFY)
    {
        OCResourceHandle handle;
        if (getHandleFromUri(workItem->m_uri, &handle))
        {
            OCNotifyAllObservers(handle, OC_NA_QOS);
        }
    }
    else if (workItem->m_type == WorkItemType::DELETE)
    {
        OCResourceHandle handle;
        if (getHandleFromUri(workItem->m_uri, &handle))
        {
            OCStackResult res = OCDeleteResource(handle);
            if (res == OC_STACK_OK)
            {
                m_resourceUriToHandleMap.erase(workItem->m_uri);
                OIC_LOG_V(INFO, TAG,"Deleted %s", workItem->m_uri.c_str());
            }
            else
            {
                OIC_LOG_V(ERROR, TAG,"Failed to delete %s", workItem->m_uri.c_str());
            }
        }
        else
        {
            OIC_LOG_V(ERROR, TAG,"Did not find %s to DELETE", workItem->m_uri.c_str());
        }
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Unknown work item");
    }
}

void ConcurrentIotivityUtils::startWorkerThreads()
{
    if (m_threadStarted)
    {
        throw "Work Queue Processor already started";
    }
    m_processWorkQueueThread = std::thread(&ConcurrentIotivityUtils::processWorkQueue, this);
    m_ocProcessThread = std::thread(&ConcurrentIotivityUtils::callOCProcess, this);
    m_threadStarted = true;
}

void ConcurrentIotivityUtils::stopWorkerThreads()
{
    m_shutDownOCProcessThread = true;
    m_queue->shutdown();
    m_processWorkQueueThread.join();
    m_ocProcessThread.join();
}

void ConcurrentIotivityUtils::createResource(const std::string &uri, const std::string &resourceType,
                                             const std::string &interface, OCEntityHandler entityHandler,
                                             void *callbackParam, uint8_t resourceProperties)
{
    std::unique_ptr<IotivityWorkItem> item = make_unique<CreateResourceItem>(
                    uri, resourceType, interface, entityHandler,callbackParam, resourceProperties
            );

    m_queue->put(std::move(item));
}

void ConcurrentIotivityUtils::respondToRequest(OCEntityHandlerRequest *request, OCRepPayload *payload, OCEntityHandlerResult responseCode)
{
    std::unique_ptr<OCEntityHandlerResponse> response = make_unique<OCEntityHandlerResponse>();

    response->requestHandle = request->requestHandle;
    response->resourceHandle = request->resource;
    response->ehResult = responseCode;

    // Clone a copy since this allocation is going across thread boundaries.
    response->payload = (OCPayload *) OCRepPayloadClone(payload);

    std::unique_ptr<IotivityWorkItem> item = make_unique<SendResponseItem>(std::move(response));
    m_queue->put(std::move(item));
}

void ConcurrentIotivityUtils::respondToRequestWithError(OCEntityHandlerRequest *request, const std::string &errorMessage,
                                              OCEntityHandlerResult errorCode)
{
    OCRepPayload *errorPayload = NULL;

    if (!errorMessage.empty())
    {
        errorPayload = OCRepPayloadCreate();
        OCRepPayloadSetPropString(errorPayload, "x.com.intel.error", errorMessage.c_str());
    }

    respondToRequest(request, errorPayload, errorCode);

    if (errorPayload)
    {
        OCRepPayloadDestroy(errorPayload);
    }
}

bool ConcurrentIotivityUtils::notifyObservers(const std::string &resourceUri)
{
    std::unique_ptr<IotivityWorkItem> item = make_unique<NotifyObserversItem>(resourceUri);
    m_queue->put(std::move(item));
    return true;
}

void ConcurrentIotivityUtils::deleteResource(const std::string &uri)
{
    std::unique_ptr<IotivityWorkItem> item = make_unique<DeleteResourceItem>(uri);
    m_queue->put(std::move(item));
}


bool ConcurrentIotivityUtils::getHandleFromUri(std::string uri, OCResourceHandle *handle)
{
    std::map<std::string, OCResourceHandle>::iterator
        it = m_resourceUriToHandleMap.find(uri);

    if (it == m_resourceUriToHandleMap.end())
    {
        return false;
    }

    *handle = m_resourceUriToHandleMap[uri];

    return true;
}

bool ConcurrentIotivityUtils::getUriFromHandle(OCResourceHandle handle, std::string &uri)
{
    for (auto itr : m_resourceUriToHandleMap)
    {
        if(itr.second == handle)
        {
            uri = itr.first;
            return true;
        }
    }
    return false;
}

bool ConcurrentIotivityUtils::doesResponseNeedFullRepresentation(OCEntityHandlerRequest *request)
{
    if (request->method == OC_REST_PUT || request->method == OC_REST_POST)
    {
        return true;
    }

    // GET query is non null and non empty
    if (request->method == OC_REST_GET && request->query != NULL && *(request->query) != 0)
    {
        std::string query(request->query);

        if (query.find("if=oic.if.baseline") != std::string::npos)
        {
            return true;
        }
    }
    return false;
}



