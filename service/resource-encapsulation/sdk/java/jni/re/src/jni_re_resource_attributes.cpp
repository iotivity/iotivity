/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include "jni_re_resource_attributes.h"
#include "jni_re_jvm.h"

using namespace std;

RCSResourceAttributes *JniResourceAttributes::getResourceAttributePtr(JNIEnv *env,
        jobject interfaceObject)
{
    RCSResourceAttributes *rep = REGetHandle<RCSResourceAttributes>(env, interfaceObject);
    if (env->ExceptionCheck())
    {
        LOGE("Failed to get native handle from RCSResourceAttributes");
    }
    if (!rep)
    {
        throwRCSException(env, "JNI no native pointer");
    }
    return rep;
}

JNIEXPORT jobject JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeGetValueN
(JNIEnv *env, jobject interfaceObject, jstring jKey, jint dataType)
{
    LOGD("RCSResourceAttributes_nativeGetValueN");
    if (!jKey)
    {
        throwRCSException(env, "attributeKey cannot be null");
        return nullptr;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return nullptr;

    std::string key = env->GetStringUTFChars(jKey, NULL);
    if (!(rep->contains(key)))
    {
        throwRCSException(env, "Key not Found");
    }
    else
    {
        RCSResourceAttributes::Value attrValue = rep->at(key);
        jobject jobj;

        if (0 == dataType)
        {
            LOGD("nativeGetValueN : integer");
            try
            {
                int val = attrValue.get<int>();
                jobj = env->NewObject(g_cls_Integer, g_mid_Integer_ctor, static_cast<jint>(val));
                return jobj;
            }
            catch (std::exception e)
            {
                throwRCSException(env, e.what());
            }
        }
        else if (1 == dataType)
        {
            LOGD("nativeGetValueN : Vector of Integer");
            try
            {
                std::vector<int> vectors = attrValue.get<std::vector<int>>();
                return  convertNativeIntVectorToJavaVector(env, vectors);
            }
            catch (std::exception e)
            {
                throwRCSException(env, e.what());
            }
        }
        else if (2 == dataType)
        {
            LOGD("nativeGetValueN : Vector of Vector of Integer");
            try
            {
                std::vector<std::vector<int>> vectors = attrValue.get<std::vector<std::vector<int>>>();
                return  convertNativeVectorOfVectorOfIntToJavaVector(env, vectors);
            }
            catch (std::exception e)
            {
                throwRCSException(env, e.what());
            }
        }
        else if (3 == dataType)
        {
            LOGD("nativeGetValueN : Vector of Vector of vector Integer");
            try
            {
                std::vector<std::vector<std::vector<int>>> vectors =
                    attrValue.get<std::vector<std::vector<std::vector<int>>>>();
                return  convertNativeVectorOfVectorOfVectorOfIntToJavaVector(env, vectors);
            }
            catch (std::exception e)
            {
                throwRCSException(env, e.what());
            }
        }
        else if (4 == dataType)
        {
            LOGD("nativeGetValueN : Double");
            try
            {
                double val = attrValue.get<double>();
                jobj = env->NewObject(g_cls_Double, g_mid_Double_ctor, static_cast<jdouble>(val));
                return jobj;
            }
            catch (std::exception e)
            {
                throwRCSException(env, e.what());
            }
        }
        else if (5 == dataType)
        {
            LOGD("nativeGetValueN : Vector of Double");
            try
            {
                std::vector<double> vectors = attrValue.get<std::vector<double>>();
                return  convertNativeDoubleVectorToJavaVector(env, vectors);
            }
            catch (std::exception e)
            {
                throwRCSException(env, e.what());
            }
        }
        else if (6 == dataType)
        {
            LOGD("nativeGetValueN : Vector of Vector of Double");
            try
            {
                std::vector<std::vector<double>> vectors = attrValue.get<std::vector<std::vector<double>>>();
                return  convertNativeVectorOfVectorOfDoubleToJavaVector(env, vectors);
            }
            catch (std::exception e)
            {
                throwRCSException(env, e.what());
            }
        }
        else if (7 == dataType)
        {
            LOGD("nativeGetValueN : Vector of Vector of vector Double");
            try
            {
                std::vector<std::vector<std::vector<double>>> vectors =
                    attrValue.get<std::vector<std::vector<std::vector<double>>>>();
                return  convertNativeVectorOfVectorOfVectorOfDoubleToJavaVector(env, vectors);
            }
            catch (std::exception e)
            {
                throwRCSException(env, e.what());
            }
        }
        else if (8 == dataType)
        {
            LOGD("nativeGetValueN : Boolean");
            try
            {
                bool val = attrValue.get<bool>();
                jobj = env->NewObject(g_cls_Boolean, g_mid_Boolean_ctor, static_cast<jboolean>(val));
                return jobj;
            }
            catch (std::exception e)
            {
                throwRCSException(env, e.what());
            }
        }
        else if (9 == dataType)
        {
            LOGD("nativeGetValueN : Vector of Boolean");
            try
            {
                std::vector<bool> vectors = attrValue.get<std::vector<bool>>();
                return  convertNativeBooleanVectorToJavaVector(env, vectors);
            }
            catch (std::exception e)
            {
                throwRCSException(env, e.what());
            }
        }
        else if (10 == dataType)
        {
            LOGD("nativeGetValueN : Vector of Vector of Boolean");
            try
            {
                std::vector<std::vector<bool>> vectors = attrValue.get<std::vector<std::vector<bool>>>();
                return  convertNativeVectorOfVectorOfBooleanToJavaVector(env, vectors);
            }
            catch (std::exception e)
            {
                throwRCSException(env, e.what());
            }
        }
        else if (11 == dataType)
        {
            LOGD("nativeGetValueN : Vector of Vector of Vector of Boolean");
            try
            {
                std::vector<std::vector<std::vector<bool>>> vectors =
                    attrValue.get<std::vector<std::vector<std::vector<bool>>>>();
                return  convertNativeVectorOfVectorOfVectorOfBooleanToJavaVector(env, vectors);
            }
            catch (std::exception e)
            {
                throwRCSException(env, e.what());
            }
        }
        else if (12 == dataType)
        {
            LOGD("nativeGetValueN : String");
            try
            {
                std::string val = attrValue.get<std::string>();
                jstring jstr = env->NewStringUTF(val.c_str());
                return static_cast<jobject>(jstr);
            }
            catch (std::exception e)
            {
                throwRCSException(env, e.what());
            }
        }
        else if (13 == dataType)
        {
            LOGD("nativeGetValueN : Vector of String");
            try
            {
                std::vector<std::string> vectors = attrValue.get<std::vector<std::string>>();
                return  convertNativeStringVectorToJavaVector(env, vectors);
            }
            catch (std::exception e)
            {
                throwRCSException(env, e.what());
            }
        }
        else if (14 == dataType)
        {
            LOGD("nativeGetValueN : Vector of Vector of String");
            try
            {
                std::vector<std::vector<std::string>> vectors =
                                                       attrValue.get<std::vector<std::vector<std::string>>>();
                return  convertNativeVectorOfVectorOfStringToJavaVector(env, vectors);
            }
            catch (std::exception e)
            {
                throwRCSException(env, e.what());
            }
        }
        else if (15 == dataType)
        {
            LOGD("nativeGetValueN : Vector of Vector of Vector of String");
            try
            {
                std::vector<std::vector<std::vector<std::string>>> vectors =
                    attrValue.get<std::vector<std::vector<std::vector<std::string>>>>();
                return  convertNativeVectorOfVectorOfVectorOfStringToJavaVector(env, vectors);
            }
            catch (std::exception e)
            {
                throwRCSException(env, e.what());
            }
        }
        else if (16 == dataType)
        {
            LOGD("nativeGetValueN : RCSResourceAttribute");
            try
            {
                RCSResourceAttributes  values = attrValue.get<RCSResourceAttributes>();
                RCSResourceAttributes *rep = new RCSResourceAttributes(values);
                jlong handle = reinterpret_cast<jlong>(rep);
                jobject jRepresentation = env->NewObject(g_cls_RCSRemoteResourceAttributesObject,
                                          g_mid_RCSRemoteResourceAttributesObject_ctor_bool,
                                          handle, true);
                if (!jRepresentation)
                {
                    delete rep;
                }
                return jRepresentation;
            }
            catch (std::exception e)
            {
                throwRCSException(env, e.what());
            }
        }
    }
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueInt
(JNIEnv *env, jobject interfaceObject, jstring jKey, jint jValue)
{
    LOGD("RCSResourceAttributes_setValueInteger");
    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return;

    std::string key = env->GetStringUTFChars(jKey, NULL);

    RCSResourceAttributes setAttribute;
    setAttribute[key] = static_cast<int>(jValue);
    *rep = setAttribute;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfInt
(JNIEnv *env, jobject interfaceObject, jstring jKey, jobject vectorOfInt)
{
    LOGD("RCSResourceAttributes_nativeSetValueVectorOfInt");
    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return;

    std::string key = env->GetStringUTFChars(jKey, NULL);
    RCSResourceAttributes setAttribute;
    std::vector<int> nativeVector =  covertJavaIntVectorToNativeVector(env, vectorOfInt);
    setAttribute[key] = nativeVector;
    *rep = setAttribute;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfVectorOfInt
(JNIEnv *env, jobject interfaceObject, jstring jKey, jobject vectorOfVectorOfInt)
{
    LOGD("RCSResourceAttributes_nativeSetValueVectorOfVectorOfInt");
    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return;

    std::string key = env->GetStringUTFChars(jKey, NULL);
    RCSResourceAttributes setAttribute;
    std::vector<std::vector<int>> nativeVector =  convertJavaVectorOfVectorOfIntToNativeVector(env,
                               vectorOfVectorOfInt);
    setAttribute[key] = nativeVector;
    *rep = setAttribute;
}


JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfVectorOfVectorOfInt
(JNIEnv *env, jobject interfaceObject, jstring jKey, jobject value)
{
    LOGD("RCSResourceAttributes_nativeSetValueVectorOfVectorOfVectorOfInt");
    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return;

    std::string key = env->GetStringUTFChars(jKey, NULL);
    RCSResourceAttributes setAttribute;
    std::vector<std::vector<std::vector<int>>> nativeVector =
        convertJavaVectorOfVectorOfVectorOfIntToNativeVector(env, value);
    setAttribute[key] = nativeVector;
    *rep = setAttribute;
}


JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueDouble
(JNIEnv *env, jobject interfaceObject, jstring jKey, jdouble jValue)
{
    LOGD("RCSResourceAttributes_setValueDouble");
    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return;

    std::string key = env->GetStringUTFChars(jKey, NULL);

    RCSResourceAttributes setAttribute;
    setAttribute[key] = static_cast<double>(jValue);
    *rep = setAttribute;
}


JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfDouble
(JNIEnv *env, jobject interfaceObject, jstring jKey, jobject value)
{
    LOGD("RCSResourceAttributes_nativeSetValueVectorOfDouble");
    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return;

    std::string key = env->GetStringUTFChars(jKey, NULL);
    RCSResourceAttributes setAttribute;
    std::vector<double> nativeVector =  covertJavaDoubleVectorToNativeVector(env, value);
    setAttribute[key] = nativeVector;
    *rep = setAttribute;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfVectorOfDouble
(JNIEnv *env, jobject interfaceObject, jstring jKey, jobject value)
{

    LOGD("RCSResourceAttributes_nativeSetValueVectorOfVectorOfDouble");
    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return;

    std::string key = env->GetStringUTFChars(jKey, NULL);
    RCSResourceAttributes setAttribute;
    std::vector<std::vector<double>> nativeVector =  convertJavaVectorOfVectorOfDoubleToNativeVector(
                                      env, value);
    setAttribute[key] = nativeVector;
    *rep = setAttribute;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfVectorOfVectorOfDouble
(JNIEnv *env, jobject interfaceObject, jstring jKey, jobject value)
{

    LOGD("RCSResourceAttributes_nativeSetValueVectorOfVectorOfVectorOfDouble");
    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return;

    std::string key = env->GetStringUTFChars(jKey, NULL);
    RCSResourceAttributes setAttribute;
    std::vector<std::vector<std::vector<double>>> nativeVector =
        convertJavaVectorOfVectorOfVectorOfDoubleToNativeVector(env, value);
    setAttribute[key] = nativeVector;
    *rep = setAttribute;
}


JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueBoolean
(JNIEnv *env, jobject interfaceObject, jstring jKey, jboolean jValue)
{
    LOGD("RCSResourceAttributes_setValueBoolean");
    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return;

    std::string key = env->GetStringUTFChars(jKey, NULL);

    RCSResourceAttributes setAttribute;
    setAttribute[key] = static_cast<bool>(jValue);
    *rep = setAttribute;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfBoolean
(JNIEnv *env, jobject interfaceObject, jstring jKey, jobject value)
{
    LOGD("RCSResourceAttributes_nativeSetValueVectorOfBoolean");
    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return;

    std::string key = env->GetStringUTFChars(jKey, NULL);
    RCSResourceAttributes setAttribute;
    std::vector<bool> nativeVector =  covertJavaBooleanVectorToNativeVector(env, value);
    setAttribute[key] = nativeVector;
    *rep = setAttribute;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfVectorOfBoolean
(JNIEnv *env, jobject interfaceObject, jstring jKey, jobject value)
{
    LOGD("RCSResourceAttributes_nativeSetValueVectorOfVectorOfBoolean");
    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return;

    std::string key = env->GetStringUTFChars(jKey, NULL);
    RCSResourceAttributes setAttribute;
    std::vector<std::vector<bool>> nativeVector =  convertJavaVectorOfVectorOfBooleanToNativeVector(env,
                                value);
    setAttribute[key] = nativeVector;
    *rep = setAttribute;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfVectorOfVectorOfBoolean
(JNIEnv *env, jobject interfaceObject, jstring jKey, jobject value)
{
    LOGD("RCSResourceAttributes_nativeSetValueVectorOfVectorOfVectorOfBoolean");
    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return;

    std::string key = env->GetStringUTFChars(jKey, NULL);
    RCSResourceAttributes setAttribute;
    std::vector<std::vector<std::vector<bool>>> nativeVector =
        convertJavaVectorOfVectorOfVectorOfBooleanToNativeVector(env, value);
    setAttribute[key] = nativeVector;
    *rep = setAttribute;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueString
(JNIEnv *env, jobject interfaceObject, jstring jKey, jstring jValue)
{
    LOGD("RCSResourceAttributes_setValueString");
    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return;

    std::string key = env->GetStringUTFChars(jKey, NULL);
    std::string value = env->GetStringUTFChars(jValue, NULL);

    RCSResourceAttributes setAttribute;
    setAttribute[key] = value;
    *rep = setAttribute;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfString
(JNIEnv *env, jobject interfaceObject, jstring jKey, jobject value)
{
    LOGD("RCSResourceAttributes_nativeSetValueVectorOfString");
    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return;

    std::string key = env->GetStringUTFChars(jKey, NULL);
    RCSResourceAttributes setAttribute;
    std::vector<std::string> nativeVector =  covertJavaStringVectorToNativeVector(env, value);
    setAttribute[key] = nativeVector;
    *rep = setAttribute;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfVectorOfString
(JNIEnv *env, jobject interfaceObject, jstring jKey, jobject value)
{
    LOGD("RCSResourceAttributes_nativeSetValueVectorOfVectorOfString");
    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return;

    std::string key = env->GetStringUTFChars(jKey, NULL);
    RCSResourceAttributes setAttribute;
    std::vector<std::vector<std::string>> nativeVector =
                                           convertJavaVectorOfVectorOfStringToNativeVector(env, value);
    setAttribute[key] = nativeVector;
    *rep = setAttribute;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueVectorOfVectorOfVectorOfString
(JNIEnv *env, jobject interfaceObject, jstring jKey, jobject value)
{
    LOGD("RCSResourceAttributes_nativeSetValueVectorOfVectorOfVectorOfString");
    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return;

    std::string key = env->GetStringUTFChars(jKey, NULL);
    RCSResourceAttributes setAttribute;
    std::vector<std::vector<std::vector<std::string>>> nativeVector =
        convertJavaVectorOfVectorOfVectorOfStringToNativeVector(env, value);
    setAttribute[key] = nativeVector;
    *rep = setAttribute;
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSetValueAttributes
(JNIEnv *env, jobject interfaceObject, jstring jKey, jobject jValue)
{

    LOGD("RCSResourceAttributes_nativeSetValueAttributes");
    if (!jKey)
    {
        throwRCSException(env, "key cannot be null");
        return;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return;

    std::string key = env->GetStringUTFChars(jKey, NULL);
    RCSResourceAttributes setAttribute;
    RCSResourceAttributes *value = JniResourceAttributes::getResourceAttributePtr(env, jValue);
    if (!value) return;

    setAttribute[key] = *value;
    *rep = setAttribute;
}

JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeIsEmpty
(JNIEnv *env, jobject interfaceObject)
{
    LOGD("RCSResourceAttributes_isEmpty");
    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return false;

    return static_cast<jboolean>(rep->empty());
}

JNIEXPORT jint JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeSize
(JNIEnv *env, jobject interfaceObject)
{
    LOGD("RCSResourceAttributes_nativeSize");
    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return -1;

    return static_cast<jint>(rep->size());
}

JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeRemove
(JNIEnv *env, jobject interfaceObject, jstring jAttributeKey)
{
    LOGD("RCSResourceAttributes_nativeRemove");
    if (!jAttributeKey)
    {
        throwRCSException(env, "attributeKey cannot be null");
        return false;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return false;

    std::string attributeKey = env->GetStringUTFChars(jAttributeKey, NULL);
    return static_cast<jboolean>(rep->erase(attributeKey));
}

JNIEXPORT jboolean JNICALL
Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_nativeHasAttribute
(JNIEnv *env, jobject interfaceObject, jstring jstr)
{
    LOGD("RCSResourceAttributes_nativeHasAttribute");
    if (!jstr)
    {
        throwRCSException(env, "attributeKey cannot be null");
        return false;
    }

    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);
    if (!rep) return false;

    std::string attributeKey = env->GetStringUTFChars(jstr, NULL);
    return static_cast<jboolean>(rep->contains(attributeKey));
}

JNIEXPORT void JNICALL Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_create
(JNIEnv *env, jobject interfaceObject)
{
    LOGD("RCSResourceAttributes_create");
    RCSResourceAttributes *rep = new RCSResourceAttributes();
    RESetHandle<RCSResourceAttributes>(env, interfaceObject, rep);
    if (env->ExceptionCheck())
    {
        LOGE("Failed to set native handle for RCSResourceAttributes");
        delete rep;
    }
}

JNIEXPORT void JNICALL Java_org_iotivity_ResourceEncapsulation_common_RCSResourceAttributes_dispose
(JNIEnv *env, jobject interfaceObject, jboolean jNeedsDelete)
{
    LOGD("RCSResourceAttributes_dispose");
    RCSResourceAttributes *rep = JniResourceAttributes::getResourceAttributePtr(env, interfaceObject);

    if (jNeedsDelete)
    {
        delete rep;
    }
}
