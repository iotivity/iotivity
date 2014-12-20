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
 * JNIGlobal.cpp
 *
 *  Created on: Jul 5, 2013
 *      Author: kirtigupta
 */

#include "JNIGlobal.h"
#include "iostream"
#include "string.h"
#include "stdlib.h"
#include "Log.h"
#include "Thread/Thread.h"

using namespace std ;

JavaVM *JNIGlobal::currentVM = NULL ;

jobject JNIGlobal::classLoader = NULL;

std::map<std::string, jmethodID> JNIGlobal::jniMethodCache ;

std::map<std::string, jclass> JNIGlobal::jniClassCache ;

OC::Cm::Platform::Thread::Mutex JNIGlobal::m_metex  ;

JNIGlobal::JNIGlobal()
{
    // TODO Auto-generated constructor stub
}

JNIGlobal::~JNIGlobal()
{
    // TODO Auto-generated destructor stub
}

std::string getMethodCacheKey(const char *className, const char *methodName,
                              const char *methodSignature)
{
    std::string retValue;

    if ( NULL != className )
    {
        retValue.assign( className ) ;
    }

    retValue.append("::") ;

    if ( NULL != methodName )
    {
        retValue.append( methodName ) ;
    }

    retValue.append("::") ;

    if ( NULL != methodSignature )
    {
        retValue.append( methodSignature ) ;
    }

    return retValue ;
}

char *replaceAll(char *str, char c, char s)
{
    int length = 0 ;
    char *pOutput = NULL ;
    char *pTemp = str ;

    while (*pTemp != 0x00)
    {
        length++ ;
        pTemp++ ;
    }

    pOutput = (char *)calloc(length + 1, sizeof(char)) ;
    if (NULL == pOutput)
    {
        return NULL;
    }

    pTemp = str ;
    char *pTempOut = pOutput ;

    while (*pTemp != 0x00)
    {
        if (*pTemp == c)
        {
            *pTempOut = s ;
        }
        else
        {
            *pTempOut = *pTemp ;
        }

        pTemp++ ;
        pTempOut++ ;
    }

    return pOutput ;
}

bool checkException(JNIEnv *pEnv)
{
    if (JNI_TRUE == (pEnv->ExceptionCheck()))
    {
        pEnv->ExceptionDescribe() ;
        pEnv->ExceptionClear() ;

        return true ;
    }
    else
    {
        return false ;
    }
}

jmethodID
JNIGlobal::getCachedMethod(std::string key)
{
    m_metex.lock();

    std::map<std::string, jmethodID>::iterator itr = jniMethodCache.find( key );
    jmethodID retValue = NULL ;

    if ( itr != jniMethodCache.end() )
    {
        retValue = itr->second ;
    }
    else
    {
        retValue = NULL ;
    }

    m_metex.unlock();
    return retValue ;
}

void
JNIGlobal::cacheMethod(std::string key, jmethodID methodId)
{
    m_metex.lock();

    jniMethodCache.insert(std::pair<std::string, jmethodID>(key, methodId));

    m_metex.unlock();
}

jobject loadClass( const char *className, const char *methodName, const char *methodSignature,
                   jobject object, ... )
{
    //CM_LOG_DEBUG_A(CM_JNI, "Start : %s, %s, %s", className, methodName, methodSignature) ;
    //std::cout << "executeObjectMethod: "<< className << "::" <<methodName << std::endl ;

    jclass      clsSerializable = NULL;
    jmethodID   m_clsmethod     = NULL;
    va_list     vlist ;
    bool        isAttached      = false ;
    bool        isClassAllocated = false ;

    JNIEnv *pEnv = JNIGlobal::getEnv(true, isAttached) ;
    if (pEnv == NULL)
    {
        //std::cout << "executeObjectMethod: Environment is NULL"<< std::endl ;
        return NULL;
    }

    va_start(vlist, object);

    ////std::cout << "executeObjectMethod :: Got Environment" << std::endl ;

    if (object == NULL)
    {
        //std::cout << "executeObjectMethod: object is NULL"<< std::endl ;
    }

    //Obtaining Classes
    clsSerializable = JNIGlobal::getClass(pEnv, className, object, isClassAllocated);

    ////std::cout << "executeObjectMethod :: Got class" << std::endl ;

    //Obtaining Method IDs
    if (clsSerializable != NULL)
    {
        m_clsmethod = JNIGlobal::getMethodId( pEnv, className, clsSerializable, methodName, methodSignature,
                                              false, object, true ) ;
    }
    else
    {
        //std::cout << "executeObjectMethod: clsSerializable is NULL"<< std::endl ;
    }

    ////std::cout << "executeObjectMethod :: Got method" << std::endl ;

    if (m_clsmethod == NULL)
    {
        //CM_LOG_DEBUG_A(CM_JNI, "%s", "IN LOAD CLASS after calling getmethodID && m_clsmethod is NULL") ;
        //std::cout << "executeObjectMethod: m_clsmethod is NULL"<< std::endl ;
    }

    ////std::cout << "executeObjectMethod ::invoking java call" << std::endl ;
    //CM_LOG_DEBUG_A(CM_JNI, "%s", "IN LOAD CLASS after calling getmethodID && m_clsmethod is not NULL") ;

    jobject retValue  = pEnv->CallObjectMethodV( object, m_clsmethod, vlist ) ;

    if (retValue != NULL)
    {
        //CM_LOG_DEBUG_A(CM_JNI, "%s", "CallObjectMethodV does not return NULL") ;
    }
    else
    {
        CM_LOG_DEBUG_A(CM_JNI, "%s", "CallObjectMethodV returned NULL") ;
    }
    if ( isAttached )
    {
        //CM_LOG_DEBUG_A(CM_JNI, "%s", "isAttached is true, detach the current thread") ;
        JNIGlobal::detachCurrentThread() ;
    }

    va_end(vlist) ;

    //CM_LOG_DEBUG_A(CM_JNI, "%s", "Before deleting local reference") ;

    if (isClassAllocated)
    {
        pEnv->DeleteLocalRef(clsSerializable) ;
    }

    //std::cout << "Exit: executeObjectMethod: "<< className << "::" <<methodName << std::endl ;

    //CM_LOG_DEBUG_A(CM_JNI, "%s", "LOAD_CLASS_Exit") ;

    return retValue ;
}

jclass
JNIGlobal::getClass( JNIEnv *pEnv, const char *className, jobject obj, bool &isAllocated )
{
    //CM_LOG_DEBUG_A(CM_JNI, "Start : %s", className) ;
    std::string key;
    isAllocated = false ;

    if ( className == NULL )
    {
        if (NULL != obj)
        {
            return pEnv->GetObjectClass( obj ) ;
//          isAllocated = true ;
        }
        else
        {
            return NULL ;
        }
    }

    key.assign(className) ;

    std::map<std::string, jclass>::iterator itr = jniClassCache.find(key);

    if ( itr != jniClassCache.end() )
    {
        CM_LOG_DEBUG_A(CM_JNI, "%s", "Found in Class cache") ;
        return itr->second ;
    }
    else
    {
        jclass retValue = NULL ;

        if ( NULL != obj )
        {
            //CM_LOG_DEBUG_A(CM_JNI, "%s", "Returning GetObjectClass()") ;
            retValue = pEnv->GetObjectClass( obj ) ;
            isAllocated = true ;
        }
        else
        {
            // Find class if not found load it
            char *pReplacedStr = replaceAll((char *)className, '/', '.') ;
            if (NULL == pReplacedStr)
            {
                //CM_LOG_DEBUG_A(CM_JNI, "%s", "Failed to Allocate memory: returning NULL") ;
                return NULL;
            }
            else
            {
                jstring jclassName = (pEnv)->NewStringUTF(pReplacedStr) ;
                if (JNIGlobal::classLoader == NULL)
                {
                    //CM_LOG_DEBUG_A(CM_JNI, "%s", "ClassLoader is NULL") ;
                }

                retValue = (jclass) loadClass("Ljava/lang/ClassLoader", "loadClass",
                                              "(Ljava/lang/String;Z)Ljava/lang/Class;", JNIGlobal::classLoader, jclassName, JNI_TRUE) ;
                (pEnv)->DeleteLocalRef(jclassName);
                isAllocated = true ;
                free(pReplacedStr) ;
            }

            if ( retValue == NULL )
            {
                retValue = pEnv->FindClass( className ) ;
                if (retValue != NULL)
                {
                    isAllocated = true ;
                }
            }
        }

        if ( retValue != NULL )
        {
            //jniClassCache.insert(std::pair<std::string, jclass>(key, retValue)) ;
        }

        //CM_LOG_DEBUG_A(CM_JNI, "%s", "Exit") ;
        return retValue;
    }
}

//jobject getClassObject( JNIEnv* pEnv, const char* className, jobject object )
//{
//  //std::cout << "getClassName: "<< std::endl ;
//
//  jmethodID   m_clsmethod     = NULL;
//
//  jclass classObj = pEnv->GetObjectClass(object);
//
//  //Obtaining Method IDs
//  if (classObj != NULL)
//  {
//      m_clsmethod = JNIGlobal::getMethodId( pEnv, className, classObj, "getClass", "()Ljava/lang/Class;", false, object, true ) ;
//  }
//  else
//  {
//      //std::cout << "getClassName: classObj is NULL"<< std::endl ;
//      return NULL ;
//  }
//
//  //std::cout << "getClassName :: Got method" << std::endl ;
//
//  if (m_clsmethod == NULL)
//  {
//      //std::cout << "getClassName: m_clsmethod is NULL"<< std::endl ;
//      return NULL ;
//  }
//
//  //std::cout << "getClassName ::invoking java call" << std::endl ;
//
//  jobject retValue  = pEnv->CallObjectMethod( object, m_clsmethod ) ;
//
//  //std::cout << "Exit: getClassName: " << std::endl ;
//
//  return retValue ;
//}

jstring getClassName( JNIEnv *pEnv, const char *className, jclass classObj )
{
    bool isClassAllocated = false ;
    //std::cout << "getClassName: "<< std::endl ;

    jclass cls = JNIGlobal::getClass(pEnv, "java/lang/Class", NULL, isClassAllocated) ;

    jmethodID methodId = JNIGlobal::getMethodId(pEnv, className, cls, "getName", "()Ljava/lang/String;",
                         false, classObj, true) ;

    jobject retValue  = pEnv->CallObjectMethod( classObj, methodId ) ;

    //std::cout << "Exit: getClassName: " << std::endl ;
    if (isClassAllocated)
    {
        pEnv->DeleteLocalRef(cls) ;
    }
    return (jstring)retValue ;
}

jmethodID
JNIGlobal::getMethodId( JNIEnv *pEnv, const char *className, jclass classObj,
                        const char *methodName, const char *methodSignature, bool isStatic,
                        jobject object, bool determineClassName )
{
    //CM_LOG_DEBUG_A(CM_JNI, "Start : %s", className) ;
    bool isAllocated  = false ;
    bool isClassAllocated = false ;

    if ((classObj != NULL) && (determineClassName == false))
    {
        //CM_LOG_DEBUG_A(CM_JNI, "%s", "determine class name") ;
        // Get class name from class object
        jstring classNameJava = getClassName(pEnv, className, classObj) ;
        checkException(pEnv);
        if (NULL != classNameJava)
        {
            const char *jniClasName = pEnv->GetStringUTFChars(classNameJava, JNI_FALSE);
            if (NULL != jniClasName)
            {
                char *pReplacedStr = replaceAll((char *)jniClasName, '.', '/') ;
                if (NULL != pReplacedStr)
                {
                    className = pReplacedStr ;
                    isAllocated = true ;
                }

                pEnv->ReleaseStringUTFChars(classNameJava, jniClasName);
            }

            pEnv->DeleteLocalRef(classNameJava);
        }
    }

    //CM_LOG_DEBUG_A(CM_JNI, "%s", "get CachedMethod") ;
    std::string key = getMethodCacheKey( className, methodName, methodSignature ) ;

    jmethodID retValue = getCachedMethod(key) ;

    if ( NULL != retValue )
    {
        //CM_LOG_DEBUG_A(CM_JNI, "%s", "Got CachedMethod") ;
        if ((isAllocated) && (NULL != className))
        {
            free((char *)className);
        }

        //CM_LOG_DEBUG_A(CM_JNI, "%s", "Exit") ;
        return retValue ;
    }
    else
    {
        //CM_LOG_DEBUG_A(CM_JNI, "%s", "Not available Cached Method") ;

        jmethodID retValue = NULL ;

        if ( classObj == NULL )
        {
            classObj = JNIGlobal::getClass(pEnv, className, object, isClassAllocated);
        }

        if ( classObj != NULL )
        {
            if ( isStatic )
            {
                //CM_LOG_DEBUG_A(CM_JNI, "%s", "classObj not NULL and isStatic is TRUE") ;
                retValue = pEnv->GetStaticMethodID(classObj, methodName, methodSignature) ;
            }
            else
            {
                //CM_LOG_DEBUG_A(CM_JNI, "%s", "classObj not NULL and isStatic is FALSE") ;
                retValue = pEnv->GetMethodID(classObj, methodName, methodSignature) ;
            }
        }

        if (retValue != NULL)
        {
            //CM_LOG_DEBUG_A(CM_JNI, "%s", "retValue is not NULL") ;
            cacheMethod(key, retValue) ;
        }

        if ((isAllocated) && (NULL != className))
        {
            free((char *)className);
        }
        if (isClassAllocated)
        {
            pEnv->DeleteLocalRef(classObj) ;
        }
        return retValue ;
    }
}

JNIEnv *
JNIGlobal::getEnv(bool attachIfRequired, bool &isAttached)
{
//  CM_LOG_DEBUG_A(CM_JNI, "%s", "JNIGlobal:getEnv :: Start") ;

    int retry_count = 0;
    while (retry_count < 2)
    {
        JNIEnv *currentEnv = NULL ;
        jint returnValue =  currentVM->GetEnv( (void **)&currentEnv, CM_JNI_VERSION ) ;

        isAttached = false ;

        //  CM_LOG_DEBUG_A(CM_JNI, "JNIGlobal:getEnv :: returnValue %d", returnValue) ;

        if (returnValue == JNI_OK)
        {
            //      CM_LOG_DEBUG_A(CM_JNI, "%s", "JNIGlobal:getEnv :: Exit") ;
            return currentEnv ;
        }
        else if (returnValue == JNI_EDETACHED)
        {
            if (attachIfRequired)
            {
                returnValue = JNIGlobal::attachCurrentThread(&currentEnv) ;
                if (returnValue == JNI_OK)
                {
                    isAttached = true ;
                    //              CM_LOG_DEBUG_A(CM_API, "%s", "JNIGlobal:getEnv :: Exit") ;
                    return currentEnv ;
                }
            }
        }
        retry_count++;
    }

//  CM_LOG_DEBUG_A(CM_JNI, "%s", "JNIGlobal:getEnv :: Exit") ;
    return NULL ;
}
void jniThreadExitCallBack()
{
    CM_LOG_DEBUG_A(CM_PAL, "%s", "Detaching Current Thread") ;
    JNIGlobal::currentVM->DetachCurrentThread() ;
}
jint
JNIGlobal::attachCurrentThread( JNIEnv **p_env )
{
#ifdef __ANDROID__
    jint retValue = currentVM->AttachCurrentThread( p_env, NULL ) ;
#else
    jint retValue = currentVM->AttachCurrentThread( (void **)(p_env), NULL ) ;
#endif
    if (JNI_OK == retValue)
    {
        OC::Cm::Platform::Thread::Thread::AddThreadExitCallBack(jniThreadExitCallBack);
    }

    return retValue;

//#else
//  return currentVM->AttachCurrentThread( (void**)(p_env), NULL ) ;
//#endif

}

jint
JNIGlobal::detachCurrentThread( )
{
    //return currentVM->DetachCurrentThread() ;
    return 0;
}

bool
JNIGlobal::convertToStdString(jstring jinput, std::string &out)
{
    bool        isAttached      = false ;
    bool        bRet            = false ;

    if ( NULL == jinput )
    {
        return false;
    }

    JNIEnv *pEnv = JNIGlobal::getEnv(true, isAttached) ;
    if (NULL == pEnv)
    {
        return bRet ;
    }

    const char *resPath = pEnv->GetStringUTFChars(jinput, JNI_FALSE);
    if (NULL != resPath)
    {
        out.assign(resPath) ;
        pEnv->ReleaseStringUTFChars(jinput, resPath);
        bRet = true ;
    }

    if ( isAttached )
    {
        JNIGlobal::detachCurrentThread() ;
    }

    return bRet ;
}

jstring
JNIGlobal::convertToJString( std::string value )
{
    bool        isAttached      = false ;
    jstring     retValue        = NULL ;

    JNIEnv *pEnv = JNIGlobal::getEnv(true, isAttached) ;
    if (NULL != pEnv)
    {
        retValue = pEnv->NewStringUTF( value.c_str() );
        if (checkException(pEnv))
        {
            retValue = NULL ;
        }
    }

    if ( isAttached )
    {
        JNIGlobal::detachCurrentThread() ;
    }

    return retValue ;
}

void
JNIGlobal::throwException(const char *errorMessage)
{
    bool        isAttached      = false ;

    JNIEnv *pEnv = JNIGlobal::getEnv(true, isAttached) ;
    if (NULL != pEnv)
    {
        jclass mapClass = pEnv->FindClass("java/lang/Exception");
        if (false == checkException(pEnv))
        {
            (pEnv)->ThrowNew(mapClass, errorMessage) ;
        }

        JNIGlobal::releaseLocalRef(mapClass);
    }

    if ( isAttached )
    {
        JNIGlobal::detachCurrentThread() ;
    }
}

jobject
JNIGlobal::getGlobleRef(jobject obj)
{
    bool        isAttached = false ;
    jobject     retObject = NULL ;

    JNIEnv *pEnv = JNIGlobal::getEnv(true, isAttached) ;
    if (NULL == pEnv)
    {
        return NULL;
    }

    retObject = pEnv->NewGlobalRef(obj) ;

    if ( isAttached )
    {
        JNIGlobal::detachCurrentThread() ;
    }

    return retObject ;
}

void
JNIGlobal::releaseGlobleRef(jobject obj)
{
    bool  isAttached        = false ;
    JNIEnv *pEnv = JNIGlobal::getEnv(true, isAttached) ;
    if (NULL == pEnv)
    {
        return;
    }

    pEnv->DeleteGlobalRef(obj) ;

    if ( isAttached )
    {
        JNIGlobal::detachCurrentThread() ;
    }
}


void
JNIGlobal::releaseLocalRef(jobject obj)
{

    if (obj == NULL)
    {
        return ;
    }

    bool  isAttached        = false ;
    JNIEnv *pEnv = JNIGlobal::getEnv(true, isAttached) ;
    if (NULL == pEnv)
    {
        return;
    }

    pEnv->DeleteLocalRef(obj) ;

    if ( isAttached )
    {
        JNIGlobal::detachCurrentThread() ;
    }
}

jobject
JNIGlobal::getEnum(const char *className, const char *method, const char *signature, int enumValue)
{
    bool isClassAllocated = false ;
    CM_LOG_DEBUG_A(CM_JNI, "Start : %s, %s, %s", className, method, signature) ;

    jclass      ClsErrorCode    = NULL;
    jmethodID   m_getErrorCode  = NULL;
    bool        isAttached      = false ;
    jobject     jretObject      = NULL ;

    JNIEnv *pEnv = JNIGlobal::getEnv(true, isAttached) ;
    if (NULL == pEnv)
    {
        CM_LOG_ERROR_A(CM_JNI, "%s", "pEnv is NULL") ;
        return NULL;
    }

    ClsErrorCode    = getClass(pEnv, className, NULL, isClassAllocated);
    m_getErrorCode  = getMethodId( pEnv, className, ClsErrorCode, method, signature, true, NULL ) ;

    if ((ClsErrorCode == NULL) || ( m_getErrorCode == NULL))
    {
        // TODO: Return NULL
    }

    // invoke method
    jretObject = pEnv->CallStaticObjectMethod(ClsErrorCode, m_getErrorCode, (int)enumValue);

    if ( isAttached )
    {
        JNIGlobal::detachCurrentThread() ;
    }
    if (isClassAllocated)
    {
        pEnv->DeleteLocalRef(ClsErrorCode) ;
    }

//  CM_LOG_DEBUG_A(CM_JNI, "Exit: getEnum: %s::%s", className, method) ;

    return jretObject ;
}

jobject
JNIGlobal::constructObject( const char *className, const char *methodSignature, ... )
{
    //std::cout << "constructObject: "<< className << "::" <<methodSignature << std::endl ;
    jclass      clsSer              = NULL;
    jmethodID   m_constructor       = NULL;
    jobject     jretObject          = NULL  ;
    va_list     vlist ;
    bool        isAttached          = false ;
    bool        isClassAllocated    = false;

    JNIEnv *pEnv = JNIGlobal::getEnv(true, isAttached) ;
    if (NULL == pEnv)
    {
        return NULL;
    }

    va_start(vlist, methodSignature);

    clsSer  = getClass(pEnv, className, NULL, isClassAllocated);
    m_constructor   = getMethodId( pEnv, className, clsSer, "<init>", methodSignature, false, NULL ) ;

    if ((clsSer == NULL) || ( m_constructor == NULL))
    {
        // TODO: Return NULL
    }

    jretObject = pEnv->NewObjectV(clsSer, m_constructor, vlist);

    va_end(vlist) ;

    if ( isAttached )
    {
        JNIGlobal::detachCurrentThread() ;
    }

    //std::cout << "Exit: constructObject: "<< className << "::" <<methodSignature << std::endl ;
    if (isClassAllocated)
    {
        pEnv->DeleteLocalRef(clsSer) ;
    }
    return jretObject ;
}


bool
JNIGlobal::executeBoolMethod( const char *className, const char *methodName,
                              const char *methodSignature, jobject object, ... )
{

    //std::cout << "executeBoolMethod: "<< className << "::" <<methodName << std::endl ;

    jclass      clsSerializable = NULL;
    jmethodID   m_clsmethod     = NULL;
    va_list     vlist ;
    bool        isAttached      = false ;
    bool isClassAllocated       = false;

    bool        execMthdReturn;

    JNIEnv *pEnv = JNIGlobal::getEnv(true, isAttached) ;

    if (pEnv == NULL)
    {
        //std::cout << "executeBoolMethod: Environment is NULL"<< std::endl ;
        return false;
    }

    va_start(vlist, object);

    //std::cout << "executeBoolMethod :: Got Environment" << std::endl ;

    if (object == NULL)
    {
        //std::cout << "executeBoolMethod: object is NULL"<< std::endl ;
    }

    //Obtaining Classes
    clsSerializable = getClass(pEnv, className, object, isClassAllocated);

    //std::cout << "executeBoolMethod :: Got class" << std::endl ;

    //Obtaining Method IDs
    if (clsSerializable != NULL)
    {
        m_clsmethod = getMethodId( pEnv, className, clsSerializable, methodName, methodSignature, false,
                                   object ) ;
    }
    else
    {
        //std::cout << "executeBoolMethod: clsSerializable is NULL"<< std::endl ;
    }

    //std::cout << "executeBoolMethod :: Got method" << std::endl ;
    if (m_clsmethod == NULL)
    {
        //std::cout << "executeBoolMethod: m_clsmethod is NULL"<< std::endl ;
    }

    //std::cout << "executeBoolMethod ::invoking java call" << std::endl ;

    jboolean methodReturn = pEnv->CallBooleanMethodV( object, m_clsmethod, vlist );

    if (methodReturn == JNI_TRUE)
    {
        execMthdReturn = true ;
    }
    else
    {
        execMthdReturn = false ;
    }

    if ( isAttached )
    {
        JNIGlobal::detachCurrentThread() ;
    }

    va_end(vlist) ;
    if (isClassAllocated)
    {
        pEnv->DeleteLocalRef(clsSerializable) ;
    }
    //std::cout << "Exit: executeBoolMethod: "<< className << "::" <<methodName << std::endl ;
    return execMthdReturn;
}

void
JNIGlobal::executeVoidMethod( const char *className, const char *methodName,
                              const char *methodSignature, jobject object, ... )
{
    //std::cout << "executeVoidMethod: "<< className << "::" <<methodName << std::endl ;
    //CM_LOG_DEBUG_A(CM_JNI, "%s","executeVoidMethod ");

    jclass      clsSerializable = NULL;
    jmethodID   m_clsmethod     = NULL;
    va_list     vlist ;
    bool        isAttached      = false ;
    bool       isClassAllocated = false;

    JNIEnv *pEnv = JNIGlobal::getEnv(true, isAttached) ;
    if (pEnv == NULL)
    {
        //std::cout << "executeVoidMethod: Environment is NULL"<< std::endl ;
        return;
    }

    va_start(vlist, object);

    //std::cout << "executeVoidMethod :: Got Environment" << std::endl ;
    //CM_LOG_DEBUG_A(CM_JNI, "%s","executeVoidMethod :: Got Environment ");

    if (object == NULL)
    {
        //std::cout << "executeVoidMethod: object is NULL"<< std::endl ;
    }
    //Obtaining Classes

    clsSerializable = getClass(pEnv, className, object, isClassAllocated);
    //std::cout << "executeVoidMethod :: Got class" << std::endl ;

    //Obtaining Method IDs
    if (clsSerializable != NULL)
    {
        m_clsmethod = getMethodId( pEnv, className, clsSerializable, methodName, methodSignature, false,
                                   object ) ;
    }
    else
    {
        //std::cout << "executeVoidMethod: clsSerializable is NULL"<< std::endl ;
        //CM_LOG_DEBUG_A(CM_JNI, "%s","executeVoidMethod: clsSerializable is NULL ");
    }

    //std::cout << "executeVoidMethod :: Got method" << std::endl ;
    //CM_LOG_DEBUG_A(CM_JNI, "%s","executeVoidMethod :: Got method ");

    if (m_clsmethod == NULL)
    {
        //std::cout << "executeVoidMethod: m_clsmethod is NULL"<< std::endl ;
        CM_LOG_DEBUG_A(CM_JNI, "%s", "executeVoidMethod: clsSerializable is NULL ");
    }
    //std::cout << "executeVoidMethod ::invoking java call" << std::endl ;
    //CM_LOG_DEBUG_A(CM_JNI, "%s","executeVoidMethod ::invoking java call ");

    pEnv->CallVoidMethodV( object, m_clsmethod, vlist );

    if ( isAttached )
    {
        JNIGlobal::detachCurrentThread() ;
    }
    if (isClassAllocated)
    {
        pEnv->DeleteLocalRef(clsSerializable) ;
    }
    va_end(vlist) ;
    //std::cout << "Exit: executeVoidMethod: "<< className << "::" <<methodName << std::endl ;
    //CM_LOG_DEBUG_A(CM_JNI, "%s","Exit: executeVoidMethod: ");
}

jobject
JNIGlobal::executeObjectMethod( const char *className, const char *methodName,
                                const char *methodSignature, jobject object, ... )
{
    //std::cout << "executeObjectMethod: "<< className << "::" <<methodName << std::endl ;

    jclass      clsSerializable = NULL;
    jmethodID   m_clsmethod     = NULL;
    va_list     vlist ;
    bool        isAttached      = false ;
    bool       isClassAllocated = false;

    JNIEnv *pEnv = JNIGlobal::getEnv(true, isAttached) ;
    if (pEnv == NULL)
    {
        //std::cout << "executeObjectMethod: Environment is NULL"<< std::endl ;
        return NULL;
    }

    va_start(vlist, object);

    //std::cout << "executeObjectMethod :: Got Environment" << std::endl ;

    if (object == NULL)
    {
        //std::cout << "executeObjectMethod: object is NULL"<< std::endl ;
    }

    //Obtaining Classes
    clsSerializable = getClass(pEnv, className, object, isClassAllocated);

    //std::cout << "executeObjectMethod :: Got class" << std::endl ;

    //Obtaining Method IDs
    if (clsSerializable != NULL)
    {
        m_clsmethod = getMethodId( pEnv, className, clsSerializable, methodName, methodSignature, false,
                                   object ) ;
    }
    else
    {
        //std::cout << "executeObjectMethod: clsSerializable is NULL"<< std::endl ;
    }

    //std::cout << "executeObjectMethod :: Got method" << std::endl ;

    if (m_clsmethod == NULL)
    {
        //std::cout << "executeObjectMethod: m_clsmethod is NULL"<< std::endl ;
    }

    //std::cout << "executeObjectMethod ::invoking java call" << std::endl ;

    jobject retValue  = pEnv->CallObjectMethodV( object, m_clsmethod, vlist ) ;

    if ( isAttached )
    {
        JNIGlobal::detachCurrentThread() ;
    }

    va_end(vlist) ;

    if (isClassAllocated)
    {
        pEnv->DeleteLocalRef(clsSerializable);
    }
    //std::cout << "Exit: executeObjectMethod: "<< className << "::" <<methodName << std::endl ;

    return retValue ;
}

jlong
JNIGlobal::executeLongMethod( const char *className, const char *methodName,
                              const char *methodSignature, jobject object, ... )
{
    jclass      clsSerializable = NULL;
    jmethodID   m_clsmethod     = NULL;
    va_list     vlist ;
    bool        isAttached      = false ;
    bool       isClassAllocated = false ;

    jlong       execMthdReturn = -1;

    JNIEnv *pEnv = JNIGlobal::getEnv(true, isAttached) ;
    if (NULL == pEnv)
    {
        return -1;
    }

    va_start(vlist, object);

    //Obtaining Classes
    clsSerializable = getClass(pEnv, className, object, isClassAllocated);

    //Obtaining Method IDs
    if (clsSerializable != NULL)
    {
        m_clsmethod = getMethodId( pEnv, className, clsSerializable, methodName, methodSignature, false,
                                   object ) ;
        execMthdReturn = pEnv->CallLongMethodV( object, m_clsmethod, vlist );
    }

    va_end(vlist) ;

    if ( isAttached )
    {
        JNIGlobal::detachCurrentThread() ;
    }

    if (isClassAllocated)
    {
        pEnv->DeleteLocalRef(clsSerializable) ;
    }
    return execMthdReturn;
}

jint
JNIGlobal::executeIntMethod( const char *className, const char *methodName,
                             const char *methodSignature, jobject object, ... )
{
    jclass      clsSerializable = NULL;
    jmethodID   m_clsmethod     = NULL;
    va_list     vlist ;
    bool        isAttached      = false ;
    bool       isClassAllocated = false;
    jint        execMthdReturn = -1;

    JNIEnv *pEnv = JNIGlobal::getEnv(true, isAttached) ;
    if (NULL == pEnv)
    {
        return -1;
    }

    va_start(vlist, object);

    //Obtaining Classes
    clsSerializable = getClass(pEnv, className, object, isClassAllocated);

    //Obtaining Method IDs
    if (clsSerializable != NULL)
    {
        m_clsmethod = getMethodId( pEnv, className, clsSerializable, methodName, methodSignature, false,
                                   object ) ;
        execMthdReturn = pEnv->CallIntMethodV( object, m_clsmethod, vlist );
    }

    va_end(vlist) ;

    if ( isAttached )
    {
        JNIGlobal::detachCurrentThread() ;
    }

    if (isClassAllocated)
    {
        pEnv->DeleteLocalRef(clsSerializable) ;
    }
    return execMthdReturn;
}
