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

#include "abstract_resource/creator_list.h"
#include "abstract_resource/creator_simple_object_strategy.h"
#include "abstract_resource/interface_simple_object_strategy.h"
#include "abstract_resource/simple_list.h"
#include "base/values.h"
#include "base_resource/interface_http_method.h"
#include "base_resource/base_list.h"
#include "http_header/http_request_header.h"
#include "http_header/http_response_header.h"
#include "http_start_line/http_request_line.h"
#include "http_start_line/http_status_code_defined.h"
#include "base/logging.h"

namespace webservice
{

    /**
      * Create the object which is found from the client's request URI
      * @return IHTTPMethod* : the object instance
      */
    IHTTPMethod *SimpleList::CreateObject(std::string option = "")
    {
        DLOG(INFO) << "SimpleList::CreateObject : " << option;

        CreatorList &factory = CreatorList::GetInstance();
        return factory.CreateObject(this->GetObjectName());
    }

    /**
      * Get simple object type
      * This function is used to classify each simple object
      * @return SimpleObjectType
      */
    const SimpleObjectType SimpleList::GetSimpleObjectType()
    {
        return enSimpleList;
    }

    /**
      * Parse Request Body as dictionary value type
      * @param[in] http_method : check whether request type is post or not
      * @param[in/out] p_value_parent : parent request body instance
      * @param[out] p_value_child : child request body instance
      * @return bool
      */
    bool SimpleList::ParseRequestDict(const int &http_method, base::DictionaryValue *p_value_parent,
                                      base::Value **p_value_child)
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
            if (Value::TYPE_LIST != subval->GetType()) return false;

            base::ListValue *p_value_child_list;
            p_value_parent->GetList(object_name, &p_value_child_list);
            *p_value_child = static_cast<base::Value *>(p_value_child_list);
            return true;
        }
        else
        {
            if (web_util::HTTP_METHOD_POST != http_method) return false;
            std::map<std::string, BaseSimpleObject *>::const_iterator it;
            const std::map<std::string, BaseSimpleObject *> &child_map = this->GetChildMap();
            // Find Child Object, and run child object.
            for (it = child_map.begin(); it != child_map.end(); it++)
            {
                BaseSimpleObject *pChildObj = it->second;
                const std::string &child_object_name = pChildObj->GetObjectKeyName();

                DLOG(ERROR) << "Parsing request body, child object name is " << child_object_name;
                // This is for POST method, almost POST method has own's child object name. (SHP spec.)
                if (p_value_parent->HasKey(child_object_name))
                {
                    base::Value *subval = NULL;
                    p_value_parent->GetWithoutPathExpansion(child_object_name, &subval);
                    if (Value::TYPE_DICTIONARY != subval->GetType()) continue;

                    base::DictionaryValue *p_value_child_dict;
                    p_value_parent->GetDictionary(child_object_name, &p_value_child_dict);
                    *p_value_child = static_cast<base::Value *>(p_value_child_dict);
                    return true;
                }
            }
            return false;
        }
    }

    /**
      * Create response body to dictionary value type
      * @param[out] p_value_child : child request body instance
      * @return base::DictionaryValue*
      */
    base::DictionaryValue *SimpleList::CreateResponseDict(base::Value **p_value_child)
    {
        base::DictionaryValue *p_value = new base::DictionaryValue;
        base::ListValue *p_value_child_list = new base::ListValue;
        p_value->Set(GetObjectKeyName(), p_value_child_list);
        *p_value_child = static_cast<base::Value *>(p_value_child_list);
        return p_value;
    }

    /**
      * Create child response body to dictionary value type
      * @param[out] p_value_child : child request body instance
      * @return bool
      */
    bool SimpleList::CreateChildResponseBody(base::Value **p_value_child)
    {
        base::ListValue *p_value_child_list = new base::ListValue;
        if (NULL == p_value_child_list) return false;

        *p_value_child = static_cast<base::Value *>(p_value_child_list);
        return true;
    }

    /**
      * Connect response body to parent body
      * This class is for list object, so its parent class is always dictionary value type.
      * @param[in/out] p_value : parent response body instance
      * @param[in] p_value_child : child response body instance
      * @param[in] response_error_code
      * @param[in] is_force : if this is true, uncoditionally set child response body
      * @return void
      */
    void SimpleList::SetChildResponseBody(base::Value *p_value, base::Value *p_value_child,
                                          HTTPResponseType response_error_code, bool is_force)
    {
        if (NULL == p_value  || NULL == p_value_child) return;
        base::DictionaryValue *p_value_dict = static_cast<base::DictionaryValue *>(p_value);
        base::ListValue *p_value_child_list = static_cast<base::ListValue *>(p_value_child);
        if (is_force || (response_error_code >= RT_200_START && response_error_code < RT_300_START
                         && p_value_child_list->GetSize() > 0))
        {
            p_value_dict->Set(this->GetObjectKeyName(), p_value_child_list);
        }
        else
        {
            delete p_value_child_list;
        }
    }

    /**
      * Main process of simple object
      * Create found object and search its child object
      * @param[in] request_line : http request line
      * @param[in] request_header : http request header
      * @param[in] request_body : request body as value type
      * @param[out] response_header : http response header
      * @param[out] response_body : response body as value type
      * @param[in] id_deps : save all id of identified object instance
      * @param[in] current_uri : this mean current URI's depth
      * @return HTTPResponseType
      */
    HTTPResponseType SimpleList::ProcessRequest(const HTTPRequestLine &request_line,
            const HTTPRequestHeader &request_header, base::Value *request_body,
            HTTPResponseHeader &response_header, base::Value *response_body,
            std::map<std::string, std::string> &id_deps, const std::string &current_uri)
    {
        HTTPResponseType response_error_code = RT_UNKNOWN;
        IHTTPMethod *p_obj = NULL;

        // Set Origin URI
        const std::string &origin_uri = request_line.GetOriginUri();
        DLOG(INFO) << "Origin Uri : " << origin_uri << ", Current Uri : " << current_uri;
        SetOriginUri(origin_uri);

        // Run Object
        // Create list
        p_obj = CreateObject();

        // If p_obj is null, this means the object is not implemented
        if (NULL == p_obj) return RT_501_RESOURCE_OBJECT;
        response_error_code = p_obj->ProcessHTTPRequestMethod(request_line, request_header, request_body,
                              response_header, response_body, id_deps);
        ReleaseObject(p_obj);

        // If Parent object is not ok, then return
        if (RT_200_START > response_error_code
            || RT_300_START <= response_error_code) return response_error_code;

        // Find Child Object, and run child object.
        HTTPResponseType child_response_error_code = RT_UNKNOWN;
        ISimpleObjectStrategy *child_strategy = CreatorSimpleObjectStrategy::GetInstance().SetChildPolicy(
                request_line, request_header, request_body, current_uri);
        if (NULL == child_strategy)
        {
            LOG(ERROR) << "Child object strategy is NULL";
            return RT_500_ALLOC_FAIL;
        }

        // Go to child object
        child_response_error_code = child_strategy->FindAndProcessChildObject(request_line, request_header,
                                    request_body, response_header, response_body, id_deps, current_uri, this);

        // Release child_strategy instance
        CreatorSimpleObjectStrategy::GetInstance().ReleaseChildStrategyObject(child_strategy);
        if (RT_200_START <= child_response_error_code && RT_300_START > child_response_error_code)
        {
            return response_error_code;
        }
        else
        {
            return child_response_error_code;
        }
    }

}
