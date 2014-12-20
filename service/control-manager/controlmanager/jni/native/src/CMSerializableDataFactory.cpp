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




#include "CMSerializableDataFactory.h"


using namespace std ;

CMSerializableDataFactory::CMSerializableDataFactory(jobject dataFactoryObj)
{
    m_dataFactoryObj = JNIGlobal::getGlobleRef(dataFactoryObj) ;
}

CMSerializableDataFactory::~CMSerializableDataFactory()
{
    JNIGlobal::executeVoidMethod("OC/Cm/Serialization/SerializableDataFactory", "onNativeDelete", "()V",
                                 m_dataFactoryObj);
    JNIGlobal::releaseGlobleRef(m_dataFactoryObj) ;
}

bool
CMSerializableDataFactory::createRequestSerializable( int resourceType, std::string &methodId,
        OC::Cm::Serialization::ISerializable **pp_output )
{
    jstring jstrMethodId = JNIGlobal::convertToJString(methodId) ;

//  jlong lSerializable = JNIGlobal::executeLongMethod("OC/Cm/Serialization/SerializableDataFactory", "getRequestSerializable", "(ILjava/lang/String;)J", m_dataFactoryObj, resourceType, jstrMethodId);
//  *pp_output = (OC::Cm::Serialization::ISerializable*)lSerializable ;

    jobject pSerializable =
        JNIGlobal::executeObjectMethod("OC/Cm/Serialization/SerializableDataFactory",
                                       "createRequestSerializable", "(ILjava/lang/String;)LOC/Cm/Serialization/ISerializable;",
                                       m_dataFactoryObj, resourceType, jstrMethodId);

    if (NULL != pSerializable)
    {
        jlong lSerializable = JNIGlobal::executeLongMethod("OC/Cm/Serialization/ISerializable",
                              "getNativeHandle", "()J", pSerializable) ;

        *pp_output = (OC::Cm::Serialization::ISerializable *)lSerializable ;

#ifdef __ANDROID__
        JNIGlobal::releaseLocalRef(pSerializable);
#endif

    }
    else
    {
        *pp_output = NULL ;
    }

    JNIGlobal::releaseLocalRef(jstrMethodId) ;
    return true ;
}

bool
CMSerializableDataFactory::createResponseSerializable( int resourceType, std::string &methodId,
        OC::Cm::Serialization::ISerializable **pp_output )
{
    jstring jstrMethodId = JNIGlobal::convertToJString(methodId) ;

//  jlong lSerializable = JNIGlobal::executeLongMethod("OC/Cm/Serialization/SerializableDataFactory", "getResponseSerializable", "(ILjava/lang/String;)J", m_dataFactoryObj, resourceType, jstrMethodId);
//  *pp_output = (OC::Cm::Serialization::ISerializable*)lSerializable ;

    jobject pSerializable =
        JNIGlobal::executeObjectMethod( "OC/Cm/Serialization/SerializableDataFactory",
                                        "createResponseSerializable", "(ILjava/lang/String;)LOC/Cm/Serialization/ISerializable;",
                                        m_dataFactoryObj, resourceType, jstrMethodId );
    if (NULL != pSerializable)
    {
        jlong lSerializable = JNIGlobal::executeLongMethod( "OC/Cm/Serialization/ISerializable",
                              "getNativeHandle", "()J", pSerializable ) ;
        *pp_output = (OC::Cm::Serialization::ISerializable *)lSerializable ;

#ifdef __ANDROID__
        JNIGlobal::releaseLocalRef(pSerializable);
#endif

    }
    else
    {
        *pp_output = NULL ;
    }

    JNIGlobal::releaseLocalRef(jstrMethodId) ;
    return true ;
}


bool
CMSerializableDataFactory::createMethodSerializables( int resourceType, std::string &methodId,
        OC::Cm::Serialization::ISerializable **pp_request,
        OC::Cm::Serialization::ISerializable **pp_response )
{
    bool bRet = false ;

    if (false == bRet)
    {
        bRet = this->createRequestSerializable(resourceType, methodId, pp_request) ;

        if (bRet == true)
        {
            bRet = this->createResponseSerializable(resourceType, methodId, pp_response) ;
        }
    }

    return bRet ;
}

bool
CMSerializableDataFactory::createSerializable( std::string &elementType,
        OC::Cm::Serialization::ISerializable **pp_output )
{
    jstring jstrElementType = JNIGlobal::convertToJString( elementType ) ;

//  jlong lSerializable = JNIGlobal::executeLongMethod( "OC/Cm/Serialization/SerializableDataFactory", "getSerializable", "(Ljava/lang/String;)J",m_dataFactoryObj, jstrElementType);
//  *pp_output = (OC::Cm::Serialization::ISerializable*)lSerializable ;

    jobject pSerializable =
        JNIGlobal::executeObjectMethod("OC/Cm/Serialization/SerializableDataFactory", "createSerializable",
                                       "(Ljava/lang/String;)LOC/Cm/Serialization/ISerializable;", m_dataFactoryObj, jstrElementType);
    if ( NULL != pSerializable )
    {
        jlong lSerializable = JNIGlobal::executeLongMethod( "OC/Cm/Serialization/ISerializable",
                              "getNativeHandle", "()J", pSerializable ) ;
        *pp_output = (OC::Cm::Serialization::ISerializable *)lSerializable ;

#ifdef __ANDROID__
        JNIGlobal::releaseLocalRef(pSerializable);
#endif

    }
    else
    {
        *pp_output = NULL ;
    }

    JNIGlobal::releaseLocalRef(jstrElementType) ;
    return true;
}
