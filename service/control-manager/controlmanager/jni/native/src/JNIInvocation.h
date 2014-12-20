//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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
 * JNIInvocation.h
 *
 *  Created on: Jul 10, 2013
 *      Author: kirtigupta
 */

#ifndef JNIINVOCATION_H_
#define JNIINVOCATION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <jni.h>

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) ;

JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved);


#ifdef __cplusplus
}
#endif
#endif /* JNIINVOCATION_H_ */
