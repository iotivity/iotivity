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
#include "OC_Cm_Serialization_ISerializable.h"
#include "CMDataTypes.h"



using namespace std ;


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

    OC::Cm::Serialization::ISerializer *pSerial = (OC::Cm::Serialization::ISerializer *)(nativeHandle) ;

    OC::Cm::Serialization::ISerializable *pSerializable = (OC::Cm::Serialization::ISerializable *)obj ;

    jboolean serializable = (jboolean)pSerializable->serialize(*pSerial) ;

    return serializable ;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_ISerializable_deSerialize
(JNIEnv *pEnv, jobject curent, jlong obj, jlong nativeHandle)
{
//  jlong nativeHandle = JNIGlobal::executeLongMethod("OC/Cm/Serialization/IDeserializer","getNativeHandle","()J", deserializer);

    OC::Cm::Serialization::IDeserializer *pdSerial = (OC::Cm::Serialization::IDeserializer *)(
                nativeHandle) ;

    OC::Cm::Serialization::ISerializable *pSerializable = (OC::Cm::Serialization::ISerializable *)obj ;

    jboolean deser = (jboolean)pSerializable->deSerialize(*pdSerial) ;

    return deser;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_ISerializable_validateContent
(JNIEnv *pEnv, jobject thisObject, jlong nativeHandle)
{
    Serializable *pObj = ( Serializable *) nativeHandle ;
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
    Serializable *pObj = ( Serializable *) nativeHandle ;
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
    Serializable *pObj = ( Serializable *) nativeHandle ;
    if (NULL == pObj)
    {
        return ;
    }

    pObj->setList(true);
}


