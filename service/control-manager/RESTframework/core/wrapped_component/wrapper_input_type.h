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

#ifndef WEBSERVICE_WRAPPER_INPUT_TYPE_H_
#define WEBSERVICE_WRAPPER_INPUT_TYPE_H_

#include "base/values.h"
#include "base_resource/http_message.h"

namespace webservice
{
// Data structure contains pointers to all information which are
// required for Get/Set/Post wraper functions of SHP data types
// It does not own any of it's members so need not delete them
// at end it's scope

// In future, more members can be added to this data structure if required
    typedef struct WrapperInput
    {
        // Dictionary which holds all identifieed objects IDs
        base::DictionaryValue *dictionary;
        // Structure which holds pointer to HTTP information of the current ReST API
        HTTPMessage *http;
        WrapperInput(HTTPMessage *p_http = NULL, base::DictionaryValue *p_dict = NULL)
        {
            dictionary = p_dict;
            http = p_http;
        }
        ~WrapperInput()
        {
            // it does not own members so assign them to NULL but do not delete
            dictionary = NULL;
            http = NULL;
        }
    } WrapperInput;
}
#endif  //  WEBSERVICE_WRAPPER_INPUT_TYPE_H_