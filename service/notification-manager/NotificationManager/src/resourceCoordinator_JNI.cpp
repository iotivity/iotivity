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
#include "hosting.h"
}
#include "resourceCoordinator_JNI.h"

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
            //OCProcess ERROR
        }

        sleep(2);
    }
}
/*
 * To callback log message from C++ to Java for android
 */
void messageCallback(JNIEnv *env, jobject obj, const char *c_str)
{
    jstring jstr = (env)->NewStringUTF(c_str);
    jclass cls = env->GetObjectClass(obj);
    jmethodID cbMessage = env->GetMethodID(cls, "cbMessage", "(Ljava/lang/String;)V");
    env->CallVoidMethod(obj, cbMessage, jstr);
}
/*
 *  for Hosting Device Side
 */
JNIEXPORT void JNICALL
Java_com_example_resourcehostingsampleapp_ResourceHosting_OICCoordinatorStart(JNIEnv *env,
        jobject obj)
{

    if (threadRun == true)
    {
        messageCallback(env, obj, "already execute OICCoordinatorStart");
        return;
    }
    else
    {
        messageCallback(env, obj, "OICCoordinatorStart");
        int result;
        result = 0;
        result = OICStartCoordinate();
        string str = "OICStartCoordinate result : ";
        string result_str = boost::lexical_cast<std::string>(result);
        str += result_str;
        messageCallback(env, obj, str.c_str());
        threadRun = true;
        ocProcessThread = thread(ocProcessFunc);
    }


}

JNIEXPORT void JNICALL Java_com_example_resourcehostingsampleapp_ResourceHosting_OICCoordinatorStop(
    JNIEnv *env, jobject obj)
{
    messageCallback(env, obj, "OICCoordinatorStop");
    //terminate Thread
    if (ocProcessThread.joinable())
    {
        threadRun = false;
        ocProcessThread.join();
    }
    else
    {
        messageCallback(env, obj, "The thread may be not running.");
    }

    int result;
    result = 0;
    result = OICStopCoordinate();
    string str = "OICStopCoordinate result : ";
    string result_str = boost::lexical_cast<std::string>(result);
    str += result_str;
    messageCallback(env, obj, str.c_str());
}

JNIEXPORT void JNICALL Java_com_example_resourcehostingsampleapp_ResourceHosting_ResourceHostingInit
(JNIEnv *env, jobject obj, jstring j_addr)
{
    messageCallback(env, obj, "ResourceHostingInit");
    const char *addr = env->GetStringUTFChars(j_addr, NULL);
    __android_log_print(ANDROID_LOG_ERROR, "DOIL", "addr : %s", addr);
    uint8_t interfaceAddress[20] = {0};
    uint8_t *coordinatingAddress = NULL;
    uint8_t interfaceName[] = "eth0";


    if (OCInit(addr, USE_RANDOM_PORT, OC_CLIENT_SERVER) != OC_STACK_OK)
    {
        messageCallback(env, obj, "OCStack init Error");
        return;
    }

    env->ReleaseStringUTFChars(j_addr, addr);


}

JNIEXPORT void JNICALL
Java_com_example_resourcehostingsampleapp_ResourceHosting_ResourceHostingTerminate(JNIEnv *env,
        jobject obj)
{
    messageCallback(env, obj, "ResourceHostingTerminate");
    if (OCStop() != OC_STACK_OK)
    {
        messageCallback(env, obj, "OCStack stop error");
    }
    //terminate Thread
    if (ocProcessThread.joinable())
    {
        threadRun = false;
        ocProcessThread.join();
    }
    else
    {
        messageCallback(env, obj, "The thread may be not running.");
    }
}
