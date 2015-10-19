/* *****************************************************************
*
* Copyright 2015 Samsung Electronics All Rights Reserved.
*
*----------------------------------------------------------------------
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
******************************************************************************/

/**
 * @file
 *
 * This file contains the declaration of node apis and common utilities.
 */

#ifndef COMMON_UTIL_H
#define  COMMON_UTIL_H


#include <node.h>
#include <v8.h>
#include <uv.h>

/**
*namespace declared.
*/
using namespace v8;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;
using namespace std;

/**
 * This function is used to converts V8:String to string type
 *
 *@param[in] value - v8 input string
 *
 * @return none
 */
const char *ToCString(const v8::String::Utf8Value &value);

/**
 * This function is used to initialize iotivity
 *
 *@param[in]  javascript arguments
 *
 * @return none
 */
void InitPlatform(const FunctionCallbackInfo<Value> &args);

/**
 * This function is used to discover resources
 *
 *@param[in]  javascript arguments
 *
 * @return none
 */
void DiscoverResource(const FunctionCallbackInfo<Value> &args);

/**
 * This function is used to call discover resources by type.
 *
 * @param[in]  javascript arguments
 *
 * @return none
 */
void DiscoverResourceByType(const FunctionCallbackInfo<Value> &args);

/**
 * This function is used to call get resource uri.
 *
 * @param[in]  javascript arguments
 *
 * @return none
 */
void GetUri(const FunctionCallbackInfo<Value> &args);

#endif // COMMON_UTIL_H

