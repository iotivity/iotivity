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

#ifndef WEBSERVICE_BASE_SIMPLE_OBJECT_H_
#define WEBSERVICE_BASE_SIMPLE_OBJECT_H_

#include <map>
#include <string>

#include "base/values.h"
#include "base_resource/interface_http_method.h"
#include "wrapped_component/wrapper_input_type.h"
#include "http_header/http_request_header.h"
#include "http_header/http_response_header.h"
#include "http_start_line/http_request_line.h"
#include "http_start_line/http_status_code_defined.h"
#include "wrapped_component/wrapper_input_type.h"

namespace webservice
{

    /* Type of simpe object */
    typedef enum
    {
        /* This is a root class to provide common naming attributes for all classes needing naming attributes. */
        enSimpleIdentifiedObject,
        /* Many resources within this specification are derived from the list object.
          * A list is a container to hold a collection of object instances or references.
          * Throughout this specification, these resources will collectively be referred to as list resources. */
        enSimpleList,
        /* Link provides a reference, via URI, to another resource or to a list resource.
          * Link is only used for response body of GET method to decrease the body size of the response. */
        enSimpleLink,
        /* A resource is an addressable unit of information, either a collection(List) or instnace of an object.
          * Resource are normally created by resource server itself.
          * However, client can create a new subordinate resource in the list using POST method. */
        enSimpleResource,
        /* Linked Child is not defined in SHP document. Linked Child can always connect to resource.
          * But, a client can not access it directly. */
        enSimpleLinkedChild,
        enSimpleUnknownType
    } SimpleObjectType;

    class BaseSimpleObject
    {
        public:
            BaseSimpleObject(std::string key_str, std::string str);
            virtual ~BaseSimpleObject(void);

            void SetMethodAllowed(const bool &allowed_get, const bool &allowed_put, const bool &allowed_post,
                                  const bool &allowed_delete);

            const std::string &GetObjectKeyName(void);
            const std::string &GetObjectName(void);
            const bool GetMethodAllowed(const int &http_method);
            const bool IsMandatoryObject(void);
            int GetObjectIDList(const std::string &object_name, std::vector<std::string> *id_list,
                                HTTPMessage *http = NULL);
            const std::map<std::string, BaseSimpleObject *> &GetChildMap(void) const;

            void AddChildObject(BaseSimpleObject *pObj);
            BaseSimpleObject *FindChildOfSimpleObject(std::string name, HTTPMessage *http);

            /**
              * Create child response body to dictionary value type
              * @param[out] p_value_child : child request body instance
              * @return bool
              */
            virtual bool CreateChildResponseBody(base::Value **p_value_child);

            /**
              * Parse Request Body as dictionary value type
              * @param[in] http_method : check whether request type is post or not
              * @param[in/out] p_value_parent : parent request body instance
              * @param[out] p_value_child : child request body instance
              * @return bool
              */
            virtual bool ParseRequestDict(const int &http_method, base::DictionaryValue *p_value_parent,
                                          base::Value **p_value_child);
            virtual bool ReleaseRequestDict(base::DictionaryValue *p_value);

            virtual base::DictionaryValue *CreateResponseDict(base::Value **p_value_child);
            virtual bool ReleaseResponseDict(base::DictionaryValue *p_value);

            virtual bool ParseChildRequestBody(int http_method, base::Value *request_body,
                                               base::Value **request_child, const std::string &id);

            /* Following is Pure Virtual Functions */
            /**
              * Get simple object type
              * This function is used to classify each simple object
              * @return SimpleObjectType
              */
            virtual const SimpleObjectType GetSimpleObjectType(void) = 0;

            /**
              * Connect response body to parent body
              * @param[in/out] p_value : parent response body instance
              * @param[in] p_value_child : child response body instance
              * @param[in] response_error_code
              * @param[in] is_force : if this is true, uncoditionally set child response body
              * @return void
              */
            virtual void SetChildResponseBody(base::Value *p_value, base::Value *p_value_child,
                                              HTTPResponseType response_error_code, bool is_force) = 0;

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
            virtual HTTPResponseType ProcessRequest(const HTTPRequestLine &request_line,
                                                    const HTTPRequestHeader &request_header, base::Value *request_body,
                                                    HTTPResponseHeader &response_header, base::Value *response_body,
                                                    std::map<std::string, std::string> &id_deps, const std::string &current_uri) = 0;

        protected:
            /**
              * Create the object which is found from the client's request URI
              * @param[in] id : this class is for identified object, it has own id.
              * @return IHTTPMethod* : the object instance
              */
            virtual IHTTPMethod *CreateObject(std::string id) = 0;
            bool ReleaseObject(IHTTPMethod *pObj);
            const std::string &GetOriginUri(void) const;
            void SetOriginUri(const std::string &origin_uri);

        private:
            /**
              * Check validity and Get the object
              * Each object id manager is used to check id validation
              * @param[in] input_param1 : id of request uri
              * @param[in] input_param2 : defined object name
              * @return bool
              */
            virtual bool GetValidObject(const std::string &input_param1, const std::string &input_param2,
                                        HTTPMessage *http);

        private:
            std::string key_name_;
            std::string name_;
            std::string origin_uri_;
            BaseSimpleObject *parent_obj_;
            std::map<std::string, BaseSimpleObject *> child_map_;
            bool allowed_get_;
            bool allowed_put_;
            bool allowed_post_;
            bool allowed_delete_;
            bool is_mandatory_;
    };
}

#endif /* WEBSERVICE_BASE_SIMPLE_OBJECT_H_ */