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
#include <SHBaseRemoteDeviceBuilder.h>
#include <SHBaseRemoteDevice_Impl.h>
#include <CommonApi.h>
#include <RemoteLightDevice.h>
#include <RemoteRobotCleanerDevice.h>
#include <RemoteSmartLockDevice.h>
#include <RemoteDryerDevice.h>
#include <RemoteWaterValveDevice.h>
#include <RemoteBlindDevice.h>
#include <RemoteCameraDevice.h>
#include <RemotePrinterDevice.h>
#include <RemoteBinarySwitchResource.h>
#include <RemoteModeResource.h>
#include <RemoteLockStatusResource.h>
#include <RemoteOpenLevelResource.h>
#include <RemoteMediaResource.h>
#include <RemoteOperationalStateResource.h>
#include "logger.h"
#include <typeinfo>

#define TAG "OIC_SH_CLIENT_SHBASEREMOTEDEVICE_BUILDER"

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            SHBaseRemoteDevice* SHBaseRemoteDeviceBuilder::createSHBaseRemoteDevice(
                    OCDiscoveryPayload *payload)
            {
                OCResourcePayload *resource = payload->resources;
                OCResourcePayload *deviceResource = NULL;
                while (resource)
                {
                    if (0 == RESERVED_URI::DEVICE.compare(resource->uri))
                    {
                        deviceResource = resource;
                        OIC_LOG(DEBUG, TAG, "device resource find!");
                        break;
                    }
                    resource = resource->next;
                }

                if (!deviceResource)
                {
                    OIC_LOG_V(ERROR, TAG, "can't find %s", RESERVED_URI::DEVICE.c_str());
                    return NULL;
                }

                SHBaseRemoteDevice *device = makeSHBaseRemoteDevice(deviceResource,
                                                                    payload->sid);
                if (!device)
                {
                    OIC_LOG(ERROR, TAG, "makeSHBaseRemoteDevice failed!");
                    return NULL;
                }

                return device;
            }

            bool SHBaseRemoteDeviceBuilder::mappingResource(SHBaseRemoteDevice *device,
                                                            SHBaseRemoteResource *resource)
            {
                if (!device || !resource)
                {
                    OIC_LOG(ERROR, TAG, "invalid parameter");
                    return false;
                }

                try
                {
                    if (device->hasDeviceType(DEVICE_TYPE::LIGHT))
                    {
                        RemoteLightDevice *light = dynamic_cast<RemoteLightDevice*>(device);

                        if (resource->hasResourceType(RESOURCE_TYPE::BINARYSWITCH))
                        {
                            light->m_remoteBinarySwitch =
                                dynamic_cast<RemoteBinarySwitchResource*>(resource);
                        }
                    }
                    else if (device->hasDeviceType(DEVICE_TYPE::ROBOTCLEANER))
                    {
                        RemoteRobotCleanerDevice *robotCleaner =
                            dynamic_cast<RemoteRobotCleanerDevice*>(device);

                        if (resource->hasResourceType(RESOURCE_TYPE::BINARYSWITCH))
                        {
                            robotCleaner->m_remoteBinarySwitch =
                                dynamic_cast<RemoteBinarySwitchResource*>(resource);
                        }
                        else if (resource->hasResourceType(RESOURCE_TYPE::MODE))
                        {
                            robotCleaner->m_remoteMode =
                                dynamic_cast<RemoteModeResource*>(resource);
                        }
                    }
                    else if (device->hasDeviceType(DEVICE_TYPE::SMART_LOCK))
                    {
                        RemoteSmartLockDevice *smartLock =
                            dynamic_cast<RemoteSmartLockDevice*>(device);

                        if (resource->hasResourceType(RESOURCE_TYPE::LOCK_STATUS))
                        {
                            smartLock->m_remoteLockStatus =
                                dynamic_cast<RemoteLockStatusResource*>(resource);
                        }
                    }
                    else if (device->hasDeviceType(DEVICE_TYPE::DRYER))
                    {
                        RemoteDryerDevice *dryer = dynamic_cast<RemoteDryerDevice*>(device);

                        if (resource->hasResourceType(RESOURCE_TYPE::BINARYSWITCH))
                        {
                            dryer->m_remoteBinarySwitch =
                                dynamic_cast<RemoteBinarySwitchResource*>(resource);
                        }
                    }
                    else if (device->hasDeviceType(DEVICE_TYPE::WATER_VALVE))
                    {
                        RemoteWaterValveDevice *waterValve =
                            dynamic_cast<RemoteWaterValveDevice*>(device);

                        if (resource->hasResourceType(RESOURCE_TYPE::OPENLEVEL))
                        {
                            waterValve->m_remoteOpenLevel =
                                dynamic_cast<RemoteOpenLevelResource*>(resource);
                        }
                    }
                    else if (device->hasDeviceType(DEVICE_TYPE::BLIND))
                    {
                        RemoteBlindDevice *blind = dynamic_cast<RemoteBlindDevice*>(device);

                        if (resource->hasResourceType(RESOURCE_TYPE::OPENLEVEL))
                        {
                            blind->m_remoteOpenLevel =
                                dynamic_cast<RemoteOpenLevelResource*>(resource);
                        }
                    }
                    else if (device->hasDeviceType(DEVICE_TYPE::CAMERA))
                    {
                        RemoteCameraDevice *camera = dynamic_cast<RemoteCameraDevice*>(device);

                        if (resource->hasResourceType(RESOURCE_TYPE::MEDIA))
                        {
                            camera->m_remoteMedia =
                                dynamic_cast<RemoteMediaResource*>(resource);
                        }
                    }
                    else if (device->hasDeviceType(DEVICE_TYPE::PRINTER))
                    {
                        RemotePrinterDevice *printer = dynamic_cast<RemotePrinterDevice*>(device);

                        if (resource->hasResourceType(RESOURCE_TYPE::BINARYSWITCH))
                        {
                            printer->m_remoteBinarySwitch =
                                dynamic_cast<RemoteBinarySwitchResource*>(resource);
                        }
                        else if (resource->hasResourceType(RESOURCE_TYPE::OPERATIONAL_STATE))
                        {
                            printer->m_remoteOperationalState =
                                dynamic_cast<RemoteOperationalStateResource*>(resource);
                        }
                    }
                    //TODO any other pre-defined device will be added here.
                }
                catch (const std::bad_cast& e)
                {
                    OIC_LOG_V(ERROR, TAG, "bad cast : %s", e.what());
                    return false;
                }

                device->m_remoteDeviceImpl->addResource(resource);
                return true;
            }

            std::list<std::string> SHBaseRemoteDeviceBuilder::convertStringLLToList(
                    OCStringLL *ll)
            {
                std::list<std::string> stringList;
                while (ll)
                {
                    stringList.push_back(ll->value);
                    ll = ll->next;
                }
                return stringList;
            }

            SHBaseRemoteDevice* SHBaseRemoteDeviceBuilder::makeSHBaseRemoteDevice(
                    OCResourcePayload *deviceResource, char *deviceId)
            {
                SHBaseRemoteDevice* device = NULL;
                bool isVerified = false;
                OCStringLL* deviceTypes = deviceResource->types;
                while (deviceTypes)
                {
                    char *deviceType = deviceTypes->value;

                    if (0 == DEVICE_TYPE::LIGHT.compare(deviceType))
                    {
                        if (!isVerified)
                        {
                            device = new RemoteLightDevice;
                            isVerified = true;
                        }
                        else
                        {
                            if (device)
                            {
                                delete device;
                                device = new SHBaseRemoteDevice;
                                break;
                            }
                        }
                    }
                    else if (0 == DEVICE_TYPE::ROBOTCLEANER.compare(deviceType))
                    {
                        if (!isVerified)
                        {
                            device = new RemoteRobotCleanerDevice;
                            isVerified = true;
                        }
                        else
                        {
                            if (device)
                            {
                                delete device;
                                device = new SHBaseRemoteDevice;
                                break;
                            }
                        }
                    }
                    else if (0 == DEVICE_TYPE::SMART_LOCK.compare(deviceType))
                    {
                        if (!isVerified)
                        {
                            device = new RemoteSmartLockDevice;
                            isVerified = true;
                        }
                        else
                        {
                            if (device)
                            {
                                delete device;
                                device = new SHBaseRemoteDevice;
                                break;
                            }
                        }
                    }
                    else if (0 == DEVICE_TYPE::DRYER.compare(deviceType))
                    {
                        if (!isVerified)
                        {
                            device = new RemoteDryerDevice;
                            isVerified = true;
                        }
                        else
                        {
                            if (device)
                            {
                                delete device;
                                device = new SHBaseRemoteDevice;
                                break;
                            }
                        }
                    }
                    else if (0 == DEVICE_TYPE::WATER_VALVE.compare(deviceType))
                    {
                        if (!isVerified)
                        {
                            device = new RemoteWaterValveDevice;
                            isVerified = true;
                        }
                        else
                        {
                            if (device)
                            {
                                delete device;
                                device = new SHBaseRemoteDevice;
                                break;
                            }
                        }
                    }
                    else if (0 == DEVICE_TYPE::BLIND.compare(deviceType))
                    {
                        if (!isVerified)
                        {
                            device = new RemoteBlindDevice;
                            isVerified = true;
                        }
                        else
                        {
                            if (device)
                            {
                                delete device;
                                device = new SHBaseRemoteDevice;
                                break;
                            }
                        }
                    }
                    else if (0 == DEVICE_TYPE::CAMERA.compare(deviceType))
                    {
                        if (!isVerified)
                        {
                            device = new RemoteCameraDevice;
                            isVerified = true;
                        }
                        else
                        {
                            if (device)
                            {
                                delete device;
                                device = new SHBaseRemoteDevice;
                                break;
                            }
                        }
                    }
                    else if (0 == DEVICE_TYPE::PRINTER.compare(deviceType))
                    {
                        if (!isVerified)
                        {
                            device = new RemotePrinterDevice;
                            isVerified = true;
                        }
                        else
                        {
                            if (device)
                            {
                                delete device;
                                device = new SHBaseRemoteDevice;
                                break;
                            }
                        }
                    }
                    //TODO check pre-defined class

                    deviceTypes = deviceTypes->next;
                }

                if (!isVerified)
                {
                    device = new SHBaseRemoteDevice;
                }

                device->m_remoteDeviceImpl->setTypes(convertStringLLToList(deviceResource->types));
                device->m_remoteDeviceImpl->setDeviceId(std::string(deviceId));

                return device;
            }
        }
    }
}
