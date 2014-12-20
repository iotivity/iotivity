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

#ifndef WEBSERVICE_BASE_RESOURCE_WRAPPER_FUNCTION_H_
#define WEBSERVICE_BASE_RESOURCE_WRAPPER_FUNCTION_H_

#include <string>

#include "base/values.h"
#include "data_types/primitive_data_type.h"
#include "http_start_line/http_status_code_defined.h"
#include "wrapped_component/wrapper_input_type.h"

namespace webservice
{

    class ProcessAttribute
    {
        public:
            HTTPResponseType GetAttribute(std::string name, BaseAttributeType *attr, WrapperInput *input,
                                          base::DictionaryValue *response_dict);
            HTTPResponseType GetAttributeWithFunctionResponse(std::string name, BaseAttributeType *attr,
                    base::DictionaryValue *function_response, base::DictionaryValue *response_dict);
            HTTPResponseType PutAttribute(std::string name, BaseAttributeType *attr, WrapperInput *input,
                                          base::DictionaryValue *request_dict);
            HTTPResponseType PutAttributeWithFunctionResponse(std::string name, BaseAttributeType *attr,
                    base::DictionaryValue *function_response);
            HTTPResponseType PutPreChecking(std::string input_key,
                                            std::map<std::string, BaseAttributeType *> &elements, base::DictionaryValue *request_dict);
            HTTPResponseType PostAttribute(std::string name, BaseAttributeType *attr, WrapperInput *input,
                                           base::DictionaryValue *request_dict);
            HTTPResponseType PostResponseAttributeWithFunctionResponse(std::string name,
                    BaseAttributeType *attr, base::DictionaryValue *function_response,
                    base::DictionaryValue *response_dict);
            HTTPResponseType PostResponseAttribute(std::string name, BaseAttributeType *attr,
                                                   WrapperInput *input, base::DictionaryValue *response_dict);
            HTTPResponseType PostPreChecking(std::string input_key,
                                             std::map<std::string, BaseAttributeType *> &elements, base::DictionaryValue *request_dict);

        private:
            HTTPResponseType GetStringDefalutAttribute(std::string name, BaseAttributeType *attr,
                    base::DictionaryValue *response_dict);
            HTTPResponseType GetBooleanDefalutAttribute(std::string name, BaseAttributeType *attr,
                    base::DictionaryValue *response_dict);
            HTTPResponseType GetIntegerDefalutAttribute(std::string name, BaseAttributeType *attr,
                    base::DictionaryValue *response_dict);
            HTTPResponseType GetDoubleDefalutAttribute(std::string name, BaseAttributeType *attr,
                    base::DictionaryValue *response_dict);
            HTTPResponseType GetListAttribute(std::string name, BaseAttributeType *attr, WrapperInput *input,
                                              base::DictionaryValue *response_dict);
            HTTPResponseType PostResponseListAttribute(std::string name, BaseAttributeType *attr,
                    WrapperInput *input, base::DictionaryValue *response_dict);
            HTTPResponseType SetListAttribute(std::string name, BaseAttributeType *attr, WrapperInput *input,
                                              base::DictionaryValue *request_dict);
    };


}

#endif /* WEBSERVICE_BASE_RESOURCE_WRAPPER_FUNCTION_H_ */

