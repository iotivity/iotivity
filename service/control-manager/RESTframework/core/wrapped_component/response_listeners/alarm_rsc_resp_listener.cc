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

#include "wrapped_component/response_listeners/alarm_rsc_resp_listener.h"
#include "wrapped_component/control_manager_wrapper.h"

namespace webservice
{

    typedef Singleton<AlarmResourceResponseListener, LeakySingletonTraits<AlarmResourceResponseListener> >
    AlarmResourceResponseListenerSingleton;
    AlarmResourceResponseListener *AlarmResourceResponseListener::alarm_rsc_resp_listener_instance_ =
        NULL;

    AlarmResourceResponseListener::AlarmResourceResponseListener()
    {
    }

    AlarmResourceResponseListener::~AlarmResourceResponseListener()
    {
    }

    AlarmResourceResponseListener *AlarmResourceResponseListener::GetInstance()
    {
        DLOG(INFO) << " : Enter";
        if (!alarm_rsc_resp_listener_instance_)
        {
            alarm_rsc_resp_listener_instance_ = AlarmResourceResponseListenerSingleton::get();
        }
        DLOG(INFO) << " : Exit";
        return alarm_rsc_resp_listener_instance_;
    }

    void AlarmResourceResponseListener::DestroyInstance()
    {
        DLOG(INFO) << " : Enter";
        static base::Lock destroy_lock;
        base::AutoLock auto_lock(destroy_lock);
        if (alarm_rsc_resp_listener_instance_)
        {
            delete alarm_rsc_resp_listener_instance_;
            alarm_rsc_resp_listener_instance_ = NULL;
        }
        DLOG(INFO) << " : Exit";
    }

    bool AlarmResourceResponseListener::ongetAlarm(int &requestId, int status, ::Alarm *respData)
    {
        LOG(ERROR) << "ongetAlarm:: " << requestId;
        // create a new instance of 'Alarm' and copy all the details from 'respData' and store it for in the map for future use.
        // Otherwise the CM will delete the respData as soon as the CB returns.
        Alarm *alarm = NULL;
        if (respData)
        {
            alarm = new Alarm();
            if (!alarm)
            {
                return false;
            }
            alarm->clone(respData);
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, alarm, status);
        }
        return true;
    }

    void AlarmResourceResponseListener::onSubscribeResponseReceived( int requestId, int status,
            std::string &subscriptionUri)
    {
    }

    void AlarmResourceResponseListener::onUnSubscribeResponseReceived(int requestId, int status)
    {
    }

    void AlarmResourceResponseListener::onError(int requestId, int statusCode, OC::Cm::CMError error,
            OC::Cm::Serialization::Xsd::CMErrorMessage *errorMessage)
    {
    }

}
