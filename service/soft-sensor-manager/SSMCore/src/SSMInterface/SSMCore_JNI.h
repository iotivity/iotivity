/******************************************************************
*
* Copyright 2014 Samsung Electronics All Rights Reserved.
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
#ifndef _SSMCore_JNI_H_
#define _SSMCore_JNI_H_

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_org_iotivity_service_ssm_CoreController_initializeSSMCore
(JNIEnv *env, jclass clz, jstring jstrXmlDescription);

JNIEXPORT void JNICALL Java_org_iotivity_service_ssm_CoreController_startSSMCore
(JNIEnv *env, jclass clz);

JNIEXPORT void JNICALL Java_org_iotivity_service_ssm_CoreController_stopSSMCore
(JNIEnv *env, jclass clz);

JNIEXPORT void JNICALL Java_org_iotivity_service_ssm_CoreController_terminateSSMCore
(JNIEnv *env, jclass clz);

JNIEXPORT jobject JNICALL Java_org_iotivity_service_ssm_CoreController_createQueryEngine
(JNIEnv *env, jclass clz);

JNIEXPORT jint JNICALL Java_org_iotivity_service_ssm_CoreController_releaseQueryEngine
(JNIEnv *env, jclass clz, jobject queryEngine);

JNIEXPORT jint JNICALL Java_org_iotivity_service_ssm_CoreController_executeContextQuery
(JNIEnv *env, jclass clz, jint pQueryEngineInstance, jstring jstrContextQuery);

JNIEXPORT void JNICALL Java_org_iotivity_service_ssm_CoreController_registerQueryEvent
(JNIEnv *env, jclass clz, jint pQueryEngineInstance, jobject queryEngineEvent);

JNIEXPORT void JNICALL Java_org_iotivity_service_ssm_CoreController_killContextQuery
(JNIEnv *env, jclass clz, jint pQueryEngineInstance, jint cqid);

JNIEXPORT jint JNICALL Java_org_iotivity_service_ssm_CoreController_getDataId
(JNIEnv *env, jclass clz, jint pDataReaderInstance);

JNIEXPORT jobject JNICALL Java_org_iotivity_service_ssm_CoreController_getAffectedModels
(JNIEnv *env, jclass clz, jint pDataReaderInstance);

JNIEXPORT jint JNICALL Java_org_iotivity_service_ssm_CoreController_getModelDataCount
(JNIEnv *env, jclass clz, jint pDataReaderInstance, jstring jstrModelName);

JNIEXPORT jobject JNICALL Java_org_iotivity_service_ssm_CoreController_getModelData
(JNIEnv *env, jclass clz, jint pDataReaderInstance, jstring jstrModelName, jint jintDataIndex );

JNIEXPORT jint JNICALL Java_org_iotivity_service_ssm_CoreController_getPropertyCount
(JNIEnv *env, jclass clz, jint pIModelDataInstance );

JNIEXPORT jstring JNICALL Java_org_iotivity_service_ssm_CoreController_getPropertyName
(JNIEnv *env, jclass clz, jint pIModelDataInstance, jint propertyIndex );

JNIEXPORT jstring JNICALL Java_org_iotivity_service_ssm_CoreController_getPropertyValue
(JNIEnv *env, jclass clz, jint pIModelDataInstance, jint propertyIndex );

JNIEXPORT void JNICALL Java_org_iotivity_service_ssm_CoreController_registerReportReceiver
(JNIEnv *env, jclass clz, jobject reportReceiver );

#ifdef __cplusplus
}
#endif

#endif