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

#include "wrapped_component/response_listeners/configuration_rsc_resp_listener.h"
#include "wrapped_component/control_manager_wrapper.h"

namespace webservice
{

    typedef Singleton<ConfigurationResourceResponseListener, LeakySingletonTraits<ConfigurationResourceResponseListener> >
    ConfigurationResourceResponseListenerSingleton;
    ConfigurationResourceResponseListener
    *ConfigurationResourceResponseListener::configuration_rsc_resp_listener_instance_ = NULL;

    ConfigurationResourceResponseListener::ConfigurationResourceResponseListener()
    {
    }

    ConfigurationResourceResponseListener::~ConfigurationResourceResponseListener()
    {
    }

    ConfigurationResourceResponseListener *ConfigurationResourceResponseListener::GetInstance()
    {
        DLOG(INFO) << " : Enter";
        if (!configuration_rsc_resp_listener_instance_)
        {
            configuration_rsc_resp_listener_instance_ = ConfigurationResourceResponseListenerSingleton::get();
        }
        DLOG(INFO) << " : Exit";
        return configuration_rsc_resp_listener_instance_;
    }

    void ConfigurationResourceResponseListener::DestroyInstance()
    {
        DLOG(INFO) << " : Enter";
        static base::Lock destroy_lock;
        base::AutoLock auto_lock(destroy_lock);
        if (configuration_rsc_resp_listener_instance_)
        {
            delete configuration_rsc_resp_listener_instance_;
            configuration_rsc_resp_listener_instance_ = NULL;
        }
        DLOG(INFO) << " : Exit";
    }

    bool ConfigurationResourceResponseListener::ongetConfiguration(int &requestId, int status,
            ::Configuration *respData)
    {
        LOG(ERROR) << "ongetConfiguration:: " << requestId;
        // create a new instance of 'Configuration' and copy all the details from 'respData' and store it for in the map for future use.
        // Otherwise the CM will delete the respData as soon as the CB returns.
        Configuration *configuration = NULL;
        if (respData)
        {
            configuration = new Configuration();
            if (!configuration)
                return false;
            configuration->clone(respData);
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, configuration, status);
        }
        return true;
    }

    bool ConfigurationResourceResponseListener::onputConfiguration(int &requestId, int status)
    {
        LOG(ERROR) << "onputConfiguration:: " << requestId << "  status:  " << status;
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, 0, status);
        }
        return true;
    }

    void ConfigurationResourceResponseListener::onSubscribeResponseReceived( int requestId, int status,
            std::string &subscriptionUri)
    {
    }

    void ConfigurationResourceResponseListener::onUnSubscribeResponseReceived(int requestId,
            int status)
    {
    }

    void ConfigurationResourceResponseListener::onError(int requestId, int statusCode,
            OC::Cm::CMError error, OC::Cm::Serialization::Xsd::CMErrorMessage *errorMessage)
    {
    }

}
