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

#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "base/values.h"
#include "base/memory/scoped_ptr.h"
#include "http_start_line/http_status_code_defined.h"
#include "http_header.h"
#include "web_filter/web_filter_wrapper_functions.h"

namespace webservice
{

    /**
       * Process web filter
       * @param[in] req_type
       * @param[in] list_of_key
       * @param[in] response_error_code
       * @param[in] in_response_dict
       * @param[out] out_response_dict
       * @return bool
       */
    bool WebFilterWrapperFunctions::ProcessWebFilter(const int &req_type,
            std::vector<std::string> *list_of_key, HTTPResponseType response_error_code,
            const base::DictionaryValue *in_response_dict, base::DictionaryValue *out_response_dict)
    {
        if (NULL == in_response_dict || NULL == out_response_dict) return false;
        if (RT_200_START > response_error_code || RT_300_START <= response_error_code) return false;
        if (web_util::HTTP_METHOD_GET != req_type) return false;

        return RetrieveResponse(list_of_key, in_response_dict, out_response_dict);
    }

    /**
       * Process web filter
       * @param req_type[in] : http request method from a client
       * @param query_keys[in] : key of query parameter in http request from a client
       * @param response_code[out] : http status code for response
       * @param response_data[out] : http response data
       * @return bool
       */
    bool WebFilterWrapperFunctions::ProcessWebFilter(const int &req_type,
            const std::string  &query_keys, int *response_code, std::string *response_data)
    {
        bool return_value = false;

        // Condition of Web Filter
        // It allow only GET method,
        // And It allow only that response code is 2xx.
        if ((req_type == web_util::HTTP_METHOD_GET) &&
            (*response_code >= kHttpStatusCodeSuccessOk &&
             *response_code < kHttpStatusCodeRedirectSpecialResponse))
        {
            // Find "Filter"
            std::vector<std::string> *list_of_key = new std::vector<std::string>;
            if ( ParseKeysInQueryParameters(query_keys, list_of_key) )
            {
                // Search key value in response data
                return_value = CheckResponse(list_of_key, response_code, response_data);
            }
            delete list_of_key;
        }
        else if (req_type != web_util::HTTP_METHOD_GET)
        {
            // This is entered by web filter, (query key : filter)
            // and Web Filter allow only GET method,
            // so in case of other case (PUT or POST), a server should response "405 Method not allowed".
            *response_code = kHttpStatusCodeErrClientNotAllowed;
            *response_data = "Method not allowed";
        }
        else
        {
            // Just bypass.
        }
        return return_value;
    }

    /**
     * Find search_key for web filter in query parameter from request URI
     * @param query_keys[in] : keys to filter int query parameter
     * @param list_query_key[out] : list of key to filter response data
     * @return bool
     */
    bool WebFilterWrapperFunctions::ParseKeysInQueryParameters(const std::string &query_keys,
            std::vector<std::string> *list_query_key)
    {
        std::string dummy_string = query_keys;
        std::string found_key = "";

        for (size_t delimiter_position = dummy_string.find(',');
             delimiter_position != std::string::npos;
             delimiter_position = dummy_string.find(','))
        {
            found_key = dummy_string.substr(0, delimiter_position);
            list_query_key->push_back(found_key);
            dummy_string.erase(0, delimiter_position + 1);
        }
        found_key = dummy_string;
        list_query_key->push_back(found_key);

        return true;
    }

    /**
     * Check response data for web filter
     * @param input_key[in] : it is same with query_key in FindKeyInQueryParameters
     * @param response_code[out] : http status code for response
     * @param response_data[out] : http response data
     * @return bool
    */
    bool WebFilterWrapperFunctions::CheckResponse(std::vector<std::string> *list_query_key,
            int *response_code, std::string *response_data)
    {
        bool ret_val = false;
        scoped_ptr<Value> root_node;
        DictionaryValue *root_filtered_response = new DictionaryValue;

        root_node.reset(base::JSONReader::Read(*response_data));
        if (root_node.get() && (root_node->IsType(Value::TYPE_DICTIONARY)))
        {
            DictionaryValue *dict_val = static_cast<DictionaryValue *>(root_node.get());

            ret_val = RetrieveResponse(list_query_key, dict_val, root_filtered_response);
            if (ret_val != false)
            {
                *response_data = "";
                base::JSONWriter::Write(root_filtered_response, response_data);
            }
            else
            {
                *response_data = "Not Found";
                *response_code = kHttpStatusCodeErrClientNotFound;
            }
        }
        else
        {
            *response_data = "Internal Server Error";
            *response_code = kHttpStatusCodeErrServerInternalServerError;
        }
        root_filtered_response->Clear();
        delete root_filtered_response;

        return ret_val;
    }

    /**
     * Retrieval response to find value of query key
     * @param[in] dict_val : input dictionary node before web filter
     * @param[in] input_key : it is same with query_key in FindKeyInQueryParameters
     * @param[out] dic_root_response : output dictionary node, it is filter out data
     * @return bool
     */
    bool WebFilterWrapperFunctions::RetrieveResponse(std::vector<std::string> *list_query_key,
            const base::DictionaryValue *dict_val, base::DictionaryValue *dic_root_response)
    {
        bool return_value = false;
        bool bRet = false;
        std::string input_key = "";
        std::vector<std::string>::iterator it;

        // Check response data
        for (DictionaryValue::key_iterator dic_key_it = dict_val->begin_keys();
             dic_key_it != dict_val->end_keys(); ++dic_key_it)
        {
            bool bFound = false;
            for (it = list_query_key->begin(); it != list_query_key->end(); it++)
            {
                input_key = *it;
                if (*dic_key_it == input_key)
                {
                    bFound = true;
                    break;
                }
            }

            Value *subval = NULL;
            if (dict_val->GetWithoutPathExpansion(*dic_key_it, &subval))
            {
                if (bFound)
                {
                    switch (subval->GetType())
                    {
                        case Value::TYPE_BOOLEAN:
                            {
                                // If node of key is same with key for filtering and value type is boolean,
                                // then output node saves them.
                                bool bValue = false;
                                bRet = subval->GetAsBoolean(&bValue);
                                if (bRet)
                                {
                                    dic_root_response->SetWithoutPathExpansion(input_key, base::Value::CreateBooleanValue(bValue));
                                    return_value = true;
                                }
                                else
                                {
                                    return_value = false;
                                }
                            } break;

                        case Value::TYPE_INTEGER:
                            {
                                // If node of key is same with key for filtering and value type is integer,
                                // then output node saves them.
                                int nValue = 0;
                                bRet = subval->GetAsInteger(&nValue);
                                if (bRet)
                                {
                                    dic_root_response->SetWithoutPathExpansion(input_key, base::Value::CreateIntegerValue(nValue));
                                    return_value = true;
                                }
                                else
                                {
                                    return_value = false;
                                }
                            } break;

                        case Value::TYPE_DOUBLE:
                            {
                                // If node of key is same with key for filtering and value type is double,
                                // then output node saves them.
                                double dValue = 0;
                                bRet = subval->GetAsDouble(&dValue);
                                if (bRet)
                                {
                                    dic_root_response->SetWithoutPathExpansion(input_key, base::Value::CreateDoubleValue(dValue));
                                    return_value = true;
                                }
                                else
                                {
                                    return_value = false;
                                }
                            } break;

                        case Value::TYPE_STRING:
                            {
                                // If node of key is the same with a key for filtering and value type is string,
                                // then output node saves them.
                                std::string strValue2;

                                bRet = subval->GetAsString(&strValue2);
                                if (bRet)
                                {
                                    dic_root_response->SetWithoutPathExpansion(input_key, base::Value::CreateStringValue(strValue2));
                                    return_value = true;
                                }
                                else
                                {
                                    return_value = false;
                                }
                            } break;

                        case Value::TYPE_LIST:
                            {
                                // If node of key is same with key for filtering and value type is list,
                                // then output node saves them.
                                ListValue *in_list_value = NULL;
                                ListValue *out_list_value = NULL;

                                bRet = subval->GetAsList(&in_list_value);
                                if (bRet && (NULL != in_list_value))
                                {
                                    out_list_value = in_list_value->DeepCopy();
                                    dic_root_response->Set(input_key, out_list_value);
                                    return_value = true;
                                }
                                else
                                {
                                    return_value = false;
                                }
                            } break;

                        case Value::TYPE_DICTIONARY:
                            {
                                // If node of key is same with key for filtering and value type is dictionary,
                                // then output node saves them.
                                DictionaryValue *in_dict_value = NULL;
                                DictionaryValue *out_dict_value = NULL;

                                bRet = subval->GetAsDictionary(&in_dict_value);
                                if (bRet && (NULL != in_dict_value))
                                {
                                    out_dict_value = in_dict_value->DeepCopy();
                                    dic_root_response->Set(input_key, out_dict_value);
                                    return_value = true;
                                }
                                else
                                {
                                    return_value = false;
                                }
                            } break;

                        default:
                            {
                            } break;
                    }
                }
                else
                {
                    switch (subval->GetType())
                    {
                        case Value::TYPE_LIST:
                            {
                                // If node of key is NOT same with key for filtering and value type is list,
                                // then retrieval child node.
                                ListValue *in_list_value = NULL;
                                ListValue *out_list_value = new ListValue;
                                int num_of_checked_list = 0;

                                bRet = subval->GetAsList(&in_list_value);
                                if (bRet && (NULL != in_list_value))
                                {
                                    for (size_t index_cnt = 0; index_cnt < in_list_value->GetSize(); index_cnt++)
                                    {
                                        DictionaryValue *in_dic_of_child;
                                        DictionaryValue *out_dic_of_child = new DictionaryValue;
                                        bool return_sub_value = false;

                                        // SHP does NOT support below case (connected list type, like below case)
                                        // {"key1":[["subkey1":"1234", "subkey2":"2345"],[...]]
                                        if (in_list_value->GetDictionary(index_cnt, &in_dic_of_child))
                                        {
                                            // retrieval child node.
                                            return_sub_value = RetrieveResponse(list_query_key, in_dic_of_child, out_dic_of_child);

                                            if (return_sub_value == true)
                                            {
                                                num_of_checked_list++;
                                                out_list_value->Append(out_dic_of_child);
                                            }
                                            else
                                            {
                                                delete out_dic_of_child;
                                            }
                                        }
                                        else
                                        {
                                            delete out_dic_of_child;
                                        }
                                    }

                                    if (num_of_checked_list > 0)
                                    {
                                        dic_root_response->Set(*dic_key_it, out_list_value);
                                        return_value = true;
                                    }
                                    else
                                    {
                                        delete out_list_value;
                                    }
                                }
                                else
                                {
                                    delete out_list_value;
                                }
                            } break;

                        case Value::TYPE_DICTIONARY:
                            {
                                // If node of key is NOT same with key for filtering and value type is dictionary,
                                // then retrieval child node.
                                DictionaryValue *in_dict_value = NULL;
                                DictionaryValue *out_dict_value = new DictionaryValue;
                                bool return_sub_value = false;

                                if (subval->GetAsDictionary(&in_dict_value))
                                {
                                    // retrieval child node.
                                    return_sub_value = RetrieveResponse(list_query_key, in_dict_value, out_dict_value);

                                    if (return_sub_value == true)
                                    {
                                        dic_root_response->Set(*dic_key_it, out_dict_value);
                                        return_value = true;
                                    }
                                    else
                                    {
                                        delete out_dict_value;
                                    }
                                }
                                else
                                {
                                    delete out_dict_value;
                                }
                            } break;

                        default:
                            {
                            } break;
                    }
                }
            }
        }
        return return_value;
    }

}

