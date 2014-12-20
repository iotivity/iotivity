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




#include <jni.h>
#include "OC_Cm_Serialization_ISerializer.h"
#include "CMSerialization.h"
//#include "iostream"
#include "JNIGlobal.h"

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_ISerializer_setStringPropertyValue(JNIEnv *env,
        jobject thisObj, jlong serializerObj, jstring propertyName, jstring value)
{
    //std::cout <<"setStringPropertyValue : start"<<std::endl ;

    OC::Cm::Serialization::ISerializer *serializer = (OC::Cm::Serialization::ISerializer *)
            serializerObj ;
    JNI_TryReturn(NULL != serializer, JNI_FALSE, "Native instance is invalid");

    if (NULL == propertyName)
    {
        return false ;
    }

    if (NULL == value)
    {
        return false ;
    }

    std::string pName ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(propertyName, pName)) , JNI_FALSE,
                   "propertyName convertion failed") ;

    std::string m_val ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(value, m_val)) , JNI_FALSE,
                   "propertyName convertion failed") ;

    //std::cout <<"setStringPropertyValue : end"<<std::endl ;
    return (serializer->setStringPropertyValue(pName, m_val)) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_ISerializer_setIntPropertyValue(JNIEnv *env,
        jobject thisObj, jlong serializerObj, jstring propertyName, jint value)
{
    OC::Cm::Serialization::ISerializer *serializer = (OC::Cm::Serialization::ISerializer *)
            serializerObj ;
    JNI_TryReturn(NULL != serializer, JNI_FALSE, "Native instance is invalid");

    if (NULL == propertyName)
    {
        return false ;
    }

    std::string pName ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(propertyName, pName)) , JNI_FALSE,
                   "propertyName convertion failed") ;

    return (serializer->setIntPropertyValue(pName, (int)value)) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_ISerializer_setDoublePropertyValue(JNIEnv *env,
        jobject thisObj, jlong serializerObj, jstring propertyName, jdouble value)
{
    OC::Cm::Serialization::ISerializer *serializer = (OC::Cm::Serialization::ISerializer *)
            serializerObj ;
    JNI_TryReturn(NULL != serializer, JNI_FALSE, "Native instance is invalid");

    if (NULL == propertyName)
    {
        return false ;
    }

    std::string pName ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(propertyName, pName)) , JNI_FALSE,
                   "propertyName conversion failed") ;

    return (serializer->setDoublePropertyValue(pName, value)) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_ISerializer_setBoolPropertyValue(JNIEnv *env,
        jobject thisObj, jlong serializerObj, jstring propertyName, jboolean value)
{
    //std::cout <<"setBoolPropertyValue : start"<<std::endl ;

    OC::Cm::Serialization::ISerializer *serializer = (OC::Cm::Serialization::ISerializer *)
            serializerObj ;
    JNI_TryReturn(NULL != serializer, JNI_FALSE, "Native instance is invalid");

    if (NULL == propertyName)
    {
        return false ;
    }

    std::string pName ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(propertyName, pName)) , JNI_FALSE,
                   "propertyName conversion failed") ;

    //std::cout <<"setBoolPropertyValue : end"<<std::endl ;
    jboolean boolVal = (jboolean)(serializer->setBoolPropertyValue( pName, (value == JNI_TRUE)));

    return boolVal;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_ISerializer_startDocument(JNIEnv *env,
        jobject thisObj, jlong serializerObj, jstring nodename)
{
    if (NULL == nodename)
        return false;

    OC::Cm::Serialization::ISerializer *serializer = (OC::Cm::Serialization::ISerializer *)
            serializerObj ;
    JNI_TryReturn(NULL != serializer, JNI_FALSE, "Native instance is invalid");

    std::string name ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(nodename, name)) , JNI_FALSE,
                   "nodename conversion failed") ;

    return (serializer->startDocument()) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_ISerializer_startGroupProperty(JNIEnv *env,
        jobject thisObj, jlong serializerObj, jstring nodename)
{
    if (NULL == nodename)
    {
        return false;
    }

    OC::Cm::Serialization::ISerializer *serializer = (OC::Cm::Serialization::ISerializer *)
            serializerObj ;
    JNI_TryReturn(NULL != serializer, JNI_FALSE, "Native instance is invalid");

    std::string name ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(nodename, name)) , JNI_FALSE,
                   "nodename conversion failed") ;

    return (serializer->startGroupProperty(name)) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_ISerializer_endGroupProperty(JNIEnv *env,
        jobject thisObj, jlong serializerObj, jstring nodename)
{
    if (NULL == nodename)
        return false;

    OC::Cm::Serialization::ISerializer *serializer = (OC::Cm::Serialization::ISerializer *)
            serializerObj ;
    JNI_TryReturn(NULL != serializer, JNI_FALSE, "Native instance is invalid");

    std::string name ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(nodename, name)) , JNI_FALSE,
                   "nodename conversion failed") ;

    return (serializer->endGroupProperty(name)) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_ISerializer_endDocument (JNIEnv *env,
        jobject thisObj, jlong serializerObj)
{
    OC::Cm::Serialization::ISerializer *serializer = (OC::Cm::Serialization::ISerializer *)
            serializerObj ;
    JNI_TryReturn(NULL != serializer, JNI_FALSE, "Native instance is invalid");

    return (serializer->endDocument()) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jstring JNICALL Java_OC_Cm_Serialization_ISerializer_getContentType (JNIEnv *env,
        jobject thisObj, jlong serializerObj)
{
    OC::Cm::Serialization::ISerializer *serializer = (OC::Cm::Serialization::ISerializer *)
            serializerObj ;
    JNI_TryReturn(NULL != serializer, JNI_FALSE, "Native instance is invalid");

    return JNIGlobal::convertToJString(serializer->getContentType());
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_ISerializer_setSchemaInstanceType(JNIEnv *env,
        jobject thisObj, jlong serializerObj, jstring schemaType)
{
    if (NULL == schemaType)
        return false;

    OC::Cm::Serialization::ISerializer *serializer = (OC::Cm::Serialization::ISerializer *)
            serializerObj ;
    JNI_TryReturn(NULL != serializer, JNI_FALSE, "Native instance is invalid");

    std::string type ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(schemaType, type)) , JNI_FALSE,
                   "schemaType conversion failed") ;

    return serializer->setSchemaInstanceType(type) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_ISerializer_isInFilteringMode
(JNIEnv *env, jobject thisObj, jlong deserializer, jstring propertyName)
{
    OC::Cm::Serialization::ISerializer *serializer = (OC::Cm::Serialization::ISerializer *) deserializer
            ;
    JNI_TryReturn(NULL != serializer, JNI_FALSE, "Native instance is invalid");

    std::string pName ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(propertyName, pName)) , JNI_FALSE,
                   "propertyName conversion failed") ;

    return (serializer->isInFilteringMode(pName)) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_ISerializer_isRecursive
(JNIEnv *env, jobject thisObj, jlong deserializer)
{
    OC::Cm::Serialization::ISerializer *serializer = (OC::Cm::Serialization::ISerializer *) deserializer
            ;
    JNI_TryReturn(NULL != serializer, JNI_FALSE, "Native instance is invalid");

    return (serializer->isRecursive()) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_ISerializer_validateMandatory
(JNIEnv *env, jobject thisObj, jlong deserializer)
{
    OC::Cm::Serialization::ISerializer *serializer = (OC::Cm::Serialization::ISerializer *) deserializer
            ;
    JNI_TryReturn(NULL != serializer, JNI_FALSE, "Native instance is invalid");

    return (serializer->validateMandatory()) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_ISerializer_setLongPropertyValue
(JNIEnv *env, jobject thisObj, jlong deserializer, jstring propertyName, jlong value)
{
    OC::Cm::Serialization::ISerializer *serializer = (OC::Cm::Serialization::ISerializer *) deserializer
            ;
    JNI_TryReturn(NULL != serializer, JNI_FALSE, "Native instance is invalid");

    std::string pName ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(propertyName, pName)) , JNI_FALSE,
                   "propertyName conversion failed") ;

    return (serializer->setPropertyValue(pName, value)) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL Java_OC_Cm_Serialization_ISerializer_setErrorMessage
(JNIEnv *env, jobject thisObj, jlong deserializer, jstring message)
{
    OC::Cm::Serialization::ISerializer *serializer = (OC::Cm::Serialization::ISerializer *) deserializer
            ;
    JNI_TryReturnVoid(NULL != serializer, "Native instance is invalid");

    const char *property = env->GetStringUTFChars(message, 0);
    std::string messageStr(property);
    env->ReleaseStringUTFChars(message, property);

    serializer->setErrorMessage(messageStr);
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_ISerializer_startListProperty
(JNIEnv *env, jobject thisObj, jlong deserializer, jstring nodeName)
{
    OC::Cm::Serialization::ISerializer *serializer = (OC::Cm::Serialization::ISerializer *) deserializer
            ;
    JNI_TryReturn(NULL != serializer, JNI_FALSE, "Native instance is invalid");

    std::string cnodeName ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(nodeName, cnodeName)), JNI_FALSE,
                   "string conversion failed") ;
    return (serializer->startListProperty(cnodeName)) ? JNI_TRUE : JNI_FALSE ;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_ISerializer_endListProperty
(JNIEnv *env, jobject thisObj, jlong deserializer, jstring nodeName)
{
    OC::Cm::Serialization::ISerializer *serializer = (OC::Cm::Serialization::ISerializer *) deserializer
            ;
    JNI_TryReturn(NULL != serializer, JNI_FALSE, "Native instance is invalid");

    std::string cnodeName ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(nodeName, cnodeName)), JNI_FALSE,
                   "string conversion failed") ;
    return (serializer->endListProperty(cnodeName)) ? JNI_TRUE : JNI_FALSE ;
}


JNIEXPORT void JNICALL Java_OC_Cm_Serialization_ISerializer_setIDExcemption
(JNIEnv *env, jobject thisObj, jlong deserializer, jboolean jexcemption)
{
    OC::Cm::Serialization::ISerializer *deserialization = (OC::Cm::Serialization::ISerializer *)
            deserializer ;
    JNI_TryReturnVoid( ( NULL != deserialization) , "Native instance is invalid") ;

    deserialization->setIDExcemption(jexcemption == JNI_TRUE) ;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_ISerializer_getIDExcemption
(JNIEnv *env, jobject thisObj, jlong deserializer)
{
    OC::Cm::Serialization::ISerializer *deserialization = (OC::Cm::Serialization::ISerializer *)
            deserializer ;
    JNI_TryReturn( ( NULL != deserialization) , false, "Native instance is invalid") ;

    return ((deserialization->getIDExcemption()) ? JNI_TRUE : JNI_FALSE);
}
