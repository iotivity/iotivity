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

#include "abstract_resource/creator_link.h"
#include "abstract_resource/creator_list.h"
#include "abstract_resource/creator_resource.h"
#include "abstract_resource/creator_simple_object_strategy.h"
#include "abstract_resource/interface_simple_object_strategy.h"
#include "abstract_resource/simple_link.h"
#include "base/values.h"
#include "base_resource/interface_http_method.h"
#include "base_resource/base_link.h"
#include "base_resource/base_list.h"
#include "base_resource/base_resource.h"
#include "http_header/http_request_header.h"
#include "http_header/http_response_header.h"
#include "http_start_line/http_request_line.h"
#include "http_start_line/http_status_code_defined.h"
#include "base/logging.h"

namespace webservice
{

    /* This is not normal case.  this is for GET /capability URI. */
    const char *const kUriCapability = "capability";

    /**
      * Create the object which is found from the client's request URI
      * @param[in] option : this class is for identified object, it has own id.
      * @return IHTTPMethod* : the object instance
      */
    IHTTPMethod *SimpleLink::CreateObject(std::string option)
    {
        DLOG(INFO) << "SimpleLink::CreateObject : " << option;

        /* Request URI from a client */
        const std::string &origin_uri = this->GetOriginUri();
        /* Get object name */
        const std::string &object_name = this->GetObjectName();
        /* Define delmiter as "/" */
        size_t found_last = origin_uri.find_last_of("/");
        std::string name = origin_uri.substr(found_last + 1);

        /* This is child object */
        if (object_name != name)
        {
            /* Check exception case */
            std::string uri = ProcessAndCheckException(option);
            /* When this object is child of any resource, this is just link object. */
            CreatorLink &link_factory = CreatorLink::GetInstance();
            return link_factory.CreateObject(object_name, uri);
        }

        // This is own object
        size_t found_first = origin_uri.find("/");
        std::string own_name = origin_uri.substr(0, found_first);
        if (kUriCapability != own_name)
        {
            /* If URI is not capability, the system should run linked URI to resource. */
            CreatorResource &resource_factory = CreatorResource::GetInstance();
            IHTTPMethod *pResourceObj = resource_factory.CreateObject(object_name);
            if (NULL != pResourceObj) return pResourceObj;

            /* If URI is capability, the system should run it to list type. */
            CreatorList &list_factory = CreatorList::GetInstance();
            return list_factory.CreateObject(object_name);
        }
        return NULL;
    }

    /**
      * Get simple object type
      * This function is used to classify each simple object
      * @return SimpleObjectType
      */
    const SimpleObjectType SimpleLink::GetSimpleObjectType()
    {
        return enSimpleLink;
    }

    /**
      * Parse Request Body as dictionary value type
      * @param[in] http_method : check whether request type is post or not
      * @param[in/out] p_value_parent : parent request body instance
      * @param[out] p_value_child : child request body instance
      * @return void
      */
    bool SimpleLink::ParseRequestDict(const int &http_method, base::DictionaryValue *p_value_parent,
                                      base::Value **p_value_child)
    {
        if (web_util::HTTP_METHOD_PUT != http_method
            && web_util::HTTP_METHOD_POST != http_method) return true;
        if (NULL == p_value_parent) return true;

        const std::string &object_name = GetObjectKeyName();
        DLOG(ERROR) << "Parsing request body, object name is " << object_name;
        if (p_value_parent->HasKey(object_name))
        {
            if (IsListType(object_name))
            {
                base::ListValue *p_value_child_list;
                if (!p_value_parent->GetList(object_name, &p_value_child_list)) return false;
                *p_value_child = static_cast<base::Value *>(p_value_child_list);
            }
            else
            {
                base::DictionaryValue *p_value_child_dict;
                if (!p_value_parent->GetDictionary(object_name, &p_value_child_dict)) return false;
                *p_value_child = static_cast<base::Value *>(p_value_child_dict);
            }
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

                DLOG(INFO) << "Parsing request body, child object name is " << child_object_name;
                // This is for POST method, almost POST method has own's child object name. (SHP spec.)
                if (p_value_parent->HasKey(child_object_name))
                {
                    base::DictionaryValue *p_value_child_dict;
                    if (!p_value_parent->GetDictionary(child_object_name, &p_value_child_dict)) return false;
                    *p_value_child = static_cast<base::Value *>(p_value_child_dict);
                    return true;
                }
            }
            return false;
        }
    }

    /**
      * Process and check exception case
      * @param[in] uri
      * @return std::string
      */
    std::string SimpleLink::ProcessAndCheckException(const std::string &uri) const
    {
        std::string ret_str = uri;
        size_t found = uri.find("/");
        std::string name = uri.substr(0, found);

        // Exception Case (capability)
        if (kUriCapability == name)
        {
            ret_str = uri.substr(found + 1);
        }
        return ret_str;
    }

    /**
      * Connect response body to parent body
      * This class is for link object, so its parent class is always dictionary value type.
      * @param[in/out] p_value : parent response body instance
      * @param[in] p_value_child : child response body instance
      * @param[in] response_error_code
      * @param[in] is_force : if this is true, uncoditionally set child response body
      * @return void
      */
    void SimpleLink::SetChildResponseBody(base::Value *p_value, base::Value *p_value_child,
                                          HTTPResponseType response_error_code, bool is_force)
    {
        if (NULL == p_value  || NULL == p_value_child) return;
        /* Parent object type is always dictionary value type */
        base::DictionaryValue *p_value_dict = static_cast<base::DictionaryValue *>(p_value);
        /* This object is for link object, so child object type is alwasys dictionary value type */
        base::DictionaryValue *p_value_child_dict = static_cast<base::DictionaryValue *>(p_value_child);
        if (is_force || (response_error_code >= RT_200_START && response_error_code < RT_300_START
                         && p_value_child_dict->size() > 0))
        {
            /* This is link type, so, should add "Link" string. */
            std::string link_string = this->GetObjectKeyName() + "Link";
            /* when response error code is not error, only connect it to parent */
            p_value_dict->Set(link_string, p_value_child_dict);
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
    HTTPResponseType SimpleLink::ProcessRequest(const HTTPRequestLine &request_line,
            const HTTPRequestHeader &request_header, base::Value *request_body,
            HTTPResponseHeader &response_header, base::Value *response_body,
            std::map<std::string, std::string> &id_deps, const std::string &current_uri)
    {
        HTTPResponseType response_error_code = RT_UNKNOWN;
        IHTTPMethod *p_obj = NULL;

        // Set Origin URI
        const std::string &origin_uri = request_line.GetOriginUri();
        SetOriginUri(origin_uri);

        // Run Object
        DLOG(INFO) << "Origin Uri : " << origin_uri << ", Current Uri : " << current_uri;

        // Link type object need current uri to set it in attribute field.
        // Create identified object
        p_obj = CreateObject(current_uri);
        // If p_obj is null, this means the object is not implemented or not found URI.
        if (NULL == p_obj)
        {
            size_t found = current_uri.find("/");
            std::string name = current_uri.substr(0, found);
            if (kUriCapability != name) return RT_501_RESOURCE_OBJECT;
            else return RT_404_NOT_FOUND_URI;
        }
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
            LOG(ERROR) << "Child stratey instance is NULL!";
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

    /**
      * Check value type using object name
      * @param[in] object_name
      * @return bool
      */
    bool SimpleLink::IsListType(const std::string &object_name)
    {
        DLOG(INFO) << "object name : " << object_name;
        const char &end_char = object_name.at(object_name.length() - 1);
        if ('s' == end_char) return true;
        else return false;
    }

}
