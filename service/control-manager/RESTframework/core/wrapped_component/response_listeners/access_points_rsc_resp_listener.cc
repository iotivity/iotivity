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

#include "wrapped_component/response_listeners/access_points_rsc_resp_listener.h"
#include "wrapped_component/control_manager_wrapper.h"

namespace webservice
{

    typedef Singleton<AccessPointsResourceResponseListener, LeakySingletonTraits<AccessPointsResourceResponseListener> >
    AccessPointsResourceResponseListenerSingleton;
    AccessPointsResourceResponseListener
    *AccessPointsResourceResponseListener::access_points_rsc_resp_listener_instance_ = NULL;

    AccessPointsResourceResponseListener::AccessPointsResourceResponseListener()
    {
    }

    AccessPointsResourceResponseListener::~AccessPointsResourceResponseListener()
    {
    }

    AccessPointsResourceResponseListener *AccessPointsResourceResponseListener::GetInstance()
    {
        DLOG(INFO) << " : Enter";
        if (!access_points_rsc_resp_listener_instance_)
        {
            access_points_rsc_resp_listener_instance_ = AccessPointsResourceResponseListenerSingleton::get();
        }
        DLOG(INFO) << " : Exit";
        return access_points_rsc_resp_listener_instance_;
    }

    void AccessPointsResourceResponseListener::DestroyInstance()
    {
        DLOG(INFO) << " : Enter";
        static base::Lock destroy_lock;
        base::AutoLock auto_lock(destroy_lock);
        if (access_points_rsc_resp_listener_instance_)
        {
            delete access_points_rsc_resp_listener_instance_;
            access_points_rsc_resp_listener_instance_ = NULL;
        }
        DLOG(INFO) << " : Exit";
    }

    bool AccessPointsResourceResponseListener::ongetAccessPoints(int &requestId, int status,
            ::AccessPoints *respData)
    {
        LOG(ERROR) << "ongetAccessPoints:: " << requestId;
        // create a new instance of 'AccessPoints' and copy all the details from 'respData' and store it for in the map for future use.
        // Otherwise the CM will delete the respData as soon as the CB returns.
        AccessPoints *ap = NULL;
        if (respData)
        {
            ap = new AccessPoints();
            if (!ap)
            {
                return false;
            }
            ap->clone(respData);
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, ap, status);
        }
        return true;
    }

    bool AccessPointsResourceResponseListener::onputAccessPoints(int &requestId, int status)
    {
        LOG(ERROR) << "onputAccessPoints:: " << requestId << "  status:  " << status;
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, 0, status);
        }
        return true;
    }

    void AccessPointsResourceResponseListener::onSubscribeResponseReceived( int requestId, int status,
            std::string &subscriptionUri)
    {
    }

    void AccessPointsResourceResponseListener::onUnSubscribeResponseReceived(int requestId, int status)
    {
    }

    void AccessPointsResourceResponseListener::onError(int requestId, int statusCode,
            OC::Cm::CMError error, OC::Cm::Serialization::Xsd::CMErrorMessage *errorMessage)
    {
    }

}
