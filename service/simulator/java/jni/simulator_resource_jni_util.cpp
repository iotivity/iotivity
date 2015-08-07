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
#include "simulator_resource_jni_util.h"

std::vector<int> convertIntegerVector(JNIEnv *env, jobject jVectorInt)
{
    std::vector<int> vectorInt;

    jclass vectorClass = env->FindClass("java/util/Vector");
    if (!vectorClass)
    {
        return vectorInt;
    }

    jmethodID size = env->GetMethodID(vectorClass, "size", "()I");
    if (NULL == size)
    {
        return vectorInt;
    }

    jmethodID get = env->GetMethodID(vectorClass, "get", "(I)""Ljava/lang/Object;");
    if (NULL == get)
    {
        return vectorInt;
    }

    jint jSize = env->CallIntMethod(jVectorInt, size);
    int sizeOfVector = jSize;

    for (int index = 0; index < sizeOfVector; index++)
    {
        jint jIndex = index;
        jint jValue = env->CallIntMethod(jVectorInt, get, jIndex);
        vectorInt.push_back((int)jValue);
    }

    return vectorInt;
}

std::vector<double> convertDoubleVector(JNIEnv *env, jobject jVectorDouble)
{
    std::vector<double> vectorDouble;

    jclass vectorClass = env->FindClass("java/util/Vector");
    if (!vectorClass)
    {
        return vectorDouble;
    }

    jmethodID size = env->GetMethodID(vectorClass, "size", "()I");
    if (NULL == size)
    {
        return vectorDouble;
    }

    jmethodID get = env->GetMethodID(vectorClass, "get", "(I)""Ljava/lang/Object;");
    if (NULL == get)
    {
        return vectorDouble;
    }

    jint jSize = env->CallIntMethod(jVectorDouble, size);
    int sizeOfVector = jSize;

    for (int index = 0; index < sizeOfVector; index++)
    {
        jint jIndex = index;
        jdouble jValue = env->CallDoubleMethod(jVectorDouble, get, jIndex);
        vectorDouble.push_back((double)jValue);
    }

    return vectorDouble;
}

std::vector<std::string> convertStringVector(JNIEnv *env, jobject jVectorString)
{
    std::vector<std::string> vectorString;

    jclass vectorClass = env->FindClass("java/util/Vector");
    if (!vectorClass)
    {
        return vectorString;
    }

    jmethodID size = env->GetMethodID(vectorClass, "size", "()I");
    if (NULL == size)
    {
        return vectorString;
    }

    jmethodID get = env->GetMethodID(vectorClass, "get", "(I)""Ljava/lang/Object;");
    if (NULL == get)
    {
        return vectorString;
    }

    jint jSize = env->CallIntMethod(jVectorString, size);
    int sizeOfVector = jSize;

    for (int index = 0; index < sizeOfVector; index++)
    {
        jint jIndex = index;
        jstring jContactInfoObj = (jstring)env->CallObjectMethod(jVectorString, get, jIndex);
        if (jContactInfoObj == NULL)
        {
            return vectorString;
        }
        const char *buff = env->GetStringUTFChars(jContactInfoObj, 0);
        if (NULL != buff)
        {
            std::string tempString = buff;
            vectorString.push_back(tempString);
        }

        env->ReleaseStringUTFChars(jContactInfoObj, buff);
    }

    return vectorString;
}
