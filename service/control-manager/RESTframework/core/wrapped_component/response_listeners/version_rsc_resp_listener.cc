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

#include "wrapped_component/response_listeners/version_rsc_resp_listener.h"
#include "wrapped_component/control_manager_wrapper.h"

namespace webservice
{

    typedef Singleton<VersionResourceResponseListener, LeakySingletonTraits<VersionResourceResponseListener> >
    VersionResourceResponseListenerSingleton;
    VersionResourceResponseListener
    *VersionResourceResponseListener::version_rsc_resp_listener_instance_ = NULL;

    VersionResourceResponseListener::VersionResourceResponseListener()
    {
    }

    VersionResourceResponseListener::~VersionResourceResponseListener()
    {
    }

    VersionResourceResponseListener *VersionResourceResponseListener::GetInstance()
    {
        DLOG(INFO) << " : Enter";
        if (!version_rsc_resp_listener_instance_)
        {
            version_rsc_resp_listener_instance_ = VersionResourceResponseListenerSingleton::get();
        }
        DLOG(INFO) << " : Exit";
        return version_rsc_resp_listener_instance_;
    }

    void VersionResourceResponseListener::DestroyInstance()
    {
        DLOG(INFO) << " : Enter";
        static base::Lock destroy_lock;
        base::AutoLock auto_lock(destroy_lock);
        if (version_rsc_resp_listener_instance_)
        {
            delete version_rsc_resp_listener_instance_;
            version_rsc_resp_listener_instance_ = NULL;
        }
        DLOG(INFO) << " : Exit";
    }

    bool VersionResourceResponseListener::ongetVersion(int &requestId, int status, ::Version *respData)
    {
        LOG(ERROR) << "ongetVersion:: " << requestId;
        // create a new instance of 'Version' and copy all the details from 'respData' and store it for in the map for future use.
        // Otherwise the CM will delete the respData as soon as the CB returns.
        Version *version = NULL;
        if (respData)
        {
            version = new Version();
            if (!version)
                return false;
            version->clone(respData);
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, version, status);
        }
        return true;
    }

    bool VersionResourceResponseListener::onputVersion(int &requestId, int status)
    {
        LOG(ERROR) << "onputVersion:: " << requestId << "  status:  " << status;
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, 0, status);
        }
        return true;
    }

    void VersionResourceResponseListener::onSubscribeResponseReceived( int requestId, int status,
            std::string &subscriptionUri)
    {
    }

    void VersionResourceResponseListener::onUnSubscribeResponseReceived(int requestId, int status)
    {
    }

    void VersionResourceResponseListener::onError(int requestId, int statusCode, OC::Cm::CMError error,
            OC::Cm::Serialization::Xsd::CMErrorMessage *errorMessage)
    {
    }

}
