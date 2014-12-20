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

#include "abstract_resource/creator_simple_object_strategy.h"
#include "abstract_resource/simple_object_common_strategy.h"
#include "abstract_resource/simple_object_delete_child_strategy.h"
#include "abstract_resource/simple_object_get_device_resources_strategy.h"
#include "abstract_resource/simple_object_get_strategy.h"
#include "abstract_resource/simple_object_notsupported_strategy.h"
#include "abstract_resource/simple_object_post_multiple_resources_strategy.h"
#include "abstract_resource/simple_object_post_strategy.h"
#include "abstract_resource/simple_object_put_child_strategy.h"
#include "abstract_resource/simple_object_put_no_child_strategy.h"
#include "base/values.h"
#include "base/string_split.h"
#include "http_header/http_request_header.h"
#include "http_start_line/http_request_line.h"
#include "http_header.h"
#include "base/logging.h"

namespace webservice
{

    /* Following is policy for child objects */
    const bool putstrategy_withchild = false;
    const bool putstrategy_withresponse = false;

    /**
      * Get the instance of this object
      * @return CreatorSimpleObjectStrategy&
      */
    CreatorSimpleObjectStrategy &CreatorSimpleObjectStrategy::GetInstance()
    {
        static CreatorSimpleObjectStrategy instance;
        return instance;
    }

    /**
      * Set policy Child of simple object
      * @param[in] request_line
      * @param[in] request_header
      * @param[in] request_body
      * @param[in] current_uri
      * @return ISimpleObjectStrategy*
      */
    ISimpleObjectStrategy *CreatorSimpleObjectStrategy::SetChildPolicy(const HTTPRequestLine
            &request_line, const HTTPRequestHeader &request_header, base::Value *request_body,
            const std::string &current_uri)
    {
        int method = request_line.GetHTTPMethod();
        const std::string &uri = request_line.GetOriginUri();
        DLOG(ERROR) << "Origin URI : " << uri << ", Current URI : " << current_uri << ", method : " <<
                    method;

        if (web_util::HTTP_METHOD_GET == method)
        {
            /* GET method */
            if (IsDeviceObject(current_uri)) return new SimpleObjectGetDeviceResourcesStrategy();
            return new SimpleObjectGetStrategy();
        }
        else if (web_util::HTTP_METHOD_PUT == method)
        {
            /* PUT method */
            if (putstrategy_withresponse) return new SimpleObjectNotSupportedStrategy();
            if (putstrategy_withchild) return new SimpleObjectPutChildStrategy();
            return new SimpleObjectPutNoChildStrategy();
        }
        else if (web_util::HTTP_METHOD_POST == method)
        {
            /* POST method */
            if (NULL == request_body) return new SimpleObjectNotSupportedStrategy();
            if (Value::TYPE_DICTIONARY == request_body->GetType()) return new SimpleObjectPostStrategy();
            else if (Value::TYPE_LIST == request_body->GetType()) return new
                        SimpleObjectPostMultipleResourcesStrategy();
            else return new SimpleObjectNotSupportedStrategy();
        }
        else if (web_util::HTTP_METHOD_DELETE == method)
        {
            /* DELETE method */
            return new SimpleObjectDeleteChildStrategy();
        }

        /* Others */
        return new SimpleObjectNotSupportedStrategy();
    }

    /**
      * Release child strategy object
      * @param[in] child_strategy
      * @return void
      */
    void CreatorSimpleObjectStrategy::ReleaseChildStrategyObject(ISimpleObjectStrategy *child_strategy)
    {
        if (NULL != child_strategy)
        {
            delete child_strategy;
            child_strategy = NULL;
        }
    }

    /**
      * Check requested URI whether it is devices/{id} or not
      * @param[in] current_uri
      * @return bool
      */
    bool CreatorSimpleObjectStrategy::IsDeviceObject(const std::string &current_uri)
    {
        bool ret = false;
        // parse the uri and query and update the uri_fields_ and query_content_
        std::vector<std::string> *uri_fields = new std::vector<std::string>;
        if (NULL == uri_fields)
        {
            LOG(ERROR) << "Cound not create a vector object \n";
            return false;
        }

        // Get resource hierarchy structure from URI path
        std::vector<std::string>::iterator it;
        // In case the URI ends with a '/' - it must be removed
        int char_pos = current_uri.length() - 1;
        char last_char = current_uri.at(char_pos);
        std::string modified_uri;

        modified_uri = current_uri;
        if (last_char == '/') modified_uri = modified_uri.substr(0, char_pos);

        base::SplitString(modified_uri, '/', uri_fields);
        it = uri_fields->begin();

        DLOG(INFO) << "URI size : " << uri_fields->size() << ", First URI : " << it[0];

        if (2 == uri_fields->size() && "devices" == it[0]) ret = true;
        delete uri_fields;

        return ret;
    }

}

