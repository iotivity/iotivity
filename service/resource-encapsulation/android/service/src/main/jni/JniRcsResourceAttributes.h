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

#ifndef JNI_RCS_RESOURCE_ATTRIBUTES_H_
#define JNI_RCS_RESOURCE_ATTRIBUTES_H_

#include <jni.h>

/** OIC namespace */
namespace OIC
{
    /** service namespace */
    namespace Service
    {
        class RCSResourceAttributes;
    }
}

class JNIEnvWrapper;

/**
 * Initialize rcs resource attributes
 */
void initRCSResourceAttributes(JNIEnvWrapper*);

/**
 * Clear rcs resource attributes
 */
void clearRCSResourceAttributes(JNIEnvWrapper*);

jobject newAttributesObject(JNIEnv*, const OIC::Service::RCSResourceAttributes&);
jobject newAttributesObject(JNIEnvWrapper*, const OIC::Service::RCSResourceAttributes&);

OIC::Service::RCSResourceAttributes toNativeAttributes(JNIEnv*, jobject);
OIC::Service::RCSResourceAttributes toNativeAttributes(JNIEnvWrapper*, jobject);

void writeNativeAttributesFromMap(JNIEnv*, jobject mapObj,
        OIC::Service::RCSResourceAttributes& targetAttrs);
void writeNativeAttributesFromMap(JNIEnvWrapper*, jobject mapObj,
        OIC::Service::RCSResourceAttributes& targetAttrs);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @class:     org_iotivity_service_RcsResourceAttributes
 * Method:     nativeIsEmpty
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_service_RcsResourceAttributes_nativeIsEmpty(JNIEnv*, jobject);

/**
 * @class:     org_iotivity_service_RcsResourceAttributes
 * Method:     nativeSize
 */
JNIEXPORT jint JNICALL
Java_org_iotivity_service_RcsResourceAttributes_nativeSize(JNIEnv*, jobject);

/**
 * @class:     org_iotivity_service_RcsResourceAttributes
 * Method:     nativeRemove
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_service_RcsResourceAttributes_nativeRemove(JNIEnv*, jobject, jstring keyObj);

/**
 * @class:     org_iotivity_service_RcsResourceAttributes
 * Method:     nativeClear
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_RcsResourceAttributes_nativeClear(JNIEnv*, jobject);

/**
 * @class:     org_iotivity_service_RcsResourceAttributes
 * Method:     nativeContains
 */
JNIEXPORT jboolean JNICALL
Java_org_iotivity_service_RcsResourceAttributes_nativeContains(JNIEnv*, jobject, jstring keyObj);

/**
 * @class:     org_iotivity_service_RcsResourceAttributes
 * Method:     nativeAddKeys
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_RcsResourceAttributes_nativeAddKeys(JNIEnv*, jobject, jstring setObj);

/**
 * @class:     org_iotivity_service_RcsResourceAttributes
 * Method:     nativeExtract
 */
JNIEXPORT jobject JNICALL
Java_org_iotivity_service_RcsResourceAttributes_nativeExtract(JNIEnv*, jobject, jstring keyObj);

/**
 * @class:     org_iotivity_service_RcsResourceAttributes
 * Method:     nativeExtractAll
 */
JNIEXPORT void JNICALL
Java_org_iotivity_service_RcsResourceAttributes_nativeExtractAll(JNIEnv*, jobject, jobject);

#ifdef __cplusplus
}
#endif

#endif  //JNI_RCS_RESOURCE_ATTRIBUTES_H_
