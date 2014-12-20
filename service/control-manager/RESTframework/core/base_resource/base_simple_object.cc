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

#include <map>
#include <string>
#include <vector>

#include "base/memory/scoped_ptr.h"
#include "base_resource/base_simple_object.h"
#include "http_start_line/http_status_code_defined.h"
#include "object_id_manager/creator_object_id_manager.h"
#include "object_id_manager/interface_object_id_manager.h"
#include "base/logging.h"

namespace webservice
{

#undef SAFE_DESTROY
#define SAFE_DESTROY(x)  if (x) { delete(x); x = NULL; }

    BaseSimpleObject::BaseSimpleObject(std::string key_str, std::string str) : key_name_(key_str),
        name_(str)
    {
        this->allowed_get_ = false;
        this->allowed_put_ = false;
        this->allowed_post_ = false;
        this->allowed_delete_ = false;
        this->is_mandatory_ = false;
        this->parent_obj_ = NULL;
    }

    BaseSimpleObject::~BaseSimpleObject()
    {
        std::map<std::string, BaseSimpleObject *>::iterator object_iterator = child_map_.begin();
        while (child_map_.end() != object_iterator)
        {
            if (object_iterator->second)
            {
                DLOG(INFO) << "delete : " << object_iterator->first;
                SAFE_DESTROY(object_iterator->second)
                ++object_iterator;
            }
        }
        child_map_.clear();
    }

    void BaseSimpleObject::SetMethodAllowed(const bool &allowed_get, const bool &allowed_put,
                                            const bool &allowed_post, const bool &allowed_delete)
    {
        this->allowed_get_ = allowed_get;
        this->allowed_put_ = allowed_put;
        this->allowed_post_ = allowed_post;
        this->allowed_delete_ = allowed_delete;
        this->is_mandatory_ = false;
    }

    bool BaseSimpleObject::ReleaseObject(IHTTPMethod *pObj)
    {
        bool ret = (NULL != pObj) ? true : false;
        if (ret) delete pObj;
        return ret;
    }

    bool BaseSimpleObject::ReleaseRequestDict(base::DictionaryValue *p_value)
    {
        bool ret = (NULL != p_value) ? true : false;
        if (ret) delete p_value;
        return ret;
    }

    /**
      * Parse Request Body as dictionary value type
      * @param[in] http_method : check whether request type is post or not
      * @param[in/out] p_value_parent : parent request body instance
      * @param[out] p_value_child : child request body instance
      * @return bool
      */
    bool BaseSimpleObject::ParseRequestDict(const int &http_method,
                                            base::DictionaryValue *p_value_parent, base::Value **p_value_child)
    {
        if (web_util::HTTP_METHOD_PUT != http_method
            && web_util::HTTP_METHOD_POST != http_method) return true;
        if (NULL == p_value_parent) return true;

        const std::string &object_name = GetObjectKeyName();
        DLOG(ERROR) << "Parsing request body, object name is " << object_name;
        if (p_value_parent->HasKey(object_name))
        {
            base::Value *subval = NULL;
            p_value_parent->GetWithoutPathExpansion(object_name, &subval);
            if (Value::TYPE_DICTIONARY != subval->GetType()) return false;

            base::DictionaryValue *p_value_child_dict;
            p_value_parent->GetDictionary(object_name, &p_value_child_dict);
            *p_value_child = static_cast<base::Value *>(p_value_child_dict);
            return true;
        }
        return false;
    }

    bool BaseSimpleObject::ParseChildRequestBody(int http_method, base::Value *request_body,
            base::Value **request_child, const std::string &id)
    {
        if (web_util::HTTP_METHOD_PUT != http_method) return true;

        const base::Value::Type value_type = request_body->GetType();
        DLOG(ERROR) << "Request body type : " << value_type << ", id : " << id;
        if (base::Value::TYPE_DICTIONARY == value_type)
        {
            base::DictionaryValue *request_dict = static_cast<base::DictionaryValue *>(request_body);
            base::DictionaryValue *request_dict_child = NULL;
            if (!request_dict->GetDictionary(this->GetObjectKeyName(), &request_dict_child)) return false;
            if (NULL == request_dict_child) return false;
            *request_child = static_cast<base::Value *>(request_dict_child);
            return true;
        }
        else if (base::Value::TYPE_LIST == value_type)
        {
            // "Temperatures" : []
            base::ListValue *request_list = static_cast<base::ListValue *>(request_body);
            base::DictionaryValue *request_dict_child = NULL;
            size_t index_cnt = 0;
            for (index_cnt = 0; index_cnt < request_list->GetSize(); index_cnt++)
            {
                if (request_list->GetDictionary(index_cnt, &request_dict_child))
                {
                    // id (string/mandatory)
                    std::string id_in_request_body = "";
                    if (request_dict_child->GetString("id", &id_in_request_body))
                    {
                        if (id == id_in_request_body)
                        {
                            *request_child = static_cast<base::Value *>(request_dict_child);
                            return true;
                        }
                    }
                }
            }
            return false;
        }
        else
        {
            return false;
        }
    }

    /**
      * Create child response body to dictionary value type
      * @param[out] p_value_child : child request body instance
      * @return bool
      */
    bool BaseSimpleObject::CreateChildResponseBody(base::Value **p_value_child)
    {
        base::DictionaryValue *p_value_child_dict = new base::DictionaryValue;
        if (NULL == p_value_child_dict) return false;

        *p_value_child = static_cast<base::Value *>(p_value_child_dict);
        return true;
    }

    base::DictionaryValue *BaseSimpleObject::CreateResponseDict(base::Value **p_value_child)
    {
        base::DictionaryValue *p_value = new base::DictionaryValue;
        base::DictionaryValue *p_value_child_dict = new base::DictionaryValue;
        p_value->Set(GetObjectKeyName(), p_value_child_dict);
        *p_value_child = static_cast<base::Value *>(p_value_child_dict);
        return p_value;
    }

    bool BaseSimpleObject::ReleaseResponseDict(base::DictionaryValue *p_value)
    {
        bool ret = (NULL != p_value) ? true : false;
        if (ret) delete p_value;
        return ret;
    }

    const std::string &BaseSimpleObject::GetObjectKeyName()
    {
        return this->key_name_;
    }

    const std::string &BaseSimpleObject::GetObjectName()
    {
        return this->name_;
    }

    const std::map<std::string, BaseSimpleObject *> &BaseSimpleObject::GetChildMap() const
    {
        return this->child_map_;
    }

    void BaseSimpleObject::AddChildObject(BaseSimpleObject *pObj)
    {
        pObj->parent_obj_ = this;
        child_map_[pObj->GetObjectName()] = pObj;
    }

    BaseSimpleObject *BaseSimpleObject::FindChildOfSimpleObject(std::string name, HTTPMessage *http)
    {
        std::map<std::string, BaseSimpleObject *>::iterator it;

        for (it = child_map_.begin(); it != child_map_.end(); it++)
        {
            std::string object_name = it->first;
            BaseSimpleObject *pChildObj = it->second;
            if (pChildObj->GetValidObject(name, object_name, http))
            {
                return pChildObj;
            }
        }
        return NULL;
    }

    int BaseSimpleObject::GetObjectIDList(const std::string &object_name,
                                          std::vector<std::string> *id_list, HTTPMessage *http)
    {
        if (NULL == id_list) return 0;

        CreatorObjectIDManager &factory = CreatorObjectIDManager::GetInstance();
        scoped_ptr<IObjectIDManager> pObj(factory.CreateObject(object_name));
        if (pObj.get())
        {
            return pObj->GetObjectIDList(id_list, http);
        }
        else
        {
            return 0;
        }
    }

    const std::string &BaseSimpleObject::GetOriginUri(void) const
    {
        return this->origin_uri_;
    }

    void BaseSimpleObject::SetOriginUri(const std::string &origin_uri)
    {
        this->origin_uri_ = origin_uri;
    }

    /**
      * Check validity and Get the object
      * Each object id manager is used to check id validation
      * @param[in] input_param1 : id of request uri
      * @param[in] input_param2 : defined object name
      * @return bool
      */
    bool BaseSimpleObject::GetValidObject(const std::string &input_param1,
                                          const std::string &input_param2, HTTPMessage *http)
    {
        if (input_param1 != input_param2) return false;
        else return true;
    }

    const bool BaseSimpleObject::GetMethodAllowed(const int &http_method)
    {
        bool ret_method_allowed = false;
        switch (http_method)
        {
            case web_util::HTTP_METHOD_GET:
                {
                    ret_method_allowed = this->allowed_get_;
                } break;
            case web_util::HTTP_METHOD_PUT:
                {
                    ret_method_allowed = this->allowed_put_;
                } break;
            case web_util::HTTP_METHOD_POST:
                {
                    ret_method_allowed = this->allowed_post_;
                } break;
            case web_util::HTTP_METHOD_DELETE:
                {
                    ret_method_allowed = this->allowed_delete_;
                } break;
            default:
                {
                    ret_method_allowed = false;
                } break;
        }
        return ret_method_allowed;
    }

    const bool BaseSimpleObject::IsMandatoryObject()
    {
        return this->is_mandatory_;
    }

}
