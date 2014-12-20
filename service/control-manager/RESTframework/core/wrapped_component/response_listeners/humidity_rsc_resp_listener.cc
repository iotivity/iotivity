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

#include "wrapped_component/response_listeners/humidity_rsc_resp_listener.h"
#include "wrapped_component/control_manager_wrapper.h"

namespace webservice
{

    typedef Singleton<HumidityResourceResponseListener, LeakySingletonTraits<HumidityResourceResponseListener> >
    HumidityResourceResponseListenerSingleton;
    HumidityResourceResponseListener
    *HumidityResourceResponseListener::humidity_rsc_resp_listener_instance_ = NULL;

    HumidityResourceResponseListener::HumidityResourceResponseListener()
    {
    }

    HumidityResourceResponseListener::~HumidityResourceResponseListener()
    {
    }

    HumidityResourceResponseListener *HumidityResourceResponseListener::GetInstance()
    {
        DLOG(INFO) << " : Enter";
        if (!humidity_rsc_resp_listener_instance_)
        {
            humidity_rsc_resp_listener_instance_ = HumidityResourceResponseListenerSingleton::get();
        }
        DLOG(INFO) << " : Exit";
        return humidity_rsc_resp_listener_instance_;
    }

    void HumidityResourceResponseListener::DestroyInstance()
    {
        DLOG(INFO) << " : Enter";
        static base::Lock destroy_lock;
        base::AutoLock auto_lock(destroy_lock);
        if (humidity_rsc_resp_listener_instance_)
        {
            delete humidity_rsc_resp_listener_instance_;
            humidity_rsc_resp_listener_instance_ = NULL;
        }
        DLOG(INFO) << " : Exit";
    }

    bool HumidityResourceResponseListener::ongetHumidity(int &requestId, int status,
            ::Humidity *respData)
    {
        LOG(ERROR) << "ongetHumidity:: " << requestId;
        // create a new instance of 'Humidity' and copy all the details from 'respData' and store it for in the map for future use.
        // Otherwise the CM will delete the respData as soon as the CB returns.
        Humidity *humidity = NULL;
        if (respData)
        {
            humidity = new Humidity();
            if (!humidity)
                return false;
            humidity->clone(respData);
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, humidity, status);
        }
        return true;
    }

    bool HumidityResourceResponseListener::onputHumidity(int &requestId, int status)
    {
        LOG(ERROR) << "onputHumidity:: " << requestId << "  status:  " << status;
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, 0, status);
        }
        return true;
    }

    void HumidityResourceResponseListener::onSubscribeResponseReceived( int requestId, int status,
            std::string &subscriptionUri)
    {
    }

    void HumidityResourceResponseListener::onUnSubscribeResponseReceived(int requestId, int status)
    {
    }

    void HumidityResourceResponseListener::onError(int requestId, int statusCode, OC::Cm::CMError error,
            OC::Cm::Serialization::Xsd::CMErrorMessage *errorMessage)
    {
    }

}
