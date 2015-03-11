//******************************************************************
//
// Copyright 2014 MediaTek All Rights Reserved.
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
/*
* //******************************************************************
* //
* // Portions Copyright 2015 Intel Corporation.
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
#include "JniOcRepresentation.h"

using namespace OC;
using namespace std;



OCRepresentation* JniOcRepresentation::getOCRepresentationPtr(JNIEnv *env, jobject thiz)
{
    OCRepresentation *rep = GetHandle<OCRepresentation>(env, thiz);
    if (env->ExceptionCheck())
    {
        LOGE("Failed to get native handle from OcRepresentation");
    }
    if (!rep)
    {
        ThrowOcException(JNI_NO_NATIVE_POINTER, "");
    }
    return rep;
}

/*
* Class:     org_iotivity_base_OcRepresentation
* Method:    getValueInt
* Signature: (Ljava/lang/String;)I
*/
JNIEXPORT jint JNICALL Java_org_iotivity_base_OcRepresentation_getValueInt
(JNIEnv *env, jobject thiz, jstring jstr)
{
    LOGD("OcRepresentation_getValueInt");
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);
    string str = env->GetStringUTFChars(jstr, NULL);
    int val;
    rep->getValue(str.c_str(), val);
    return val;
}

/*
* Class:     org_iotivity_base_OcRepresentation
* Method:    setValueInt
* Signature: (Ljava/lang/String;I)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcRepresentation_setValueInt
(JNIEnv *env, jobject thiz, jstring jstr, jint jval)
{
    LOGD("OcRepresentation_setValueInt");
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);
    string str = env->GetStringUTFChars(jstr, NULL);

    rep->setValue(str, (int)jval);
}

/*
* Class:     org_iotivity_base_OcRepresentation
* Method:    getValueBool
* Signature: (Ljava/lang/String;)Z
*/
JNIEXPORT jboolean JNICALL Java_org_iotivity_base_OcRepresentation_getValueBool
(JNIEnv *env, jobject thiz, jstring jstr)
{
    LOGD("OcRepresentation_getValueBool");
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);
    string str = env->GetStringUTFChars(jstr, NULL);
    bool val;
    rep->getValue(str, val);
    return((jboolean)val);
}

/*
* Class:     org_iotivity_base_OcRepresentation
* Method:    getValueString
* Signature: (Ljava/lang/String;)Ljava/lang/String;
*/
JNIEXPORT jstring JNICALL Java_org_iotivity_base_OcRepresentation_getValueString
(JNIEnv *env, jobject thiz, jstring jstr)
{
    LOGD("OcRepresentation_getValueString");
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);
    string str = env->GetStringUTFChars(jstr, NULL);
    string get_val;
    rep->getValue(str, get_val);
    return env->NewStringUTF(get_val.c_str());
}

/*
* Class:     org_iotivity_base_OcRepresentation
* Method:    setValueBool
* Signature: (Ljava/lang/String;Z)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcRepresentation_setValueBool
(JNIEnv *env, jobject thiz, jstring jstr, jboolean jval)
{
    LOGD("OcRepresentation_setValueBool");
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);
    string str = env->GetStringUTFChars(jstr, NULL);
    rep->setValue(str, (bool)jval);
}

/*
* Class:     org_iotivity_base_OcRepresentation
* Method:    setValueString
* Signature: (Ljava/lang/String;Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcRepresentation_setValueString
(JNIEnv *env, jobject thiz, jstring jstr, jstring jval)
{
    LOGD("OcRepresentation_setValueString");
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);
    string str = env->GetStringUTFChars(jstr, NULL);
    string val = env->GetStringUTFChars(jval, NULL);

    rep->setValue(str, val);
}

JNIEXPORT jstring JNICALL Java_org_iotivity_base_OcRepresentation_getUri
(JNIEnv *env, jobject thiz)
{
    LOGD("OcRepresentation_getUri");
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);
    string uri(rep->getUri());
    return env->NewStringUTF(uri.c_str());
}

/*
* Class:     org_iotivity_base_OcRepresentation
* Method:    setUri
* Signature: (Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcRepresentation_setUri
(JNIEnv *env, jobject thiz, jstring jUri)
{
    LOGD("OcRepresentation_setUri");
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);
    rep->setUri(env->GetStringUTFChars(jUri, NULL));
}

JNIEXPORT jboolean JNICALL Java_org_iotivity_base_OcRepresentation_hasAttribute
(JNIEnv *env, jobject thiz, jstring jstr)
{
    LOGD("OcRepresentation_hasAttribute");
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);
    string str = env->GetStringUTFChars(jstr, NULL);
    return rep->hasAttribute(str);
}

/*
* Class:     org_iotivity_base_OcRepresentation
* Method:    create
* Signature: ()V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcRepresentation_create
(JNIEnv *env, jobject thiz)
{
    LOGD("OcRepresentation_create");

    OCRepresentation *rep = new OCRepresentation();

    SetHandle<OCRepresentation>(env, thiz, rep);
    if (env->ExceptionCheck())
    {
        LOGE("Failed to set native handle to OcRepresentation");

        if (rep)
        {
            delete rep;
        }
    }
}

/*
* Class:     org_iotivity_base_OcRepresentation
* Method:    dispose
* Signature: ()V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcRepresentation_dispose
(JNIEnv *env, jobject thiz, jboolean jNeedsDelete)
{
    LOGD("OcRepresentation_dispose");
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);

    if (env->ExceptionCheck())
    {
        LOGE("Failed to get native handle from OcRepresentation");
    }

    if (jNeedsDelete)
    {
        delete rep;
    }
}
