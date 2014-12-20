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

#include "abstract_resource/creator_identified_object.h"
#include "abstract_resource/creator_simple_object_strategy.h"
#include "abstract_resource/interface_simple_object_strategy.h"
#include "abstract_resource/simple_identified_object.h"
#include "base/values.h"
#include "base/memory/scoped_ptr.h"
#include "base_resource/base_identified_object.h"
#include "base_resource/interface_http_method.h"
#include "http_header/http_request_header.h"
#include "http_header/http_response_header.h"
#include "http_start_line/http_request_line.h"
#include "http_start_line/http_status_code_defined.h"
#include "object_id_manager/creator_object_id_manager.h"
#include "object_id_manager/interface_object_id_manager.h"
#include "base/logging.h"

namespace webservice
{

    /**
      * Check validity and Get the object
      * Each object id manager is used to check id validation
      * @param[in] input_param1 : id of request uri
      * @param[in] input_param2 : defined object name
      * @return bool
      */
    bool SimpleIdentifiedObject::GetValidObject(const std::string &input_param1,
            const std::string &input_param2, HTTPMessage *http)
    {
        /* input_param1 is id of request uri, input_param2 is defined object name. */
        CreatorObjectIDManager &factory = CreatorObjectIDManager::GetInstance();
        scoped_ptr<IObjectIDManager> pObj(factory.CreateObject(input_param2));
        if (NULL == pObj.get()) return false;

        HTTPResponseType en_response_type = pObj->ValidateObjectID(input_param1, http);
        if (RT_200_START <= en_response_type && RT_300_START > en_response_type) return true;
        else return false;
    }

    /**
      * Create the object which is found from the client's request URI
      * @param[in] id : this class is for identified object, it has own id.
      * @return IHTTPMethod* : the object instance
      */
    IHTTPMethod *SimpleIdentifiedObject::CreateObject(std::string id)
    {
        DLOG(INFO) << "SimpleIdentifiedObject::CreateObject : " << id;

        CreatorIdentifiedObject &factory = CreatorIdentifiedObject::GetInstance();
        return factory.CreateObject(this->GetObjectName(), id);
    }

    /**
      * Get simple object type
      * This function is used to classify each simple object
      * @return SimpleObjectType
      */
    const SimpleObjectType SimpleIdentifiedObject::GetSimpleObjectType()
    {
        return enSimpleIdentifiedObject;
    }

    /**
      * Connect response body to parent body
      * This class is for identified object, so its parent class is always list type object.
      * @param[in/out] p_value : parent response body instance
      * @param[in] p_value_child : child response body instance
      * @param[in] response_error_code
      * @param[in] is_force : if this is true, uncoditionally set child response body
      * @return void
      */
    void SimpleIdentifiedObject::SetChildResponseBody(base::Value *p_value, base::Value *p_value_child,
            HTTPResponseType response_error_code, bool is_force)
    {
        if (NULL == p_value  || NULL == p_value_child) return;
        /* This object is for identified object, so child object type is alwasys dictionary value type */
        base::DictionaryValue *p_value_child_dict = static_cast<base::DictionaryValue *>(p_value_child);
        if (is_force || (response_error_code >= RT_200_START && response_error_code < RT_300_START
                         && p_value_child_dict->size() > 0))
        {
            /* when response error code is not error, only connect it to parent */
            if (p_value->GetType() == base::Value::TYPE_LIST)
            {
                base::ListValue *p_value_list = static_cast<base::ListValue *>(p_value);
                p_value_list->Append(p_value_child_dict);
            }
            else if (p_value->GetType() == base::Value::TYPE_DICTIONARY)
            {
                base::DictionaryValue *p_value_dict = static_cast<base::DictionaryValue *>(p_value);
                p_value_dict->MergeDictionary(p_value_child_dict);
            }
        }
        else
        {
            /* when child instance is not connected, it should be deleted */
            delete p_value_child_dict;
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
    HTTPResponseType SimpleIdentifiedObject::ProcessRequest(const HTTPRequestLine &request_line,
            const HTTPRequestHeader &request_header, base::Value *request_body,
            HTTPResponseHeader &response_header, base::Value *response_body,
            std::map<std::string, std::string> &id_deps, const std::string &current_uri)
    {
        HTTPResponseType response_error_code = RT_UNKNOWN;
        IHTTPMethod *p_obj = NULL;

        /* Set Origin URI */
        const std::string &origin_uri = request_line.GetOriginUri();
        SetOriginUri(origin_uri);

        /* Delimiter : "/" to divide URI */
        size_t found = current_uri.find_last_of("/");

        std::string id = "";
        // get id for this identified object
        if (std::string::npos != found) id = current_uri.substr(found + 1);
        DLOG(INFO) << "Origin Uri : " << origin_uri << ", Current Uri : " << current_uri << ", ID : " << id;

        // Create identified object
        p_obj = CreateObject(id);

        // If p_obj is null, this means the object is not implemented
        if (NULL == p_obj) return RT_501_RESOURCE_OBJECT;
        response_error_code = p_obj->ProcessHTTPRequestMethod(request_line, request_header, request_body,
                              response_header, response_body, id_deps);
        ReleaseObject(p_obj);

        // If Parent object is not ok, then return
        if (RT_200_START > response_error_code
            || RT_300_START <= response_error_code) return response_error_code;

        // Input Location
        int http_method = request_line.GetHTTPMethod();
        if (web_util::HTTP_METHOD_POST == http_method)
        {
            std::string created_id = id_deps["created_new_id"];
            if ("" != created_id)
            {
                response_header.SetLocation(request_header.GetHost(), origin_uri, created_id);
            }
        }

        // Find Child Object, and run child object.
        HTTPResponseType child_response_error_code = RT_UNKNOWN;
        ISimpleObjectStrategy *child_strategy = CreatorSimpleObjectStrategy::GetInstance().SetChildPolicy(
                request_line, request_header, request_body, current_uri);
        if (NULL == child_strategy)
        {
            LOG(ERROR) << "Child stratey instance is NULL!";
            return RT_500_ALLOC_FAIL;
        }

        // Go to child object
        child_response_error_code = child_strategy->FindAndProcessChildObject(request_line, request_header,
                                    request_body, response_header, response_body, id_deps, current_uri, this);

        // Release child_strategy instance
        CreatorSimpleObjectStrategy::GetInstance().ReleaseChildStrategyObject(child_strategy);
        if (RT_200_START > child_response_error_code || RT_300_START <= child_response_error_code)
        {
            return child_response_error_code;
        }
        else
        {
            return response_error_code;
        }
    }

}
