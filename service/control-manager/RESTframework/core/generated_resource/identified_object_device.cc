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
#include "generated_resource/identified_object_device.h"
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

    const char *const kObjectName   = "device";
    const char *const kObjectNameId = "device_id";

    /**
      * Register function to create this class.
      * @param[in] string
      * @param[in] BaseIdentifiedObject*
      * @return static RegisterIdentifiedObject
      */
    RegisterIdentifiedObject IdentifiedObjectDevice::regIdentifiedObject(kObjectName,
            &IdentifiedObjectDevice::Create);

    IdentifiedObjectDevice::IdentifiedObjectDevice(std::string id,
            std::string object_name) : BaseIdentifiedObject(id, object_name)
    {
        pGetFunc_ = &CommonWrapperFunctions::GetIdentifiedObjectDevice;
        pPutFunc_ = &CommonWrapperFunctions::PutIdentifiedObjectDevice;
        pPostFunc_ = NULL;
        pDeleteFunc_ = NULL;

        uuid_ = new UuidType;
        uuid_->SetProperty(1, 1, false);
        uuid_->SetFunctionPtr(NULL, NULL);
        elements_["uuid"] = uuid_;

        name_ = new String30;
        name_->SetProperty(0, 1, true);
        name_->SetFunctionPtr(NULL, NULL);
        elements_["name"] = name_;

        type_ = new DeviceType;
        type_->SetProperty(1, 1, false);
        type_->SetFunctionPtr(NULL, NULL);
        elements_["type"] = type_;

        resources_ = new ArrayOfResourceType;
        resources_->SetProperty(1, 1, false);
        resources_->SetFunctionPtr(NULL, NULL);
        elements_["resources"] = resources_;

        connected_ = new _boolean;
        connected_->SetProperty(0, 1, false);
        connected_->SetFunctionPtr(NULL, NULL);
        elements_["connected"] = connected_;
    }

    IdentifiedObjectDevice::~IdentifiedObjectDevice()
    {
        SAFE_DESTROY(this->uuid_);
        SAFE_DESTROY(this->name_);
        SAFE_DESTROY(this->type_);
        SAFE_DESTROY(this->resources_);
        SAFE_DESTROY(this->connected_);
    }

    /**
      * Create this class.
      * @return BaseIdentifiedObject*
      */
    BaseIdentifiedObject *IdentifiedObjectDevice::Create(std::string id)
    {
        DLOG(ERROR) << "IdentifiedObjectDevice is created. (" << id << ")";
        return new IdentifiedObjectDevice(id, kObjectName);
    }

    /**
      * Validation Check for ID
      * @return static HTTPResponseType
      */
    HTTPResponseType IdentifiedObjectDevice::ValidateObjectID()
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
    HTTPResponseType IdentifiedObjectDevice::CreateObjectID(void *p_in, std::string *id)
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
    HTTPResponseType IdentifiedObjectDevice::DeleteObjectID(void *p_in, const std::string &id)
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
    void IdentifiedObjectDevice::SaveCurrentIDWithName(std::map<std::string, std::string> &id_deps)
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
