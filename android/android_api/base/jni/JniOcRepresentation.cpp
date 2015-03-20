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
#include "JniUtils.h"

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
    if (!jstr)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "attributeKey cannot be null");
        return -1;
    }
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
    if (!jstr)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "attributeKey cannot be null");
        return;
    }
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);

    string str = env->GetStringUTFChars(jstr, NULL);
    rep->setValue(str, static_cast<int>(jval));
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
    if (!jstr)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "attributeKey cannot be null");
        return false;
    }
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);

    string str = env->GetStringUTFChars(jstr, NULL);
    bool value;
    rep->getValue(str, value);
    return static_cast<jboolean>(value);
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
    if (!jstr)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "attributeKey cannot be null");
        return NULL;
    }
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
    if (!jstr)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "attributeKey cannot be null");
        return;
    }
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);

    string str = env->GetStringUTFChars(jstr, NULL);
    rep->setValue(str, static_cast<jboolean>(jval));
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
    if (!jstr)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "attributeKey cannot be null");
        return;
    }
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);

    string str = env->GetStringUTFChars(jstr, NULL);
    string val = env->GetStringUTFChars(jval, NULL);

    rep->setValue(str, val);
}

/*
* Class:     org_iotivity_base_OcRepresentation
* Method:    getJSONRepresentation
* Signature: ()Ljava/lang/String;
*/
JNIEXPORT jstring JNICALL Java_org_iotivity_base_OcRepresentation_getJSONRepresentation
(JNIEnv *env, jobject thiz)
{
    LOGD("OcRepresentation_getJSONRepresentation");
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);
  
    string jsonStr = rep->getJSONRepresentation();
    return env->NewStringUTF(jsonStr.c_str());
}

/*
* Class:     org_iotivity_base_OcRepresentation
* Method:    addChild
* Signature: (Lorg/iotivity/base/OcRepresentation;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcRepresentation_addChild
(JNIEnv *env, jobject thiz, jobject jOcRepresentation)
{
    LOGD("OcRepresentation_addChild");
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);
    OCRepresentation *child = JniOcRepresentation::getOCRepresentationPtr(env, jOcRepresentation);

    rep->addChild(*child);
}

/*
* Class:     org_iotivity_base_OcRepresentation
* Method:    clearChildren
* Signature: ()V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcRepresentation_clearChildren
(JNIEnv *env, jobject thiz)
{
    LOGD("OcRepresentation_clearChildren");
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);

    rep->clearChildren();
}

/*
* Class:     org_iotivity_base_OcRepresentation
* Method:    getChildrenArray
* Signature: ()[Lorg/iotivity/base/OcRepresentation;
*/
JNIEXPORT jobjectArray JNICALL Java_org_iotivity_base_OcRepresentation_getChildrenArray
(JNIEnv *env, jobject thiz)
{
    LOGD("OcRepresentation_getChildrenArray");
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);
   
    return JniUtils::convertRepresentationVectorToJavaArray(env, rep->getChildren());
}

/*
* Class:     org_iotivity_base_OcRepresentation
* Method:    getUri
* Signature: ()Ljava/lang/String;
*/
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
    if (!jUri)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "uri cannot be null");
        return;
    }
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);
    rep->setUri(env->GetStringUTFChars(jUri, NULL));
}

JNIEXPORT jboolean JNICALL Java_org_iotivity_base_OcRepresentation_hasAttribute
(JNIEnv *env, jobject thiz, jstring jstr)
{
    LOGD("OcRepresentation_hasAttribute");
    if (!jstr)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "attributeKey cannot be null");
        return false;
    }
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);

    string str = env->GetStringUTFChars(jstr, NULL);
    return rep->hasAttribute(str);
}


/*
* Class:     org_iotivity_base_OcRepresentation
* Method:    getResourceTypes
* Signature: ()Ljava/util/List;
*/
JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcRepresentation_getResourceTypes
(JNIEnv *env, jobject thiz)
{
    LOGD("OcRepresentation_getResourceTypes");
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);

    std::vector<string> resourceTypes = rep->getResourceTypes();
    return JniUtils::convertStrVectorToJavaStrList(env, resourceTypes);
}

/*
* Class:     org_iotivity_base_OcRepresentation
* Method:    setResourceTypeArray
* Signature: ([Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcRepresentation_setResourceTypeArray
(JNIEnv *env, jobject thiz, jobjectArray jResourceTypeArray)
{
    LOGD("OcRepresentation_setResourceTypeArray");
    if (!jResourceTypeArray)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceTypeList cannot be null");
        return;
    }
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);

    std::vector<std::string> resourceTypes;
    JniUtils::convertJavaStrArrToStrVector(env, jResourceTypeArray, resourceTypes);
    rep->setResourceTypes(resourceTypes);
}
/*
* Class:     org_iotivity_base_OcRepresentation
* Method:    getResourceInterfaces
* Signature: ()Ljava/util/List;
*/
JNIEXPORT jobject JNICALL Java_org_iotivity_base_OcRepresentation_getResourceInterfaces
(JNIEnv *env, jobject thiz)
{
    LOGD("OcRepresentation_getResourceInterfaces");
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);

    std::vector<string> resourceInterfaces = rep->getResourceInterfaces();
    return JniUtils::convertStrVectorToJavaStrList(env, resourceInterfaces);
}

/*
* Class:     org_iotivity_base_OcRepresentation
* Method:    setResourceInterfaceArray
* Signature: ([Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcRepresentation_setResourceInterfaceArray
(JNIEnv *env, jobject thiz, jobjectArray jResourceInterfaceArray)
{
    LOGD("OcRepresentation_setResourceInterfaceArray");
    if (!jResourceInterfaceArray)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "resourceInterfaceList cannot be null");
        return;
    }
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);

    std::vector<std::string> resourceInterfaces;
    JniUtils::convertJavaStrArrToStrVector(env, jResourceInterfaceArray, resourceInterfaces);
    rep->setResourceInterfaces(resourceInterfaces);
}

/*
* Class:     org_iotivity_base_OcRepresentation
* Method:    isEmpty
* Signature: ()Z
*/
JNIEXPORT jboolean JNICALL Java_org_iotivity_base_OcRepresentation_isEmpty
(JNIEnv *env, jobject thiz)
{
    LOGD("OcRepresentation_isEmpty");
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);

    return static_cast<jboolean>(rep->empty());
}

/*
* Class:     org_iotivity_base_OcRepresentation
* Method:    size
* Signature: ()I
*/
JNIEXPORT jint JNICALL Java_org_iotivity_base_OcRepresentation_size
(JNIEnv *env, jobject thiz)
{
    LOGD("OcRepresentation_size");
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);

    return static_cast<jint>(rep->numberOfAttributes());
}

/*
* Class:     org_iotivity_base_OcRepresentation
* Method:    remove
* Signature: (Ljava/lang/String;)Z
*/
JNIEXPORT jboolean JNICALL Java_org_iotivity_base_OcRepresentation_remove
(JNIEnv *env, jobject thiz, jstring jAttributeKey)
{
    LOGD("OcRepresentation_remove");
    if (!jAttributeKey)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "attributeKey cannot be null");
        return false;
    }
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);

    string attributeKey = env->GetStringUTFChars(jAttributeKey, NULL);
    return static_cast<jboolean>(rep->erase(attributeKey));
}

/*
* Class:     org_iotivity_base_OcRepresentation
* Method:    setNull
* Signature: (Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_org_iotivity_base_OcRepresentation_setNull
(JNIEnv *env, jobject thiz, jstring jAttributeKey)
{
    LOGD("OcRepresentation_setNull");
    if (!jAttributeKey)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "attributeKey cannot be null");
        return;
    }
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);

    string attributeKey = env->GetStringUTFChars(jAttributeKey, NULL);
    rep->setNULL(attributeKey);
}

/*
* Class:     org_iotivity_base_OcRepresentation
* Method:    isNull
* Signature: (Ljava/lang/String;)Z
*/
JNIEXPORT jboolean JNICALL Java_org_iotivity_base_OcRepresentation_isNull
(JNIEnv *env, jobject thiz, jstring jAttributeKey)
{
    LOGD("OcRepresentation_isNull");
    if (!jAttributeKey)
    {
        ThrowOcException(OC_STACK_INVALID_PARAM, "attributeKey cannot be null");
        return false;
    }
    OCRepresentation *rep = JniOcRepresentation::getOCRepresentationPtr(env, thiz);

    string attributeKey = env->GetStringUTFChars(jAttributeKey, NULL);
    return static_cast<jboolean>(rep->isNULL(attributeKey));
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
        delete rep;
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
        //TODO uncomment when OCRepresentation is fixed
        //delete rep;
    }
}
