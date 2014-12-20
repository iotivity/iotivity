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
 * JNISerializableDataFactory.cpp
 *
 *  Created on: Jul 8, 2013
 *      Author: kirtigupta
 */

#include "OC_Cm_Serialization_SerializableDataFactory.h"
#include "CMSerializableDataFactory.h"
#include "iostream"

JNIEXPORT jlong JNICALL Java_OC_Cm_Serialization_SerializableDataFactory_constructNative
(JNIEnv *env, jobject thisObj, jobject factory)
{
    CMSerializableDataFactory *sObject = new CMSerializableDataFactory(factory);

    return (jlong) sObject ;
}

JNIEXPORT void JNICALL Java_OC_Cm_Serialization_SerializableDataFactory_deleteNative
(JNIEnv *env, jobject thisObj, jlong serDataFactory)
{
    CMSerializableDataFactory *sObject =  (CMSerializableDataFactory *) serDataFactory;

    if (NULL != sObject)
    {
        delete sObject;
    }
}



