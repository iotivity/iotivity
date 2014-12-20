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

#include "wrapped_component/response_listeners/level_rsc_resp_listener.h"
#include "wrapped_component/control_manager_wrapper.h"

namespace webservice
{

    typedef Singleton<LevelResourceResponseListener, LeakySingletonTraits<LevelResourceResponseListener> >
    LevelResourceResponseListenerSingleton;
    LevelResourceResponseListener *LevelResourceResponseListener::level_rsc_resp_listener_instance_ =
        NULL;

    LevelResourceResponseListener::LevelResourceResponseListener()
    {
    }

    LevelResourceResponseListener::~LevelResourceResponseListener()
    {
    }

    LevelResourceResponseListener *LevelResourceResponseListener::GetInstance()
    {
        DLOG(INFO) << " : Enter";
        if (!level_rsc_resp_listener_instance_)
        {
            level_rsc_resp_listener_instance_ = LevelResourceResponseListenerSingleton::get();
        }
        DLOG(INFO) << " : Exit";
        return level_rsc_resp_listener_instance_;
    }

    void LevelResourceResponseListener::DestroyInstance()
    {
        DLOG(INFO) << " : Enter";
        static base::Lock destroy_lock;
        base::AutoLock auto_lock(destroy_lock);
        if (level_rsc_resp_listener_instance_)
        {
            delete level_rsc_resp_listener_instance_;
            level_rsc_resp_listener_instance_ = NULL;
        }
        DLOG(INFO) << " : Exit";
    }

    bool LevelResourceResponseListener::ongetLevel(int &requestId, int status, ::Level *respData)
    {
        LOG(ERROR) << "ongetLevel:: " << requestId;
        // create a new instance of 'Level' and copy all the details from 'respData' and store it for in the map for future use.
        // Otherwise the CM will delete the respData as soon as the CB returns.
        Level *level = NULL;
        if (respData)
        {
            level = new Level();
            if (!level)
                return false;
            level->clone(respData);
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, level, status);
        }
        return true;
    }

    bool LevelResourceResponseListener::onputLevel(int &requestId, int status)
    {
        LOG(ERROR) << "onputLight:: " << requestId << "  status:  " << status;
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, 0, status);
        }
        return true;
    }

    void LevelResourceResponseListener::onSubscribeResponseReceived( int requestId, int status,
            std::string &subscriptionUri)
    {
    }

    void LevelResourceResponseListener::onUnSubscribeResponseReceived(int requestId, int status)
    {
    }

    void LevelResourceResponseListener::onError(int requestId, int statusCode, OC::Cm::CMError error,
            OC::Cm::Serialization::Xsd::CMErrorMessage *errorMessage)
    {
    }

}
