/*
* //******************************************************************
* //
* // Copyright 2015 Intel Corporation.
* //
* //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
* //
* // Licensed under the Apache License, Version 2.0 (the "License");
* // you may not use this file except in compliance with the License.
* // You may obtain a copy of the License at
* //
* //      http://www.apache.org/licenses/LICENSE-2.0
* //
* // Unless required by applicable law or agreed to in writing, software
* // distributed under the License is distributed on an "AS IS" BASIS,
* // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* // See the License for the specific language governing permissions and
* // limitations under the License.
* //
* //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

#include "JniUtils.h"
using namespace std;

jobject JniUtils::convertStrVectorToJavaStrList(JNIEnv *env, std::vector<string> &vector)
{
    jobject jList = env->NewObject(g_cls_LinkedList, g_mid_LinkedList_ctor);
    for (int i = 0; i < vector.size(); i++)
    {
        jstring str = env->NewStringUTF(vector[i].c_str());
        env->CallBooleanMethod(jList, g_mid_LinkedList_add_object, str);
        env->DeleteLocalRef(str);
    }
    return jList;
}

void JniUtils::convertJavaStrArrToStrVector(JNIEnv *env, jobjectArray jStrArr, std::vector<string> &vector)
{
    if (!jStrArr) return;

    int len = env->GetArrayLength(jStrArr);
    for (int i = 0; i < len; i++)
    {
        jobject jObj = env->GetObjectArrayElement(jStrArr, i);
        if (!jObj) continue;

        jstring jStr = (jstring)jObj;
        vector.push_back(env->GetStringUTFChars(jStr, NULL));

        env->DeleteLocalRef(jObj);
    }
}

void JniUtils::convertJavaHeaderOptionsArrToVector(JNIEnv *env, jobjectArray jHeaderOptions, 
    OC::HeaderOptions &headerOptions)
{
    if (!jHeaderOptions) return;

    int len = env->GetArrayLength(jHeaderOptions);

    for (int i = 0; i < len; i++)
    {
        jobject header = env->GetObjectArrayElement(jHeaderOptions, i);
        jint jId = env->CallIntMethod(header, g_mid_OcHeaderOption_get_id);
        jobject jobj = (jstring)env->CallObjectMethod(header, g_mid_OcHeaderOption_get_data);
        if (!jobj) continue;
        jstring jData = (jstring)jobj;

        OC::HeaderOption::OCHeaderOption hopt(
            (int)jId,
            env->GetStringUTFChars(jData, NULL));

        headerOptions.push_back(hopt);

        env->DeleteLocalRef(header);
        env->DeleteLocalRef(jobj);
    }
}

jobject JniUtils::convertHeaderOptionsVectorToJavaList(JNIEnv *env, const OC::HeaderOptions& headerOptions)
{
    jobject jHeaderOptionList = env->NewObject(g_cls_LinkedList, g_mid_LinkedList_ctor);
    for (int i = 0; i < headerOptions.size(); i++)
    {
        jobject jHeaderOption = env->NewObject(
            g_cls_OcHeaderOption,
            g_mid_OcHeaderOption_ctor,
            (jint)headerOptions[i].getOptionID(),
            env->NewStringUTF(headerOptions[i].getOptionData().c_str())
            );

        env->CallObjectMethod(jHeaderOptionList, g_mid_LinkedList_add_object, jHeaderOption);
        env->DeleteLocalRef(jHeaderOption);
    }

    return jHeaderOptionList;
}


void JniUtils::convertJavaMapToQueryParamsMap(JNIEnv *env, jobject hashMap, OC::QueryParamsMap &map)
{
    if (!hashMap)
    {
        return;
    }

    jobject jEntrySet = env->CallObjectMethod(hashMap, g_mid_Map_entrySet);
    jobject jIterator = env->CallObjectMethod(jEntrySet, g_mid_Set_iterator);
    while (env->CallBooleanMethod(jIterator, g_mid_Iterator_hasNext))
    {
        jobject jEntry = env->CallObjectMethod(jIterator, g_mid_Iterator_next);
        jstring jKey = (jstring)env->CallObjectMethod(jEntry, g_mid_MapEntry_getKey);
        jstring jValue = (jstring)env->CallObjectMethod(jEntry, g_mid_MapEntry_getValue);

        map.insert(std::make_pair(
            env->GetStringUTFChars(jKey, NULL),
            env->GetStringUTFChars(jValue, NULL)
            ));
    }
}

jobject JniUtils::convertQueryParamsMapToJavaMap(JNIEnv *env, const OC::QueryParamsMap &map)
{
    jobject hashMap = env->NewObject(g_cls_HashMap, g_mid_HashMap_ctor);

    for (auto it = map.begin(); it != map.end(); ++it)
    {
        string key = it->first;
        string value = it->second;

        env->CallObjectMethod(hashMap, 
            g_mid_HashMap_put, 
            env->NewStringUTF(key.c_str()),
            env->NewStringUTF(value.c_str()));
    }

    return hashMap;
}

