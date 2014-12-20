/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
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

#include "wrapped_component/response_listeners/device_rsc_resp_listener.h"
#include "wrapped_component/control_manager_wrapper.h"

namespace webservice
{

    typedef Singleton<DeviceResourceResponseListener, LeakySingletonTraits<DeviceResourceResponseListener> >
    DeviceResourceResponseListenerSingleton;
    DeviceResourceResponseListener *DeviceResourceResponseListener::device_rsc_resp_listener_instance_ =
        NULL;

    DeviceResourceResponseListener::DeviceResourceResponseListener()
    {
    }

    DeviceResourceResponseListener::~DeviceResourceResponseListener()
    {
    }

    DeviceResourceResponseListener *DeviceResourceResponseListener::GetInstance()
    {
        DLOG(INFO) << " : Enter";
        if (!device_rsc_resp_listener_instance_)
        {
            device_rsc_resp_listener_instance_ = DeviceResourceResponseListenerSingleton::get();
        }
        DLOG(INFO) << " : Exit";
        return device_rsc_resp_listener_instance_;
    }

    void DeviceResourceResponseListener::DestroyInstance()
    {
        DLOG(INFO) << " : Enter";
        static base::Lock destroy_lock;
        base::AutoLock auto_lock(destroy_lock);
        if (device_rsc_resp_listener_instance_)
        {
            delete device_rsc_resp_listener_instance_;
            device_rsc_resp_listener_instance_ = NULL;
        }
        DLOG(INFO) << " : Exit";
    }

    bool DeviceResourceResponseListener::ongetDevice(int &requestId, int status, ::Device *respData)
    {
        LOG(ERROR) << "ongetDevice:: " << requestId;
        // create a new instance of 'Device' and copy all the details from 'respData' and store it for in the map for future use.
        // Otherwise the CM will delete the respData as soon as the CB returns.
        Device *device = NULL;
        if (respData)
        {
            device = new Device();
            if (!device)
                return false;
            device->clone(respData);
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, device, status);
        }
        return true;
    }

    bool DeviceResourceResponseListener::onputDevice(int &requestId, int status)
    {
        LOG(ERROR) << "onputDevice:: " << requestId << "  status:  " << status;
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, 0, status);
        }
        return true;
    }

    bool DeviceResourceResponseListener::ondeleteDevice(int &requestId, int status)
    {
        LOG(ERROR) << "ondeleteDevice:: " << requestId << "  status:  " << status;
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, 0, status);
        }
        return true;
    }

    void DeviceResourceResponseListener::onSubscribeResponseReceived( int requestId, int status,
            std::string &subscriptionUri)
    {
    }

    void DeviceResourceResponseListener::onUnSubscribeResponseReceived(int requestId, int status)
    {
    }

    void DeviceResourceResponseListener::onError(int requestId, int statusCode, OC::Cm::CMError error,
            OC::Cm::Serialization::Xsd::CMErrorMessage *errorMessage)
    {
    }

}
