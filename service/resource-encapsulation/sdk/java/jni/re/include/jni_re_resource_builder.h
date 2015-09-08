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
  * @file   jni_re_resource_builder.h
  *
  * @brief  This file contains the resource builder function
  *            for JNI implementation
  */

#ifndef __JNI_RE_RESOURCEBUILDER_H
#define __JNI_RE_RESOURCEBUILDER_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * API for constructing a new RCSResourceObject.
 *
 *
 * @param resourceURI Resource URI value to be set
 * @param resourceType Resource type value to be set
 * @param resourceInterface Interface value to be set
 * @param observableFlag Whether resource should be observable or not.
 * @param discovervableFlag Whether resource should be discovervable or not.
 * @param attributes Attributes to be set
 *
 * @return RCSResourceObject object (It is the resource that is created)
 *
 * @throw RCSException
 *       It catches exception from registerResource API of OCPlatform and
 *       throws it to developer.
 */
JNIEXPORT jobject JNICALL Java_org_iotivity_ResourceEncapsulation_server_Builder_nativeBuild
(JNIEnv *env, jobject interfaceObject, jstring resourceURI, jstring resourceType,
 jstring resourceInterface,
 jboolean observableFlag, jboolean discovervableFlag, jobject attributes);

#ifdef __cplusplus
}
#endif
#endif //__JNI_RE_RESOURCEBUILDER_H


