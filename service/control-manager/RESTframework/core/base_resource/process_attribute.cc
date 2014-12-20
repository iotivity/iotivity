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
#include <list>

#include "base/values.h"
#include "base_resource/process_attribute.h"
#include "data_types/primitive_data_type.h"
#include "data_types/simple_array_data_type.h"
#include "data_types/simple_data_type.h"
#include "http_start_line/http_status_code_defined.h"
#include "base/logging.h"

namespace webservice
{

#define GET_ATTRIBUTE(type, type_name)  \
  DLOG(INFO) << "Run " << #type_name << " Type in Get method : " << name; \
  type value; \
  bool ret = attr->Get(reinterpret_cast<void*>(input), reinterpret_cast<void*>(&value));  \
  DLOG(INFO) << "value : " << value << ", ret : " << ret;  \
  if (false != ret) { \
    bool check_restriction = attr->CheckRestriction(reinterpret_cast<void*>(&value)); \
    if (check_restriction) response_dict->SetWithoutPathExpansion(name, base::Value::Create##type_name##Value(value));  \
    else {  \
      LOG(ERROR) << "Restriction value(" << name << ") : " << value; \
      en_response_type = RT_400_RESTRICTION_VALUE; \
    } \
  } else en_response_type = Get##type_name##DefalutAttribute(name, attr, response_dict);

#define GET_DEFAULT_ATTRIBUTE(type, type_name)  \
  type* default_value = NULL; \
  if (attr->HasDefaultValue()) default_value = reinterpret_cast<type*>(attr->GetDefault()); \
  if (NULL != default_value) {  \
    bool check_restriction = attr->CheckRestriction(reinterpret_cast<void*>(default_value));  \
    if (check_restriction) response_dict->SetWithoutPathExpansion(name, base::Value::Create##type_name##Value(*default_value)); \
    else {  \
      LOG(ERROR) << "Restriction value(" << name << ") : " << *default_value; \
      en_response_type = RT_400_RESTRICTION_VALUE; \
    } \
  } else {  \
    LOG(ERROR) << "Manatory Attribute : " << name << ", but Default Value is NULL";  \
    en_response_type = RT_404_MANDATORY_ATTRIBUTE;\
  }

#define GET_INTEGER_DEFAULT_ATTRIBUTE(type) \
  type* default_value = reinterpret_cast<type*>(attr->GetDefault());  \
  if (NULL != default_value) {  \
    value = static_cast<int>(*default_value); \
    is_default = true;  \
  }

#define PUT_ATTRIBUTE(type, type_name)  \
  DLOG(INFO) << "Run " << #type_name << " Type in PUT method : " << name; \
  type value; \
  if (request_dict->Get##type_name(name, &value)) { \
    void* in_data = reinterpret_cast<void*>(&value);  \
    bool ret = attr->Set(reinterpret_cast<void*>(input), in_data);  \
    if ((true != ret) && (NULL != attr->pSetFunc_)) en_response_type = RT_500_SERVICE_MANAGER; \
  } else { \
    /* Mandatory attribute should be set using PUT request */ \
    if (attr->IsMandatoryAttribute() && attr->IsPublicAttribute()) { \
      LOG(ERROR) << "mandatory attribute : " << name << " is not available in request body"; \
      /* return invalid body error */ \
      en_response_type = RT_400_INVALID_BODY; \
    } \
  }

#define PUT_PRE_CHECKING_RESTRICTION_ATTRIBUTE(type, type_name) \
  type value; \
  if (!request_dict->Get##type_name(input_key, &value)) { \
    LOG(ERROR) << "Request body data is not defined by SHP : " << input_key;  \
    return RT_400_INVALID_BODY; \
  } \
  in_data = reinterpret_cast<void*>(&value);  \
  if (!it->second->CheckRestriction(in_data)) { \
    LOG(ERROR) << "Request body data is not allowed. key : (" << input_key << ", " << value << ")"; \
    return RT_400_RESTRICTION_VALUE;  \
  }

#define POST_ATTRIBUTE(type, type_name) \
  DLOG(INFO) << "Run " << #type_name << " Type in POST method : " << name;  \
  type value; \
  if (request_dict->Get##type_name(name, &value)) input->dictionary->Set##type_name(name, value);

#define POST_RESPONSE_ATTRIBUTE(type, type_name)  \
  DLOG(INFO) << "Run " << #type_name << " Type in Post method : " << name; \
  type value; \
  bool ret = attr->Get(reinterpret_cast<void*>(input), reinterpret_cast<void*>(&value));  \
  if (false != ret) { \
    bool check_restriction = attr->CheckRestriction(reinterpret_cast<void*>(&value)); \
    if (check_restriction) response_dict->SetWithoutPathExpansion(name, base::Value::Create##type_name##Value(value));  \
    else {  \
      LOG(ERROR) << "Restriction value(" << name << ") : " << value; \
      en_response_type = RT_400_RESTRICTION_VALUE; \
    } \
  }

#define POST_PRE_CHECKING_RESTRICTION_ATTRIBUTE(type, type_name)  \
  type value; \
  if (!request_dict->Get##type_name(input_key, &value)) { \
    LOG(ERROR) << "Request body data is not defined by SHP : " << input_key; return RT_400_INVALID_BODY;  \
  } \
  in_data = reinterpret_cast<void*>(&value);  \
  if (!it->second->CheckRestriction(in_data)) {\
    LOG(ERROR) << "Request body data is not allowed. key : (" << input_key << ", " << value << ")"; \
    return RT_400_RESTRICTION_VALUE;  \
  }

    HTTPResponseType ProcessAttribute::GetAttribute(std::string name, BaseAttributeType *attr,
            WrapperInput *input, base::DictionaryValue *response_dict)
    {
        HTTPResponseType en_response_type = RT_200_OK;
        base::Value::Type enDefinedValueType = attr->GetSimpleDataValueType();

        DLOG(INFO) << "GetAttribute, name : " << name;

        switch (enDefinedValueType)
        {
            case Value::TYPE_BOOLEAN:
                {
                    GET_ATTRIBUTE(bool, Boolean)
                } break;

            case Value::TYPE_INTEGER:
                {
                    GET_ATTRIBUTE(int, Integer)
                } break;

            case Value::TYPE_DOUBLE:
                {
                    GET_ATTRIBUTE(double, Double)
                } break;

            case Value::TYPE_STRING:
                {
                    GET_ATTRIBUTE(std::string, String)
                } break;

            case Value::TYPE_LIST:
                {
                    en_response_type = GetListAttribute(name, attr, input, response_dict);
                } break;
            // This case is for data types which can not be represnted using any basic types
            // so it's Get wrapper return base::Dictionary in expected format
            case Value::TYPE_DICTIONARY:
                {
                    base::DictionaryValue *value = new base::DictionaryValue();
                    if (!attr->Get(reinterpret_cast<void *>(input), reinterpret_cast<void *>(value)))
                    {
                        en_response_type = RT_400_INVALID_BODY;
                    }
                    response_dict->MergeDictionary(value);
                } break;

            default:
                {
                    LOG(ERROR) << "Data Type(" << name << ") : " << enDefinedValueType;
                    en_response_type = RT_501_ATTRIBUTE_PARSER;
                } break;
        }
        return en_response_type;
    }

    HTTPResponseType ProcessAttribute::GetAttributeWithFunctionResponse(std::string name,
            BaseAttributeType *attr, base::DictionaryValue *function_response,
            base::DictionaryValue *response_dict)
    {
        HTTPResponseType en_response_type = RT_200_OK;
        base::Value::Type en_defined_value_type = attr->GetSimpleDataValueType();

        DLOG(ERROR) << "GetAttributeWithFunctionResponse, name : " << name;

        Value *subval = NULL;
        if (!function_response->GetWithoutPathExpansion(name, &subval)) return en_response_type;
        base::Value::Type en_function_response_value_type = subval->GetType();
        if (en_defined_value_type != en_function_response_value_type) return en_response_type;

        switch (en_defined_value_type)
        {
            case Value::TYPE_BOOLEAN:
                {
                    bool value;
                    if (subval->GetAsBoolean(&value))
                    {
                        response_dict->SetWithoutPathExpansion(name, base::Value::CreateBooleanValue(value));
                    }
                    else
                    {
                        en_response_type = GetBooleanDefalutAttribute(name, attr, response_dict);
                    }
                } break;

            case Value::TYPE_INTEGER:
                {
                    int value;
                    if (subval->GetAsInteger(&value))
                    {
                        response_dict->SetWithoutPathExpansion(name, base::Value::CreateIntegerValue(value));
                    }
                    else
                    {
                        en_response_type = GetIntegerDefalutAttribute(name, attr, response_dict);
                    }
                } break;

            case Value::TYPE_DOUBLE:
                {
                    double value;
                    if (subval->GetAsDouble(&value))
                    {
                        response_dict->SetWithoutPathExpansion(name, base::Value::CreateDoubleValue(value));
                    }
                    else
                    {
                        en_response_type = GetDoubleDefalutAttribute(name, attr, response_dict);
                    }
                } break;

            case Value::TYPE_STRING:
                {
                    std::string value;
                    if (subval->GetAsString(&value))
                    {
                        response_dict->SetWithoutPathExpansion(name, base::Value::CreateStringValue(value));
                    }
                    else
                    {
                        en_response_type = GetStringDefalutAttribute(name, attr, response_dict);
                    }
                } break;

            case Value::TYPE_LIST:
                {
                    base::ListValue *attr_list;
                    if (function_response->GetList(name, &attr_list))
                    {
                        base::ListValue *value = attr_list->DeepCopy();
                        response_dict->Set(name, value);
                    }
                } break;

            default:
                {
                    LOG(ERROR) << "Data Type(" << name << ") : " << en_defined_value_type;
                    en_response_type = RT_501_ATTRIBUTE_PARSER;
                } break;
        }
        return en_response_type;
    }

    HTTPResponseType ProcessAttribute::PutAttribute(std::string name, BaseAttributeType *attr,
            WrapperInput *input, base::DictionaryValue *request_dict)
    {
        HTTPResponseType en_response_type = RT_204_NO_CONTENT;
        base::Value::Type enDefinedValueType = attr->GetSimpleDataValueType();
        switch (enDefinedValueType)
        {
            case Value::TYPE_BOOLEAN:
                {
                    PUT_ATTRIBUTE(bool, Boolean)
                } break;
            case Value::TYPE_INTEGER:
                {
                    PUT_ATTRIBUTE(int, Integer)
                } break;
            case Value::TYPE_DOUBLE:
                {
                    PUT_ATTRIBUTE(double, Double)
                } break;
            case Value::TYPE_STRING:
                {
                    PUT_ATTRIBUTE(std::string, String)
                } break;
            case Value::TYPE_LIST:
                {
                    en_response_type = SetListAttribute(name, attr, input, request_dict);
                } break;
            default:
                {
                    if (request_dict->HasKey(name))
                    {
                        LOG(ERROR) << "Data Type(" << name << ") : " << enDefinedValueType;
                        en_response_type = RT_501_ATTRIBUTE_PARSER;
                    }
                } break;
        }
        return en_response_type;
    }

    HTTPResponseType ProcessAttribute::PutAttributeWithFunctionResponse(std::string name,
            BaseAttributeType *attr, base::DictionaryValue *function_response)
    {
        HTTPResponseType en_response_type = RT_204_NO_CONTENT;

        return en_response_type;
    }

    HTTPResponseType ProcessAttribute::PutPreChecking(std::string input_key,
            std::map<std::string, BaseAttributeType *> &elements, base::DictionaryValue *request_dict)
    {
        // 0. checking child node, if it is found, it should be skipped.
        base::Value *subval = NULL;
        request_dict->GetWithoutPathExpansion(input_key, &subval);
        if (NULL == subval)
        {
            LOG(ERROR) << "Request body data is incorrect. key : " << input_key;
            return RT_400_INVALID_BODY;
        }
        if (Value::TYPE_DICTIONARY == subval->GetType() || Value::TYPE_LIST == subval->GetType())
        {
            DLOG(INFO) << "Request body has child node : " << input_key;
            return RT_201_CREATE;
        }
        // 1. checking unnecessary key
        std::map<std::string, BaseAttributeType *>::iterator it;
        it = elements.find(input_key);
        if (elements.end() == it)
        {
            LOG(ERROR) << "Request body data is not defined by SHP : " << input_key;
            return RT_400_INVALID_BODY;
        }
        // 2. checking public key
        if (!it->second->IsPublicAttribute() && "id" != input_key)
        {
            LOG(ERROR) << "Protected Attribute : " << input_key;
            return RT_400_NOT_PUBLIC;
        }
        // 3. checking data type
        base::Value::Type enDefinedDataValueType = static_cast<base::Value::Type>
                (it->second->GetSimpleDataValueType());
        if (enDefinedDataValueType != subval->GetType())
        {
            LOG(ERROR) << "Request body data type is not same. key : " << input_key;
            DLOG(ERROR) << "Defined data type : " << enDefinedDataValueType << ", Data type : " <<
                        subval->GetType();
            if (Value::TYPE_DOUBLE != enDefinedDataValueType
                || Value::TYPE_INTEGER != subval->GetType()) return RT_400_INVALID_BODY;
        }
        // 4. restriction
        void *in_data = NULL;
        if (Value::TYPE_BOOLEAN == enDefinedDataValueType)
        {
            PUT_PRE_CHECKING_RESTRICTION_ATTRIBUTE(bool, Boolean)
        }
        else if (Value::TYPE_INTEGER == enDefinedDataValueType)
        {
            PUT_PRE_CHECKING_RESTRICTION_ATTRIBUTE(int, Integer)
        }
        else if (Value::TYPE_DOUBLE == enDefinedDataValueType)
        {
            PUT_PRE_CHECKING_RESTRICTION_ATTRIBUTE(double, Double)
        }
        else if (Value::TYPE_STRING == enDefinedDataValueType)
        {
            PUT_PRE_CHECKING_RESTRICTION_ATTRIBUTE(std::string, String)
        }
        else
        {
            LOG(ERROR) << "Data Type(" << input_key << ") : " << enDefinedDataValueType;
            return RT_501_ATTRIBUTE_PARSER;
        }
        return RT_201_CREATE;
    }

    HTTPResponseType ProcessAttribute::PostAttribute(std::string name, BaseAttributeType *attr,
            WrapperInput *input, base::DictionaryValue *request_dict)
    {
        HTTPResponseType en_response_type = RT_201_CREATE;
        base::Value::Type enDefinedValueType = attr->GetSimpleDataValueType();
        switch (enDefinedValueType)
        {
            case Value::TYPE_BOOLEAN:
                {
                    POST_ATTRIBUTE(bool, Boolean)
                } break;
            case Value::TYPE_INTEGER:
                {
                    POST_ATTRIBUTE(int, Integer)
                } break;
            case Value::TYPE_DOUBLE:
                {
                    POST_ATTRIBUTE(double, Double)
                } break;
            case Value::TYPE_STRING:
                {
                    POST_ATTRIBUTE(std::string, String)
                } break;
            case Value::TYPE_LIST:
                {
                    en_response_type = SetListAttribute(name, attr, input, request_dict);
                } break;
            default:
                {
                    if (request_dict->HasKey(name))
                    {
                        LOG(ERROR) << "Data Type(" << name << ") : " << enDefinedValueType;
                        en_response_type = RT_501_ATTRIBUTE_PARSER;
                    }
                } break;
        }
        return en_response_type;
    }

    HTTPResponseType ProcessAttribute::PostResponseAttribute(std::string name, BaseAttributeType *attr,
            WrapperInput *input, base::DictionaryValue *response_dict)
    {
        HTTPResponseType en_response_type = RT_200_OK;
        base::Value::Type enDefinedValueType = attr->GetSimpleDataValueType();
        switch (enDefinedValueType)
        {
            case Value::TYPE_BOOLEAN:
                {
                    POST_RESPONSE_ATTRIBUTE(bool, Boolean)
                } break;

            case Value::TYPE_INTEGER:
                {
                    POST_RESPONSE_ATTRIBUTE(int, Integer)
                } break;

            case Value::TYPE_DOUBLE:
                {
                    POST_RESPONSE_ATTRIBUTE(double, Double)
                } break;

            case Value::TYPE_STRING:
                {
                    POST_RESPONSE_ATTRIBUTE(std::string, String)
                } break;

            case Value::TYPE_LIST:
                {
                    en_response_type = PostResponseListAttribute(name, attr, input, response_dict);
                } break;
            // This case is for data types which can not be represnted using any basic types
            // so it's Get wrapper return base::Dictionary in expected format
            case Value::TYPE_DICTIONARY:
                {
                    base::DictionaryValue *value = new base::DictionaryValue();
                    if (!attr->Get(reinterpret_cast<void *>(input), reinterpret_cast<void *>(value)))
                    {
                        en_response_type = RT_400_INVALID_BODY;
                    }
                    response_dict->MergeDictionary(value);
                } break;

            default:
                {
                    LOG(ERROR) << "Data Type(" << name << ") : " << enDefinedValueType;
                    en_response_type = RT_501_ATTRIBUTE_PARSER;
                } break;
        }
        return en_response_type;
    }

    HTTPResponseType ProcessAttribute::PostResponseAttributeWithFunctionResponse(std::string name,
            BaseAttributeType *attr, base::DictionaryValue *function_response,
            base::DictionaryValue *response_dict)
    {
        HTTPResponseType en_response_type = RT_200_OK;
        base::Value::Type en_defined_value_type = attr->GetSimpleDataValueType();

        DLOG(ERROR) << "PostResponseAttributeWithFunctionResponse, name : " << name;

        Value *subval = NULL;
        if (!function_response->GetWithoutPathExpansion(name, &subval)) return en_response_type;
        base::Value::Type en_function_response_value_type = subval->GetType();
        if (en_defined_value_type != en_function_response_value_type) return en_response_type;

        switch (en_defined_value_type)
        {
            case Value::TYPE_BOOLEAN:
                {
                    bool value;
                    if (subval->GetAsBoolean(&value))
                    {
                        response_dict->SetWithoutPathExpansion(name, base::Value::CreateBooleanValue(value));
                    }
                    else
                    {
                        en_response_type = GetBooleanDefalutAttribute(name, attr, response_dict);
                    }
                } break;

            case Value::TYPE_INTEGER:
                {
                    int value;
                    if (subval->GetAsInteger(&value))
                    {
                        response_dict->SetWithoutPathExpansion(name, base::Value::CreateIntegerValue(value));
                    }
                    else
                    {
                        en_response_type = GetIntegerDefalutAttribute(name, attr, response_dict);
                    }
                } break;

            case Value::TYPE_DOUBLE:
                {
                    double value;
                    if (subval->GetAsDouble(&value))
                    {
                        response_dict->SetWithoutPathExpansion(name, base::Value::CreateDoubleValue(value));
                    }
                    else
                    {
                        en_response_type = GetDoubleDefalutAttribute(name, attr, response_dict);
                    }
                } break;

            case Value::TYPE_STRING:
                {
                    std::string value;
                    if (subval->GetAsString(&value))
                    {
                        response_dict->SetWithoutPathExpansion(name, base::Value::CreateStringValue(value));
                    }
                    else
                    {
                        en_response_type = GetStringDefalutAttribute(name, attr, response_dict);
                    }
                } break;

            default:
                {
                    LOG(ERROR) << "Data Type(" << name << ") : " << en_defined_value_type;
                    en_response_type = RT_501_ATTRIBUTE_PARSER;
                } break;
        }
        return en_response_type;
    }

    HTTPResponseType ProcessAttribute::PostPreChecking(std::string input_key,
            std::map<std::string, BaseAttributeType *> &elements, base::DictionaryValue *request_dict)
    {
        // 0. checking child node, if it is found, it should be skipped.
        base::Value *subval = NULL;
        request_dict->GetWithoutPathExpansion(input_key, &subval);
        if (NULL == subval)
        {
            LOG(ERROR) << "Request body data is incorrect. key : " << input_key;
            return RT_400_INVALID_BODY;
        }
        if (Value::TYPE_DICTIONARY == subval->GetType() || Value::TYPE_LIST == subval->GetType())
        {
            DLOG(INFO) << "Request body has child node : " << input_key;
            return RT_201_CREATE;
        }
        // 1. checking unnecessary key
        std::map<std::string, BaseAttributeType *>::iterator it;
        it = elements.find(input_key);
        if (elements.end() == it)
        {
            LOG(ERROR) << "Request body data is not defined by SHP : " << input_key;
            return RT_400_INVALID_BODY;
        }
        // 2. checking public key
        //if (!it->second->IsPublicAttribute()) {
        //  LOG(ERROR) << "Protected Attribute : " << input_key;
        //  return RT_400_NOT_PUBLIC;
        //}
        // 3. checking data type
        base::Value::Type enDefinedDataValueType = static_cast<base::Value::Type>
                (it->second->GetSimpleDataValueType());
        if (enDefinedDataValueType != subval->GetType())
        {
            LOG(ERROR) << "Request body data type is not same. key : " << input_key;
            if (Value::TYPE_DOUBLE != enDefinedDataValueType
                || Value::TYPE_INTEGER != subval->GetType()) return RT_400_INVALID_BODY;
        }
        // 4. restriction
        void *in_data = NULL;
        if (Value::TYPE_BOOLEAN == enDefinedDataValueType)
        {
            POST_PRE_CHECKING_RESTRICTION_ATTRIBUTE(bool, Boolean)
        }
        else if (Value::TYPE_INTEGER == enDefinedDataValueType)
        {
            POST_PRE_CHECKING_RESTRICTION_ATTRIBUTE(int, Integer)
        }
        else if (Value::TYPE_DOUBLE == enDefinedDataValueType)
        {
            POST_PRE_CHECKING_RESTRICTION_ATTRIBUTE(double, Double)
        }
        else if (Value::TYPE_STRING == enDefinedDataValueType)
        {
            POST_PRE_CHECKING_RESTRICTION_ATTRIBUTE(std::string, String)
        }
        else
        {
            LOG(ERROR) << "Data Type(" << input_key << ") : " << enDefinedDataValueType;
            return RT_501_ATTRIBUTE_PARSER;
        }
        return RT_201_CREATE;
    }

    HTTPResponseType ProcessAttribute::GetStringDefalutAttribute(std::string name,
            BaseAttributeType *attr, base::DictionaryValue *response_dict)
    {
        HTTPResponseType en_response_type = RT_200_OK;
        if (attr->IsMandatoryAttribute())
        {
            GET_DEFAULT_ATTRIBUTE(std::string, String)
        }
        return en_response_type;
    }

    HTTPResponseType ProcessAttribute::GetBooleanDefalutAttribute(std::string name,
            BaseAttributeType *attr, base::DictionaryValue *response_dict)
    {
        HTTPResponseType en_response_type = RT_200_OK;
        if (attr->IsMandatoryAttribute())
        {
            GET_DEFAULT_ATTRIBUTE(bool, Boolean)
        }
        return en_response_type;
    }

    HTTPResponseType ProcessAttribute::GetIntegerDefalutAttribute(std::string name,
            BaseAttributeType *attr, base::DictionaryValue *response_dict)
    {
        HTTPResponseType en_response_type = RT_200_OK;
        if (attr->IsMandatoryAttribute())
        {
            int value;
            bool is_default = false;
            if (attr->HasDefaultValue())
            {
                int enDefinedDataType = static_cast<int>(attr->GetSimpleDataType());
                if (enSignedShortType == enDefinedDataType)
                {
                    GET_INTEGER_DEFAULT_ATTRIBUTE(signed short)
                }
                else if (enUnsignedShortType == enDefinedDataType)
                {
                    GET_INTEGER_DEFAULT_ATTRIBUTE(unsigned short)
                }
                else if (enSignedIntType == enDefinedDataType)
                {
                    GET_INTEGER_DEFAULT_ATTRIBUTE(signed int)
                }
                else if (enUnsignedIntType == enDefinedDataType)
                {
                    GET_INTEGER_DEFAULT_ATTRIBUTE(unsigned int)
                }
                else if (enSignedLongType == enDefinedDataType)
                {
                    GET_INTEGER_DEFAULT_ATTRIBUTE(signed long)
                }
                else if (enUnsignedLongType == enDefinedDataType)
                {
                    GET_INTEGER_DEFAULT_ATTRIBUTE(unsigned long)
                }
            }
            if (is_default)
            {
                response_dict->SetWithoutPathExpansion(name, base::Value::CreateIntegerValue(value));
            }
            else
            {
                DLOG(ERROR) << "Manatory Attribute : " << name << ", but Default Value is NULL";
                en_response_type = RT_404_MANDATORY_ATTRIBUTE;
            }
        }
        return en_response_type;
    }

    HTTPResponseType ProcessAttribute::GetDoubleDefalutAttribute(std::string name,
            BaseAttributeType *attr, base::DictionaryValue *response_dict)
    {
        HTTPResponseType en_response_type = RT_200_OK;
        if (attr->IsMandatoryAttribute())
        {
            GET_DEFAULT_ATTRIBUTE(double, Double)
        }
        return en_response_type;
    }

    HTTPResponseType ProcessAttribute::GetListAttribute(std::string name, BaseAttributeType *attr,
            WrapperInput *input, base::DictionaryValue *response_dict)
    {
        HTTPResponseType en_response_type = RT_200_OK;
        DLOG(ERROR) << "Run Array of String Type in Get method for Resource : " << name;
        int enDefinedDataType = static_cast<int>(attr->GetSimpleDataType());
        if (enArrayOfStringType != enDefinedDataType)
        {
            LOG(ERROR) << "Data Type(" << name << ") : " << enDefinedDataType;
            en_response_type = RT_501_ATTRIBUTE_PARSER;
        }
        else
        {
            std::list<std::string> value;
            std::list<std::string>::iterator it;
            bool ret = attr->Get(reinterpret_cast<void *>(input), reinterpret_cast<void *>(&value));

            ListValue *list_value = NULL;
            if (attr->IsMandatoryAttribute() || (value.size() > 0 && false != ret))
            {
                list_value = new ListValue;
                response_dict->Set(name, list_value);
            }

            if (NULL != list_value)
            {
                for (it = value.begin(); it != value.end(); it++)
                {
                    list_value->Append(Value::CreateStringValue(*it));
                }
            }
        }
        return en_response_type;
    }

    HTTPResponseType ProcessAttribute::PostResponseListAttribute(std::string name,
            BaseAttributeType *attr, WrapperInput *input, base::DictionaryValue *response_dict)
    {
        HTTPResponseType en_response_type = RT_200_OK;
        DLOG(ERROR) << "Run Array of String Type in Get method for Resource : " << name;
        int enDefinedDataType = static_cast<int>(attr->GetSimpleDataType());
        if (enArrayOfStringType != enDefinedDataType)
        {
            LOG(ERROR) << "Data Type(" << name << ") : " << enDefinedDataType;
            en_response_type = RT_501_ATTRIBUTE_PARSER;
        }
        else
        {
            std::list<std::string> value;
            std::list<std::string>::iterator it;
            bool ret = attr->Get(reinterpret_cast<void *>(input), reinterpret_cast<void *>(&value));

            ListValue *list_value = NULL;
            if (value.size() > 0 && false != ret)
            {
                list_value = new ListValue;
                response_dict->Set(name, list_value);
            }

            if (NULL != list_value)
            {
                for (it = value.begin(); it != value.end(); it++)
                {
                    list_value->Append(Value::CreateStringValue(*it));
                }
            }
        }
        return en_response_type;
    }

    HTTPResponseType ProcessAttribute::SetListAttribute(std::string name, BaseAttributeType *attr,
            WrapperInput *input, base::DictionaryValue *request_dict)
    {
        DLOG(INFO) << "Run " << "List Type in PUT method : " << name;
        base::ListValue *list = NULL;

        // if list retreival fails
        if (!request_dict->Get(name, reinterpret_cast<base::Value **>(&list)) || !list || !list->GetSize())
        {
            /* Mandatory attribute should be set using PUT request */
            if (attr->IsMandatoryAttribute() && attr->IsPublicAttribute())
            {
                LOG(ERROR) << "Could not get Data Type(" << name << ")  dictionary value ";
                return RT_400_INVALID_BODY;
            }
            else
            {
                // non mandatory attribute so need not return error
                return RT_201_CREATE;
            }
        }

        size_t list_size = list->GetSize();
        // Find what kind of list
        DefinedDataType list_type = attr->GetSimpleDataType();
        DLOG(INFO) << name << " : is array of " << list_type;
        switch (list_type)
        {
            case enArrayOfStringType:
                {
                    std::vector<std::string> value;
                    while (list_size)
                    {
                        std::string str_value;
                        if (!list->GetString(--list_size, &str_value))
                        {
                            LOG(ERROR) << "Could not get Data Type(" << name << ")  from request dictionary's list value ";
                            return RT_400_INVALID_BODY;
                        }
                        value.push_back(str_value);
                    }
                    if (!attr->Set(reinterpret_cast<void *>(input), reinterpret_cast<void *>(&value)))
                    {
                        LOG(ERROR) << "Could not set value : " << name;
                        return RT_400_INVALID_BODY;
                    }
                } break;
            case enArrayOfBoolType:
                {
                    std::vector<bool> value;
                    while (list_size)
                    {
                        bool bool_value;
                        if (!list->GetBoolean(--list_size, &bool_value))
                        {
                            LOG(ERROR) << "Could not get Data Type(" << name << ")  from request dictionary's list value ";
                            return RT_400_INVALID_BODY;
                        }
                        value.push_back(bool_value);
                    }
                    if (!attr->Set(reinterpret_cast<void *>(input), reinterpret_cast<void *>(&value)))
                    {
                        LOG(ERROR) << "Could not set value : " << name;
                        return RT_400_INVALID_BODY;
                    }
                } break;
            // base::Value supports only 'Integer' type for numbers
            // so using 'GetInteger' for both Signed and unsigned integer
            // values irrespective of their size
            case enArrayOfSignedShortType:
            case enArrayOfSignedIntType:
            case enArrayOfSignedLongType:
                {
                    std::vector<int> value;
                    while (list_size)
                    {
                        int int_value;
                        if (!list->GetInteger(--list_size, &int_value))
                        {
                            LOG(ERROR) << "Could not get Data Type(" << name << ")  from request dictionary's list value ";
                            return RT_400_INVALID_BODY;
                        }
                        value.push_back(int_value);
                    }
                    if (!attr->Set(reinterpret_cast<void *>(input), reinterpret_cast<void *>(&value)))
                    {
                        LOG(ERROR) << "Could not set value : " << name;
                        return RT_400_INVALID_BODY;
                    }
                } break;
            case enArrayOfUnsignedIntType:
            case enArrayOfUnsignedShortType:
            case enArrayOfUnsignedLongType:
                {
                    std::vector<unsigned int> value;
                    while (list_size)
                    {
                        unsigned int int_value;
                        if (!list->GetInteger(--list_size, reinterpret_cast<int *>(&int_value)))
                        {
                            LOG(ERROR) << "Could not get Data Type(" << name << ")  from request dictionary's list value ";
                            return RT_400_INVALID_BODY;
                        }
                        value.push_back(int_value);
                    }
                    if (!attr->Set(reinterpret_cast<void *>(input), reinterpret_cast<void *>(&value)))
                    {
                        LOG(ERROR) << "Could not set value : " << name;
                        return RT_400_INVALID_BODY;
                    }
                } break;
            case enArrayOfFloatType:
                {
                    std::vector<double> value;
                    while (list_size)
                    {
                        double double_value;
                        if (!list->GetDouble(--list_size, &double_value))
                        {
                            LOG(ERROR) << "Could not get Data Type(" << name << ")  from request dictionary's list value ";
                            return RT_400_INVALID_BODY;
                        }
                        value.push_back(double_value);
                    }
                    if (!attr->Set(reinterpret_cast<void *>(input), reinterpret_cast<void *>(&value)))
                    {
                        LOG(ERROR) << "Could not set value : " << name;
                        return RT_400_INVALID_BODY;
                    }
                } break;
            default:
                {
                    if (request_dict->HasKey(name))
                    {
                        LOG(ERROR) << "Data Type(" << name << ") : " << list_type;
                        return RT_501_ATTRIBUTE_PARSER;
                    }
                } break;
        }
        return RT_201_CREATE;
    }

}

