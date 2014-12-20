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

#include <string>

#include "base/memory/scoped_ptr.h"
#include "base_resource/base_identified_object.h"
#include "generated_resource/identified_object_door.h"
#include "data_types/simple_array_data_type.h"
#include "data_types/simple_data_type.h"
#include "object_id_manager/creator_object_id_manager.h"
#include "object_id_manager/interface_object_id_manager.h"
#include "base/logging.h"
#include "wrapped_component/common_wrapper_functions.h"

namespace webservice
{

#undef SAFE_DESTROY
#define SAFE_DESTROY(x)  if (x) { delete(x); x = NULL; }

    const char *const kObjectName   = "door";
    const char *const kObjectNameId = "door_id";

    /**
      * Register function to create this class.
      * @param[in] string
      * @param[in] BaseIdentifiedObject*
      * @return static RegisterIdentifiedObject
      */
    RegisterIdentifiedObject IdentifiedObjectDoor::regIdentifiedObject(kObjectName,
            &IdentifiedObjectDoor::Create);

    IdentifiedObjectDoor::IdentifiedObjectDoor(std::string id,
            std::string object_name) : BaseIdentifiedObject(id, object_name)
    {
        pGetFunc_ = &CommonWrapperFunctions::GetIdentifiedObjectDoor;
        pPutFunc_ = &CommonWrapperFunctions::PutIdentifiedObjectDoor;
        pPostFunc_ = NULL;
        pDeleteFunc_ = NULL;

        name_ = new String10;
        name_->SetProperty(0, 1, false);
        name_->SetFunctionPtr(NULL, NULL);
        elements_["name"] = name_;

        openState_ = new OpenType;
        openState_->SetProperty(0, 1, false);
        openState_->SetFunctionPtr(NULL, NULL, "DeviceDoor_openState", "service_param");
        elements_["openState"] = openState_;

        openElapsedTime_ = new TimeType;
        openElapsedTime_->SetProperty(0, 1, false);
        openElapsedTime_->SetFunctionPtr(NULL, NULL);
        elements_["openElapsedTime"] = openElapsedTime_;

        openTime_ = new DateTimeType;
        openTime_->SetProperty(0, 1, false);
        openTime_->SetFunctionPtr(NULL, NULL);
        elements_["openTime"] = openTime_;

        lock_ = new LockType;
        lock_->SetProperty(0, 1, true);
        lock_->SetFunctionPtr(NULL, NULL, "DeviceDoor_lock", "service_param");
        elements_["lock"] = lock_;

        password_ = new String20;
        password_->SetProperty(0, 1, true);
        password_->SetFunctionPtr(NULL, NULL);
        elements_["password"] = password_;

        oneTimePassword_ = new String20;
        oneTimePassword_->SetProperty(0, 1, true);
        oneTimePassword_->SetFunctionPtr(NULL, NULL);
        elements_["oneTimePassword"] = oneTimePassword_;

        emergencyAlarm_ = new _boolean;
        emergencyAlarm_->SetProperty(0, 1, true);
        emergencyAlarm_->SetFunctionPtr(NULL, NULL);
        elements_["emergencyAlarm"] = emergencyAlarm_;
    }

    IdentifiedObjectDoor::~IdentifiedObjectDoor()
    {
        SAFE_DESTROY(this->name_);
        SAFE_DESTROY(this->openState_);
        SAFE_DESTROY(this->openElapsedTime_);
        SAFE_DESTROY(this->openTime_);
        SAFE_DESTROY(this->lock_);
        SAFE_DESTROY(this->password_);
        SAFE_DESTROY(this->oneTimePassword_);
        SAFE_DESTROY(this->emergencyAlarm_);
    }

    /**
      * Create this class.
      * @return BaseIdentifiedObject*
      */
    BaseIdentifiedObject *IdentifiedObjectDoor::Create(std::string id)
    {
        DLOG(ERROR) << "IdentifiedObjectDoor is created. (" << id << ")";
        return new IdentifiedObjectDoor(id, kObjectName);
    }

    /**
      * Validation Check for ID
      * @return static HTTPResponseType
      */
    HTTPResponseType IdentifiedObjectDoor::ValidateObjectID()
    {
        CreatorObjectIDManager &factory = CreatorObjectIDManager::GetInstance();
        scoped_ptr<IObjectIDManager> pObj(factory.CreateObject(kObjectName));
        if (pObj.get())
        {
            std::string *id = reinterpret_cast<std::string *>(id_->GetDefault());
            return pObj->ValidateObjectID(*id, http_);
        }
        else
        {
            return RT_404_NOT_FOUND_OBJECT_ID_MANAGER;
        }
    }

    /**
      * Create ID
      * @param[in] void*
      * @param[out] string*
      * @return static HTTPResponseType
      */
    HTTPResponseType IdentifiedObjectDoor::CreateObjectID(void *p_in, std::string *id)
    {
        CreatorObjectIDManager &factory = CreatorObjectIDManager::GetInstance();
        scoped_ptr<IObjectIDManager> pObj(factory.CreateObject(kObjectName));
        if (pObj.get())
        {
            return pObj->CreateObjectID(p_in, id, http_);
        }
        else
        {
            return RT_404_NOT_FOUND_OBJECT_ID_MANAGER;
        }
    }

    /**
      * Delete ID
      * @param[in] void*
      * @param[in] const string&
      * @return static HTTPResponseType
      */
    HTTPResponseType IdentifiedObjectDoor::DeleteObjectID(void *p_in, const std::string &id)
    {
        CreatorObjectIDManager &factory = CreatorObjectIDManager::GetInstance();
        scoped_ptr<IObjectIDManager> pObj(factory.CreateObject(kObjectName));
        if (pObj.get())
        {
            return pObj->DeleteObjectID(p_in, id, http_);
        }
        else
        {
            return RT_404_NOT_FOUND_OBJECT_ID_MANAGER;
        }
    }

    /**
      * Save current ID for child URI
      * @param[in] std::map&
      * @return void
      */
    void IdentifiedObjectDoor::SaveCurrentIDWithName(std::map<std::string, std::string> &id_deps)
    {
        std::string *id = reinterpret_cast<std::string *>(this->id_->GetDefault());
        if (NULL != id)
        {
            id_deps[kObjectNameId] = *id;
        }
        else
        {
            id_deps[kObjectNameId] = "";
        }
    }

}
