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

#include "wrapped_component/response_listeners/energy_consumption_rsc_resp_listener.h"
#include "wrapped_component/control_manager_wrapper.h"

namespace webservice
{

    typedef Singleton<EnergyConsumptionResourceResponseListener, LeakySingletonTraits<EnergyConsumptionResourceResponseListener> >
    EnergyConsumptionResourceResponseListenerSingleton;
    EnergyConsumptionResourceResponseListener
    *EnergyConsumptionResourceResponseListener::energy_consumption_rsc_resp_listener_instance_ = NULL;

    EnergyConsumptionResourceResponseListener::EnergyConsumptionResourceResponseListener()
    {
    }

    EnergyConsumptionResourceResponseListener::~EnergyConsumptionResourceResponseListener()
    {
    }

    EnergyConsumptionResourceResponseListener *EnergyConsumptionResourceResponseListener::GetInstance()
    {
        DLOG(INFO) << " : Enter";
        if (!energy_consumption_rsc_resp_listener_instance_)
        {
            energy_consumption_rsc_resp_listener_instance_ =
                EnergyConsumptionResourceResponseListenerSingleton::get();
        }
        DLOG(INFO) << " : Exit";
        return energy_consumption_rsc_resp_listener_instance_;
    }

    void EnergyConsumptionResourceResponseListener::DestroyInstance()
    {
        DLOG(INFO) << " : Enter";
        static base::Lock destroy_lock;
        base::AutoLock auto_lock(destroy_lock);
        if (energy_consumption_rsc_resp_listener_instance_)
        {
            delete energy_consumption_rsc_resp_listener_instance_;
            energy_consumption_rsc_resp_listener_instance_ = NULL;
        }
        DLOG(INFO) << " : Exit";
    }

    bool EnergyConsumptionResourceResponseListener::ongetEnergyConsumption(int &requestId, int status,
            ::EnergyConsumption *respData)
    {
        LOG(ERROR) << "ongetEnergyConsumption:: " << requestId;
        // create a new instance of 'EnergyConsumption' and copy all the details from 'respData' and store it for in the map for future use.
        // Otherwise the CM will delete the respData as soon as the CB returns.
        EnergyConsumption *ec = NULL;
        if (respData)
        {
            ec = new EnergyConsumption();
            if (!ec)
                return false;
            ec->clone(respData);
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, ec, status);
        }
        return true;
    }

    bool EnergyConsumptionResourceResponseListener::onputEnergyConsumption(int &requestId, int status)
    {
        LOG(ERROR) << "onputEnergyConsumption:: " << requestId << "  status:  " << status;
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, 0, status);
        }
        return true;
    }

    void EnergyConsumptionResourceResponseListener::onSubscribeResponseReceived( int requestId,
            int status, std::string &subscriptionUri)
    {
    }

    void EnergyConsumptionResourceResponseListener::onUnSubscribeResponseReceived(int requestId,
            int status)
    {
    }

    void EnergyConsumptionResourceResponseListener::onError(int requestId, int statusCode,
            OC::Cm::CMError error, OC::Cm::Serialization::Xsd::CMErrorMessage *errorMessage)
    {
    }

}
