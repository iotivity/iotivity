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

#include "wrapped_component/response_listeners/versions_rsc_resp_listener.h"
#include "wrapped_component/control_manager_wrapper.h"

namespace webservice
{

    typedef Singleton<VersionsResourceResponseListener, LeakySingletonTraits<VersionsResourceResponseListener> >
    VersionsResourceResponseListenerSingleton;
    VersionsResourceResponseListener
    *VersionsResourceResponseListener::versions_rsc_resp_listener_instance_ = NULL;

    VersionsResourceResponseListener::VersionsResourceResponseListener()
    {
    }

    VersionsResourceResponseListener::~VersionsResourceResponseListener()
    {
    }

    VersionsResourceResponseListener *VersionsResourceResponseListener::GetInstance()
    {
        DLOG(INFO) << " : Enter";
        if (!versions_rsc_resp_listener_instance_)
        {
            versions_rsc_resp_listener_instance_ = VersionsResourceResponseListenerSingleton::get();
        }
        DLOG(INFO) << " : Exit";
        return versions_rsc_resp_listener_instance_;
    }

    void VersionsResourceResponseListener::DestroyInstance()
    {
        DLOG(INFO) << " : Enter";
        static base::Lock destroy_lock;
        base::AutoLock auto_lock(destroy_lock);
        if (versions_rsc_resp_listener_instance_)
        {
            delete versions_rsc_resp_listener_instance_;
            versions_rsc_resp_listener_instance_ = NULL;
        }
        DLOG(INFO) << " : Exit";
    }

    bool VersionsResourceResponseListener::ongetVersions(int &requestId, int status,
            ::Versions *respData)
    {
        LOG(ERROR) << "ongetVersions:: " << requestId;
        // create a new instance of 'Versions' and copy all the details from 'respData' and store it for in the map for future use.
        // Otherwise the CM will delete the respData as soon as the CB returns.
        Versions *versions = NULL;
        if (respData)
        {
            versions = new Versions();
            if (!versions)
                return false;
            versions->clone(respData);
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, versions, status);
        }
        return true;
    }

    bool VersionsResourceResponseListener::onputVersions(int &requestId, int status)
    {
        LOG(ERROR) << "onputVersions:: " << requestId << "  status:  " << status;
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, 0, status);
        }
        return true;
    }

    void VersionsResourceResponseListener::onSubscribeResponseReceived( int requestId, int status,
            std::string &subscriptionUri)
    {
    }

    void VersionsResourceResponseListener::onUnSubscribeResponseReceived(int requestId, int status)
    {
    }

    void VersionsResourceResponseListener::onError(int requestId, int statusCode, OC::Cm::CMError error,
            OC::Cm::Serialization::Xsd::CMErrorMessage *errorMessage)
    {
    }

}
