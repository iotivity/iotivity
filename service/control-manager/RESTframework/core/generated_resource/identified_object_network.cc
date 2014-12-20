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
#include "generated_resource/identified_object_network.h"
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

    const char *const kObjectName   = "network";
    const char *const kObjectNameId = "network_id";

    /**
      * Register function to create this class.
      * @param[in] string
      * @param[in] BaseIdentifiedObject*
      * @return static RegisterIdentifiedObject
      */
    RegisterIdentifiedObject IdentifiedObjectNetwork::regIdentifiedObject(kObjectName,
            &IdentifiedObjectNetwork::Create);

    IdentifiedObjectNetwork::IdentifiedObjectNetwork(std::string id,
            std::string object_name) : BaseIdentifiedObject(id, object_name)
    {
        pGetFunc_ = &CommonWrapperFunctions::GetIdentifiedObjectNetwork;
        pPutFunc_ = &CommonWrapperFunctions::PutIdentifiedObjectNetwork;
        pPostFunc_ = NULL;
        pDeleteFunc_ = NULL;

        enabled_ = new _boolean;
        enabled_->SetProperty(1, 1, true);
        enabled_->SetFunctionPtr(NULL, NULL);
        elements_["enabled"] = enabled_;

        connected_ = new _boolean;
        connected_->SetProperty(1, 1, true);
        connected_->SetFunctionPtr(NULL, NULL);
        elements_["connected"] = connected_;

        priority_ = new UInt8;
        priority_->SetProperty(0, 1, true);
        priority_->SetFunctionPtr(NULL, NULL);
        elements_["priority"] = priority_;

        dhcpEnabled_ = new _boolean;
        dhcpEnabled_->SetProperty(1, 1, true);
        dhcpEnabled_->SetFunctionPtr(NULL, NULL);
        elements_["dhcpEnabled"] = dhcpEnabled_;

        ipAddress_ = new String50;
        ipAddress_->SetProperty(1, 1, true);
        ipAddress_->SetFunctionPtr(NULL, NULL);
        elements_["ipAddress"] = ipAddress_;

        subnetMask_ = new String20;
        subnetMask_->SetProperty(1, 1, true);
        subnetMask_->SetFunctionPtr(NULL, NULL);
        elements_["subnetMask"] = subnetMask_;

        gateway_ = new String50;
        gateway_->SetProperty(1, 1, true);
        gateway_->SetFunctionPtr(NULL, NULL);
        elements_["gateway"] = gateway_;

        dnsPrimary_ = new String50;
        dnsPrimary_->SetProperty(1, 1, true);
        dnsPrimary_->SetFunctionPtr(NULL, NULL);
        elements_["dnsPrimary"] = dnsPrimary_;

        dnsSecondary_ = new String50;
        dnsSecondary_->SetProperty(0, 1, true);
        dnsSecondary_->SetFunctionPtr(NULL, NULL);
        elements_["dnsSecondary"] = dnsSecondary_;

        macAddress_ = new String20;
        macAddress_->SetProperty(1, 1, false);
        macAddress_->SetFunctionPtr(NULL, NULL);
        elements_["macAddress"] = macAddress_;
    }

    IdentifiedObjectNetwork::~IdentifiedObjectNetwork()
    {
        SAFE_DESTROY(this->enabled_);
        SAFE_DESTROY(this->connected_);
        SAFE_DESTROY(this->priority_);
        SAFE_DESTROY(this->dhcpEnabled_);
        SAFE_DESTROY(this->ipAddress_);
        SAFE_DESTROY(this->subnetMask_);
        SAFE_DESTROY(this->gateway_);
        SAFE_DESTROY(this->dnsPrimary_);
        SAFE_DESTROY(this->dnsSecondary_);
        SAFE_DESTROY(this->macAddress_);
    }

    /**
      * Create this class.
      * @return BaseIdentifiedObject*
      */
    BaseIdentifiedObject *IdentifiedObjectNetwork::Create(std::string id)
    {
        DLOG(ERROR) << "IdentifiedObjectNetwork is created. (" << id << ")";
        return new IdentifiedObjectNetwork(id, kObjectName);
    }

    /**
      * Validation Check for ID
      * @return static HTTPResponseType
      */
    HTTPResponseType IdentifiedObjectNetwork::ValidateObjectID()
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
    HTTPResponseType IdentifiedObjectNetwork::CreateObjectID(void *p_in, std::string *id)
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
    HTTPResponseType IdentifiedObjectNetwork::DeleteObjectID(void *p_in, const std::string &id)
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
    void IdentifiedObjectNetwork::SaveCurrentIDWithName(std::map<std::string, std::string> &id_deps)
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
