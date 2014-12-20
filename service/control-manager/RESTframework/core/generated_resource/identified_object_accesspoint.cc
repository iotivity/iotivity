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
#include "generated_resource/identified_object_accesspoint.h"
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

    const char *const kObjectName   = "accesspoint";
    const char *const kObjectNameId = "accesspoint_id";

    /**
      * Register function to create this class.
      * @param[in] string
      * @param[in] BaseIdentifiedObject*
      * @return static RegisterIdentifiedObject
      */
    RegisterIdentifiedObject IdentifiedObjectAccessPoint::regIdentifiedObject(kObjectName,
            &IdentifiedObjectAccessPoint::Create);

    IdentifiedObjectAccessPoint::IdentifiedObjectAccessPoint(std::string id,
            std::string object_name) : BaseIdentifiedObject(id, object_name)
    {
        pGetFunc_ = &CommonWrapperFunctions::GetIdentifiedObjectAccessPoint;
        pPutFunc_ = NULL;
        pPostFunc_ = NULL;
        pDeleteFunc_ = NULL;

        channel_ = new UInt8;
        channel_->SetProperty(1, 1, false);
        channel_->SetFunctionPtr(NULL, NULL);
        elements_["channel"] = channel_;

        ssid_ = new String50;
        ssid_->SetProperty(1, 1, false);
        ssid_->SetFunctionPtr(NULL, NULL);
        elements_["ssid"] = ssid_;

        macAddress_ = new String20;
        macAddress_->SetProperty(1, 1, false);
        macAddress_->SetFunctionPtr(NULL, NULL);
        elements_["macAddress"] = macAddress_;

        maxRate_ = new UInt8;
        maxRate_->SetProperty(1, 1, false);
        maxRate_->SetFunctionPtr(NULL, NULL);
        elements_["maxRate"] = maxRate_;

        rssi_ = new Int8;
        rssi_->SetProperty(1, 1, false);
        rssi_->SetFunctionPtr(NULL, NULL);
        elements_["rssi"] = rssi_;

        securityType_ = new String20;
        securityType_->SetProperty(0, 1, false);
        securityType_->SetFunctionPtr(NULL, NULL);
        elements_["securityType"] = securityType_;

        encryptionType_ = new String20;
        encryptionType_->SetProperty(0, 1, false);
        encryptionType_->SetFunctionPtr(NULL, NULL);
        elements_["encryptionType"] = encryptionType_;
    }

    IdentifiedObjectAccessPoint::~IdentifiedObjectAccessPoint()
    {
        SAFE_DESTROY(this->channel_);
        SAFE_DESTROY(this->ssid_);
        SAFE_DESTROY(this->macAddress_);
        SAFE_DESTROY(this->maxRate_);
        SAFE_DESTROY(this->rssi_);
        SAFE_DESTROY(this->securityType_);
        SAFE_DESTROY(this->encryptionType_);
    }

    /**
      * Create this class.
      * @return BaseIdentifiedObject*
      */
    BaseIdentifiedObject *IdentifiedObjectAccessPoint::Create(std::string id)
    {
        DLOG(ERROR) << "IdentifiedObjectAccessPoint is created. (" << id << ")";
        return new IdentifiedObjectAccessPoint(id, kObjectName);
    }

    /**
      * Validation Check for ID
      * @return static HTTPResponseType
      */
    HTTPResponseType IdentifiedObjectAccessPoint::ValidateObjectID()
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
    HTTPResponseType IdentifiedObjectAccessPoint::CreateObjectID(void *p_in, std::string *id)
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
    HTTPResponseType IdentifiedObjectAccessPoint::DeleteObjectID(void *p_in, const std::string &id)
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
    void IdentifiedObjectAccessPoint::SaveCurrentIDWithName(std::map<std::string, std::string>
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
