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

#include "wrapped_component/response_listeners/thermostat_rsc_resp_listener.h"
#include "wrapped_component/control_manager_wrapper.h"

namespace webservice
{

    typedef Singleton<ThermostatResourceResponseListener, LeakySingletonTraits<ThermostatResourceResponseListener> >
    ThermostatResourceResponseListenerSingleton;
    ThermostatResourceResponseListener
    *ThermostatResourceResponseListener::thermostat_rsc_resp_listener_instance_ = NULL;

    ThermostatResourceResponseListener::ThermostatResourceResponseListener()
    {
    }

    ThermostatResourceResponseListener::~ThermostatResourceResponseListener()
    {
    }

    ThermostatResourceResponseListener *ThermostatResourceResponseListener::GetInstance()
    {
        DLOG(INFO) << " : Enter";
        if (!thermostat_rsc_resp_listener_instance_)
        {
            thermostat_rsc_resp_listener_instance_ = ThermostatResourceResponseListenerSingleton::get();
        }
        DLOG(INFO) << " : Exit";
        return thermostat_rsc_resp_listener_instance_;
    }

    void ThermostatResourceResponseListener::DestroyInstance()
    {
        DLOG(INFO) << " : Enter";
        static base::Lock destroy_lock;
        base::AutoLock auto_lock(destroy_lock);
        if (thermostat_rsc_resp_listener_instance_)
        {
            delete thermostat_rsc_resp_listener_instance_;
            thermostat_rsc_resp_listener_instance_ = NULL;
        }
        DLOG(INFO) << " : Exit";
    }

    bool ThermostatResourceResponseListener::ongetThermostat(int &requestId, int status,
            ::Thermostat *respData)
    {
        LOG(ERROR) << "ongetThermostat:: " << requestId;
        // create a new instance of 'Thermostat' and copy all the details from 'respData' and store it for in the map for future use.
        // Otherwise the CM will delete the respData as soon as the CB returns.
        Thermostat *thermostat = NULL;
        if (respData)
        {
            thermostat = new Thermostat();
            if (!thermostat)
                return false;
            thermostat->clone(respData);
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, thermostat, status);
        }
        return true;
    }

    bool ThermostatResourceResponseListener::onputThermostat(int &requestId, int status)
    {
        LOG(ERROR) << "onputThermostat:: " << requestId << "  status:  " << status;
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, 0, status);
        }
        return true;
    }

    void ThermostatResourceResponseListener::onSubscribeResponseReceived( int requestId, int status,
            std::string &subscriptionUri)
    {
    }

    void ThermostatResourceResponseListener::onUnSubscribeResponseReceived(int requestId, int status)
    {
    }

    void ThermostatResourceResponseListener::onError(int requestId, int statusCode,
            OC::Cm::CMError error, OC::Cm::Serialization::Xsd::CMErrorMessage *errorMessage)
    {
    }

}
