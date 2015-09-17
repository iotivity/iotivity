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
#include "jni_re_utils.h"
#include "jni_re_resource_attributes.h"

int covertResourceStateToInt(ResourceState state)
{
    switch (state)
    {
        case ResourceState::NONE:
            return 0;

        case ResourceState::REQUESTED:
            return 1;

        case ResourceState::ALIVE:
            return 2;

        case ResourceState::LOST_SIGNAL:
            return 3;

        case ResourceState::DESTROYED:
            return 4;
    }
    return 0;
}

int convertCacheStateToInt(CacheState state)
{
    switch (state)
    {
        case CacheState::NONE:
            return 0;

        case CacheState::UNREADY:
            return 1;

        case CacheState::READY:
            return 2;

        case CacheState::LOST_SIGNAL:
            return 3;
    }
    return 0;
}

jobject convertStrVectorToJavaStrList(JNIEnv *env, std::vector<std::string> vector)
{
    jobject jList = env->NewObject(g_cls_LinkedList, g_mid_LinkedList_ctor);
    if (!jList) return nullptr;
    for (size_t i = 0; i < vector.size(); ++i)
    {
        jstring jStr = env->NewStringUTF(vector[i].c_str());
        if (!jStr) return nullptr;
        env->CallBooleanMethod(jList, g_mid_LinkedList_add_object, jStr);
        if (env->ExceptionCheck()) return nullptr;
        env->DeleteLocalRef(jStr);
    }
    return jList;
}

OCEntityHandlerResult convertIntToOCEntityHandlerResult(int result)
{
    switch (result)
    {
        case 0:
            return OCEntityHandlerResult::OC_EH_OK;
        case 1:
            return OCEntityHandlerResult::OC_EH_ERROR;
        case 2:
            return OCEntityHandlerResult::OC_EH_RESOURCE_CREATED;
        case 3:
            return OCEntityHandlerResult::OC_EH_RESOURCE_DELETED;
        case 4:
            return OCEntityHandlerResult::OC_EH_SLOW;
        case 5:
            return OCEntityHandlerResult::OC_EH_FORBIDDEN;
        case 6:
            return OCEntityHandlerResult::OC_EH_RESOURCE_NOT_FOUND;
    }
    return OCEntityHandlerResult::OC_EH_ERROR;
}

void throwRCSException(JNIEnv *env, std::string reason)
{
    jobject ex = env->NewObject(g_cls_RCSException, g_mid_RCSException_ctor,
                                env->NewStringUTF(reason.c_str()));
    if (!ex)
    {
        LOGI("throwRCSException jobject == NULL");
    }
    env->Throw((jthrowable)ex);
}

jfieldID  ResourceStateWrapper::ALIVE_ID = 0;
jfieldID  ResourceStateWrapper::REQUESTED_ID = 0;
jfieldID  ResourceStateWrapper::LOST_SIGNAL_ID = 0;
jfieldID  ResourceStateWrapper::DESTROYED_ID = 0;
jfieldID  ResourceStateWrapper::NONE_ID = 0;

jmethodID ResourceStateWrapper::ordinal_ID = 0;
jmethodID ResourceStateWrapper::toString_ID = 0;
jmethodID ResourceStateWrapper::valueOf_ID = 0;
jclass ResourceStateWrapper::clazz = 0;

jobject ResourceStateWrapper::getALIVE(JNIEnv *env)
{
    return env->GetStaticObjectField(clazz, ALIVE_ID);
}
jobject ResourceStateWrapper::getREQUESTED(JNIEnv *env)
{
    return env->GetStaticObjectField(clazz, REQUESTED_ID);
}
jobject ResourceStateWrapper::getLOST_SIGNAL(JNIEnv *env)
{
    return env->GetStaticObjectField(clazz, LOST_SIGNAL_ID);
}
jobject ResourceStateWrapper::getDESTROYED(JNIEnv *env)
{
    return env->GetStaticObjectField(clazz, DESTROYED_ID);
}
jobject ResourceStateWrapper::getNONE(JNIEnv *env)
{
    return env->GetStaticObjectField(clazz, NONE_ID);
}
jint ResourceStateWrapper::ordinal(JNIEnv *env, jobject enumObj)
{
    return env->CallIntMethod(enumObj, ordinal_ID);
}
jstring ResourceStateWrapper::toString(JNIEnv *env, jobject enumObj)
{
    return reinterpret_cast<jstring>(env->CallObjectMethod(enumObj, toString_ID));
}
jobject ResourceStateWrapper::valueOf(JNIEnv *env, jstring enumName)
{
    return env->CallStaticObjectMethod(clazz, valueOf_ID, (jobject)enumName);
}

jobject convertNativeResourceStateToJavaResourceState(ResourceState nativeResourceState,
        JNIEnv *env)
{
    LOGI("convertNativeResourceStateToJavaResourceState enter");

    ResourceStateWrapper::ALIVE_ID = env->GetStaticFieldID(ResourceStateWrapper::clazz, "ALIVE",
                   "Lorg/iotivity/ResourceEncapsulation/client/RCSRemoteResourceObject$ResourceState;");
    ResourceStateWrapper::REQUESTED_ID = env->GetStaticFieldID(ResourceStateWrapper::clazz, "REQUESTED",
                   "Lorg/iotivity/ResourceEncapsulation/client/RCSRemoteResourceObject$ResourceState;");
    ResourceStateWrapper::LOST_SIGNAL_ID = env->GetStaticFieldID(ResourceStateWrapper::clazz,
                    "LOST_SIGNAL", "Lorg/iotivity/ResourceEncapsulation/client/RCSRemoteResourceObject$ResourceState;");
    ResourceStateWrapper::DESTROYED_ID = env->GetStaticFieldID(ResourceStateWrapper::clazz, "DESTROYED",
                    "Lorg/iotivity/ResourceEncapsulation/client/RCSRemoteResourceObject$ResourceState;");
    ResourceStateWrapper::NONE_ID = env->GetStaticFieldID(ResourceStateWrapper::clazz, "NONE",
                    "Lorg/iotivity/ResourceEncapsulation/client/RCSRemoteResourceObject$ResourceState;");

    ResourceStateWrapper::ordinal_ID = env->GetMethodID(ResourceStateWrapper::clazz, "ordinal", "()I");
    ResourceStateWrapper::toString_ID = env->GetMethodID(ResourceStateWrapper::clazz, "toString",
                                        "()Ljava/lang/String;");
    ResourceStateWrapper::valueOf_ID = env->GetStaticMethodID(ResourceStateWrapper::clazz, "valueOf",
                   "(Ljava/lang/String;)Lorg/iotivity/ResourceEncapsulation/client/RCSRemoteResourceObject$ResourceState;");

    LOGI("convertNativeResourceStateToJavaResourceState ResourceStateWrapper::clazz == NULL first time");

    switch (nativeResourceState)
    {
        case ResourceState::NONE:
            return ResourceStateWrapper::getNONE(env);

        case ResourceState::REQUESTED:
            return ResourceStateWrapper::getREQUESTED(env);

        case ResourceState::ALIVE:
            return ResourceStateWrapper::getALIVE(env);

        case ResourceState::LOST_SIGNAL:
            return ResourceStateWrapper::getLOST_SIGNAL(env);

        case ResourceState::DESTROYED:
            return ResourceStateWrapper::getDESTROYED(env);
    }
}

RCSResourceObject::AutoNotifyPolicy convertJavaNotifyPolicyToNativePolicy( int policy)
{
    switch (policy)
    {
        case 0:
            return RCSResourceObject::AutoNotifyPolicy::NEVER;

        case 1:
            return RCSResourceObject::AutoNotifyPolicy::ALWAYS;

        case 2:
            return RCSResourceObject::AutoNotifyPolicy::UPDATED;
    }
}

RCSResourceObject::SetRequestHandlerPolicy convertJavasetRequestHandlerPolicyToNativePolicy(
    int policy)
{
    switch (policy)
    {
        case 0:
            return RCSResourceObject::SetRequestHandlerPolicy::NEVER;

        case 1:
            return RCSResourceObject::SetRequestHandlerPolicy::ACCEPTANCE;
    }
}

int convertAutoNotifyPolicyToInt(RCSResourceObject::AutoNotifyPolicy policy)
{
    switch (policy)
    {
        case RCSResourceObject::AutoNotifyPolicy::NEVER:
            return 0;

        case  RCSResourceObject::AutoNotifyPolicy::ALWAYS:
            return 1;

        case  RCSResourceObject::AutoNotifyPolicy::UPDATED:
            return 2;
    }
    return 0;
}

int convertSetRequestHandlerPolicyToInt(RCSResourceObject::SetRequestHandlerPolicy policy)
{
    switch (policy)
    {
        case  RCSResourceObject::SetRequestHandlerPolicy::NEVER:
            return 0;

        case  RCSResourceObject::SetRequestHandlerPolicy::ACCEPTANCE:
            return 1;
    }
    return 0;
}

void convertJavaMapToParamsMap(JNIEnv *env, jobject hashMap,
                               std::map<std::string, std::string> &params)
{
    if (!hashMap)
    {
        return;
    }

    jobject jEntrySet = env->CallObjectMethod(hashMap, g_mid_Map_entrySet);
    jobject jIterator = env->CallObjectMethod(jEntrySet, g_mid_Set_iterator);
    if (!jEntrySet || !jIterator || env->ExceptionCheck())
    {
        return;
    }

    while (env->CallBooleanMethod(jIterator, g_mid_Iterator_hasNext))
    {
        jobject jEntry = env->CallObjectMethod(jIterator, g_mid_Iterator_next);
        if (!jEntry) return;
        jstring jKey = (jstring)env->CallObjectMethod(jEntry, g_mid_MapEntry_getKey);
        if (!jKey) return;
        jstring jValue = (jstring)env->CallObjectMethod(jEntry, g_mid_MapEntry_getValue);
        if (!jValue) return;

        params.insert(std::make_pair(env->GetStringUTFChars(jKey, NULL),
                                     env->GetStringUTFChars(jValue, NULL)));

        if (env->ExceptionCheck()) return;
        env->DeleteLocalRef(jEntry);
        env->DeleteLocalRef(jKey);
        env->DeleteLocalRef(jValue);
    }
}

jobject convertStrListToJavaStrList(JNIEnv *env, std::list<std::string> &nativeList)
{
    jobject jList = env->NewObject(g_cls_LinkedList, g_mid_LinkedList_ctor);
    if (!jList) return nullptr;

    for (std::list<std::string>::iterator it = nativeList.begin(); it != nativeList.end(); ++it)
    {
        jstring jStr = env->NewStringUTF((*it).c_str());
        if (!jStr) return nullptr;
        env->CallBooleanMethod(jList, g_mid_LinkedList_add_object, jStr);
        if (env->ExceptionCheck()) return nullptr;
        env->DeleteLocalRef(jStr);
    }
    return jList;
}

std::vector<int> covertJavaIntVectorToNativeVector(JNIEnv *env, jobject javaVector)
{
    std::vector<int> vectorInt;
    LOGI("covertJavaIntVectorToNativeVector");

    jint jSize = env->CallIntMethod(javaVector, g_mid_Vector_size);
    int sizeOfVector = jSize;
    int nativeValue;
    jint jIndex;

    for (int index = 0; index < sizeOfVector; index++)
    {
        jIndex = index;
        jobject intValue = env->CallObjectMethod(javaVector, g_mid_Vector_get, jIndex);
        nativeValue = env->CallIntMethod( intValue, g_mid_Integer_getInt);
        vectorInt.push_back(nativeValue);
    }
    return vectorInt;
}

std::vector<std::vector<int>> convertJavaVectorOfVectorOfIntToNativeVector(JNIEnv *env,
                           jobject javaVector)
{
    std::vector<std::vector<int>> vectorInt;
    LOGI("covertJavaIntVectorToNativeVector");

    jint jSizeOne = env->CallIntMethod(javaVector, g_mid_Vector_size);
    int sizeOfVectorOne = jSizeOne;
    int nativeValue;
    jint jIndex1;
    jint jIndex2;

    for (int index1 = 0; index1 < sizeOfVectorOne; index1++)
    {
        jIndex1 = index1;

        jobject vectorTwoObj = env->CallObjectMethod(javaVector, g_mid_Vector_get, jIndex1);

        jint jSizeTwo = env->CallIntMethod(vectorTwoObj, g_mid_Vector_size);
        int sizeOfVectorTwo = jSizeTwo;
        std::vector<int> vectorTwo;
        for (int index2 = 0; index2 < sizeOfVectorTwo; index2++)
        {
            jIndex2 = index2;
            jobject intValue = env->CallObjectMethod(vectorTwoObj, g_mid_Vector_get, jIndex2);
            nativeValue = env->CallIntMethod( intValue, g_mid_Integer_getInt);
            vectorTwo.push_back(nativeValue);
        }
        vectorInt.push_back(vectorTwo);
    }
    return vectorInt;
}

std::vector<std::vector<std::vector<int>>> convertJavaVectorOfVectorOfVectorOfIntToNativeVector(
    JNIEnv *env, jobject javaVector)
{
    std::vector<std::vector<std::vector<int>>> vectorInt;
    LOGI("convertJavaVectorOfVectorOfVectorOfIntToNativeVector");

    jint jSizeOne = env->CallIntMethod(javaVector, g_mid_Vector_size);
    int sizeOfVectorOne = jSizeOne;
    int nativeValue;
    jint jIndex1;
    jint jIndex2;
    jint jIndex3;

    for (int index1 = 0; index1 < sizeOfVectorOne; index1++)
    {
        jIndex1 = index1;

        jobject vectorTwoObj = env->CallObjectMethod(javaVector, g_mid_Vector_get, jIndex1);

        jint jSizeTwo = env->CallIntMethod(vectorTwoObj, g_mid_Vector_size);
        int sizeOfVectorTwo = jSizeTwo;
        std::vector<std::vector<int>> vectorTwo;
        for (int index2 = 0; index2 < sizeOfVectorTwo; index2++)
        {
            jIndex2 = index2;

            jobject vectorThreeObj = env->CallObjectMethod(vectorTwoObj, g_mid_Vector_get, jIndex2);

            jint jSizeThree = env->CallIntMethod(vectorThreeObj, g_mid_Vector_size);
            int sizeOfVectorThree = jSizeThree;
            std::vector<int> vectorThree;
            for (int index3 = 0; index3 < sizeOfVectorTwo; index3++)
            {
                jIndex3 = index3;
                jobject intValue = env->CallObjectMethod(vectorThreeObj, g_mid_Vector_get, jIndex3);
                nativeValue = env->CallIntMethod( intValue, g_mid_Integer_getInt);
                vectorThree.push_back(nativeValue);
            }
            vectorTwo.push_back(vectorThree);
        }
        vectorInt.push_back(vectorTwo);
    }
    return vectorInt;
}

jobject convertNativeIntVectorToJavaVector(JNIEnv *env, std::vector<int> &nativeVector)
{
    LOGI("convertNativeIntVectorToJavaVector");
    jobject javaVector = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);

    if (!javaVector) return nullptr;

    for (std::vector<int>::iterator it = nativeVector.begin(); it != nativeVector.end(); ++it)
    {
        jint jInt = (jint)(*it);
        jobject value = env->NewObject(g_cls_Integer, g_mid_Integer_ctor, jInt);
        env->CallBooleanMethod(javaVector, g_mid_Vector_add, value);
    }
    return javaVector;
}

jobject convertNativeVectorOfVectorOfIntToJavaVector(JNIEnv *env,
        std::vector<std::vector<int>> &nativeVector)
{
    LOGI("convertNativeVectorOfVectorOfIntToJavaVector");
    jobject javaVector1 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);

    if (!javaVector1) return nullptr;

    std::vector<std::vector<int>>::iterator firstIt;
    std::vector<int>::iterator secondIt;

    jobject javaVector2 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);

    for (firstIt = nativeVector.begin(); firstIt != nativeVector.end(); firstIt++)
    {
        jobject javaVector1 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);
        for (secondIt = firstIt->begin(); secondIt != firstIt->end(); secondIt++)
        {
            jint jInt = (jint)(*secondIt);
            jobject value = env->NewObject(g_cls_Integer, g_mid_Integer_ctor, jInt);
            env->CallBooleanMethod(javaVector1, g_mid_Vector_add, value);
        }
        env->CallBooleanMethod(javaVector2, g_mid_Vector_add, javaVector1);
    }
    return javaVector2;
}

jobject convertNativeVectorOfVectorOfVectorOfIntToJavaVector(JNIEnv *env,
        std::vector<std::vector<std::vector<int>>> &nativeVector)
{

    LOGI("convertNativeVectorOfVectorOfVectorOfIntToJavaVector");
    jobject javaVector1 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);

    std::vector<std::vector<std::vector<int>>>::iterator firstIt;
    std::vector<std::vector<int>>::iterator secondIt;
    std::vector<int>::iterator thirdIt;

    jobject javaVector3 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);

    for (firstIt = nativeVector.begin(); firstIt != nativeVector.end(); firstIt++)
    {
        jobject javaVector1 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);
        for (secondIt = firstIt->begin(); secondIt != firstIt->end(); secondIt++)
        {
            jobject javaVector2 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);
            for (thirdIt = secondIt->begin(); thirdIt != secondIt->end(); thirdIt++)
            {
                jint jInt = (jint)(*thirdIt);
                jobject value = env->NewObject(g_cls_Integer, g_mid_Integer_ctor, jInt);
                env->CallBooleanMethod(javaVector2, g_mid_Vector_add, value);
            }
            env->CallBooleanMethod(javaVector1, g_mid_Vector_add, javaVector2);
        }
        env->CallBooleanMethod(javaVector3, g_mid_Vector_add, javaVector1);
    }
    return javaVector3;
}


//double
std::vector<double> covertJavaDoubleVectorToNativeVector(JNIEnv *env, jobject javaVector)
{
    std::vector<double> vectorDouble;
    LOGI("covertJavaDoubleVectorToNativeVector");

    jint jSize = env->CallIntMethod(javaVector, g_mid_Vector_size);
    int sizeOfVector = jSize;
    double nativeValue;
    jint jIndex;

    for (int index = 0; index < sizeOfVector; index++)
    {
        jIndex = index;
        jobject doubleValue = env->CallObjectMethod(javaVector, g_mid_Vector_get, jIndex);
        nativeValue = env->CallDoubleMethod( doubleValue, g_mid_Double_getDouble);

        vectorDouble.push_back(nativeValue);
    }
    return vectorDouble;
}

std::vector<std::vector<double>> convertJavaVectorOfVectorOfDoubleToNativeVector(JNIEnv *env,
                              jobject javaVector)
{

    std::vector<std::vector<double>> vectorDouble;
    LOGI("convertJavaVectorOfVectorOfDoubleToNativeVector");

    jint jSizeOne = env->CallIntMethod(javaVector, g_mid_Vector_size);
    int sizeOfVectorOne = jSizeOne;
    double nativeValue;
    jint jIndex1;
    jint jIndex2;

    for (int index1 = 0; index1 < sizeOfVectorOne; index1++)
    {
        jIndex1 = index1;

        jobject vectorTwoObj = env->CallObjectMethod(javaVector, g_mid_Vector_get, jIndex1);

        jint jSizeTwo = env->CallIntMethod(vectorTwoObj, g_mid_Vector_size);
        int sizeOfVectorTwo = jSizeTwo;
        std::vector<double> vectorTwo;
        for (int index2 = 0; index2 < sizeOfVectorTwo; index2++)
        {
            jIndex2 = index2;
            jobject doubleValue = env->CallObjectMethod(vectorTwoObj, g_mid_Vector_get, jIndex2);
            nativeValue = env->CallDoubleMethod( doubleValue, g_mid_Double_getDouble);
            vectorTwo.push_back(nativeValue);
        }
        vectorDouble.push_back(vectorTwo);
    }
    return vectorDouble;
}

std::vector<std::vector<std::vector<double>>>
convertJavaVectorOfVectorOfVectorOfDoubleToNativeVector(JNIEnv *env, jobject javaVector)
{

    std::vector<std::vector<std::vector<double>>> vectorDouble;
    LOGI("convertJavaVectorOfVectorOfVectorOfDoubleToNativeVector");

    jint jSizeOne = env->CallIntMethod(javaVector, g_mid_Vector_size);
    int sizeOfVectorOne = jSizeOne;
    double nativeValue;
    jint jIndex1;
    jint jIndex2;
    jint jIndex3;

    for (int index1 = 0; index1 < sizeOfVectorOne; index1++)
    {
        jIndex1 = index1;

        jobject vectorTwoObj = env->CallObjectMethod(javaVector, g_mid_Vector_get, jIndex1);

        jint jSizeTwo = env->CallIntMethod(vectorTwoObj, g_mid_Vector_size);
        int sizeOfVectorTwo = jSizeTwo;
        std::vector<std::vector<double>> vectorTwo;
        for (int index2 = 0; index2 < sizeOfVectorTwo; index2++)
        {
            jIndex2 = index2;

            jobject vectorThreeObj = env->CallObjectMethod(vectorTwoObj, g_mid_Vector_get, jIndex2);

            jint jSizeThree = env->CallIntMethod(vectorThreeObj, g_mid_Vector_size);
            int sizeOfVectorThree = jSizeThree;
            std::vector<double> vectorThree;
            for (int index3 = 0; index3 < sizeOfVectorTwo; index3++)
            {
                jIndex3 = index3;

                jobject doubleValue = env->CallObjectMethod(vectorThreeObj, g_mid_Vector_get, jIndex3);
                nativeValue = env->CallDoubleMethod( doubleValue, g_mid_Double_getDouble);

                vectorThree.push_back(nativeValue);
            }
            vectorTwo.push_back(vectorThree);
        }
        vectorDouble.push_back(vectorTwo);
    }
    return vectorDouble;
}

jobject convertNativeDoubleVectorToJavaVector(JNIEnv *env, std::vector<double> &nativeVector)
{
    LOGI("convertNativeIntVectorToJavaVector");
    jobject javaVector = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);
    if (!javaVector) return nullptr;

    for (std::vector<double>::iterator it = nativeVector.begin(); it != nativeVector.end(); ++it)
    {
        jdouble jDouble = (jdouble)(*it);
        jobject value = env->NewObject(g_cls_Double, g_mid_Double_ctor, jDouble);
        env->CallBooleanMethod(javaVector, g_mid_Vector_add, value);
    }
    return javaVector;
}

jobject convertNativeVectorOfVectorOfDoubleToJavaVector(JNIEnv *env,
        std::vector<std::vector<double>> &nativeVector)
{
    LOGI("convertNativeVectorOfVectorOfIntToJavaVector");

    jobject javaVector1 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);
    if (!javaVector1) return nullptr;

    std::vector<std::vector<double>>::iterator firstIt;
    std::vector<double>::iterator secondIt;

    jobject javaVector2 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);

    for (firstIt = nativeVector.begin(); firstIt != nativeVector.end(); firstIt++)
    {
        jobject javaVector1 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);
        for (secondIt = firstIt->begin(); secondIt != firstIt->end(); secondIt++)
        {
            jdouble jDouble = (jdouble)(*secondIt);
            jobject value = env->NewObject(g_cls_Double, g_mid_Double_ctor, jDouble);
            env->CallBooleanMethod(javaVector1, g_mid_Vector_add, value);
        }
        env->CallBooleanMethod(javaVector2, g_mid_Vector_add, javaVector1);
    }
    return javaVector2;
}

jobject convertNativeVectorOfVectorOfVectorOfDoubleToJavaVector(JNIEnv *env,
        std::vector<std::vector<std::vector<double>>> &nativeVector)
{
    LOGI("convertNativeVectorOfVectorOfVectorOfDoubleToJavaVector");
    jobject javaVector1 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);

    std::vector<std::vector<std::vector<double>>>::iterator firstIt;
    std::vector<std::vector<double>>::iterator secondIt;
    std::vector<double>::iterator thirdIt;

    jobject javaVector3 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);

    for (firstIt = nativeVector.begin(); firstIt != nativeVector.end(); firstIt++)
    {
        jobject javaVector1 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);
        for (secondIt = firstIt->begin(); secondIt != firstIt->end(); secondIt++)
        {
            jobject javaVector2 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);
            for (thirdIt = secondIt->begin(); thirdIt != secondIt->end(); thirdIt++)
            {
                jdouble jDouble = (jdouble)(*thirdIt);
                jobject value = env->NewObject(g_cls_Double, g_mid_Double_ctor, jDouble);
                env->CallBooleanMethod(javaVector2, g_mid_Vector_add, value);
            }
            env->CallBooleanMethod(javaVector1, g_mid_Vector_add, javaVector2);
        }
        env->CallBooleanMethod(javaVector3, g_mid_Vector_add, javaVector1);
    }
    return javaVector3;
}

//booean
std::vector<bool> covertJavaBooleanVectorToNativeVector(JNIEnv *env, jobject javaVector)
{
    std::vector<bool> vectorBoolean;
    LOGI("covertJavaBooleanVectorToNativeVector");

    jint jSize = env->CallIntMethod(javaVector, g_mid_Vector_size);
    int sizeOfVector = jSize;
    bool nativeValue;
    jint jIndex;

    for (int index = 0; index < sizeOfVector; index++)
    {
        jIndex = index;
        jobject boolValue = env->CallObjectMethod(javaVector, g_mid_Vector_get, jIndex);
        nativeValue = env->CallBooleanMethod( boolValue, g_mid_Boolean_getBoolean);
        vectorBoolean.push_back(nativeValue);
    }
    return vectorBoolean;
}

std::vector<std::vector<bool>> convertJavaVectorOfVectorOfBooleanToNativeVector(JNIEnv *env,
                            jobject javaVector)
{
    std::vector<std::vector<bool>> vectorBoolean;
    LOGI("convertJavaVectorOfVectorOfBooleanToNativeVector");

    jint jSizeOne = env->CallIntMethod(javaVector, g_mid_Vector_size);
    int sizeOfVectorOne = jSizeOne;
    bool nativeValue;
    jint jIndex1;
    jint jIndex2;

    for (int index1 = 0; index1 < sizeOfVectorOne; index1++)
    {
        jIndex1 = index1;

        jobject vectorTwoObj = env->CallObjectMethod(javaVector, g_mid_Vector_get, jIndex1);

        jint jSizeTwo = env->CallIntMethod(vectorTwoObj, g_mid_Vector_size);
        int sizeOfVectorTwo = jSizeTwo;
        std::vector<bool> vectorTwo;
        for (int index2 = 0; index2 < sizeOfVectorTwo; index2++)
        {
            jIndex2 = index2;
            jobject boolValue = env->CallObjectMethod(vectorTwoObj, g_mid_Vector_get, jIndex2);
            nativeValue = env->CallBooleanMethod( boolValue, g_mid_Boolean_getBoolean);
            vectorTwo.push_back(nativeValue);
        }
        vectorBoolean.push_back(vectorTwo);
    }
    return vectorBoolean;
}

std::vector<std::vector<std::vector<bool>>>
convertJavaVectorOfVectorOfVectorOfBooleanToNativeVector(JNIEnv *env, jobject javaVector)
{
    std::vector<std::vector<std::vector<bool>>> vectorBoolean;
    LOGI("convertJavaVectorOfVectorOfVectorOfDoubleToNativeVector");

    jint jSizeOne = env->CallIntMethod(javaVector, g_mid_Vector_size);
    int sizeOfVectorOne = jSizeOne;
    bool nativeValue;
    jint jIndex1;
    jint jIndex2;
    jint jIndex3;

    for (int index1 = 0; index1 < sizeOfVectorOne; index1++)
    {
        jIndex1 = index1;

        jobject vectorTwoObj = env->CallObjectMethod(javaVector, g_mid_Vector_get, jIndex1);

        jint jSizeTwo = env->CallIntMethod(vectorTwoObj, g_mid_Vector_size);
        int sizeOfVectorTwo = jSizeTwo;
        std::vector<std::vector<bool>> vectorTwo;
        for (int index2 = 0; index2 < sizeOfVectorTwo; index2++)
        {
            jIndex2 = index2;

            jobject vectorThreeObj = env->CallObjectMethod(vectorTwoObj, g_mid_Vector_get, jIndex2);

            jint jSizeThree = env->CallIntMethod(vectorThreeObj, g_mid_Vector_size);
            int sizeOfVectorThree = jSizeThree;
            std::vector<bool> vectorThree;
            for (int index3 = 0; index3 < sizeOfVectorTwo; index3++)
            {
                jIndex3 = index3;

                jobject boolValue = env->CallObjectMethod(vectorThreeObj, g_mid_Vector_get, jIndex3);
                nativeValue = env->CallBooleanMethod( boolValue, g_mid_Boolean_getBoolean);

                vectorThree.push_back(nativeValue);
            }
            vectorTwo.push_back(vectorThree);
        }
        vectorBoolean.push_back(vectorTwo);
    }
    return vectorBoolean;
}

jobject convertNativeBooleanVectorToJavaVector(JNIEnv *env, std::vector<bool> &nativeVector)
{
    LOGI("convertNativeBooleanVectorToJavaVector");

    jobject javaVector = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);
    if (!javaVector) return nullptr;

    for (std::vector<bool>::iterator it = nativeVector.begin(); it != nativeVector.end(); ++it)
    {
        jboolean jBoolean = (jboolean)(*it);
        jobject value = env->NewObject(g_cls_Boolean, g_mid_Boolean_ctor, jBoolean);
        env->CallBooleanMethod(javaVector, g_mid_Vector_add, value);
    }
    return javaVector;
}

jobject convertNativeVectorOfVectorOfBooleanToJavaVector(JNIEnv *env,
        std::vector<std::vector<bool>> &nativeVector)
{
    LOGI("convertNativeVectorOfVectorOfBooleanToJavaVector");

    jobject javaVector1 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);
    if (!javaVector1) return nullptr;

    std::vector<std::vector<bool>>::iterator firstIt;
    std::vector<bool>::iterator secondIt;

    jobject javaVector2 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);

    for (firstIt = nativeVector.begin(); firstIt != nativeVector.end(); firstIt++)
    {
        jobject javaVector1 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);
        for (secondIt = firstIt->begin(); secondIt != firstIt->end(); secondIt++)
        {
            jboolean jBoolean = (jboolean)(*secondIt);
            jobject value = env->NewObject(g_cls_Boolean, g_mid_Boolean_ctor, jBoolean);
            env->CallBooleanMethod(javaVector1, g_mid_Vector_add, value);
        }
        env->CallBooleanMethod(javaVector2, g_mid_Vector_add, javaVector1);
    }
    return javaVector2;
}

jobject convertNativeVectorOfVectorOfVectorOfBooleanToJavaVector(JNIEnv *env,
        std::vector<std::vector<std::vector<bool>>> &nativeVector)
{
    LOGI("convertNativeVectorOfVectorOfVectorOfBooleanToJavaVector");
    jobject javaVector1 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);

    std::vector<std::vector<std::vector<bool>>>::iterator firstIt;
    std::vector<std::vector<bool>>::iterator secondIt;
    std::vector<bool>::iterator thirdIt;

    jobject javaVector3 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);

    for (firstIt = nativeVector.begin(); firstIt != nativeVector.end(); firstIt++)
    {
        jobject javaVector1 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);
        for (secondIt = firstIt->begin(); secondIt != firstIt->end(); secondIt++)
        {
            jobject javaVector2 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);
            for (thirdIt = secondIt->begin(); thirdIt != secondIt->end(); thirdIt++)
            {
                jboolean jBoolean = (jboolean)(*thirdIt);
                jobject value = env->NewObject(g_cls_Boolean, g_mid_Boolean_ctor, jBoolean);
                env->CallBooleanMethod(javaVector2, g_mid_Vector_add, value);
            }
            env->CallBooleanMethod(javaVector1, g_mid_Vector_add, javaVector2);
        }
        env->CallBooleanMethod(javaVector3, g_mid_Vector_add, javaVector1);
    }
    return javaVector3;
}

//String
std::vector<std::string> covertJavaStringVectorToNativeVector(JNIEnv *env, jobject javaVector)
{
    std::vector<std::string> vectorString;
    LOGI("covertJavaBooleanVectorToNativeVector");

    jint jSize = env->CallIntMethod(javaVector, g_mid_Vector_size);
    int sizeOfVector = jSize;
    std::string nativeValue;
    jint jIndex;

    for (int index = 0; index < sizeOfVector; index++)
    {
        jIndex = index;
        jstring StringValue = (jstring)env->CallObjectMethod(javaVector, g_mid_Vector_get, jIndex);
        nativeValue = env->GetStringUTFChars(StringValue, NULL);
        vectorString.push_back(nativeValue);
    }
    return vectorString;
}

std::vector<std::vector<std::string>> convertJavaVectorOfVectorOfStringToNativeVector(JNIEnv *env,
                                   jobject javaVector)
{
    std::vector<std::vector<std::string>> vectorString;
    LOGI("convertJavaVectorOfVectorOfBooleanToNativeVector");

    jint jSizeOne = env->CallIntMethod(javaVector, g_mid_Vector_size);
    int sizeOfVectorOne = jSizeOne;
    std::string nativeValue;
    jint jIndex1;
    jint jIndex2;

    for (int index1 = 0; index1 < sizeOfVectorOne; index1++)
    {
        jIndex1 = index1;

        jobject vectorTwoObj = env->CallObjectMethod(javaVector, g_mid_Vector_get, jIndex1);

        jint jSizeTwo = env->CallIntMethod(vectorTwoObj, g_mid_Vector_size);
        int sizeOfVectorTwo = jSizeTwo;
        std::vector<std::string> vectorTwo;
        for (int index2 = 0; index2 < sizeOfVectorTwo; index2++)
        {
            jIndex2 = index2;
            jstring StringValue = (jstring)env->CallObjectMethod(vectorTwoObj, g_mid_Vector_get, jIndex2);
            nativeValue = env->GetStringUTFChars(StringValue, NULL);
            vectorTwo.push_back(nativeValue);
        }
        vectorString.push_back(vectorTwo);
    }
    return vectorString;
}

std::vector<std::vector<std::vector<std::string>>>
convertJavaVectorOfVectorOfVectorOfStringToNativeVector(JNIEnv *env, jobject javaVector)
{
    std::vector<std::vector<std::vector<std::string>>> vectorString;
    LOGI("convertJavaVectorOfVectorOfVectorOfDoubleToNativeVector");

    jint jSizeOne = env->CallIntMethod(javaVector, g_mid_Vector_size);
    int sizeOfVectorOne = jSizeOne;
    std::string nativeValue;
    jint jIndex1;
    jint jIndex2;
    jint jIndex3;

    for (int index1 = 0; index1 < sizeOfVectorOne; index1++)
    {
        jIndex1 = index1;
        jobject vectorTwoObj = env->CallObjectMethod(javaVector, g_mid_Vector_get, jIndex1);
        jint jSizeTwo = env->CallIntMethod(vectorTwoObj, g_mid_Vector_size);
        int sizeOfVectorTwo = jSizeTwo;
        std::vector<std::vector<std::string>> vectorTwo;
        for (int index2 = 0; index2 < sizeOfVectorTwo; index2++)
        {
            jIndex2 = index2;

            jobject vectorThreeObj = env->CallObjectMethod(vectorTwoObj, g_mid_Vector_get, jIndex2);

            jint jSizeThree = env->CallIntMethod(vectorThreeObj, g_mid_Vector_size);
            int sizeOfVectorThree = jSizeThree;
            std::vector<std::string> vectorThree;
            for (int index3 = 0; index3 < sizeOfVectorTwo; index3++)
            {
                jIndex3 = index3;

                jstring StringValue = (jstring)env->CallObjectMethod(vectorThreeObj, g_mid_Vector_get, jIndex3);
                nativeValue = env->GetStringUTFChars(StringValue, NULL);

                vectorThree.push_back(nativeValue);
            }
            vectorTwo.push_back(vectorThree);
        }
        vectorString.push_back(vectorTwo);
    }
    return vectorString;
}

jobject convertNativeStringVectorToJavaVector(JNIEnv *env, std::vector<std::string> &nativeVector)
{
    LOGI("convertNativeStringVectorToJavaVector");

    jobject javaVector = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);
    if (!javaVector) return nullptr;

    for (std::vector<std::string>::iterator it = nativeVector.begin(); it != nativeVector.end(); ++it)
    {
        env->CallBooleanMethod(javaVector, g_mid_Vector_add, env -> NewStringUTF((*it).c_str()));
    }
    return javaVector;
}

jobject convertNativeVectorOfVectorOfStringToJavaVector(JNIEnv *env,
        std::vector<std::vector<std::string>> &nativeVector)
{
    LOGI("convertNativeVectorOfVectorOfBooleanToJavaVector");

    jobject javaVector1 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);
    if (!javaVector1) return nullptr;

    std::vector<std::vector<std::string>>::iterator firstIt;
    std::vector<std::string>::iterator secondIt;

    jobject javaVector2 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);

    for (firstIt = nativeVector.begin(); firstIt != nativeVector.end(); firstIt++)
    {
        jobject javaVector1 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);
        for (secondIt = firstIt->begin(); secondIt != firstIt->end(); secondIt++)
        {
            env->CallBooleanMethod(javaVector1, g_mid_Vector_add, env -> NewStringUTF((*secondIt).c_str()));
        }
        env->CallBooleanMethod(javaVector2, g_mid_Vector_add, javaVector1);
    }
    return javaVector2;
}

jobject convertNativeVectorOfVectorOfVectorOfStringToJavaVector(JNIEnv *env,
        std::vector<std::vector<std::vector<std::string>>> &nativeVector)
{
    LOGI("convertNativeVectorOfVectorOfVectorOfStringToJavaVector");
    jobject javaVector1 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);

    std::vector<std::vector<std::vector<std::string>>>::iterator firstIt;
    std::vector<std::vector<std::string>>::iterator secondIt;
    std::vector<std::string>::iterator thirdIt;

    jobject javaVector3 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);

    for (firstIt = nativeVector.begin(); firstIt != nativeVector.end(); firstIt++)
    {
        jobject javaVector1 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);
        for (secondIt = firstIt->begin(); secondIt != firstIt->end(); secondIt++)
        {
            jobject javaVector2 = env->NewObject(g_cls_Vector, g_mid_Vector_ctor);
            for (thirdIt = secondIt->begin(); thirdIt != secondIt->end(); thirdIt++)
            {
                env->CallBooleanMethod(javaVector2, g_mid_Vector_add, env -> NewStringUTF((*thirdIt).c_str()));
            }
            env->CallBooleanMethod(javaVector1, g_mid_Vector_add, javaVector2);
        }
        env->CallBooleanMethod(javaVector3, g_mid_Vector_add, javaVector1);
    }
    return javaVector3;
}