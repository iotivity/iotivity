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
 * JNIGlobal.h
 *
 *  Created on: Jul 5, 2013
 *      Author: kirtigupta
 */

#ifndef JNIGLOBAL_H_
#define JNIGLOBAL_H_

#include <jni.h>
#include "string"
#include "map"
#include "Thread/Mutex.h"

#include <android/log.h>

#define LOGV(TAG,...) __android_log_print(ANDROID_LOG_VERBOSE, TAG,__VA_ARGS__)
#define LOGD(TAG,...) __android_log_print(ANDROID_LOG_DEBUG  , TAG,__VA_ARGS__)
#define LOGI(TAG,...) __android_log_print(ANDROID_LOG_INFO   , TAG,__VA_ARGS__)
#define LOGW(TAG,...) __android_log_print(ANDROID_LOG_WARN   , TAG,__VA_ARGS__)
#define LOGE(TAG,...) __android_log_print(ANDROID_LOG_ERROR  , TAG,__VA_ARGS__)

using namespace std ;

#ifdef __ANDROID__

#define CM_JNI_VERSION JNI_VERSION_1_2

#else

#define CM_JNI_VERSION JNI_VERSION_1_1

#endif

class JNIEXPORT JNIGlobal
{

    public:

        JNIGlobal();

        virtual ~JNIGlobal();

        static jobject getGlobleRef(jobject obj) ;

        static void releaseGlobleRef(jobject obj) ;

        static void releaseLocalRef(jobject obj) ;

        static void throwException(const char *errorMessage) ;

        static bool convertToStdString(jstring jinput, std::string &out);

        static jstring convertToJString( std::string value ) ;

        static jobject getEnum(const char *className, const char *method, const char *signature,
                               int enumValue) ;

        static jobject constructObject( const char *className, const char *methodSignature, ... );

        static bool executeBoolMethod( const char *className, const char *methodName,
                                       const char *methodSignature, jobject object, ... );

        static jlong executeLongMethod( const char *className, const char *methodName,
                                        const char *methodSignature, jobject object, ... );

        static jint executeIntMethod( const char *className, const char *methodName,
                                      const char *methodSignature, jobject object, ... );

        static void executeVoidMethod( const char *className, const char *methodName,
                                       const char *methodSignature, jobject object, ... );

        static jobject executeObjectMethod( const char *className, const char *methodName,
                                            const char *methodSignature, jobject object, ... );

        static JNIEnv *getEnv(bool attachIfRequired, bool &isAttached) ;

        static jint attachCurrentThread( JNIEnv **p_env ) ;

        static jint detachCurrentThread() ;

        static jmethodID getMethodId( JNIEnv *pEnv, const char *className, jclass classObj,
                                      const char *methodName, const char *methodSignature, bool isStatic,
                                      jobject object, bool determineClassName = false ) ;

        static jclass getClass( JNIEnv *pEnv, const char *className, jobject obj, bool &isAllocated ) ;

        static JavaVM *currentVM ;

        static jobject classLoader ;

        static std::map<std::string, jmethodID> jniMethodCache ;

        static std::map<std::string, jclass> jniClassCache ;

    private:

        static jmethodID getCachedMethod(std::string key) ;

        static void cacheMethod(std::string key, jmethodID methodId) ;

        static OC::Cm::Platform::Thread::Mutex m_metex ;
};


#define JNI_TryReturn(condition, returnValue, message) \
    if (!(condition)) { \
        JNIGlobal::throwException(message) ; \
        return returnValue; \
    } else {;}

#define JNI_TryReturnVoid(condition, message) \
    if (!(condition)) { \
        JNIGlobal::throwException(message) ; \
        return ; \
    } else {;}

#define JNI_TryReturnEx(condition, returnValue, expressions, message) \
    if (!(condition)) { \
        JNIGlobal::throwException(message) ; \
        expressions; \
        return returnValue; \
    } else {;}

#define JNI_TryReturnVoidEx(condition, expressions, message) \
    if (!(condition)) { \
        JNIGlobal::throwException(message) ; \
        expressions; \
        return ; \
    } else {;}

#endif /* JNIGLOBAL_H_ */
