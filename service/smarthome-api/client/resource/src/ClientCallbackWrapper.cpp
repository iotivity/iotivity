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
#include <SHBaseRemoteDevice.h>
#include <SHBaseRemoteDevice_Impl.h>
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
                void CallbackHelper::destroyRemoteDevice(
                        SHBaseRemoteDevice_Impl* remoteDevice_Impl)
                {
                    if (!remoteDevice_Impl)
                    {
                        OIC_LOG(ERROR, TAG, "invaild argument");
                        return;
                    }

                    std::list<SHBaseRemoteResource*> resourceList =
                            remoteDevice_Impl->getAllResources();

                    for (std::list<SHBaseRemoteResource*>::iterator resItr = resourceList.begin();
                            resItr != resourceList.end(); resItr++)
                    {
                        SHBaseRemoteResource* temp = *resItr;
                        if (temp)
                        {
                            OICFree(temp);
                        }
                    }
                }

                bool CallbackHelper::setRemoteDevice(SHBaseRemoteDevice_Impl* remoteDevice_Impl,
                        OCDiscoveryPayload* discoveryPayload)
                {
                    OIC_LOG(DEBUG, TAG, "Entered setRemoteDevice");

                    if (!remoteDevice_Impl || !discoveryPayload)
                    {
                        OIC_LOG(ERROR, TAG, "invaild argument");
                        return false;
                    }

                    bool hasDefault = false, hasSmartHomeType = false;

                    if (discoveryPayload->type)
                    {
                        for (OCStringLL* typeItr = discoveryPayload->type; typeItr != NULL;
                                typeItr = typeItr->next)
                        {
                            if (0 == DEVICE_TYPE::UNKNOWN.compare(typeItr->value))
                            {
                                hasDefault = true;
                            }
                            else if (0 == DEVICE_TYPE::LIGHT.compare(typeItr->value))
                            {
                                remoteDevice_Impl->setType(DEVICE_TYPE::LIGHT);
                                hasSmartHomeType = true;
                            }
                            else if (0 == DEVICE_TYPE::ROBOTCLEANER.compare(typeItr->value))
                            {
                                remoteDevice_Impl->setType(DEVICE_TYPE::ROBOTCLEANER);
                                hasSmartHomeType = true;
                            }
                            else if (0 == DEVICE_TYPE::SMART_LOCK.compare(typeItr->value))
                            {
                                remoteDevice_Impl->setType(DEVICE_TYPE::SMART_LOCK);
                                hasSmartHomeType = true;
                            }
                            else if (0 == DEVICE_TYPE::DRYER.compare(typeItr->value))
                            {
                                remoteDevice_Impl->setType(DEVICE_TYPE::DRYER);
                                hasSmartHomeType = true;
                            }
                        }
                    }

                    if (!hasSmartHomeType)
                    {
                        remoteDevice_Impl->setType(DEVICE_TYPE::UNKNOWN);
                    }

                    bool isSuccess = true;
                    OCResourcePayload* resPayload = discoveryPayload->resources;
                    while (resPayload)
                    {
                        // TODO: remove annotation after became firend class
                        // SHBaseRemoteResource* remoteResource = new SHBaseRemoteResource();
                        SHBaseRemoteResource* remoteResource;

                        if (!CallbackHelper::setRemoteResource(remoteResource, resPayload))
                        {
                            OICFree(remoteResource);
                            isSuccess = false;
                            break;
                        }

                        remoteDevice_Impl->addResource(remoteResource);

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

                bool CallbackHelper::setRemoteResource(SHBaseRemoteResource* remoteResource,
                        OCResourcePayload* resourcePayload)
                {
                    //TODO
                }

                void CallbackHelper::CallbackContextDeleter(void* context)
                {

                }

                OCStackApplicationResult ClientCallbackWrapper::DeviceDiscoveryCallback
                ::findRemoteDevices(void *context, OCDoHandle handle,
                        OCClientResponse * clientResponse)
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

                    OCDiscoveryPayload* disPayload =
                            (OCDiscoveryPayload*) (clientResponse->payload);
                    if (!disPayload)
                    {
                        OIC_LOG(ERROR, TAG, "OCDiscoveryPayload is NULL!!!");
                        return OC_STACK_KEEP_TRANSACTION;
                    }

                    OCDiscoveryPayload* device = disPayload;
                    while (device)
                    {
                        // TODO: remove annotation after became firend class
                        // SHBaseRemoteDevice* remoteDevice = new SHBaseRemoteDevice();
                        SHBaseRemoteDevice* remoteDevice;
                        if (!remoteDevice)
                        {
                            OIC_LOG(ERROR, TAG, "RemoteDevice is NULL!!!");
                            return OC_STACK_KEEP_TRANSACTION;
                        }

                        SHBaseRemoteDevice_Impl* remoteDevice_Impl =
                                dynamic_cast<SHBaseRemoteDevice_Impl*>(remoteDevice);
                        if (!remoteDevice_Impl)
                        {
                            OIC_LOG(ERROR, TAG, "RemoteDevice_Impl is NULL!!!");
                            OICFree(remoteDevice_Impl);
                            return OC_STACK_KEEP_TRANSACTION;
                        }

                        if (!CallbackHelper::setRemoteDevice(remoteDevice_Impl, disPayload))
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
                        // CallbackHelper::destroyRemoteDevice(remoteDevice_Impl);

                        device = device->next;
                    }

                    return OC_STACK_KEEP_TRANSACTION;
                }

                OCStackApplicationResult ClientCallbackWrapper::DeviceDiscoveryCallback
                ::findRemoteDevicesWithQuery(void *context, OCDoHandle handle,
                        OCClientResponse * clientResponse)
                {
                    return OC_STACK_DELETE_TRANSACTION;
                }

                OCStackApplicationResult ClientCallbackWrapper::DeviceDiscoveryCallback
                ::findRemoteDevicesAll(void *context, OCDoHandle handle,
                        OCClientResponse * clientResponse)
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

                    OCDiscoveryPayload* disPayload =
                            (OCDiscoveryPayload*) (clientResponse->payload);
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
                        // SHBaseRemoteDevice* remoteDevice = new SHBaseRemoteDevice();
                        SHBaseRemoteDevice* remoteDevice;
                        if (!remoteDevice)
                        {
                            OIC_LOG(ERROR, TAG, "RemoteDevice is NULL!!!");
                            return OC_STACK_KEEP_TRANSACTION;
                        }

                        SHBaseRemoteDevice_Impl* remoteDevice_Impl =
                                dynamic_cast<SHBaseRemoteDevice_Impl*>(remoteDevice);
                        if (!remoteDevice_Impl)
                        {
                            OIC_LOG(ERROR, TAG, "RemoteDevice_Impl is NULL!!!");
                            OICFree(remoteDevice_Impl);
                            return OC_STACK_KEEP_TRANSACTION;
                        }

                        if (!CallbackHelper::setRemoteDevice(remoteDevice_Impl, disPayload))
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
                        SHBaseRemoteDevice_Impl* remoteDevice_Impl =
                                dynamic_cast<SHBaseRemoteDevice_Impl*>(*itr);
                        if (!remoteDevice_Impl)
                        {
                            OIC_LOG(ERROR, TAG, "RemoteDevice_Impl is NULL!!!");
                            return OC_STACK_KEEP_TRANSACTION;
                        }

                        // TODO
                        // remove annotation after apply smartPtr.
                        // issue: there is no copy ctor on SHBaseRemoteDevice.
                        // CallbackHelper::destroyRemoteDevice(remoteDevice_Impl);
                    }

                    return OC_STACK_KEEP_TRANSACTION;
                }

                OCStackApplicationResult ClientCallbackWrapper::DeviceDiscoveryCallback
                ::findRemoteDevicesAllWithQuery(void *context, OCDoHandle handle,
                        OCClientResponse * clientResponse)
                {
                    return OC_STACK_DELETE_TRANSACTION;
                }

                OCStackApplicationResult ClientCallbackWrapper::RemoteResourceCallback::onGet(
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

                    OIC_LOG_V(DEBUG, TAG, "%s: call user onGet Delegate callback", __func__);
                    delegate->onGet(bundle,
                            (clientResponse->result == OC_STACK_OK) ? SUCCESS : FAIL);

                    return OC_STACK_DELETE_TRANSACTION;
                }

                OCStackApplicationResult ClientCallbackWrapper::RemoteResourceCallback::onSet(
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
            }
        }
    }
}

