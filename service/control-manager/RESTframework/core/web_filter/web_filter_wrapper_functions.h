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

#ifndef WEBSERVICE_WEB_FILTER_WRAPPER_FUNCTIONS_H_
#define WEBSERVICE_WEB_FILTER_WRAPPER_FUNCTIONS_H_

#include <string>
#include <vector>

#include "base/values.h"
#include "http_start_line/http_status_code_defined.h"


namespace webservice
{

    class WebFilterWrapperFunctions
    {
        public:
            /**
               * Process web filter
               * @param[in] req_type
               * @param[in] list_of_key
               * @param[in] response_error_code
               * @param[in] in_response_dict
               * @param[out] out_response_dict
               * @return HTTPResponseType
               */
            static bool ProcessWebFilter(const int &req_type, std::vector<std::string> *list_of_key,
                                         HTTPResponseType response_error_code, const base::DictionaryValue *in_response_dict,
                                         base::DictionaryValue *out_response_dict);

            /**
               * Process web filter
               * @param req_type[in] : http request method from a client
               * @param query_keys[in] : key of query parameter in http request from a client
               * @param response_code[out] : http status code for response
               * @param response_data[out] : http response data
               * @return bool
               */
            static bool ProcessWebFilter(const int &req_type, const std::string  &query_keys,
                                         int *response_code, std::string *response_data);

        private:
            /**
               * Find search_key for web filter in query parameter from request URI
               * @param[in] query_keys : keys to filter int query parameter
               * @param list_query_key[out] : list of key to filter response data
               * @return bool
               */
            static bool ParseKeysInQueryParameters(const std::string &query_keys,
                                                   std::vector<std::string> *list_query_key);

            /**
               * Check response data for web filter
               * @param input_key[in] : it is same with query_key in FindKeyInQueryParameters
               * @param response_code[out] : http status code for response
               * @param response_data[out] : http response data
               * @return bool
              */
            static bool CheckResponse(std::vector<std::string> *list_query_key, int *response_code,
                                      std::string *response_data);

            /**
               * Retrieval response to find value of query key
               * @param[in] dict_val : input dictionary node before web filter
               * @param[in] input_key : it is same with query_key in FindKeyInQueryParameters
               * @param[out] dic_root_response : output dictionary node, it is filter out data
               * @return bool
               */
            static bool RetrieveResponse(std::vector<std::string> *list_query_key,
                                         const base::DictionaryValue *dict_val, base::DictionaryValue *dic_root_response);
    };
}

#endif /* WEBSERVICE_WEB_FILTER_WRAPPER_FUNCTIONS_H_ */

