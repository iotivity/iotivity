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

#include "wrapped_component/response_listeners/alarms_rsc_resp_listener.h"
#include "wrapped_component/control_manager_wrapper.h"

namespace webservice
{

    typedef Singleton<AlarmsResourceResponseListener, LeakySingletonTraits<AlarmsResourceResponseListener> >
    AlarmsResourceResponseListenerSingleton;
    AlarmsResourceResponseListener *AlarmsResourceResponseListener::alarms_rsc_resp_listener_instance_ =
        NULL;

    AlarmsResourceResponseListener::AlarmsResourceResponseListener()
    {
    }

    AlarmsResourceResponseListener::~AlarmsResourceResponseListener()
    {
    }

    AlarmsResourceResponseListener *AlarmsResourceResponseListener::GetInstance()
    {
        DLOG(INFO) << " : Enter";
        if (!alarms_rsc_resp_listener_instance_)
        {
            alarms_rsc_resp_listener_instance_ = AlarmsResourceResponseListenerSingleton::get();
        }
        DLOG(INFO) << " : Exit";
        return alarms_rsc_resp_listener_instance_;
    }

    void AlarmsResourceResponseListener::DestroyInstance()
    {
        DLOG(INFO) << " : Enter";
        static base::Lock destroy_lock;
        base::AutoLock auto_lock(destroy_lock);
        if (alarms_rsc_resp_listener_instance_)
        {
            delete alarms_rsc_resp_listener_instance_;
            alarms_rsc_resp_listener_instance_ = NULL;
        }
        DLOG(INFO) << " : Exit";
    }

    bool AlarmsResourceResponseListener::ongetAlarms(int &requestId, int status, ::Alarms *respData)
    {
        LOG(ERROR) << "ongetAlarms:: " << requestId;
        // create a new instance of 'Alarms' and copy all the details from 'respData' and store it for in the map for future use.
        // Otherwise the CM will delete the respData as soon as the CB returns.
        Alarms *alarms = NULL;
        if (respData)
        {
            Alarms *alarms = new Alarms();
            if (!alarms)
                return false;
            alarms->clone(respData);
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, alarms, status);
        }
        return true;
    }

    void AlarmsResourceResponseListener::onSubscribeResponseReceived( int requestId, int status,
            std::string &subscriptionUri)
    {
    }

    void AlarmsResourceResponseListener::onUnSubscribeResponseReceived(int requestId, int status)
    {
    }

    void AlarmsResourceResponseListener::onError(int requestId, int statusCode, OC::Cm::CMError error,
            OC::Cm::Serialization::Xsd::CMErrorMessage *errorMessage)
    {
    }

}
