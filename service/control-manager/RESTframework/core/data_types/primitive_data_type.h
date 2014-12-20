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

#ifndef WEBSERVICE_PRIMITIVE_DATA_TYPE_H_
#define WEBSERVICE_PRIMITIVE_DATA_TYPE_H_

#include "base/string_util.h"
#include "base/values.h"
#include "http_header/http_request_header.h"
#include "http_start_line/http_request_line.h"
#include "base/logging.h"

namespace webservice
{

    typedef enum
    {
        enUnknown = 0,
        enStringType,
        enCharPtrType,
        enBoolType,
        enSignedShortType,
        enUnsignedShortType,
        enSignedIntType,
        enUnsignedIntType,
        enSignedLongType,
        enUnsignedLongType,
        enFloatType,
        //
        enArrayOfStringType,
        enArrayOfBoolType,
        enArrayOfSignedShortType,
        enArrayOfUnsignedShortType,
        enArrayOfSignedIntType,
        enArrayOfUnsignedIntType,
        enArrayOfSignedLongType,
        enArrayOfUnsignedLongType,
        enArrayOfFloatType,
        //
        enDictionaryValueType,
        enTotalOfDataType
    } DefinedDataType;

    inline bool IsComparedWith(bool is_policy, const std::string &in_param1,
                               const std::string &in_param2)
    {

        if (is_policy)
        {
            if (StringToLowerASCII(in_param1) != StringToLowerASCII(in_param2)) return false;
            else return true;
        }
        else
        {
            if (in_param1 != in_param2) return false;
            else return true;
        }
    }

    class BaseAttributeType
    {
            typedef bool (*CreatorGetFunc)(void *, void *, const std::string &, const std::string &);
            typedef bool (*CreatorSetFunc)(void *, void *, const std::string &, const std::string &);

        public:
            virtual DefinedDataType GetSimpleDataType(void) = 0;
            virtual base::Value::Type GetSimpleDataValueType(void) = 0;
            void SetProperty(const int &min_occurs, const int &max_occurs, const bool &is_public)
            {
                this->min_occurs_ = min_occurs;
                this->max_occurs_ = max_occurs;
                this->is_public_ = is_public;
            }

            void SetFunctionPtr(CreatorGetFunc pGetFunc, CreatorSetFunc pSetFunc,
                                const std::string &service_name, const std::string &service_param)
            {
                this->pGetFunc_ = pGetFunc;
                this->get_service_name_ = "Get" + service_name;
                this->get_output_name_ = service_param;

                this->pSetFunc_ = pSetFunc;
                this->set_service_name_ = "Set" + service_name;
                this->set_input_name_ = service_param;
            }
            void SetFunctionPtr(CreatorGetFunc pGetFunc, CreatorSetFunc pSetFunc, const std::string &param1)
            {
                this->pGetFunc_ = pGetFunc;
                this->get_service_name_ = param1;
                this->get_output_name_ = "";

                this->pSetFunc_ = pSetFunc;
                this->set_service_name_ = param1;
                this->set_input_name_ = "";
            }
            void SetFunctionPtr(CreatorGetFunc pGetFunc, CreatorSetFunc pSetFunc)
            {
                this->pGetFunc_ = pGetFunc;
                this->get_service_name_ = "";
                this->get_output_name_ = "";

                this->pSetFunc_ = pSetFunc;
                this->set_service_name_ = "";
                this->set_input_name_ = "";
            }
            CreatorGetFunc pGetFunc_;
            CreatorSetFunc pSetFunc_;

            virtual void *GetDefault(void) = 0;
            bool Get(void *p_in, void *p_out)
            {
                if (NULL != this->pGetFunc_)
                {
                    return (*(this->pGetFunc_))(p_in, p_out, this->get_service_name_, this->get_output_name_);
                }
                return false;
            }
            bool Set(void *p_in, void *p_out)
            {
                if (NULL != this->pSetFunc_)
                {
                    return (*(this->pSetFunc_))(p_in, p_out, this->set_service_name_, this->set_input_name_);
                }
                return false;
            }
            const bool &IsPublicAttribute(void) const { return this->is_public_; }
            const bool IsMandatoryAttribute(void)
            {
                if (this->min_occurs_ >= 1) return true;
                else return false;
            }
            const bool &HasDefaultValue(void) { return this->has_default_value; }
            virtual bool CheckRestriction(void *in_data) = 0;

        protected:
            int min_occurs_;
            int max_occurs_;
            bool is_public_;
            bool has_default_value;
            std::string get_service_name_;
            std::string get_output_name_;
            std::string set_service_name_;
            std::string set_input_name_;
    };

    class _anyURI : public BaseAttributeType
    {
        public:
            _anyURI(void) { this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = false; }
            _anyURI(const std::string &data) : data_(data) { this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = true; }
            virtual ~_anyURI(void) {}
            virtual bool CheckRestriction(void *in_data) { return true; }
            DefinedDataType GetSimpleDataType(void) { return enStringType; }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_STRING; }
            static DefinedDataType GetDataTypeForArray(void) { return enArrayOfStringType; }
            void *GetDefault(void) { return reinterpret_cast<void *>(&(this->data_)); }

        private:
            std::string data_;
    };

    class _base64Binary : public BaseAttributeType
    {
        public:
            _base64Binary(void) { this->data_ = NULL; this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = false; }
            virtual ~_base64Binary(void)
            {
                if (this->data_ != NULL)
                {
                    delete this->data_;
                }
            }
            virtual bool CheckRestriction(void *in_data) { return true; }
            DefinedDataType GetSimpleDataType(void) { return enCharPtrType; }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_BINARY; }
            void *GetDefault(void) { return reinterpret_cast<void *>(&(this->data_)); }

        private:
            char *data_;
    };

    class _boolean : public BaseAttributeType
    {
        public:
            _boolean(void) { this->data_ = false; this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = false; }
            _boolean(const bool &data) : data_(data) { this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = true; }
            virtual ~_boolean(void) {}
            virtual bool CheckRestriction(void *in_data) { return true; }
            DefinedDataType GetSimpleDataType(void) { return enBoolType; }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_BOOLEAN; }
            static DefinedDataType GetDataTypeForArray(void) { return enArrayOfBoolType; }
            void *GetDefault(void) { return reinterpret_cast<void *>(&(this->data_)); }

        private:
            bool data_;
    };

    class _byte : public BaseAttributeType
    {
        public:
            _byte(void) { this->data_ = 0; this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = false; }
            _byte(const signed int &data) : data_(data) { this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = true; }
            virtual ~_byte(void) {}
            virtual bool CheckRestriction(void *in_data)
            {
                signed int *int_data = reinterpret_cast<signed int *>(in_data);
                if (*int_data >= -128 && *int_data <= 127) return true;
                else return false;
            }
            DefinedDataType GetSimpleDataType(void) { return enSignedIntType; }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_INTEGER; }
            static DefinedDataType GetDataTypeForArray(void) { return enArrayOfSignedIntType; }
            void *GetDefault(void) { return reinterpret_cast<void *>(&(this->data_)); }

        private:
            signed int data_;
    };

    class _dateTime : public BaseAttributeType
    {
        public:
            _dateTime(void) { this->data_ = ""; this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = false; }
            _dateTime(const std::string &data) : data_(data) { this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = true; }
            virtual ~_dateTime(void) {}
            virtual bool CheckRestriction(void *in_data)
            {
                std::string *str_data = reinterpret_cast<std::string *>(in_data);
                if (strlen("YYYY-MM-DDTHH:MM:SS") != str_data->size()) return false;
                return true;
            }
            DefinedDataType GetSimpleDataType(void) { return enStringType; }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_STRING; }
            static DefinedDataType GetDataTypeForArray(void) { return enArrayOfStringType; }
            void *GetDefault(void) { return reinterpret_cast<void *>(&(this->data_)); }
        private:
            std::string data_;
    };

    class _float : public BaseAttributeType
    {
        public:
            _float(void) { this->data_ = 0; this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = false; }
            _float(const float &data) : data_(data) { this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = true; }
            virtual ~_float(void) {}
            virtual bool CheckRestriction(void *in_data) { return true; }
            DefinedDataType GetSimpleDataType(void) { return enFloatType; }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_DOUBLE; }
            static DefinedDataType GetDataTypeForArray(void) { return enArrayOfFloatType; }
            void *GetDefault(void) { return reinterpret_cast<void *>(&(this->data_)); }

        private:
            float data_;
    };

    class _int : public BaseAttributeType
    {
        public:
            _int(void) { this->data_ = 0; this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = false; }
            _int(const signed int &data) : data_(data) { this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = true; }
            virtual ~_int(void) {}
            virtual bool CheckRestriction(void *in_data) { return true; }
            DefinedDataType GetSimpleDataType(void) { return enSignedIntType; }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_INTEGER; }
            static DefinedDataType GetDataTypeForArray(void) { return enArrayOfSignedIntType; }
            void *GetDefault(void) { return reinterpret_cast<void *>(&(this->data_)); }

        private:
            signed int data_;
    };

    class _long : public BaseAttributeType
    {
        public:
            _long(void) { this->data_ = 0; this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = false; }
            _long(const signed long &data) : data_(data) { this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = true; }
            virtual ~_long(void) {}
            virtual bool CheckRestriction(void *in_data) { return true; }
            DefinedDataType GetSimpleDataType(void) { return enSignedLongType; }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_INTEGER; }
            static DefinedDataType GetDataTypeForArray(void) { return enArrayOfSignedLongType; }
            void *GetDefault(void) { return reinterpret_cast<void *>(&(this->data_)); }

        private:
            signed long data_;
    };

    class _short : public BaseAttributeType
    {
        public:
            _short(void) { this->data_ = 0; this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = false; }
            _short(const signed short &data) : data_(data) { this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = true; }
            virtual ~_short(void) {}
            virtual bool CheckRestriction(void *in_data)
            {
                signed short *short_data = reinterpret_cast<signed short *>(in_data);
                if (*short_data >= -32768 && *short_data <= 32767) return true;
                else return false;
            }
            DefinedDataType GetSimpleDataType(void) { return enSignedShortType; }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_INTEGER; }
            static DefinedDataType GetDataTypeForArray(void) { return enArrayOfSignedShortType; }
            void *GetDefault(void) { return reinterpret_cast<void *>(&(this->data_)); }

        private:
            signed short data_;
    };


    class _string : public BaseAttributeType
    {
        public:
            _string(void) { this->data_ = ""; this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = false; }
            _string(const std::string &data) : data_(data) { this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = true; }
            virtual ~_string(void) {}
            DefinedDataType GetSimpleDataType(void) { return enStringType; }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_STRING; }
            static DefinedDataType GetDataTypeForArray(void) { return enArrayOfStringType; }
            void *GetDefault(void) { return reinterpret_cast<void *>(&(this->data_)); }
            virtual bool CheckRestriction(void *in_data)
            {
                if (v_restriction.size() > 0)
                {
                    std::vector<std::string>::iterator it;
                    for (it = this->v_restriction.begin(); it != this->v_restriction.end(); it++)
                    {
                        std::string *str_data = reinterpret_cast<std::string *>(in_data);
                        if (IsComparedWith(false, *str_data, *it))
                        {
                            *str_data = *it;
                            in_data = reinterpret_cast<void *>(str_data);
                            return true;
                        }
                    }
                    return false;
                }
                else
                {
                    return true;
                }
            }

        protected:
            std::string data_;
            std::vector<std::string> v_restriction;
    };

    class _time : public BaseAttributeType
    {
        public:
            _time(void) { this->data_ = ""; this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = false; }
            _time(const std::string &data) : data_(data) { this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = true; }
            virtual ~_time(void) {}
            virtual bool CheckRestriction(void *in_data)
            {
                std::string *str_data = reinterpret_cast<std::string *>(in_data);
                if (strlen("HH:MM:SS") != str_data->size()) return false;
                return true;
            }
            DefinedDataType GetSimpleDataType(void) { return enStringType; }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_STRING; }
            static DefinedDataType GetDataTypeForArray(void) { return enArrayOfStringType; }
            void *GetDefault(void) { return reinterpret_cast<void *>(&(this->data_)); }

        private:
            std::string data_;
    };

    class _unsignedByte : public BaseAttributeType
    {
        public:
            _unsignedByte(void) { this->data_ = 0; this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = false; }
            _unsignedByte(const unsigned int &data) : data_(data) { this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = true; }
            virtual ~_unsignedByte(void) {}
            bool CheckRestriction(void *in_data)
            {
                unsigned int *int_data = reinterpret_cast<unsigned int *>(in_data);
                if (*int_data <= 255) return true;
                else return false;
            }
            DefinedDataType GetSimpleDataType(void) { return enUnsignedIntType; }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_INTEGER; }
            static DefinedDataType GetDataTypeForArray(void) { return enArrayOfUnsignedIntType; }
            void *GetDefault(void) { return reinterpret_cast<void *>(&(this->data_)); }

        private:
            unsigned int data_;
    };


    class _unsignedInt : public BaseAttributeType
    {
        public:
            _unsignedInt(void) { this->data_ = 0; this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = false; }
            _unsignedInt(const unsigned int &data) : data_(data) { this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = true; }
            virtual ~_unsignedInt(void) {}
            virtual bool CheckRestriction(void *in_data) { return true; }
            DefinedDataType GetSimpleDataType(void) { return enUnsignedIntType; }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_INTEGER; }
            static DefinedDataType GetDataTypeForArray(void) { return enArrayOfUnsignedIntType; }
            void *GetDefault(void) { return reinterpret_cast<void *>(&(this->data_)); }

        private:
            unsigned int data_;
    };

    class _unsignedLong : public BaseAttributeType
    {
        public:
            _unsignedLong(void) { this->data_ = 0; this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = false; }
            _unsignedLong(const unsigned long &data) : data_(data) { this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = true; }
            virtual ~_unsignedLong(void) {}
            virtual bool CheckRestriction(void *in_data) { return true; }
            DefinedDataType GetSimpleDataType(void) { return enSignedLongType; }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_INTEGER; }
            static DefinedDataType GetDataTypeForArray(void) { return enArrayOfSignedLongType; }
            void *GetDefault(void) { return reinterpret_cast<void *>(&(this->data_)); }

        private:
            unsigned long data_;
    };

    class _unsignedShort : public BaseAttributeType
    {
        public:
            _unsignedShort(void) { this->data_ = 0; this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = false; }
            _unsignedShort(const unsigned short &data) : data_(data) { this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = true; }
            virtual ~_unsignedShort(void) {}
            bool CheckRestriction(void *in_data)
            {
                unsigned short *short_data = reinterpret_cast<unsigned short *>(in_data);
                if (*short_data <= 65535) return true;
                else return false;
            }
            DefinedDataType GetSimpleDataType(void) { return enUnsignedShortType; }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_INTEGER; }
            static DefinedDataType GetDataTypeForArray(void) { return enArrayOfUnsignedShortType; }
            void *GetDefault(void) { return reinterpret_cast<void *>(&(this->data_)); }

        private:
            unsigned short data_;
    };

    /**
     This type is for types which can not be made into required JSON format
     even though defined in shp.xsd
     Example: 'Events'

     NOTE: it's Get wrapper should return framed base::Value in expected JSON format
    **/
    class _dictionary_value : public BaseAttributeType
    {
        public:
            _dictionary_value(void) { this->data_ = NULL; this->pGetFunc_ = NULL; this->pSetFunc_ = NULL; this->has_default_value = false; }
            virtual ~_dictionary_value(void) {}
            bool CheckRestriction(void *in_data) { return true; }
            DefinedDataType GetSimpleDataType(void) { return enDictionaryValueType; }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_DICTIONARY; }
            static DefinedDataType GetDataTypeForArray(void) { return enUnknown; }
            void *GetDefault(void) { return reinterpret_cast<void *>(this->data_); }

        private:
            base::DictionaryValue *data_;
    };

}

#endif /* WEBSERVICE_PRIMITIVE_DATA_TYPE_H_ */


