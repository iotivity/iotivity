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

/**
 * @file   simulator_exceptions_jni.h
 */

#ifndef SIMULATOR_EXCEPTIONS_JNI_H_
#define SIMULATOR_EXCEPTIONS_JNI_H_

#include <jni.h>

#include "simulator_error_codes.h"

/**
 * throw the simulator exception
 * @param env          JNI interface pointer
 * @param code         simulator result value
 */
void ThrowSimulatorException(JNIEnv *env, SimulatorResult code, const char *message);

/**
 * throw the invalid argument exception
 * @param env          JNI interface pointer
 * @param code         simulator result value
 */
void ThrowInvalidArgsException(JNIEnv *env, SimulatorResult code, const char *message);

/**
 * throw the not supported exception
 * @param env          JNI interface pointer
 * @param code         simulator result value
 */
void ThrowNoSupportException(JNIEnv *env, const char *message);

/**
 * throw the operation in progress exception
 * @param env          JNI interface pointer
 * @param code         simulator result value
 */
void ThrowOperationInProgressException(JNIEnv *env, const char *message);

/**
 * throw the bad object exception
 * @param env          JNI interface pointer
 * @param code         simulator result value
 */
void ThrowBadObjectException(JNIEnv *env, const char *message);

#endif
