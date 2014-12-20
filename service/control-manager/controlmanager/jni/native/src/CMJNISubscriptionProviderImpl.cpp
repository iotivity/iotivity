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
 * CMJNISubscriptionProviderImpl.cpp
 *
 *  Created on: Sep 24, 2013
 *      Author: bhupathi.r
 */

#include "CMJNISubscriptionProviderImpl.h"
#include "IContext.h"
#include "JNIGlobal.h"
#include "JNICMDev.h"

CMJNISubscriptionProviderImpl::CMJNISubscriptionProviderImpl(jobject provider)
{
    mProvider = JNIGlobal::getGlobleRef(provider) ;
}

CMJNISubscriptionProviderImpl::~CMJNISubscriptionProviderImpl()
{
    JNIGlobal::releaseGlobleRef(mProvider) ;
}


bool
CMJNISubscriptionProviderImpl::shouldAllowSubscriptionRequest( OC::Cm::Device &device,
        std::string &resourcePath )
{
    bool isAttached = false ;

    JNIEnv *pEnv = JNIGlobal::getEnv(true, isAttached) ;
    if (NULL == pEnv)
    {
        return false;
    }

    JNICMDevice *jniDevice = (JNICMDevice *) (&device) ;
    jobject javaDeviceObject = jniDevice->m_device ;

    const char *cResourcePath = resourcePath.c_str();
    jstring javaResourceStr = pEnv->NewStringUTF(cResourcePath);


    jboolean retValue = JNIGlobal::executeBoolMethod("OC/Cm/Notification/ISubscriptionProvider",
                        "shouldAllowSubscriptionRequest",
                        "(LOC/Cm/Device;Ljava/lang/String;)Z", javaDeviceObject, javaResourceStr) ;

    if ( isAttached )
    {
        JNIGlobal::detachCurrentThread() ;
    }

    return (retValue == JNI_TRUE);
}
