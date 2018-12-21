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

#ifndef SIMULATOR_UTILS_JNI_H_
#define SIMULATOR_UTILS_JNI_H_

#include <jni.h>
#include "simulator_error_codes.h"

typedef struct
{
    jclass objectCls;   /**< Object class object */
    jclass integerCls;  /**< Integer class object */
    jclass doubleCls;   /**< Double class object */
    jclass booleanCls;  /**< Boolean class object */
    jclass stringCls;   /**< String class object */
    jclass hashMapCls;  /**< hash map class object */
    jclass vectorCls;   /**< Vector class object */
    jclass mapCls;      /**< Map class object */
    jclass mapEntryCls; /**< Map entry class object */
    jclass setCls;      /**< Set class object */
    jclass iteratorCls; /**< Iterator class object */

    jclass simulatorResourceCls;           /**< Simulator resource class object */
    jclass simulatorResourceTypeCls;       /**< Simulator resource type class object */
    jclass simulatorSingleResourceCls;     /**< Simulator single resource class object */
    jclass simulatorCollectionResourceCls; /**< Simulator collection resource class object */
    jclass simulatorResourceModelCls;      /**< Simulator resource model class object */
    jclass simulatorResourceAttributeCls;  /**< Simulator resource attribute class object */
    jclass attributeValueCls;              /**< Attribute value class object */
    jclass attributeValueTypeCls;          /**< Attribute value type class object */
    jclass attributeTypeInfoCls;           /**< Attribute type information class object */
    jclass attributePropertyCls;           /**< Attribute property class object */
    jclass attributePropertyTypeCls;       /**< Attribute property type class object */
    jclass integerPropertyCls;             /**< Integer property class object */
    jclass integerPropertyBuilderCls;      /**< Integer property builder class object */
    jclass doublePropertyCls;              /**< Double property class object */
    jclass doublePropertyBuilderCls;       /**< Double property builder class object */
    jclass booleanPropertyCls;             /**< Boolean property class object */
    jclass booleanPropertyBuilderCls;      /**< Boolean property builder class object */
    jclass stringPropertyCls;              /**< String property class object */
    jclass stringPropertyBuilderCls;       /**< String property builder class object */
    jclass arrayPropertyCls;               /**< Array property class object */
    jclass arrayPropertyBuilderCls;        /**< Array property builder class object */
    jclass modelPropertyCls;               /**< Model property class object */
    jclass autoUpdateTypeCls;              /**< Auto update type class object */

    jclass simulatorRemoteResourceCls;   /**< Simulator remote resource class object */
    jclass requestTypeCls;               /**< Request type class object */
    jclass observerCls;                  /**< Observer class object */
    jclass deviceInfoCls;                /**< Device information class object */
    jclass platformInfoCls;              /**< Platform information class object */
    jclass simulatorRequestModelCls;     /**< Simulator request model class object */

    jclass simulatorExceptionCls;           /**< Simulator exception class object */
    jclass invalidArgsExceptionCls;         /**< Invalid args exception class object */
    jclass noSupportExceptionCls;           /**< No support exception class object */
    jclass operationInProgressExceptionCls; /**< Operation in progress exception class object */
    jclass simulatorResultCls;              /**< Simulator result class object */

    jclass integer1DArrayCls;           /**< Integer 1d array class object */
    jclass integer2DArrayCls;           /**< Integer 2d array class object */
    jclass integer3DArrayCls;           /**< Integer 3d array class object */
    jclass double1DArrayCls;            /**< Double 1d array class object */
    jclass double2DArrayCls;            /**< Double 2d array class object */
    jclass double3DArrayCls;            /**< Double 3d array class object */
    jclass boolean1DArrayCls;           /**< Boolean 1d array class object */
    jclass boolean2DArrayCls;           /**< Boolean 2d array class object */
    jclass boolean3DArrayCls;           /**< Boolean 3d array class object */
    jclass string1DArrayCls;            /**< String 1d array class object */
    jclass string2DArrayCls;            /**< String 2d array class object */
    jclass string3DArrayCls;            /**< String 3d array class object */
    jclass simulatorResModel1DArrayCls; /**< Simulator resource model 1d array class object */
    jclass simulatorResModel2DArrayCls; /**< Simulator resource model 2d array class object */
    jclass simulatorResModel3DArrayCls; /**< Simulator resource model 3d array class object */
} SimulatorClassRefs;

/** Validate value */
#define VALIDATE_INPUT(ENV, CONDITION, MSG) if (CONDITION) {ThrowInvalidArgsException(ENV, SIMULATOR_INVALID_PARAM, MSG); return;}
/** Validate value and return */
#define VALIDATE_INPUT_RET(ENV, CONDITION, MSG, RET) if (CONDITION) {ThrowInvalidArgsException(ENV, SIMULATOR_INVALID_PARAM, MSG); return RET;}

/** Validate callback */
#define VALIDATE_CALLBACK(ENV, CALLBACK) if (!CALLBACK){ThrowInvalidArgsException(env, SIMULATOR_INVALID_CALLBACK, "Invalid callback!"); return;}
/** Validate callback and return */
#define VALIDATE_CALLBACK_RET(ENV, CALLBACK, RET) if (!CALLBACK){ThrowInvalidArgsException(env, SIMULATOR_INVALID_CALLBACK, "Invalid callback!"); return RET;}

/** get handle field */
static jfieldID getHandleField(JNIEnv *env, jobject jobj)
{
    jclass cls = env->GetObjectClass(jobj);
    return env->GetFieldID(cls, "mNativeHandle", "J");
}

/** get handle */
template <typename T>
static T *getHandle(JNIEnv *env, jobject jobj)
{
    jlong handle = env->GetLongField(jobj, getHandleField(env, jobj));
    return reinterpret_cast<T *>(handle);
}

/** set handle */
template <typename T>
static void setHandle(JNIEnv *env, jobject jobj, T *type)
{
    jlong handle = reinterpret_cast<jlong>(type);
    env->SetLongField(jobj, getHandleField(env, jobj), handle);
}

extern JNIEnv *GetEnv();    /**< get the environment information */
extern void ReleaseEnv();   /**< release the environment information */
extern SimulatorClassRefs gSimulatorClassRefs;

jobject SimulatorResultToJava(JNIEnv *env, SimulatorResult errorCode);

#endif
