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

#include "wrapped_component/response_listeners/doors_rsc_resp_listener.h"
#include "wrapped_component/control_manager_wrapper.h"

namespace webservice
{

    typedef Singleton<DoorsResourceResponseListener, LeakySingletonTraits<DoorsResourceResponseListener> >
    DoorsResourceResponseListenerSingleton;
    DoorsResourceResponseListener *DoorsResourceResponseListener::doors_rsc_resp_listener_instance_ =
        NULL;

    DoorsResourceResponseListener::DoorsResourceResponseListener()
    {
    }

    DoorsResourceResponseListener::~DoorsResourceResponseListener()
    {
    }

    DoorsResourceResponseListener *DoorsResourceResponseListener::GetInstance()
    {
        DLOG(INFO) << " : Enter";
        if (!doors_rsc_resp_listener_instance_)
        {
            doors_rsc_resp_listener_instance_ = DoorsResourceResponseListenerSingleton::get();
        }
        DLOG(INFO) << " : Exit";
        return doors_rsc_resp_listener_instance_;
    }

    void DoorsResourceResponseListener::DestroyInstance()
    {
        DLOG(INFO) << " : Enter";
        static base::Lock destroy_lock;
        base::AutoLock auto_lock(destroy_lock);
        if (doors_rsc_resp_listener_instance_)
        {
            delete doors_rsc_resp_listener_instance_;
            doors_rsc_resp_listener_instance_ = NULL;
        }
        DLOG(INFO) << " : Exit";
    }

    bool DoorsResourceResponseListener::ongetDoors(int &requestId, int status, ::Doors *respData)
    {
        LOG(ERROR) << "ongetDoor:: " << requestId;
        // create a new instance of 'Doors' and copy all the details from 'respData' and store it for in the map for future use.
        // Otherwise the CM will delete the respData as soon as the CB returns.
        Doors *doors = NULL;
        if (respData)
        {
            doors = new Doors();
            if (!doors)
                return false;
            doors->clone(respData);
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, doors, status);
        }
        return true;
    }

    bool DoorsResourceResponseListener::onputDoors(int &requestId, int status)
    {
        LOG(ERROR) << "onputDoors:: " << requestId << "  status:  " << status;
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, 0, status);
        }
        return true;
    }

    void DoorsResourceResponseListener::onSubscribeResponseReceived( int requestId, int status,
            std::string &subscriptionUri)
    {
    }

    void DoorsResourceResponseListener::onUnSubscribeResponseReceived(int requestId, int status)
    {
    }

    void DoorsResourceResponseListener::onError(int requestId, int statusCode, OC::Cm::CMError error,
            OC::Cm::Serialization::Xsd::CMErrorMessage *errorMessage)
    {
    }

}
