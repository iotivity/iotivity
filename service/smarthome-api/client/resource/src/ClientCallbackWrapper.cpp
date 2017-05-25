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

#include <list>
#include <ClientCallbackWrapper.h>
#include <CommonApi.h>
#include <SHBaseRemoteResource.h>
#include <SHBaseRemoteResourceBuilder.h>
#include <SHBaseRemoteDevice.h>
#include <SHBaseRemoteDevice_Impl.h>
#include <SHBaseRemoteDeviceBuilder.h>
#include <ClientCallbackContext.h>
#include <PropertyBundle.h>
#include "logger.h"
#include "oic_malloc.h"
#include <DeviceDiscovery.h>
#include <PayloadConverter.h>

#define TAG "SMD_CLIENT_CALLBACK_WRAPPER"

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            namespace ClientCallbackWrapper
            {
                void CallbackHelper::destroyRemoteDevice(SHBaseRemoteDevice* remoteDevice)
                {
                    if (!remoteDevice)
                    {
                        OIC_LOG(ERROR, TAG, "invaild argument");
                        return;
                    }

                    OICFree(remoteDevice);
                }

                bool CallbackHelper::setRemoteDevice(SHBaseRemoteDevice* remoteDevice,
                                                     OCDevAddr& devAddr,
                                                     OCDiscoveryPayload* discoveryPayload)
                {
                    OIC_LOG(DEBUG, TAG, "Entered setRemoteDevice");

                    if (!remoteDevice || !discoveryPayload)
                    {
                        OIC_LOG(ERROR, TAG, "invaild argument");
                        return false;
                    }

                    bool isSuccess = true;
                    OCResourcePayload* resPayload = discoveryPayload->resources;
                    while (resPayload)
                    {
                        SHBaseRemoteResource* remoteResource =
                            SHBaseRemoteResourceBuilder::createSHBaseRemoteResource(devAddr,
                                                                                    resPayload);

                        if (!SHBaseRemoteDeviceBuilder::mappingResource(remoteDevice,
                                                                        remoteResource))
                        {
                            OICFree(remoteResource);
                            isSuccess = false;
                            break;
                        }

                        resPayload = resPayload->next;
                    }

                    if (!isSuccess)
                    {
                        // TODO
                        // remove annotation after apply smartPtr.
                        // issue: there is no copy ctor on SHBaseRemoteDevice.
                        // CallbackHelper::destroyRemoteDevice(remoteDevice_Impl);
                        return false;
                    }

                    return true;
                }

                void CallbackHelper::CallbackContextDeleter(void* context)
                {

                }

                OCStackApplicationResult DeviceDiscoveryCallback::findRemoteDevices(
                        void *context, OCDoHandle handle, OCClientResponse * clientResponse)
                {
                    OIC_LOG(DEBUG, TAG, "Entered findRemoteDevices");

                    if (!context || !handle || !clientResponse)
                    {
                        OIC_LOG(ERROR, TAG, "invaild argument");
                        return OC_STACK_KEEP_TRANSACTION;
                    }

                    if (clientResponse->result != OC_STACK_OK)
                    {
                        OIC_LOG_V(ERROR, TAG, "clientResponse is %d", clientResponse->result);
                        return OC_STACK_KEEP_TRANSACTION;
                    }

                    if (!clientResponse->payload
                            || clientResponse->payload->type != PAYLOAD_TYPE_DISCOVERY)
                    {
                        OIC_LOG(ERROR, TAG, "clientResponse payload was null or the wrong type");
                        return OC_STACK_KEEP_TRANSACTION;
                    }

                    ClientCallbackContext* ctx = static_cast<ClientCallbackContext*>(context);

                    void* delegatePtr = ctx->getDelegate();
                    if (!delegatePtr)
                    {
                        OIC_LOG(ERROR, TAG, "delegatePtr is NULL!!!");
                        return OC_STACK_KEEP_TRANSACTION;
                    }

                    DeviceDiscoveryDelegate* delegate =
                            static_cast<DeviceDiscoveryDelegate*>(delegatePtr);
                    if (!delegate)
                    {
                        OIC_LOG(ERROR, TAG, "DeviceDiscoveryDelegate is NULL!!!");
                        return OC_STACK_KEEP_TRANSACTION;
                    }

                    OCDiscoveryPayload* disPayload = (OCDiscoveryPayload*)(clientResponse->payload);
                    if (!disPayload)
                    {
                        OIC_LOG(ERROR, TAG, "OCDiscoveryPayload is NULL!!!");
                        return OC_STACK_KEEP_TRANSACTION;
                    }

                    OCDiscoveryPayload* device = disPayload;
                    while (device)
                    {
                        SHBaseRemoteDevice* remoteDevice =
                            SHBaseRemoteDeviceBuilder::createSHBaseRemoteDevice(device);
                        if (!remoteDevice)
                        {
                            OIC_LOG(ERROR, TAG, "RemoteDevice is NULL!!!");
                            return OC_STACK_KEEP_TRANSACTION;
                        }

                        if (!CallbackHelper::setRemoteDevice(remoteDevice,
                                                             clientResponse->devAddr,
                                                             disPayload))
                        {
                            // TODO
                            // Call Error Callback
                            return OC_STACK_KEEP_TRANSACTION;
                        }

                        OIC_LOG_V(DEBUG, TAG, "%s: call user onFindRemoteDevice Delegate callback",
                                __func__);
                        delegate->onFindRemoteDevice(remoteDevice);

                        // TODO
                        // remove annotation after apply smartPtr.
                        // issue: there is no copy ctor on SHBaseRemoteDevice.
                        // CallbackHelper::destroyRemoteDevice(remoteDevice);

                        device = device->next;
                    }

                    return OC_STACK_KEEP_TRANSACTION;
                }

                OCStackApplicationResult DeviceDiscoveryCallback::findRemoteDevicesWithQuery(
                        void *context, OCDoHandle handle, OCClientResponse * clientResponse)
                {
                    return OC_STACK_DELETE_TRANSACTION;
                }

                OCStackApplicationResult DeviceDiscoveryCallback::findRemoteDevicesAll(
                        void *context, OCDoHandle handle, OCClientResponse * clientResponse)
                {
                    OIC_LOG(DEBUG, TAG, "Entered findRemoteDevicesAll");

                    if (!context || !handle || !clientResponse)
                    {
                        OIC_LOG(ERROR, TAG, "invaild argument");
                        return OC_STACK_KEEP_TRANSACTION;
                    }

                    if (clientResponse->result != OC_STACK_OK)
                    {
                        OIC_LOG_V(ERROR, TAG, "clientResponse is %d", clientResponse->result);
                        return OC_STACK_KEEP_TRANSACTION;
                    }

                    if (!clientResponse->payload
                            || clientResponse->payload->type != PAYLOAD_TYPE_DISCOVERY)
                    {
                        OIC_LOG(ERROR, TAG, "clientResponse payload was null or the wrong type");
                        return OC_STACK_KEEP_TRANSACTION;
                    }

                    ClientCallbackContext* ctx = static_cast<ClientCallbackContext*>(context);
                    if (!ctx)
                    {
                        OIC_LOG(ERROR, TAG, "ClientCallbackContext is NULL!!!");
                        return OC_STACK_KEEP_TRANSACTION;
                    }

                    void* delegatePtr = ctx->getDelegate();
                    if (!delegatePtr)
                    {
                        OIC_LOG(ERROR, TAG, "delegatePtr is NULL!!!");
                        return OC_STACK_KEEP_TRANSACTION;
                    }

                    DeviceDiscoveryDelegate* delegate =
                            static_cast<DeviceDiscoveryDelegate*>(delegatePtr);
                    if (!delegate)
                    {
                        OIC_LOG(ERROR, TAG, "DeviceDiscoveryDelegate is NULL!!!");
                        return OC_STACK_KEEP_TRANSACTION;
                    }

                    OCDiscoveryPayload* disPayload = (OCDiscoveryPayload*)(clientResponse->payload);
                    if (!disPayload)
                    {
                        OIC_LOG(ERROR, TAG, "OCDiscoveryPayload is NULL!!!");
                        return OC_STACK_KEEP_TRANSACTION;
                    }

                    std::list<SHBaseRemoteDevice*> remoteDevices;
                    OCDiscoveryPayload* device = disPayload;
                    while (device)
                    {
                        // TODO: remove annotation after became firend class
                        SHBaseRemoteDevice* remoteDevice =
                            SHBaseRemoteDeviceBuilder::createSHBaseRemoteDevice(device);
                        if (!remoteDevice)
                        {
                            OIC_LOG(ERROR, TAG, "RemoteDevice is NULL!!!");
                            return OC_STACK_KEEP_TRANSACTION;
                        }

                        if (!CallbackHelper::setRemoteDevice(remoteDevice,
                                                             clientResponse->devAddr,
                                                             disPayload))
                        {
                            // TODO
                            // Call Error Callback
                            return OC_STACK_KEEP_TRANSACTION;
                        }

                        remoteDevices.push_back(remoteDevice);

                        device = device->next;
                    }

                    OIC_LOG_V(DEBUG, TAG, "%s: call user onFindeRemoteDevice Delegate callback",
                            __func__);
                    delegate->onFindRemoteDevice(remoteDevices);

                    for (std::list<SHBaseRemoteDevice*>::iterator itr = remoteDevices.begin();
                            itr != remoteDevices.end(); itr++)
                    {
                        // TODO
                        // remove annotation after apply smartPtr.
                        // issue: there is no copy ctor on SHBaseRemoteDevice.
                        // CallbackHelper::destroyRemoteDevice(remoteDevice_Impl);
                    }

                    return OC_STACK_KEEP_TRANSACTION;
                }

                OCStackApplicationResult DeviceDiscoveryCallback::findRemoteDevicesAllWithQuery(
                        void *context, OCDoHandle handle, OCClientResponse * clientResponse)
                {
                    return OC_STACK_DELETE_TRANSACTION;
                }

                OCStackApplicationResult RemoteResourceCallback::onGet(
                        void *context, OCDoHandle handle, OCClientResponse * clientResponse)
                {
                    OIC_LOG(DEBUG, TAG, "Entered onGet");

                    if (!context || !handle || !clientResponse)
                    {
                        OIC_LOG(ERROR, TAG, "invaild argument");
                        return OC_STACK_DELETE_TRANSACTION;
                    }

                    ClientCallbackContext* ctx = static_cast<ClientCallbackContext*>(context);
                    if (!ctx)
                    {
                        OIC_LOG(ERROR, TAG, "ClientCallbackContext is NULL!!!");
                        return OC_STACK_DELETE_TRANSACTION;
                    }

                    void* delegatePtr = ctx->getDelegate();
                    if (!delegatePtr)
                    {
                        OIC_LOG(ERROR, TAG, "delegatePtr is NULL!!!");
                        return OC_STACK_DELETE_TRANSACTION;
                    }

                    SHBaseRemoteResourceDelegate* delegate =
                            static_cast<SHBaseRemoteResourceDelegate*>(delegatePtr);
                    if (!delegate)
                    {
                        OIC_LOG(ERROR, TAG, "SHBaseRemoteResourceDelegate is NULL!!!");
                        return OC_STACK_DELETE_TRANSACTION;
                    }

                    if (!clientResponse->payload
                            || clientResponse->payload->type != PAYLOAD_TYPE_REPRESENTATION)
                    {
                        OIC_LOG(ERROR, TAG, "clientResponse payload was null or the wrong type");

                        PropertyBundle bundle;
                        OIC_LOG_V(DEBUG, TAG, "%s: call user onGet Delegate callback", __func__);
                        delegate->onGet(bundle,
                                (clientResponse->result == OC_STACK_OK) ? SUCCESS : FAIL);

                        return OC_STACK_DELETE_TRANSACTION;
                    }

                    OCRepPayload* repPayload = (OCRepPayload*) (clientResponse->payload);
                    if (!repPayload)
                    {
                        OIC_LOG(ERROR, TAG, "OCRepPayload is NULL!!!");

                        PropertyBundle bundle;

                        OIC_LOG_V(DEBUG, TAG, "%s: call user onGet Delegate callback", __func__);
                        delegate->onGet(bundle,
                                (clientResponse->result == OC_STACK_OK) ? SUCCESS : FAIL);
                        return OC_STACK_DELETE_TRANSACTION;
                    }

                    PropertyBundle bundle;

                    if (Converter::convertPayloadToBundle((OCPayload*)repPayload, bundle))
                    {
                        OIC_LOG(DEBUG, TAG, "Success at convert payload to bundle");
                    }
                    else
                    {
                        OIC_LOG(ERROR, TAG, "Fail at convert payload to bundle");
                        // TODO
                        // call error callback
                        // return OC_STACK_DELETE_TRANSACTION;
                    }

                    OIC_LOG_V(DEBUG, TAG, "%s: call user onGet Delegate callback", __func__);
                    delegate->onGet(bundle,
                            (clientResponse->result == OC_STACK_OK) ? SUCCESS : FAIL);

                    return OC_STACK_DELETE_TRANSACTION;
                }

                OCStackApplicationResult RemoteResourceCallback::onSet(
                        void *context, OCDoHandle handle, OCClientResponse * clientResponse)
                {
                    OIC_LOG(DEBUG, TAG, "Entered onSet");

                    if (!context || !handle || !clientResponse)
                    {
                        OIC_LOG(ERROR, TAG, "invaild argument");
                        return OC_STACK_DELETE_TRANSACTION;
                    }

                    ClientCallbackContext* ctx = static_cast<ClientCallbackContext*>(context);
                    if (!ctx)
                    {
                        OIC_LOG(ERROR, TAG, "ClientCallbackContext is NULL!!!");
                        return OC_STACK_DELETE_TRANSACTION;
                    }

                    void* delegatePtr = ctx->getDelegate();
                    if (!delegatePtr)
                    {
                        OIC_LOG(ERROR, TAG, "delegatePtr is NULL!!!");
                        return OC_STACK_DELETE_TRANSACTION;
                    }

                    SHBaseRemoteResourceDelegate* delegate =
                            static_cast<SHBaseRemoteResourceDelegate*>(delegatePtr);
                    if (!delegate)
                    {
                        OIC_LOG(ERROR, TAG, "SHBaseRemoteResourceDelegate is NULL!!!");
                        return OC_STACK_DELETE_TRANSACTION;
                    }

                    if (!clientResponse->payload
                            || clientResponse->payload->type != PAYLOAD_TYPE_REPRESENTATION)
                    {
                        OIC_LOG(ERROR, TAG, "clientResponse payload was null or the wrong type");

                        PropertyBundle bundle;

                        OIC_LOG_V(DEBUG, TAG, "%s: call user onSet Delegate callback", __func__);
                        delegate->onSet(bundle,
                                (clientResponse->result == OC_STACK_OK
                                        || clientResponse->result == OC_STACK_RESOURCE_CREATED
                                        || clientResponse->result == OC_STACK_RESOURCE_CHANGED) ?
                                        SUCCESS : FAIL);

                        return OC_STACK_DELETE_TRANSACTION;
                    }

                    OCRepPayload* repPayload = (OCRepPayload*) (clientResponse->payload);
                    if (!repPayload)
                    {
                        OIC_LOG(ERROR, TAG, "OCRepPayload is NULL!!!");

                        PropertyBundle bundle;

                        OIC_LOG_V(DEBUG, TAG, "%s: call user onSet Delegate callback", __func__);
                        delegate->onSet(bundle,
                                (clientResponse->result == OC_STACK_OK
                                        || clientResponse->result == OC_STACK_RESOURCE_CREATED
                                        || clientResponse->result == OC_STACK_RESOURCE_CHANGED) ?
                                        SUCCESS : FAIL);

                        return OC_STACK_DELETE_TRANSACTION;
                    }

                    PropertyBundle bundle;
                    if (Converter::convertPayloadToBundle((OCPayload*) repPayload, bundle))
                    {
                        OIC_LOG(DEBUG, TAG, "Success at convert payload to bundle");
                    }
                    else
                    {
                        OIC_LOG(ERROR, TAG, "Fail at convert payload to bundle");
                        // TODO
                        // call error callback
                        // return OC_STACK_DELETE_TRANSACTION;
                    }

                    OIC_LOG_V(DEBUG, TAG, "%s: call user onSet Delegate callback", __func__);
                    delegate->onSet(bundle,
                            (clientResponse->result == OC_STACK_OK
                                    || clientResponse->result == OC_STACK_RESOURCE_CREATED
                                    || clientResponse->result == OC_STACK_RESOURCE_CHANGED) ?
                                    SUCCESS : FAIL);

                    return OC_STACK_DELETE_TRANSACTION;
                }

                OCStackApplicationResult RemoteResourceCallback::onObserve(
                        void *context, OCDoHandle handle, OCClientResponse * clientResponse)
                {
                    OIC_LOG(DEBUG, TAG, "Entered onObserve");

                    if (!context || !handle || !clientResponse)
                    {
                        OIC_LOG(ERROR, TAG, "invaild argument");
                        return OC_STACK_KEEP_TRANSACTION;
                    }

                    ClientCallbackContext *ctx = static_cast<ClientCallbackContext*>(context);
                    if (!ctx)
                    {
                        OIC_LOG(ERROR, TAG, "ClientCallbackContext is NULL!!!");
                        return OC_STACK_KEEP_TRANSACTION;
                    }

                    void* delegatePtr = ctx->getDelegate();
                    if (!delegatePtr)
                    {
                        OIC_LOG(ERROR, TAG, "delegatePtr is NULL!!!");
                        return OC_STACK_KEEP_TRANSACTION;
                    }

                    SHBaseRemoteResourceDelegate *delegate =
                        static_cast<SHBaseRemoteResourceDelegate*>(delegatePtr);
                    if (!delegate)
                    {
                        OIC_LOG(ERROR, TAG, "SHBaseRemoteResourceDelegate is NULL!!!");
                        return OC_STACK_KEEP_TRANSACTION;
                    }

                    ObserveResponse obsType = REGISTER;
                    OIC_LOG_V(DEBUG, TAG, "observe sequence number : %d",
                            clientResponse->sequenceNumber);
                    if (clientResponse->sequenceNumber == OC_OBSERVE_REGISTER)
                    {
                        obsType = REGISTER;
                    }
                    else if (clientResponse->sequenceNumber > MAX_SEQUENCE_NUMBER)
                    {
                        obsType = UNREGISTER;
                        OIC_LOG(DEBUG, TAG, "this is observe cancel response");

                        PropertyBundle bundle;
                        delegate->onObserve(bundle, obsType,
                                        (clientResponse->result == OC_STACK_OK) ? SUCCESS : FAIL);

                        return OC_STACK_DELETE_TRANSACTION;
                    }
                    else
                    {
                        obsType = NOTIFY;
                    }

                    if (!clientResponse->payload ||
                        clientResponse->payload->type != PAYLOAD_TYPE_REPRESENTATION)
                    {
                        OIC_LOG(ERROR, TAG, "clientResponse payload was null or the wrong type");

                        PropertyBundle bundle;

                        OIC_LOG_V(DEBUG, TAG, "%s: call user onObserve Delegate callback",
                                __func__);
                        delegate->onObserve(bundle, obsType,
                                        (clientResponse->result == OC_STACK_OK) ? SUCCESS : FAIL);

                        return OC_STACK_KEEP_TRANSACTION;
                    }

                    OCRepPayload *repPayload = (OCRepPayload*)(clientResponse->payload);
                    if (!repPayload)
                    {
                        OIC_LOG(ERROR, TAG, "OCRepPayload is NULL!!!");

                        PropertyBundle bundle;

                        OIC_LOG_V(DEBUG, TAG, "%s: call user onObserve Delegate callback",
                                __func__);
                        delegate->onObserve(bundle, obsType,
                                        (clientResponse->result == OC_STACK_OK) ? SUCCESS : FAIL);

                        return OC_STACK_KEEP_TRANSACTION;
                    }

                    PropertyBundle bundle;
                    if (Converter::convertPayloadToBundle((OCPayload*)repPayload, bundle))
                    {
                        OIC_LOG(DEBUG, TAG, "Success at convert payload to bundle");
                    }
                    else
                    {
                        OIC_LOG(ERROR, TAG, "Fail at convert payload to bundle");
                        // TODO
                        // call error callback
                        // return OC_STACK_KEEP_TRANSACTION;
                    }

                    OIC_LOG_V(DEBUG, TAG, "%s: call user onObserve Delegate callback", __func__);
                    delegate->onObserve(bundle, obsType,
                                    (clientResponse->result == OC_STACK_OK) ? SUCCESS : FAIL);

                    return OC_STACK_KEEP_TRANSACTION;
                }
            }
        }
    }
}

