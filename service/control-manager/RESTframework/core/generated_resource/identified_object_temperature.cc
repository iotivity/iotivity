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
#include "generated_resource/identified_object_temperature.h"
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

    const char *const kObjectName   = "temperature";
    const char *const kObjectNameId = "temperature_id";

    /**
      * Register function to create this class.
      * @param[in] string
      * @param[in] BaseIdentifiedObject*
      * @return static RegisterIdentifiedObject
      */
    RegisterIdentifiedObject IdentifiedObjectTemperature::regIdentifiedObject(kObjectName,
            &IdentifiedObjectTemperature::Create);

    IdentifiedObjectTemperature::IdentifiedObjectTemperature(std::string id,
            std::string object_name) : BaseIdentifiedObject(id, object_name)
    {
        pGetFunc_ = &CommonWrapperFunctions::GetIdentifiedObjectTemperature;
        pPutFunc_ = &CommonWrapperFunctions::PutIdentifiedObjectTemperature;
        pPostFunc_ = NULL;
        pDeleteFunc_ = NULL;

        name_ = new String10;
        name_->SetProperty(0, 1, false);
        name_->SetFunctionPtr(NULL, NULL);
        elements_["name"] = name_;

        current_ = new Float32;
        current_->SetProperty(0, 1, false);
        current_->SetFunctionPtr(NULL, NULL, "DeviceTemperature_current", "service_param");
        elements_["current"] = current_;

        desired_ = new Float32;
        desired_->SetProperty(0, 1, true);
        desired_->SetFunctionPtr(NULL, NULL, "DeviceTemperature_desired", "service_param");
        elements_["desired"] = desired_;

        increment_ = new Float32;
        increment_->SetProperty(0, 1, true);
        increment_->SetFunctionPtr(NULL, NULL);
        elements_["increment"] = increment_;

        maximum_ = new Float32;
        maximum_->SetProperty(0, 1, false);
        maximum_->SetFunctionPtr(NULL, NULL);
        elements_["maximum"] = maximum_;

        minimum_ = new Float32;
        minimum_->SetProperty(0, 1, false);
        minimum_->SetFunctionPtr(NULL, NULL);
        elements_["minimum"] = minimum_;

        unit_ = new TemperatureUnitType;
        unit_->SetProperty(1, 1, true);
        unit_->SetFunctionPtr(NULL, NULL, "DeviceTemperature_unit", "service_param");
        elements_["unit"] = unit_;
    }

    IdentifiedObjectTemperature::~IdentifiedObjectTemperature()
    {
        SAFE_DESTROY(this->name_);
        SAFE_DESTROY(this->current_);
        SAFE_DESTROY(this->desired_);
        SAFE_DESTROY(this->increment_);
        SAFE_DESTROY(this->maximum_);
        SAFE_DESTROY(this->minimum_);
        SAFE_DESTROY(this->unit_);
    }

    /**
      * Create this class.
      * @return BaseIdentifiedObject*
      */
    BaseIdentifiedObject *IdentifiedObjectTemperature::Create(std::string id)
    {
        DLOG(ERROR) << "IdentifiedObjectTemperature is created. (" << id << ")";
        return new IdentifiedObjectTemperature(id, kObjectName);
    }

    /**
      * Validation Check for ID
      * @return static HTTPResponseType
      */
    HTTPResponseType IdentifiedObjectTemperature::ValidateObjectID()
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
    HTTPResponseType IdentifiedObjectTemperature::CreateObjectID(void *p_in, std::string *id)
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
    HTTPResponseType IdentifiedObjectTemperature::DeleteObjectID(void *p_in, const std::string &id)
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
    void IdentifiedObjectTemperature::SaveCurrentIDWithName(std::map<std::string, std::string>
            &id_deps)
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
