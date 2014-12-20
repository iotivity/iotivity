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




#include "OC_Cm_Connector_Session.h"
#include "CMConnector.h"

/*
 * Class:     OC_Cm_Connector_Session
 * Method:    getRequest
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_OC_Cm_Connector_Session_getRequest
(JNIEnv *env, jobject thisObj, jlong session)
{
    OC::Cm::Connector::Session *sSession = (OC::Cm::Connector::Session *) session ;
    return (jlong) & (sSession->getRequest());
}

/*
 * Class:     OC_Cm_Connector_Session
 * Method:    getResponse
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_OC_Cm_Connector_Session_getResponse
(JNIEnv *env, jobject thisObj, jlong session)
{
    OC::Cm::Connector::Session *sSession = (OC::Cm::Connector::Session *) session ;
    return (jlong) & (sSession->getResponse());
}

/*
 * Class:     OC_Cm_Connector_Session
 * Method:    getSessionId
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_OC_Cm_Connector_Session_getSessionId
(JNIEnv *env, jobject thisObj, jlong session)
{
    OC::Cm::Connector::Session *sSession = (OC::Cm::Connector::Session *) session ;
    return (jint)(sSession->getSessionId());
}
