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
#include "OC_Cm_Connector_HeadersList.h"
#include "CMConnector.h"
#include "JNIGlobal.h"
#include "list"

JNIEXPORT jlong JNICALL Java_OC_Cm_Connector_HeadersList_constructNative(JNIEnv *env,
        jobject thisObj)
{
    OC::Cm::Connector::HeadersList *headersList = new OC::Cm::Connector::HeadersList();
    return (jlong) (headersList);
}

JNIEXPORT void JNICALL Java_OC_Cm_Connector_HeadersList_deleteNative
(JNIEnv *env, jobject thisObj, jlong headersList)
{
    OC::Cm::Connector::HeadersList *headersListObj = (OC::Cm::Connector::HeadersList *) headersList;
    if (headersListObj != NULL)
    {
        delete headersListObj;
    }
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_Connector_HeadersList_setHeader(JNIEnv *env, jobject thisObj,
        jlong headersList, jstring name, jstring value)
{
    OC::Cm::Connector::HeadersList *headersListObj = (OC::Cm::Connector::HeadersList *) headersList;
    if ((jlong)0 == headersList)
    {
        return JNI_FALSE ;
    }

    std::string hName ;
    if (false == JNIGlobal::convertToStdString(name, hName))
    {
        return JNI_FALSE;
    }

    std::string hVal ;
    if (false == JNIGlobal::convertToStdString(value, hVal))
    {
        return JNI_FALSE;
    }

    return headersListObj->setHeader(hName, hVal);
}


JNIEXPORT jstring JNICALL Java_OC_Cm_Connector_HeadersList_getHeader(JNIEnv *env, jobject thisObj,
        jlong headersList, jstring name)
{
    const char *headerName = env->GetStringUTFChars(name, 0);

    jstring headerValue ;

    OC::Cm::Connector::HeadersList *headersListObj = (OC::Cm::Connector::HeadersList *) headersList;

    std::string hName( headerName );
    std::string value ;

    bool getH = headersListObj->getHeader(hName, value);

    env->ReleaseStringUTFChars(name, headerName);

    if (getH == true)
    {
        headerValue = env->NewStringUTF(value.c_str());
        return headerValue;
    }
    else
        return NULL;
}

JNIEXPORT jobjectArray JNICALL Java_OC_Cm_Connector_HeadersList_getHeaderNameList(JNIEnv *env,
        jobject thisObj, jlong headersList)
{
    jobjectArray resultList = NULL ;

    OC::Cm::Connector::HeadersList *headersListObj = (OC::Cm::Connector::HeadersList *) headersList;

    OC::Cm::Connector::IHeadersListIterator *headers = headersListObj->getIterator();

    std::list <std::string> headerNameList ;

    while ( headers->isEnd() == false )
    {
        std::string name, value ;

        if ( true == headers->get(name, value) )
        {
            headerNameList.push_back(name) ;
        }

        headers->moveToNext() ;
    }

    delete headers ;

    resultList = NULL ;

    int index = 0 ;
    for (std::list<std::string>::const_iterator itr = headerNameList.begin();
         itr != headerNameList.end(); itr++, index++ )
    {
        std::string headerName = (*itr) ;
        jstring jheader = env->NewStringUTF(headerName.c_str()) ;

        if (NULL == resultList)
        {
            jclass javaStringClass = env->GetObjectClass(jheader) ;
            resultList = env->NewObjectArray(headerNameList.size(), javaStringClass, NULL) ;
        }

        env->SetObjectArrayElement( resultList, index , jheader ) ;
    }

    return resultList ;
}
