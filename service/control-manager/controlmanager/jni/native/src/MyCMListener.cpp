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




#include "MyCMListener.h"

using namespace std ;

CMListener::CMListener(jobject myListener)
{
    listener = JNIGlobal::getGlobleRef(myListener) ;
}

CMListener::~CMListener()
{
    JNIGlobal::releaseGlobleRef(listener) ;
}

void
CMListener::onStarted(std::string address)
{
    bool isAttached = false ;

    JNIEnv *pEnv = JNIGlobal::getEnv(true, isAttached) ;
    if (NULL == pEnv)
    {
        return;
    }

    const char *cAddress = address.c_str();
    jstring javaAddress = pEnv->NewStringUTF(cAddress);


    JNIGlobal::executeVoidMethod("OC/Cm/ICMListener", "onStarted", "(Ljava/lang/String;)V", listener,
                                 javaAddress) ;
    if ( isAttached )
    {
        JNIGlobal::detachCurrentThread() ;
    }
}

void
CMListener::onStopped()
{
    JNIGlobal::executeVoidMethod("OC/Cm/ICMListener", "onStopped", "()V", listener);
}

void
CMListener::onError(OC::Cm::CMError error)
{
    jobject errorObj = JNIGlobal::getEnum("OC/Cm/CMErrorCode", "getErrorCode", "(I)LOC/Cm/CMErrorCode;",
                                          (int)error.getErrorCode());
    jstring errMsg   = JNIGlobal::convertToJString(error.getErrorMessage()) ;

    JNIGlobal::executeVoidMethod("OC/Cm/ICMListener", "onError",
                                 "(LOC/Cm/CMErrorCode;Ljava/lang/String;)V", listener, errorObj, errMsg);

    JNIGlobal::releaseLocalRef(errorObj);
    JNIGlobal::releaseLocalRef(errMsg);
}


