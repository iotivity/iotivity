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





#include "OC_Cm_Server_ResourceHandler_ResourceHandlerFactory.h"
#include "CMResourceHandlerFactory.h"

JNIEXPORT jlong JNICALL Java_OC_Cm_Server_ResourceHandler_ResourceHandlerFactory_constructNative
(JNIEnv *env, jobject thisObj, jobject factory)
{
    MyCMResourceHandlerFactory *rObject = new MyCMResourceHandlerFactory(factory);
    return (jlong) rObject;
}

JNIEXPORT void JNICALL Java_OC_Cm_Server_ResourceHandler_ResourceHandlerFactory_deleteNative
(JNIEnv *env, jobject thisObj, jlong rFactory)
{
    MyCMResourceHandlerFactory *rObject =  ( MyCMResourceHandlerFactory *) rFactory;
    if (NULL != rObject)
    {
        delete rObject;
    }
}

JNIEXPORT void JNICALL Java_OC_Cm_Server_ResourceHandler_ResourceHandlerFactory_registerResourceType
(JNIEnv *env, jobject thisObj, jlong rFactory, jint type, jstring path)
{
    MyCMResourceHandlerFactory *rObject =  ( MyCMResourceHandlerFactory *) rFactory;
    JNI_TryReturnVoid( NULL != rObject, "Native instance is invalid")

    std::string pathS ;
    JNI_TryReturnVoid( ( true == JNIGlobal::convertToStdString(path, pathS)), "pathS conversion failed")

    rObject->registerResourceHandler(type, pathS) ;
}

JNIEXPORT jint JNICALL Java_OC_Cm_Server_ResourceHandler_ResourceHandlerFactory_getResourceType
(JNIEnv *pEnv, jobject thisObj, jlong rFactory, jstring path)
{
    MyCMResourceHandlerFactory *rObject =  ( MyCMResourceHandlerFactory *) rFactory;
    JNI_TryReturn( NULL != rObject, -1, "Native instance is invalid")

    std::string pathS ;
    JNI_TryReturn( ( true == JNIGlobal::convertToStdString(path, pathS)), -1, "pathS conversion failed")

    std::map<std::string, std::string> templateParams ;
    return rObject->getResourceType(pathS, templateParams) ;
}




