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

#ifndef WEBSERVICE_SIMPLE_ARRAY_DATA_TYPE_H_
#define WEBSERVICE_SIMPLE_ARRAY_DATA_TYPE_H_

#include "data_types/primitive_data_type.h"
#include "data_types/simple_data_type.h"

namespace webservice
{

// Following is default generated code.
    class ArrayOfUuid : public BaseAttributeType
    {
        public:
            DefinedDataType GetSimpleDataType(void) { return UuidType::GetDataTypeForArray(); }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_LIST; }
            void *GetDefault(void) { return NULL; }
            virtual bool CheckRestriction(void *in_data) { return true; }
    };

    class ArrayOfDayType : public BaseAttributeType
    {
        public:
            DefinedDataType GetSimpleDataType(void) { return DayType::GetDataTypeForArray(); }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_LIST; }
            virtual void *GetDefault(void) { return NULL; }
            virtual bool CheckRestriction(void *in_data) { return true; }
    };


    class ArrayOfResourceType : public BaseAttributeType
    {
        public:
            DefinedDataType GetSimpleDataType(void) { return ResourceType::GetDataTypeForArray(); }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_LIST; }
            virtual void *GetDefault(void) { return NULL; }
            virtual bool CheckRestriction(void *in_data) { return true; }
    };

    class ArrayOfString : public BaseAttributeType
    {
        public:
            DefinedDataType GetSimpleDataType(void) { return _string::GetDataTypeForArray(); }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_LIST; }
            virtual void *GetDefault(void) { return NULL; }
            virtual bool CheckRestriction(void *in_data) { return true; }
    };

    class ArrayOfDispenserShapeType : public BaseAttributeType
    {
        public:
            DefinedDataType GetSimpleDataType(void) { return DispenserShapeType::GetDataTypeForArray(); }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_LIST; }
            virtual void *GetDefault(void) { return NULL; }
            virtual bool CheckRestriction(void *in_data) { return true; }
    };

    class ArrayOfConnectivityType : public BaseAttributeType
    {
        public:
            DefinedDataType GetSimpleDataType(void) { return ConnectivityType::GetDataTypeForArray(); }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_LIST; }
            virtual void *GetDefault(void) { return NULL; }
            virtual bool CheckRestriction(void *in_data) { return true; }
    };

    class ArrayOfAnyURI : public BaseAttributeType
    {
        public:
            DefinedDataType GetSimpleDataType(void) { return _anyURI::GetDataTypeForArray(); }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_LIST; }
            virtual void *GetDefault(void) { return NULL; }
            virtual bool CheckRestriction(void *in_data) { return true; }
    };

    class ArrayOfTimeType : public BaseAttributeType
    {
        public:
            DefinedDataType GetSimpleDataType(void) { return TimeType::GetDataTypeForArray(); }
            base::Value::Type GetSimpleDataValueType(void) { return base::Value::TYPE_LIST; }
            virtual void *GetDefault(void) { return NULL; }
            virtual bool CheckRestriction(void *in_data) { return true; }
    };

}

#endif /* WEBSERVICE_SIMPLE_ARRAY_DATA_TYPE_H_ */

