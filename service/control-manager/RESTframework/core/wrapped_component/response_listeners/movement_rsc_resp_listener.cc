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

#include "wrapped_component/response_listeners/movement_rsc_resp_listener.h"
#include "wrapped_component/control_manager_wrapper.h"

namespace webservice
{

    typedef Singleton<MovementResourceResponseListener, LeakySingletonTraits<MovementResourceResponseListener> >
    MovementResourceResponseListenerSingleton;
    MovementResourceResponseListener
    *MovementResourceResponseListener::movement_rsc_resp_listener_instance_ = NULL;

    MovementResourceResponseListener::MovementResourceResponseListener()
    {
    }

    MovementResourceResponseListener::~MovementResourceResponseListener()
    {
    }

    MovementResourceResponseListener *MovementResourceResponseListener::GetInstance()
    {
        DLOG(INFO) << " : Enter";
        if (!movement_rsc_resp_listener_instance_)
        {
            movement_rsc_resp_listener_instance_ = MovementResourceResponseListenerSingleton::get();
        }
        DLOG(INFO) << " : Exit";
        return movement_rsc_resp_listener_instance_;
    }

    void MovementResourceResponseListener::DestroyInstance()
    {
        DLOG(INFO) << " : Enter";
        static base::Lock destroy_lock;
        base::AutoLock auto_lock(destroy_lock);
        if (movement_rsc_resp_listener_instance_)
        {
            delete movement_rsc_resp_listener_instance_;
            movement_rsc_resp_listener_instance_ = NULL;
        }
        DLOG(INFO) << " : Exit";
    }

    bool MovementResourceResponseListener::ongetMovement(int &requestId, int status,
            ::Movement *respData)
    {
        LOG(ERROR) << "ongetMovement:: " << requestId;
        // create a new instance of 'Movement' and copy all the details from 'respData' and store it for in the map for future use.
        // Otherwise the CM will delete the respData as soon as the CB returns.
        Movement *movement = NULL;
        if (respData)
        {
            movement = new Movement();
            if (!movement)
                return false;
            movement->clone(respData);
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, movement, status);
        }
        return true;
    }

    bool MovementResourceResponseListener::onputMovement(int &requestId, int status)
    {
        LOG(ERROR) << "onputMovement:: " << requestId << "  status:  " << status;
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, 0, status);
        }
        return true;
    }

    void MovementResourceResponseListener::onSubscribeResponseReceived( int requestId, int status,
            std::string &subscriptionUri)
    {
    }

    void MovementResourceResponseListener::onUnSubscribeResponseReceived(int requestId, int status)
    {
    }

    void MovementResourceResponseListener::onError(int requestId, int statusCode, OC::Cm::CMError error,
            OC::Cm::Serialization::Xsd::CMErrorMessage *errorMessage)
    {
    }

}
