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




#include "Serializable.h"
#include "CMSerialization.h"
#include "OC_Cm_Serialization_ISerializer.h"

#include "Serialization/Json/JsonDeserializer.h"
#include "CMDataTypes.h"
#include "OC_Cm_Serialization_CMJsonValidator.h"
#include "Thread/Thread.h"

using namespace std ;

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_CMJsonValidator_validatePayload
(JNIEnv *pEnv, jclass classObj, jlong context, jstring jsonData, jlong serializable,
 jstring jmethod, jstring jFilter, jboolean jRecursive)
{
    Serializable *pObj = (Serializable *) serializable;
    if (NULL == pObj)
    {
        JNIGlobal::throwException("Invalid Serializable data");
        return JNI_FALSE;
    }

    std::string payload ;
    if (false == JNIGlobal::convertToStdString(jsonData, payload))
    {
        JNIGlobal::throwException("Failed to convert std string");
        return JNI_FALSE;
    }

    std::string filter ;
    if (false == JNIGlobal::convertToStdString(jFilter, filter))
    {
        JNIGlobal::throwException("Failed to convert std string");
        return JNI_FALSE;
    }

    std::string method ;
    if (false == JNIGlobal::convertToStdString(jmethod, method))
    {
        JNIGlobal::throwException("Failed to convert std string");
        return JNI_FALSE;
    }

    OC::Cm::Serialization::IDeserializer *p_deserializer = new
    OC::Cm::Core::Serialization::Json::JsonDeserializer((OC::Cm::IContext *)context);
    if (p_deserializer == NULL)
    {
        return JNI_FALSE;
    }

    if (filter.size() > 0)
    {
        p_deserializer->setFilter(filter);
    }

    if (JNI_FALSE == jRecursive)
    {
        p_deserializer->setRecursive(false);
    }
    else
    {
        p_deserializer->setRecursive(true);
    }

    if (0 == shp_strcasecmp(method.c_str(), "PUT"))
    {
        p_deserializer->setValidateMandatory(false) ;
    }
    else if (0 == shp_strcasecmp(method.c_str(), "POST"))
    {
        p_deserializer->setIDExcemption(true) ;
    }

    if (false == p_deserializer->parse(payload, payload.size()))
    {
        JNIGlobal::throwException("Failed to parse json: invalid json content") ;
        delete p_deserializer;
        return JNI_FALSE;
    }

    if (false == pObj->deSerialize( *p_deserializer ))
    {
        JNIGlobal::throwException(p_deserializer->getErrorMessage().c_str()) ;
        delete p_deserializer;
        return JNI_FALSE;
    }

    delete p_deserializer ;
    return JNI_TRUE ;
}

/*
JNIEXPORT jlong JNICALL Java_OC_Cm_Serialization_ISerializable_constructNative
 (JNIEnv *pEnv, jobject obj)
{
    Serializable *pObj = new Serializable(obj) ;

    return (jlong)(pObj);
}


JNIEXPORT void JNICALL Java_OC_Cm_Serialization_ISerializable_deleteNative
 (JNIEnv *pEnv, jobject curent, jlong obj)
{
    Serializable *pObj = (Serializable *) obj;
    if (NULL != pObj)
    {
        delete(pObj) ;
    }
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_ISerializable_serialize
  (JNIEnv *pEnv, jobject curent, jlong obj, jlong nativeHandle)
{
    //jlong nativeHandle = JNIGlobal::executeLongMethod("OC/Cm/Serialization/ISerializer","getNativeHandle","()J", serializer);

    OC::Cm::Serialization::ISerializer* pSerial = (OC::Cm::Serialization::ISerializer*)(nativeHandle) ;

    OC::Cm::Serialization::ISerializable* pSerializable = (OC::Cm::Serialization::ISerializable*)obj ;

    jboolean serializable = (jboolean)pSerializable->serialize(*pSerial) ;

    return serializable ;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_ISerializable_deSerialize
  (JNIEnv *pEnv, jobject curent, jlong obj, jlong nativeHandle)
{
//  jlong nativeHandle = JNIGlobal::executeLongMethod("OC/Cm/Serialization/IDeserializer","getNativeHandle","()J", deserializer);

    OC::Cm::Serialization::IDeserializer* pdSerial = (OC::Cm::Serialization::IDeserializer*)(nativeHandle) ;

    OC::Cm::Serialization::ISerializable* pSerializable = (OC::Cm::Serialization::ISerializable*)obj ;

    jboolean deser = (jboolean)pSerializable->deSerialize(*pdSerial) ;

    return deser;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_ISerializable_validateContent
  (JNIEnv *pEnv, jobject thisObject, jlong nativeHandle)
{
    Serializable *pObj = ( Serializable * ) nativeHandle ;
    if (NULL == pObj)
    {
        return JNI_FALSE ;
    }

    OC::Cm::CMError error = pObj->validateContent();
    if (error.getErrorCode() != OC::Cm::CM_SUCCESS)
    {
        JNIGlobal::throwException(error.getErrorMessage().c_str()) ;
    }

    return JNI_TRUE ;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_ISerializable_setInstanceName
  (JNIEnv *pEnv, jobject thisObject, jlong nativeHandle, jstring elementName)
{
    Serializable *pObj = ( Serializable * ) nativeHandle ;
    if (NULL == pObj)
    {
        return JNI_FALSE ;
    }

    std::string eName ;
    if ( false == JNIGlobal::convertToStdString( elementName, eName ) )
    {
        return JNI_FALSE ;
    }

    pObj->setInstanceName(eName) ;
    return JNI_TRUE ;
}


JNIEXPORT void JNICALL Java_OC_Cm_Serialization_ISerializable_setList
  (JNIEnv *pEnv, jobject thisObject, jlong nativeHandle)
{
    Serializable *pObj = ( Serializable * ) nativeHandle ;
    if (NULL == pObj)
    {
        return ;
    }

    pObj->setList(true);
}

*/

Serializable::Serializable(jobject handler)
{
    ser_handler = JNIGlobal::getGlobleRef(handler) ;
    m_pSchema_instance_data = NULL ;
}

Serializable::~Serializable()
{
    JNIGlobal::executeVoidMethod("OC/Cm/Serialization/ISerializable", "onNativeDelete", "()V",
                                 ser_handler);
    JNIGlobal::releaseGlobleRef(ser_handler) ;
}

std::string
Serializable::getElementName()
{
    std::string retValue ;
    jstring elementName = (jstring) JNIGlobal::executeObjectMethod("OC/Cm/Serialization/ISerializable",
                          "getElementName", "()Ljava/lang/String;", ser_handler);

    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(elementName, retValue)), "",
                   "string conversion failed") ;

    JNIGlobal::releaseLocalRef(elementName);

    return retValue ;
}

bool
Serializable::serializeContents(OC::Cm::Serialization::ISerializer  &serializer)
{
    jobject     jserializer = NULL  ;

    jserializer = JNIGlobal::constructObject("OC/Cm/Serialization/ISerializer", "(J)V",
                  (jlong)(&serializer)) ;

    if (NULL == jserializer)
    {
        return false ;
    }

    bool bRet = JNIGlobal::executeBoolMethod("OC/Cm/Serialization/ISerializable", "serializeContents",
                "(LOC/Cm/Serialization/ISerializer;)Z", ser_handler, jserializer);

    JNIGlobal::releaseLocalRef(jserializer);

    return bRet ;
}

bool
Serializable::deSerializeContents(OC::Cm::Serialization::IDeserializer  &deserializer)
{
    jobject     jdeserializer = NULL  ;

    jdeserializer = JNIGlobal::constructObject("OC/Cm/Serialization/IDeserializer", "(J)V",
                    (jlong)(&deserializer)) ;

    if (NULL == jdeserializer)
    {
        return false ;
    }

    bool bRet = JNIGlobal::executeBoolMethod("OC/Cm/Serialization/ISerializable", "deSerializeContents",
                "(LOC/Cm/Serialization/IDeserializer;)Z", ser_handler, jdeserializer);

    JNIGlobal::releaseLocalRef(jdeserializer);

    return bRet ;
}

