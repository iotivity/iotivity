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

#include "wrapped_component/response_listeners/access_point_rsc_resp_listener.h"
#include "wrapped_component/control_manager_wrapper.h"

namespace webservice
{

    typedef Singleton<AccessPointResourceResponseListener, LeakySingletonTraits<AccessPointResourceResponseListener> >
    AccessPointResourceResponseListenerSingleton;
    AccessPointResourceResponseListener
    *AccessPointResourceResponseListener::device_rsc_resp_listener_instance_ = NULL;

    AccessPointResourceResponseListener::AccessPointResourceResponseListener()
    {
    }

    AccessPointResourceResponseListener::~AccessPointResourceResponseListener()
    {
    }

    AccessPointResourceResponseListener *AccessPointResourceResponseListener::GetInstance()
    {
        DLOG(INFO) << " : Enter";
        if (!device_rsc_resp_listener_instance_)
        {
            device_rsc_resp_listener_instance_ = AccessPointResourceResponseListenerSingleton::get();
        }
        DLOG(INFO) << " : Exit";
        return device_rsc_resp_listener_instance_;
    }

    void AccessPointResourceResponseListener::DestroyInstance()
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

    bool AccessPointResourceResponseListener::ongetAccessPoint(int &requestId, int status,
            ::AccessPoint *respData)
    {
        LOG(ERROR) << "ongetAccessPoint:: " << requestId << " Status:" << status;
        // create a new instance of 'AccessPoint' and copy all the details from 'respData' and store it for in the map for future use.
        // Otherwise the CM will delete the respData as soon as the CB returns.
        AccessPoint *ap = NULL;
        if (respData)
        {
            ap = new AccessPoint();
            if (!ap)
            {
                return false;
            }
            ap->clone(respData);
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, ap, status);
        }
        return true;
    }

    bool AccessPointResourceResponseListener::onputAccessPoint(int &requestId, int status)
    {
        LOG(ERROR) << "onputAccessPoint:: " << requestId << "  status:  " << status;
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, 0, status);
        }
        return true;
    }

    void AccessPointResourceResponseListener::onSubscribeResponseReceived( int requestId, int status,
            std::string &subscriptionUri)
    {
    }

    void AccessPointResourceResponseListener::onUnSubscribeResponseReceived(int requestId, int status)
    {
    }

    void AccessPointResourceResponseListener::onError(int requestId, int statusCode,
            OC::Cm::CMError error, OC::Cm::Serialization::Xsd::CMErrorMessage *errorMessage)
    {
    }

}
