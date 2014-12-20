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
 * JNIDeviceFactory.cpp
 *
 *  Created on: Jul 8, 2013
 *      Author: kirtigupta
 */
#include "OC_Cm_DeviceFinder_DeviceFactory.h"
#include "CMDeviceFactory.h"

JNIEXPORT jlong JNICALL Java_OC_Cm_DeviceFinder_DeviceFactory_constructNative
(JNIEnv *env, jobject thisObj, jobject factory)
{
    CMDeviceFactory *dObject = new CMDeviceFactory(factory);

    if (NULL == dObject)
    {
        //JNIGlobal::throwException("Failed to allocate SHPDeviceFactory : OutOfMemory");
    }

    return (jlong) dObject;
}


JNIEXPORT void JNICALL Java_OC_Cm_DeviceFinder_DeviceFactory_deleteNative
(JNIEnv *env, jobject thisObj, jlong dfactory)
{
    CMDeviceFactory *dObject =  (CMDeviceFactory *) dfactory;

    if (NULL != dObject)
    {
        delete dObject;
    }
}




