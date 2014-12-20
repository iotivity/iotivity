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

#include "wrapped_component/response_listeners/networks_rsc_resp_listener.h"
#include "wrapped_component/control_manager_wrapper.h"

namespace webservice
{

    typedef Singleton<NetworksResourceResponseListener, LeakySingletonTraits<NetworksResourceResponseListener> >
    NetworksResourceResponseListenerSingleton;
    NetworksResourceResponseListener
    *NetworksResourceResponseListener::networks_rsc_resp_listener_instance_ = NULL;

    NetworksResourceResponseListener::NetworksResourceResponseListener()
    {
    }

    NetworksResourceResponseListener::~NetworksResourceResponseListener()
    {
    }

    NetworksResourceResponseListener *NetworksResourceResponseListener::GetInstance()
    {
        DLOG(INFO) << " : Enter";
        if (!networks_rsc_resp_listener_instance_)
        {
            networks_rsc_resp_listener_instance_ = NetworksResourceResponseListenerSingleton::get();
        }
        DLOG(INFO) << " : Exit";
        return networks_rsc_resp_listener_instance_;
    }

    void NetworksResourceResponseListener::DestroyInstance()
    {
        DLOG(INFO) << " : Enter";
        static base::Lock destroy_lock;
        base::AutoLock auto_lock(destroy_lock);
        if (networks_rsc_resp_listener_instance_)
        {
            delete networks_rsc_resp_listener_instance_;
            networks_rsc_resp_listener_instance_ = NULL;
        }
        DLOG(INFO) << " : Exit";
    }

    bool NetworksResourceResponseListener::ongetNetworks(int &requestId, int status,
            ::Networks *respData)
    {
        LOG(ERROR) << "ongetNetworks:: " << requestId;
        // create a new instance of 'Networks' and copy all the details from 'respData' and store it for in the map for future use.
        // Otherwise the CM will delete the respData as soon as the CB returns.
        Networks *networks = NULL;
        if (respData)
        {
            networks = new Networks();
            if (!networks)
                return false;
            networks->clone(respData);
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, networks, status);
        }
        return true;
    }

    bool NetworksResourceResponseListener::onputNetworks(int &requestId, int status)
    {
        LOG(ERROR) << "onputNetworks:: " << requestId << "  status:  " << status;
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, 0, status);
        }
        return true;
    }

    void NetworksResourceResponseListener::onSubscribeResponseReceived( int requestId, int status,
            std::string &subscriptionUri)
    {
    }

    void NetworksResourceResponseListener::onUnSubscribeResponseReceived(int requestId, int status)
    {
    }

    void NetworksResourceResponseListener::onError(int requestId, int statusCode, OC::Cm::CMError error,
            OC::Cm::Serialization::Xsd::CMErrorMessage *errorMessage)
    {
    }

}
