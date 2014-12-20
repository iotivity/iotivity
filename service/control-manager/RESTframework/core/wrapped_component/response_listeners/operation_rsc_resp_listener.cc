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

#include "wrapped_component/response_listeners/operation_rsc_resp_listener.h"
#include "wrapped_component/control_manager_wrapper.h"

namespace webservice
{

    typedef Singleton<OperationResourceResponseListener, LeakySingletonTraits<OperationResourceResponseListener> >
    OperationResourceResponseListenerSingleton;
    OperationResourceResponseListener
    *OperationResourceResponseListener::operation_rsc_resp_listener_instance_ = NULL;

    OperationResourceResponseListener::OperationResourceResponseListener()
    {
    }

    OperationResourceResponseListener::~OperationResourceResponseListener()
    {
    }

    OperationResourceResponseListener *OperationResourceResponseListener::GetInstance()
    {
        DLOG(INFO) << " : Enter";
        if (!operation_rsc_resp_listener_instance_)
        {
            operation_rsc_resp_listener_instance_ = OperationResourceResponseListenerSingleton::get();
        }
        DLOG(INFO) << " : Exit";
        return operation_rsc_resp_listener_instance_;
    }

    void OperationResourceResponseListener::DestroyInstance()
    {
        DLOG(INFO) << " : Enter";
        static base::Lock destroy_lock;
        base::AutoLock auto_lock(destroy_lock);
        if (operation_rsc_resp_listener_instance_)
        {
            delete operation_rsc_resp_listener_instance_;
            operation_rsc_resp_listener_instance_ = NULL;
        }
        DLOG(INFO) << " : Exit";
    }

    bool OperationResourceResponseListener::ongetOperation(int &requestId, int status,
            ::Operation *respData)
    {
        LOG(ERROR) << "ongetOperation:: " << requestId;
        // create a new instance of 'Operation' and copy all the details from 'respData' and store it for in the map for future use.
        // Otherwise the CM will delete the respData as soon as the CB returns.
        Operation *operation = NULL;
        if (respData)
        {
            operation = new Operation();
            if (!operation)
                return false;
            operation->clone(respData);
        }
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, operation, status);
        }
        return true;
    }

    bool OperationResourceResponseListener::onputOperation(int &requestId, int status)
    {
        LOG(ERROR) << "onputOperation:: " << requestId << "  status:  " << status;
        ControlManagerWrapper *cm_wrapper = ControlManagerWrapper::GetInstance();
        if (cm_wrapper)
        {
            cm_wrapper->DataReceivedForRequest(requestId, 0, status);
        }
        return true;
    }

    void OperationResourceResponseListener::onSubscribeResponseReceived( int requestId, int status,
            std::string &subscriptionUri)
    {
    }

    void OperationResourceResponseListener::onUnSubscribeResponseReceived(int requestId, int status)
    {
    }

    void OperationResourceResponseListener::onError(int requestId, int statusCode,
            OC::Cm::CMError error, OC::Cm::Serialization::Xsd::CMErrorMessage *errorMessage)
    {
    }

}
