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




#ifndef CMSERIALIZABLEDATAFACTORY_H_
#define CMSERIALIZABLEDATAFACTORY_H_

#include <jni.h>
#include "JNIGlobal.h"
#include "Serializable.h"
#include "Serialization/SerializableDataFactory.h"
#include "iostream"
//#include "Serialization/SubscriptionSerializationFactory.h"

using namespace std ;

class CMSerializableDataFactory : public OC::Cm::Serialization::SerializableDataFactory
{

    public:

        CMSerializableDataFactory(jobject dataFactoryObj);

        virtual ~CMSerializableDataFactory();

        virtual bool createRequestSerializable( int resourceType, std::string &methodId,
                                                OC::Cm::Serialization::ISerializable **pp_output ) ;

        virtual bool createResponseSerializable( int resourceType, std::string &methodId,
                OC::Cm::Serialization::ISerializable **pp_output ) ;

        virtual bool createSerializable( std::string &elementType,
                                         OC::Cm::Serialization::ISerializable **pp_output ) ;

        virtual bool createMethodSerializables( int resourceType, std::string &methodId,
                                                OC::Cm::Serialization::ISerializable **pp_request,
                                                OC::Cm::Serialization::ISerializable **pp_response ) ;


        jobject m_dataFactoryObj;

        //SubscriptionSerializationFactory subscriptionSerializationFactory ;
};

#endif /* CMSERIALIZABLEDATAFACTORY_H_ */
