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

#ifndef WEBSERVICE_NEW_RESOURCE_REQUEST_HANDLER_CONTROL_MANAGER_WRAPPER_H_
#define WEBSERVICE_NEW_RESOURCE_REQUEST_HANDLER_CONTROL_MANAGER_WRAPPER_H_

#include "base/memory/singleton.h"
#include "ControlManager.h"
#include "Device.h"
#include "MyDevice.h"
#include "thread/mutex.hpp"
#include "thread/condition_variable.hpp"
#include "date_time/posix_time/posix_time.hpp"
#include "rest_engine_export.h"
#include "http_start_line/http_status_code_defined.h"
#include "wrapped_component/resource_types.h"

namespace webservice
{

    typedef struct CmCbData
    {
        void *data;
        // this is used to typecast the 'data' object during object deletion
        ResourceType rsc_type;
        HTTPResponseType resp_status;
        boost::mutex lock;
        boost::condition_variable condition;
        CmCbData();
        ~CmCbData();
    } CmCbData;

// request_id is unique for each request, so using it as key for the below map.
    typedef std::map<int, CmCbData *> CmCBDataMap;
    typedef std::map<int, CmCbData *>::iterator CmCBDataMapIter;
// (TODO) add code comments

// This class is just a wrapper over Control Manager.. This creates a control manager instance and stores it for future use. This maintains a 'CmCBDataMap' to store request and response data received from control manager.
    class REST_ENGINE_EXPORT ControlManagerWrapper : public OC::Cm::IControlManagerListener
    {
        public:
            /**
               * Returns pointer to ControlManagerWrapper singleton object
               * @param void
               * @return ControlManagerWrapper*
               */
            static webservice::ControlManagerWrapper *GetInstance(void);

            /**
               * Deletes ControlManagerWrapper singleton object
               * @param void
               * @return void
               */
            static void DestroyInstance(void);

            /**
               * Initializes the control manager module
               * @param void
               * @return bool
               */
            bool Init(void);

            /**
               * Un-initializes the control manager module
               * @param void
               * @return bool
               */
            bool DeInit(void);
            OC::Cm::ControlManager *GetControlManagerInstance();
            void AddRequestToCBMap(int request_id, CmCbData *cb_data);
            void GetdataFromCBMap(int request_id, void **ptr, HTTPResponseType *resp_status);
            void RemoveRequestFromCBMap(int request_id);
            void DataReceivedForRequest(int request_id, void *ptr, int resp_status,
                                        const std::string &object_type = "");
            // IControlManagerListener api's
            void tokenRequestNotification(std::string uuid);
            std::string getMyDeviceToken(void);
            void onStarted(std::string address);
            void onStopped();
            void onError(OC::Cm::CMError error);
            void updateUUIDAndTokenMap(std::string uuid, std::string deviceToken);
        private:
            ControlManagerWrapper();
            ~ControlManagerWrapper();
            bool InitializeControlManager();
            bool GetDeviceIP(std::string *ip_str);
            void DestroyRespListeners();
            // singleton instance of ControlManagerWrapper
            static ControlManagerWrapper *control_manager_wrapper_instance_;
            // instance of ControlManager
            OC::Cm::ControlManager *control_manager_instance_;
            MyDevice *my_device_;
            // to synchronize access to 'cm_cb_data_map_'
            base::Lock lock_;
            boost::mutex cm_start_lock;
            boost::condition_variable cm_start_condition;
            CmCBDataMap cm_cb_data_map_;
            // friend member, it creates LogManagerImpl object
            friend struct DefaultSingletonTraits<webservice::ControlManagerWrapper>;
    };
}

#endif /* WEBSERVICE_NEW_RESOURCE_REQUEST_HANDLER_CONTROL_MANAGER_WRAPPER_H_ */
