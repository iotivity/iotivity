
//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

extern "C" {
#include "Hosting.h"
}
#include "ResourceHosing_JNI.h"
#include "OCAndroid.h"

using namespace std;

atomic_bool threadRun;
thread ocProcessThread;
/*
 * To execute OCProcess when threadRun value is only true
 */
void ocProcessFunc()
{
    while (threadRun)
    {

        if (OCProcess() != OC_STACK_OK)
        {
            return ;
        }

        sleep(2);
    }
}

/*
 *  for Hosting Device Side
 */
JNIEXPORT jint JNICALL Java_org_iotivity_ResourceHosting_ResourceHosting_OICCoordinatorStart
(JNIEnv *env, jobject obj)
{
    jint result = 0;
    if(threadRun==true)
    {
        result = (jint)HOSTING_THREAD_ERROR;
        return result;
    }
    else
    {
        result = (jint)OICStartCoordinate();

        threadRun = true;
        ocProcessThread = thread(ocProcessFunc);
        return result;
    }
}

JNIEXPORT jint JNICALL Java_org_iotivity_ResourceHosting_ResourceHosting_OICCoordinatorStop
(JNIEnv *env, jobject obj)
{
    jint result = 0;
    //terminate Thread
    if (ocProcessThread.joinable())
    {
        threadRun = false;
        ocProcessThread.join();
    }
    else
    {
            result = (jint)HOSTING_THREAD_ERROR;
            return result;
    }
    result = (jint)OICStopCoordinate();

    return result;
}

JNIEXPORT jint JNICALL Java_org_iotivity_ResourceHosting_ResourceHosting_ResourceHostingInit
(JNIEnv *env, jobject obj,jstring j_addr)
{
    if (NULL == j_addr)
    {
        return (jint)OCSTACK_ERROR;
    }

    if(OCInit1(OC_CLIENT_SERVER, OC_DEFAULT_FLAGS, OC_DEFAULT_FLAGS))
    {
        return (jint)OCSTACK_ERROR;
    }

    return (jint)OCSTACK_OK;
}

JNIEXPORT jint JNICALL Java_org_iotivity_ResourceHosting_ResourceHosting_ResourceHostingTerminate
(JNIEnv *env, jobject obj)
{
    if (OCStop() != OC_STACK_OK)
    {
        return (jint)OCSTACK_ERROR;
    }
    //terminate Thread
    if (ocProcessThread.joinable())
    {
        threadRun = false;
        ocProcessThread.join();
    }

    return (jint)OCSTACK_OK;
}
