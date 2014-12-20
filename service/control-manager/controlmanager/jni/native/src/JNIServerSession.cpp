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




#include <jni.h>
#include "Connector/Server/ServerSession.h"
#include "JNICMDev.h"
#include "JNIGlobal.h"

JNIEXPORT jlong JNICALL Java_OC_Cm_Connector_Server_ServerSession_constructNativeSession
(JNIEnv *env, jobject thisObj,  jint serverId, jlong context, jstring jclientProtocol,
 jstring jdeviceAddress)
{
    std::string deviceAddress ;
    if (false == JNIGlobal::convertToStdString(jdeviceAddress, deviceAddress))
    {
        return 0;
    }
    std::string clientProtocol ;
    if (false == JNIGlobal::convertToStdString(jclientProtocol, clientProtocol))
    {
        return 0;
    }

    OC::Cm::Connector::Server::ServerSession *sSession = new OC::Cm::Connector::Server::ServerSession(
        serverId, clientProtocol, deviceAddress) ;
    return (jlong)sSession;
}

JNIEXPORT jint JNICALL Java_OC_Cm_Connector_Server_ServerSession_getServerId
(JNIEnv *env, jobject thisObj, jlong servSession)
{
    OC::Cm::Connector::Server::ServerSession *sSession = (OC::Cm::Connector::Server::ServerSession *)
            servSession ;

    return (jint)sSession->getServerId();

}


JNIEXPORT jstring JNICALL Java_OC_Cm_Connector_Server_ServerSession_getClientAddress
(JNIEnv *env, jobject thisObj, jlong servSession)
{
    OC::Cm::Connector::Server::ServerSession *sSession = (OC::Cm::Connector::Server::ServerSession *)
            servSession ;

    std::string cAddress = sSession->getClientAddress();

    return env->NewStringUTF(cAddress.c_str());
}

JNIEXPORT jobject JNICALL Java_OC_Cm_Connector_Server_ServerSession_getClientDevice
(JNIEnv *env, jobject thisObj, jlong servSession)
{
    OC::Cm::Connector::Server::ServerSession *sSession = (OC::Cm::Connector::Server::ServerSession *)
            servSession ;

    JNICMDevice *jniDevice = (JNICMDevice *) (sSession->getClientDevice()) ;
    jobject javaDeviceObject = jniDevice->m_device ;

    return javaDeviceObject ;
}

extern "C" JNIEXPORT jboolean JNICALL Java_OC_Cm_Connector_Server_ServerSession_setResponseHeader
(JNIEnv *env, jobject thisObj, jlong servSession, jstring name, jstring value)
{
    OC::Cm::Connector::Server::ServerSession *sSession = (OC::Cm::Connector::Server::ServerSession *)
            servSession ;
    if ( NULL == sSession)
    {
        return JNI_FALSE;
    }

    const char *property = env->GetStringUTFChars(name, 0);
    std::string nameStr(property);
    env->ReleaseStringUTFChars(name, property);

    const char *pValue = env->GetStringUTFChars(value, 0);
    std::string valueStr(pValue);
    env->ReleaseStringUTFChars(value, pValue);

    if (sSession->getResponse().m_headers.setHeader(nameStr, valueStr))
    {
        return JNI_TRUE;
    }
    else
    {
        return JNI_FALSE;
    }
}

extern "C" JNIEXPORT jstring JNICALL Java_OC_Cm_Connector_Server_ServerSession_getTemplateParameter
(JNIEnv *env, jobject thisObj, jlong servSession1, jstring name)
{
    OC::Cm::Connector::Server::ServerSession *sSession = (OC::Cm::Connector::Server::ServerSession *)
            servSession1 ;
    if ( NULL == sSession)
    {
        return JNI_FALSE;
    }

    const char *property = env->GetStringUTFChars(name, 0);
    std::string nameStr(property);
    env->ReleaseStringUTFChars(name, property);

    std::string value = sSession->getRequest().m_template_parameters[nameStr];

    return env->NewStringUTF(value.c_str());
}



