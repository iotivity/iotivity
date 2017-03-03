//******************************************************************
//
// Copyright 2017 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//


#ifndef __JSONHELPER_H__
#define __JSONHELPER_H__

#include "rapidjson.h"
#include "document.h"
#include "stringbuffer.h"
#include "writer.h"
#include <string>

#include <typeinfo>

class JsonHelper
{
    public:

        static std::string toString(rapidjson::Value &doc)
        {
            rapidjson::StringBuffer sb;
            std::string jsonRep;

            rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
            doc.Accept(writer);
            return sb.GetString();
        }

        static std::string toString(rapidjson::Value::ConstMemberIterator &it)
        {
            rapidjson::StringBuffer sb;
            std::string jsonRep;

            rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
            it->value.Accept(writer);
            return sb.GetString();
        }

        template<typename T>
        static void setMember(rapidjson::Document &doc, std::string obj,
                              std::string name, T &value)
        {
            if (doc.HasMember(obj.c_str()))
            {
                rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();
                doc[obj.c_str()].RemoveMember(name.c_str());
                rapidjson::Value nameValue(name.c_str(), allocator);
                doc[obj.c_str()].AddMember(nameValue, value, allocator);
            }
        }

        template<typename T>
        static void setMember(rapidjson::Document &doc, const std::string &name, T &value)
        {
            if (doc.HasMember(name.c_str()))
            {
                doc[name.c_str()] = value;
            }
            else
            {
                rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();
                rapidjson::Value nameValue(name.c_str(), allocator);
                doc.AddMember(nameValue, value, allocator);
            }

        }

        template<typename T>
        static bool getMember(rapidjson::Document &doc, std::string name, T &value)
        {
            if (doc.HasMember(name.c_str()))
            {
                if (typeid(T) == typeid(std::string))
                {
                    value = doc[name.c_str()].GetString();
                }
                return true;
            }
            return false;
        }

        static void tryRemoveMember(rapidjson::Document &doc, std::string name)
        {
            if (doc.HasMember(name.c_str()))
            {
                doc.RemoveMember(name.c_str());
            }
        }
};

#endif /* __JSON_HELPER_H__ */
