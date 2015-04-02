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
  * @file   jni_things_manager_util.h
  *
  * @brief  This file contains the utility functions for conversions from java to CPP
  * and viceversa
  */

#ifndef __JNI_THINGS_MANAGER_UTILS_H_
#define __JNI_THINGS_MANAGER_UTILS_H_


#include <vector>
#include <map>
#include "jni_things_manager_jvm.h"

/**
  * Utility function for converting a Java Vector of Strings to CPP Vector of Strings
  *
  * @param env - Default JNI Environment pointer
  * @param jVectorString - Java Vector of Strings
  *
  * @return void
  */
std::vector<std::string> convertStringVector(JNIEnv *env, jobject jVectorString);

/**
  * Utility function for converting a Hash Map of Strings to CPP Map of Strings
  *
  * @param env - Default JNI Environment pointer
  * @param jMapString - Java Map of Strings
  *
  * @return void
  */
std::map<std::string, std::string> convertStringMap(JNIEnv *env, jobject jMapString);
#endif //__JNI_THINGS_MANAGER_UTILS_H_

