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

#include <sstream>
#include "octypes.h"
#include "ConcurrentIotivityUtils.h"
#include "ocpayload.h"
#include "experimental/logger.h"

#define TAG "CONCURRENT_IOTIVITY_UTILS"

using namespace OC::Bridging;

// Static member initializations.
std::unique_ptr<WorkQueue<std::unique_ptr<IotivityWorkItem>>> ConcurrentIotivityUtils::m_queue;

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
    m_threadStarted = false;
}

OCStackResult ConcurrentIotivityUtils::queueCreateResource(const std::string &uri,
        const std::string &resourceType,
        const std::string &interface, OCEntityHandler entityHandler,
        void *callbackParam, uint8_t resourceProperties)
{
    std::unique_ptr<IotivityWorkItem> item = make_unique<CreateResourceItem>(
                uri, resourceType, interface, entityHandler, callbackParam, resourceProperties
            );

    m_queue->put(std::move(item));

    return OC_STACK_OK;
}

OCStackResult ConcurrentIotivityUtils::respondToRequest(OCEntityHandlerRequest *request,
        OCRepPayload *payload, OCEntityHandlerResult responseCode)
{
    std::unique_ptr<OCEntityHandlerResponse> response = make_unique<OCEntityHandlerResponse>();

    response->requestHandle = request->requestHandle;
    response->ehResult = responseCode;

    // Clone a copy since this allocation is going across thread boundaries.
    response->payload = (OCPayload *) OCRepPayloadClone(payload);

    if (payload != NULL && response->payload == NULL)
    {
        return OC_STACK_NO_MEMORY;
    }

    std::unique_ptr<IotivityWorkItem> item = make_unique<SendResponseItem>(std::move(response));
    m_queue->put(std::move(item));

    return OC_STACK_OK;
}

OCStackResult ConcurrentIotivityUtils::respondToRequestWithError(OCEntityHandlerRequest *request,
        const std::string &errorMessage,
        OCEntityHandlerResult errorCode)
{
    OCRepPayload *errorPayload = NULL;

    if (!errorMessage.empty())
    {
        errorPayload = OCRepPayloadCreate();

        if (!errorPayload)
        {
            return OC_STACK_NO_MEMORY;
        }

        OCRepPayloadSetPropString(errorPayload, "x.org.iotivity.error", errorMessage.c_str());
    }

    OCStackResult res = respondToRequest(request, errorPayload, errorCode);

    if (errorPayload)
    {
        OCRepPayloadDestroy(errorPayload);
    }

    return res;
}

OCStackResult ConcurrentIotivityUtils::queueNotifyObservers(const std::string &resourceUri)
{
    std::unique_ptr<IotivityWorkItem> item = make_unique<NotifyObserversItem>(resourceUri);
    m_queue->put(std::move(item));
    return OC_STACK_OK;
}

OCStackResult ConcurrentIotivityUtils::queueDeleteResource(const std::string &uri)
{
    std::unique_ptr<IotivityWorkItem> item = make_unique<DeleteResourceItem>(uri);
    m_queue->put(std::move(item));
    return OC_STACK_OK;
}

bool ConcurrentIotivityUtils::getUriFromHandle(OCResourceHandle handle, std::string &uri)
{
    const char *uri_c = OCGetResourceUri(handle);

    if (uri_c == NULL)
    {
        return false;
    }

    uri = uri_c;
    return true;
}

void ConcurrentIotivityUtils::getKeyValueParams(const std::string &query,
        std::map<std::string, std::string> &keyValueMap)
{
    if (query.empty())
    {
        return;
    }

    std::stringstream ss(query);

    std::string keyValuePair;

    while (std::getline(ss, keyValuePair, '&'))
    {
        auto keyValueSeparator = keyValuePair.find('=');

        if (keyValueSeparator == std::string::npos)
        {
            continue;
        }

        std::string key = keyValuePair.substr(0, keyValueSeparator);
        std::string value = keyValuePair.substr(keyValueSeparator + 1);

        keyValueMap[key] = value;
    }
}

bool ConcurrentIotivityUtils::isRequestForDefaultInterface(const std::string &query)
{
    if (query.empty())
    {
        return false;
    }
    std::map<std::string, std::string> keyValueParams;

    getKeyValueParams(query, keyValueParams);

    auto it = keyValueParams.find(OC_RSRVD_INTERFACE);

    if (it == keyValueParams.end())
    {
        return false;
    }

    return it->second == OC_RSRVD_INTERFACE_DEFAULT;
}
