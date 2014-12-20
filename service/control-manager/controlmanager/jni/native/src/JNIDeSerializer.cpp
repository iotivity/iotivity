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

#include "OC_Cm_Serialization_IDeserializer.h"
#include "CMSerialization.h"
#include "Serialization/IDeserializer.h"
#include "JNIGlobal.h"


JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_IDeserializer_parse
(JNIEnv *env, jobject thisObj, jlong deserializer, jstring buff, jint len)
{
    OC::Cm::Serialization::IDeserializer *deserialization = (OC::Cm::Serialization::IDeserializer *)
            deserializer ;
    if ( NULL == deserialization)
    {
        JNIGlobal::throwException("deserialization object is NULL") ;
        return JNI_FALSE ;
    }

    std::string strbuffer ;
    if ( false == JNIGlobal::convertToStdString(buff, strbuffer) )
    {
        JNIGlobal::throwException("convertToStdString failed: json buffer") ;
        return JNI_FALSE ;
    }

    return (deserialization->parse(strbuffer, (int)len)) ? JNI_TRUE : JNI_FALSE ;
}


JNIEXPORT jstring JNICALL Java_OC_Cm_Serialization_IDeserializer_getStringPropertyValue
(JNIEnv *env, jobject thisObj, jlong deserializer, jstring propName)
{
    OC::Cm::Serialization::IDeserializer *deserialization = (OC::Cm::Serialization::IDeserializer *)
            deserializer ;
    JNI_TryReturn( ( NULL != deserialization) , NULL, "deserialization object is NULL") ;

    std::string strproperty ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(propName, strproperty)) , NULL,
                   "propertyName convertion failed") ;

    std::string strvalue ;
    JNI_TryReturn( ( true == deserialization->getPropertyValue(strproperty, &strvalue)) , NULL,
                   "property not found") ;

    return JNIGlobal::convertToJString(strvalue);
}


JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_IDeserializer_getBoolPropertyValue
(JNIEnv *env, jobject thisObj, jlong deserializer, jstring propName)
{
    OC::Cm::Serialization::IDeserializer *deserialization = (OC::Cm::Serialization::IDeserializer *)
            deserializer ;
    JNI_TryReturn( ( NULL != deserialization) , JNI_FALSE, "deserialization object is NULL") ;

    std::string strproperty ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(propName, strproperty)) , JNI_FALSE,
                   "propertyName convertion failed") ;

    bool result = false;
    JNI_TryReturn( ( true == deserialization->getPropertyValue(strproperty, &result)) , JNI_FALSE,
                   "property not found") ;

    return (result) ? JNI_TRUE : JNI_FALSE ;
}


JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_IDeserializer_getGroupProperty
(JNIEnv *env, jobject thisObj, jlong deserializer, jstring propName)
{
    OC::Cm::Serialization::IDeserializer *deserialization = (OC::Cm::Serialization::IDeserializer *)
            deserializer ;
    JNI_TryReturn( ( NULL != deserialization) , JNI_FALSE, "deserialization object is NULL") ;

    std::string strproperty ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(propName, strproperty)) , JNI_FALSE,
                   "propertyName convertion failed") ;

    bool result = deserialization->getGroupProperty(strproperty) ;

    return (result) ? JNI_TRUE : JNI_FALSE ;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_IDeserializer_endGroupProperty
(JNIEnv *env, jobject thisObj, jlong deserializer, jstring propName)
{
    OC::Cm::Serialization::IDeserializer *deserialization = (OC::Cm::Serialization::IDeserializer *)
            deserializer ;
    JNI_TryReturn( ( NULL != deserialization) , JNI_FALSE, "deserialization object is NULL") ;

    std::string strproperty ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(propName, strproperty)) , JNI_FALSE,
                   "propertyName convertion failed") ;

    bool result = deserialization->endGroupProperty(strproperty) ;

    return (result) ? JNI_TRUE : JNI_FALSE ;
}

JNIEXPORT jint JNICALL Java_OC_Cm_Serialization_IDeserializer_getIntPropertyValue
(JNIEnv *env, jobject thisObj, jlong deserializer, jstring propName)
{
    OC::Cm::Serialization::IDeserializer *deserialization = (OC::Cm::Serialization::IDeserializer *)
            deserializer ;
    JNI_TryReturn( ( NULL != deserialization) , -1, "deserialization object is NULL") ;

    std::string strproperty ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(propName, strproperty)) , -1,
                   "propertyName convertion failed") ;

    int val;
    JNI_TryReturn( ( true == deserialization->getPropertyValue( strproperty, &val)) , -1,
                   "property not found") ;

    return val ;
}

JNIEXPORT jdouble JNICALL Java_OC_Cm_Serialization_IDeserializer_getDoublePropertyValue
(JNIEnv *env, jobject thisObj, jlong deserializer, jstring propName)
{
    OC::Cm::Serialization::IDeserializer *deserialization = (OC::Cm::Serialization::IDeserializer *)
            deserializer ;
    JNI_TryReturn( ( NULL != deserialization) , -1, "deserialization object is NULL") ;

    std::string strproperty ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(propName, strproperty)) , -1,
                   "propertyName convertion failed") ;

    double val;
    JNI_TryReturn( ( true == deserialization->getPropertyValue( strproperty, &val)) , -1,
                   "property not found") ;

    return val ;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_IDeserializer_getIsArray
(JNIEnv *env, jobject thisObj, jlong deserializer, jstring propName)
{
    OC::Cm::Serialization::IDeserializer *deserialization = (OC::Cm::Serialization::IDeserializer *)
            deserializer ;
    JNI_TryReturn( ( NULL != deserialization) , JNI_FALSE, "deserialization object is NULL") ;

    std::string strproperty ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(propName, strproperty)) , JNI_FALSE,
                   "propertyName conversion failed") ;

    bool result = deserialization->getIsArray(strproperty) ;

    return (result) ? JNI_TRUE : JNI_FALSE ;
}

JNIEXPORT jint JNICALL Java_OC_Cm_Serialization_IDeserializer_getSize
(JNIEnv *env, jobject thisObj, jlong deserializer, jstring propName)
{
    OC::Cm::Serialization::IDeserializer *deserialization = (OC::Cm::Serialization::IDeserializer *)
            deserializer ;
    JNI_TryReturn( ( NULL != deserialization) , -1, "deserialization object is NULL") ;

    std::string strproperty ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(propName, strproperty)) , -1,
                   "propertyName conversion failed") ;

    return (jint)(deserialization->getSize(strproperty)) ;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_IDeserializer_setCurrentProperty
(JNIEnv *env, jobject thisObj, jlong deserializer, jstring propName, jint index)
{
    OC::Cm::Serialization::IDeserializer *deserialization = (OC::Cm::Serialization::IDeserializer *)
            deserializer ;
    JNI_TryReturn( ( NULL != deserialization) , JNI_FALSE, "deserialization object is NULL") ;

    std::string strproperty ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(propName, strproperty)) , JNI_FALSE,
                   "propertyName conversion failed") ;

    bool result = deserialization->setCurrentArrayIndex(strproperty, (int) index) ;
    return (result) ? JNI_TRUE : JNI_FALSE ;
}

JNIEXPORT jstring JNICALL Java_OC_Cm_Serialization_IDeserializer_getSchemaInstanceType
(JNIEnv *env, jobject thisObj, jlong deserializer)
{
    OC::Cm::Serialization::IDeserializer *deserialization = (OC::Cm::Serialization::IDeserializer *)
            deserializer ;
    JNI_TryReturn( ( NULL != deserialization) , NULL, "deserialization object is NULL") ;

    std::string strType;
    deserialization->getSchemaInstanceType(strType);

    return JNIGlobal::convertToJString(strType);
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_IDeserializer_hasGroupProperty
(JNIEnv *env, jobject thisObj, jlong deserializer, jstring propertyName)
{
    OC::Cm::Serialization::IDeserializer *deserialization = (OC::Cm::Serialization::IDeserializer *)
            deserializer ;
    JNI_TryReturn( ( NULL != deserialization) , JNI_FALSE, "deserialization object is NULL") ;

    std::string strproperty ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(propertyName, strproperty)) , JNI_FALSE,
                   "propertyName conversion failed") ;

    bool result = deserialization->hasGroupProperty(strproperty) ;
    return (result) ? JNI_TRUE : JNI_FALSE ;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_IDeserializer_hasProperty
(JNIEnv *env, jobject thisObj, jlong deserializer, jstring propertyName)
{
    OC::Cm::Serialization::IDeserializer *deserialization = (OC::Cm::Serialization::IDeserializer *)
            deserializer ;
    JNI_TryReturn( ( NULL != deserialization) , JNI_FALSE, "deserialization object is NULL") ;

    std::string strproperty ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(propertyName, strproperty)) , JNI_FALSE,
                   "propertyName conversion failed") ;

    bool result = deserialization->hasProperty(strproperty) ;
    return (result) ? JNI_TRUE : JNI_FALSE ;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_IDeserializer_setCurrentArrayIndex
(JNIEnv *env, jobject thisObj, jlong deserializer, jstring propertyName, jint index)
{
    OC::Cm::Serialization::IDeserializer *deserialization = (OC::Cm::Serialization::IDeserializer *)
            deserializer ;
    JNI_TryReturn( ( NULL != deserialization) , JNI_FALSE, "deserialization object is NULL") ;

    std::string strproperty ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(propertyName, strproperty)) , JNI_FALSE,
                   "propertyName conversion failed") ;

    bool result = deserialization->setCurrentArrayIndex(strproperty, (int) index) ;
    return (result) ? JNI_TRUE : JNI_FALSE ;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_IDeserializer_isInFilteringMode
(JNIEnv *env, jobject thisObj, jlong deserializer, jstring propertyName)
{
    OC::Cm::Serialization::IDeserializer *deserialization = (OC::Cm::Serialization::IDeserializer *)
            deserializer ;
    JNI_TryReturn( ( NULL != deserialization) , JNI_FALSE, "deserialization object is NULL") ;

    std::string strproperty ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(propertyName, strproperty)) , JNI_FALSE,
                   "propertyName conversion failed") ;

    bool result = deserialization->isInFilteringMode(strproperty) ;
    return (result) ? JNI_TRUE : JNI_FALSE ;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_IDeserializer_isRecursive
(JNIEnv *env, jobject thisObj, jlong deserializer)
{
    OC::Cm::Serialization::IDeserializer *deserialization = (OC::Cm::Serialization::IDeserializer *)
            deserializer ;
    JNI_TryReturn( ( NULL != deserialization) , JNI_FALSE, "deserialization object is NULL") ;

    bool result = deserialization->isRecursive() ;
    return (result) ? JNI_TRUE : JNI_FALSE ;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_IDeserializer_validateMandatory
(JNIEnv *env, jobject thisObj, jlong deserializer)
{
    OC::Cm::Serialization::IDeserializer *deserialization = (OC::Cm::Serialization::IDeserializer *)
            deserializer ;
    JNI_TryReturn( ( NULL != deserialization) , JNI_FALSE, "deserialization object is NULL") ;

    bool result = deserialization->validateMandatory() ;
    return (result) ? JNI_TRUE : JNI_FALSE ;
}

JNIEXPORT jlong JNICALL Java_OC_Cm_Serialization_IDeserializer_getLongPropertyValue
(JNIEnv *env, jobject thisObj, jlong deserializer, jstring propertyName)
{
    OC::Cm::Serialization::IDeserializer *deserialization = (OC::Cm::Serialization::IDeserializer *)
            deserializer ;
    JNI_TryReturn( ( NULL != deserialization) , -1, "deserialization object is NULL") ;

    std::string strproperty ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(propertyName, strproperty)) , -1,
                   "propertyName convertion failed") ;

    int64_t val;
    JNI_TryReturn( ( true == deserialization->getPropertyValue( strproperty, &val)) , -1,
                   "property not found") ;

    return (jlong)val ;
}

JNIEXPORT void JNICALL Java_OC_Cm_Serialization_IDeserializer_setErrorMessage
(JNIEnv *env, jobject thisObj, jlong deserializer, jstring message)
{
    OC::Cm::Serialization::IDeserializer *deserialization = (OC::Cm::Serialization::IDeserializer *)
            deserializer ;
    JNI_TryReturnVoid( ( NULL != deserialization) , "deserialization object is NULL") ;

    std::string messageStr ;
    JNI_TryReturnVoid( ( true == JNIGlobal::convertToStdString(message, messageStr)),
                       "propertyName conversion failed") ;

    deserialization->setErrorMessage(messageStr);
}

JNIEXPORT void JNICALL Java_OC_Cm_Serialization_IDeserializer_setIDExcemption
(JNIEnv *env, jobject thisObj, jlong deserializer, jboolean jexcemption)
{
    OC::Cm::Serialization::IDeserializer *deserialization = (OC::Cm::Serialization::IDeserializer *)
            deserializer ;
    JNI_TryReturnVoid( ( NULL != deserialization) , "deserialization object is NULL") ;

    deserialization->setIDExcemption(jexcemption == JNI_TRUE) ;
}


JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_IDeserializer_getIDExcemption
(JNIEnv *env, jobject thisObj, jlong deserializer)
{
    OC::Cm::Serialization::IDeserializer *deserialization = (OC::Cm::Serialization::IDeserializer *)
            deserializer ;
    JNI_TryReturn( ( NULL != deserialization) , false, "deserialization object is NULL") ;

    return ((deserialization->getIDExcemption()) ? JNI_TRUE : JNI_FALSE);
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Serialization_IDeserializer_removeParameter
(JNIEnv *env, jobject thisObj, jlong deserializer, jstring jparamName)
{
    OC::Cm::Serialization::IDeserializer *deserialization = (OC::Cm::Serialization::IDeserializer *)
            deserializer ;
    JNI_TryReturn( ( NULL != deserialization) , false, "deserialization object is NULL") ;

    std::string paramName ;
    if (false == JNIGlobal::convertToStdString(jparamName, paramName))
    {
        return JNI_FALSE;
    }

    return ((deserialization->removeParameter(paramName)) ? JNI_TRUE : JNI_FALSE);
}
