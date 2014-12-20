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

#include "wrapped_component/response_listeners/time_rsc_resp_listener.h"
#include "wrapped_component/control_manager_wrapper.h"

namespace webservice
{

    typedef Singleton<TimeResourceResponseListener, LeakySingletonTraits<TimeResourceResponseListener> >
    TimeResourceResponseListenerSingleton;
    TimeResourceResponseListener *TimeResourceResponseListener::time_rsc_resp_listener_instance_ = NULL;

    TimeResourceResponseListener::TimeResourceResponseListener()
    {
    }

    TimeResourceResponseListener::~TimeResourceResponseListener()
    {
    }

    TimeResourceResponseListener *TimeResourceResponseListener::GetInstance()
    {
        DLOG(INFO) << " : Enter";
        if (!time_rsc_resp_listener_instance_)
        {
            time_rsc_resp_listener_instance_ = TimeResourceResponseListenerSingleton::get();
        }
        DLOG(INFO) << " : Exit";
        return time_rsc_resp_listener_instance_;
    }

    void TimeResourceResponseListener::DestroyInstance()
    {
        DLOG(INFO) << " : Enter";
        static base::Lock destroy_lock;
        base::AutoLock auto_lock(destroy_lock);
        if (time_rsc_resp_listener_instance_)
        {
            delete time_rsc_resp_listener_instance_;
            time_rsc_resp_listener_instance_ = NULL;
        }
        DLOG(INFO) << " : Exit";
    }

    bool TimeResourceResponseListener::ongetTime(int &requestId, int status, ::Time *respData)
    {
        LOG(ERROR) << "ongetTime:: " << requestId;
        // create a new instance of 'Time' and copy all the details from 'respData' and store it for in the map for future use.
        // Otherwise the CM will delete the respData as soon as the CB returns.
        Time *time = NULL;
        if (respData)
        {
            time = new Time();
            if (!time)
                return false;
            time->clone(respData);
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, time, status);
        }
        return true;
    }

    bool TimeResourceResponseListener::onputTime(int &requestId, int status)
    {
        LOG(ERROR) << "onputTime:: " << requestId << "  status:  " << status;
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, 0, status);
        }
        return true;
    }

    void TimeResourceResponseListener::onSubscribeResponseReceived( int requestId, int status,
            std::string &subscriptionUri)
    {
    }

    void TimeResourceResponseListener::onUnSubscribeResponseReceived(int requestId, int status)
    {
    }

    void TimeResourceResponseListener::onError(int requestId, int statusCode, OC::Cm::CMError error,
            OC::Cm::Serialization::Xsd::CMErrorMessage *errorMessage)
    {
    }

}
