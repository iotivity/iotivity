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

#include "wrapped_component/response_listeners/temperature_rsc_resp_listener.h"
#include "wrapped_component/control_manager_wrapper.h"

namespace webservice
{

    typedef Singleton<TemperatureResourceResponseListener, LeakySingletonTraits<TemperatureResourceResponseListener> >
    TemperatureResourceResponseListenerSingleton;
    TemperatureResourceResponseListener
    *TemperatureResourceResponseListener::temperature_rsc_resp_listener_instance_ = NULL;

    TemperatureResourceResponseListener::TemperatureResourceResponseListener()
    {
    }

    TemperatureResourceResponseListener::~TemperatureResourceResponseListener()
    {
    }

    TemperatureResourceResponseListener *TemperatureResourceResponseListener::GetInstance()
    {
        DLOG(INFO) << " : Enter";
        if (!temperature_rsc_resp_listener_instance_)
        {
            temperature_rsc_resp_listener_instance_ = TemperatureResourceResponseListenerSingleton::get();
        }
        DLOG(INFO) << " : Exit";
        return temperature_rsc_resp_listener_instance_;
    }

    void TemperatureResourceResponseListener::DestroyInstance()
    {
        DLOG(INFO) << " : Enter";
        static base::Lock destroy_lock;
        base::AutoLock auto_lock(destroy_lock);
        if (temperature_rsc_resp_listener_instance_)
        {
            delete temperature_rsc_resp_listener_instance_;
            temperature_rsc_resp_listener_instance_ = NULL;
        }
        DLOG(INFO) << " : Exit";
    }

    bool TemperatureResourceResponseListener::ongetTemperature(int &requestId, int status,
            ::Temperature *respData)
    {
        LOG(ERROR) << "ongetTemperature:: " << requestId;
        // create a new instance of 'Temperature' and copy all the details from 'respData' and store it for in the map for future use.
        // Otherwise the CM will delete the respData as soon as the CB returns.
        Temperature *temperature = NULL;
        if (respData)
        {
            temperature = new Temperature();
            if (!temperature)
                return false;
            temperature->clone(respData);
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, temperature, status);
        }
        return true;
    }

    bool TemperatureResourceResponseListener::onputTemperature(int &requestId, int status)
    {
        LOG(ERROR) << "onputTemperature:: " << requestId << "  status:  " << status;
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, 0, status);
        }
        return true;
    }

    void TemperatureResourceResponseListener::onSubscribeResponseReceived( int requestId, int status,
            std::string &subscriptionUri)
    {
    }

    void TemperatureResourceResponseListener::onUnSubscribeResponseReceived(int requestId, int status)
    {
    }

    void TemperatureResourceResponseListener::onError(int requestId, int statusCode,
            OC::Cm::CMError error, OC::Cm::Serialization::Xsd::CMErrorMessage *errorMessage)
    {
    }

}
