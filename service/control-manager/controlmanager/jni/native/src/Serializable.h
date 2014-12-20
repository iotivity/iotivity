//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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




#ifndef SERIALIZABLE_H_
#define SERIALIZABLE_H_

#include <jni.h>
#include "JNIGlobal.h"
#include "CMSerialization.h"

using namespace std ;

class Serializable : public OC::Cm::Serialization::ISerializable
{

    public:

        Serializable(jobject handler);

        virtual ~Serializable();

        std::string getElementName();

        bool serializeContents(OC::Cm::Serialization::ISerializer  &serializer);

        bool deSerializeContents(OC::Cm::Serialization::IDeserializer  &deserializer);

//  bool serialize(OC::Cm::Serialization::ISerializer&  serializer) ;

//  bool deSerialize(OC::Cm::Serialization::IDeserializer&  deserializer) ;


        std::string m_schema_instance_type ;

        OC::Cm::Serialization::ISerializable *m_pSchema_instance_data ;

        jobject ser_handler ;
};

#endif /* SERIALIZABLE_H_ */
