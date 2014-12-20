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

#include "wrapped_component/response_listeners/mode_rsc_resp_listener.h"
#include "wrapped_component/control_manager_wrapper.h"

namespace webservice
{

    typedef Singleton<ModeResourceResponseListener, LeakySingletonTraits<ModeResourceResponseListener> >
    ModeResourceResponseListenerSingleton;
    ModeResourceResponseListener *ModeResourceResponseListener::mode_rsc_resp_listener_instance_ = NULL;

    ModeResourceResponseListener::ModeResourceResponseListener()
    {
    }

    ModeResourceResponseListener::~ModeResourceResponseListener()
    {
    }

    ModeResourceResponseListener *ModeResourceResponseListener::GetInstance()
    {
        DLOG(INFO) << " : Enter";
        if (!mode_rsc_resp_listener_instance_)
        {
            mode_rsc_resp_listener_instance_ = ModeResourceResponseListenerSingleton::get();
        }
        DLOG(INFO) << " : Exit";
        return mode_rsc_resp_listener_instance_;
    }

    void ModeResourceResponseListener::DestroyInstance()
    {
        DLOG(INFO) << " : Enter";
        static base::Lock destroy_lock;
        base::AutoLock auto_lock(destroy_lock);
        if (mode_rsc_resp_listener_instance_)
        {
            delete mode_rsc_resp_listener_instance_;
            mode_rsc_resp_listener_instance_ = NULL;
        }
        DLOG(INFO) << " : Exit";
    }

    bool ModeResourceResponseListener::ongetMode(int &requestId, int status, ::Mode *respData)
    {
        LOG(ERROR) << "ongetMode:: " << requestId;
        // create a new instance of 'Mode' and copy all the details from 'respData' and store it for in the map for future use.
        // Otherwise the CM will delete the respData as soon as the CB returns.
        Mode *mode = NULL;
        if (respData)
        {
            mode = new Mode();
            if (!mode)
                return false;
            mode->clone(respData);
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, mode, status);
        }
        return true;
    }

    bool ModeResourceResponseListener::onputMode(int &requestId, int status)
    {
        LOG(ERROR) << "onputMode:: " << requestId << "  status:  " << status;
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, 0, status);
        }
        return true;
    }

    void ModeResourceResponseListener::onSubscribeResponseReceived( int requestId, int status,
            std::string &subscriptionUri)
    {
    }

    void ModeResourceResponseListener::onUnSubscribeResponseReceived(int requestId, int status)
    {
    }

    void ModeResourceResponseListener::onError(int requestId, int statusCode, OC::Cm::CMError error,
            OC::Cm::Serialization::Xsd::CMErrorMessage *errorMessage)
    {
    }

}
