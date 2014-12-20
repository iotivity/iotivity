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

#include "wrapped_component/response_listeners/light_rsc_resp_listener.h"
#include "wrapped_component/control_manager_wrapper.h"

namespace webservice
{

    typedef Singleton<LightResourceResponseListener, LeakySingletonTraits<LightResourceResponseListener> >
    LightResourceResponseListenerSingleton;
    LightResourceResponseListener *LightResourceResponseListener::light_rsc_resp_listener_instance_ =
        NULL;

    LightResourceResponseListener::LightResourceResponseListener()
    {
    }

    LightResourceResponseListener::~LightResourceResponseListener()
    {
    }

    LightResourceResponseListener *LightResourceResponseListener::GetInstance()
    {
        DLOG(INFO) << " : Enter";
        if (!light_rsc_resp_listener_instance_)
        {
            light_rsc_resp_listener_instance_ = LightResourceResponseListenerSingleton::get();
        }
        DLOG(INFO) << " : Exit";
        return light_rsc_resp_listener_instance_;
    }

    void LightResourceResponseListener::DestroyInstance()
    {
        DLOG(INFO) << " : Enter";
        static base::Lock destroy_lock;
        base::AutoLock auto_lock(destroy_lock);
        if (light_rsc_resp_listener_instance_)
        {
            delete light_rsc_resp_listener_instance_;
            light_rsc_resp_listener_instance_ = NULL;
        }
        DLOG(INFO) << " : Exit";
    }

    bool LightResourceResponseListener::ongetLight(int &requestId, int status, ::Light *respData)
    {
        LOG(ERROR) << "ongetLight:: " << requestId;
        // create a new instance of 'Light' and copy all the details from 'respData' and store it for in the map for future use.
        // Otherwise the CM will delete the respData as soon as the CB returns.
        Light *light = NULL;
        if (respData)
        {
            light = new Light();
            if (!light)
                return false;
            light->clone(respData);
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, light, status);
        }
        return true;
    }

    bool LightResourceResponseListener::onputLight(int &requestId, int status)
    {
        LOG(ERROR) << "onputLight:: " << requestId << "  status:  " << status;
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, 0, status);
        }
        return true;
    }

    void LightResourceResponseListener::onSubscribeResponseReceived( int requestId, int status,
            std::string &subscriptionUri)
    {
    }

    void LightResourceResponseListener::onUnSubscribeResponseReceived(int requestId, int status)
    {
    }

    void LightResourceResponseListener::onError(int requestId, int statusCode, OC::Cm::CMError error,
            OC::Cm::Serialization::Xsd::CMErrorMessage *errorMessage)
    {
    }

}
