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

#include "wrapped_component/response_listeners/temperatures_rsc_resp_listener.h"
#include "wrapped_component/control_manager_wrapper.h"

namespace webservice
{

    typedef Singleton<TemperaturesResourceResponseListener, LeakySingletonTraits<TemperaturesResourceResponseListener> >
    TemperaturesResourceResponseListenerSingleton;
    TemperaturesResourceResponseListener
    *TemperaturesResourceResponseListener::temperatures_rsc_resp_listener_instance_ = NULL;

    TemperaturesResourceResponseListener::TemperaturesResourceResponseListener()
    {
    }

    TemperaturesResourceResponseListener::~TemperaturesResourceResponseListener()
    {
    }

    TemperaturesResourceResponseListener *TemperaturesResourceResponseListener::GetInstance()
    {
        DLOG(INFO) << " : Enter";
        if (!temperatures_rsc_resp_listener_instance_)
        {
            temperatures_rsc_resp_listener_instance_ = TemperaturesResourceResponseListenerSingleton::get();
        }
        DLOG(INFO) << " : Exit";
        return temperatures_rsc_resp_listener_instance_;
    }

    void TemperaturesResourceResponseListener::DestroyInstance()
    {
        DLOG(INFO) << " : Enter";
        static base::Lock destroy_lock;
        base::AutoLock auto_lock(destroy_lock);
        if (temperatures_rsc_resp_listener_instance_)
        {
            delete temperatures_rsc_resp_listener_instance_;
            temperatures_rsc_resp_listener_instance_ = NULL;
        }
        DLOG(INFO) << " : Exit";
    }

    bool TemperaturesResourceResponseListener::ongetTemperatures(int &requestId, int status,
            ::Temperatures *respData)
    {
        LOG(ERROR) << "ongetTemperatures:: " << requestId;
        // create a new instance of 'Temperatures' and copy all the details from 'respData' and store it for in the map for future use.
        // Otherwise the CM will delete the respData as soon as the CB returns.
        Temperatures *temperatures = NULL;
        if (respData)
        {
            temperatures = new Temperatures();
            if (!temperatures)
                return false;
            temperatures->clone(respData);
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, temperatures, status);
        }
        return true;
    }

    bool TemperaturesResourceResponseListener::onputTemperatures(int &requestId, int status)
    {
        LOG(ERROR) << "onputTemperatures:: " << requestId << "  status:  " << status;
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, 0, status);
        }
        return true;
    }

    void TemperaturesResourceResponseListener::onSubscribeResponseReceived( int requestId, int status,
            std::string &subscriptionUri)
    {
    }

    void TemperaturesResourceResponseListener::onUnSubscribeResponseReceived(int requestId, int status)
    {
    }

    void TemperaturesResourceResponseListener::onError(int requestId, int statusCode,
            OC::Cm::CMError error, OC::Cm::Serialization::Xsd::CMErrorMessage *errorMessage)
    {
    }

}
