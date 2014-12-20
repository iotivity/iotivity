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
#include "OC_Cm_ControlManager.h"
#include "ControlManager.h"
#include "Device.h"
#include "MyCMListener.h"
#include "iostream"
//#include "SubscriptionManager.h"
#include "CMResourceHandlerFactory.h"
#include "CMLogType.h"
#include "JNIGlobal.h"
#include "Log.h"
#include "MyNotificationListener.h"

#ifdef REMOTE_ACCESS_SUPPORT
#include "CMModes.h"
#endif

JNIEXPORT jlong JNICALL Java_OC_Cm_ControlManager_constructNative
(JNIEnv *pEnv, jclass cl)
{
    std::cout << "in Java_OC_Cm_ControlManager_constructNative " << std::endl ;

    LOGV("JNIControlManager", "Java_OC_Cm_ControlManager_constructNative");

    OC::Cm::ControlManager *cmObj = OC::Cm::ControlManager::getInstance() ;

    jlong native = (jlong)(cmObj);

    return native;
}

JNIEXPORT void JNICALL Java_OC_Cm_ControlManager_deleteNative
(JNIEnv *pEnv, jclass cl, jlong instance)
{
    std::cout << "in Java_OC_Cm_ControlManager_deleteNative " << std::endl ;
    OC::Cm::ControlManager *cmObj = (OC::Cm::ControlManager *) instance ;
    if (NULL != cmObj)
    {
        delete cmObj; cmObj = NULL;
    }
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_ControlManager_start
(JNIEnv *env, jobject thisObj, jlong instance, jlong device)
{
    std::cout << "in Java_OC_Cm_ControlManager_start " << std::endl ;
    CM_LOG_DEBUG_A(CM_API, "%s", "Java_OC_Cm_ControlManager_start  START");


    OC::Cm::ControlManager *cmObj = (OC::Cm::ControlManager *) instance;

    OC::Cm::Device *myDevice = (OC::Cm::Device *)device;

    if ( NULL == cmObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ControlManager object is NULL") ;
        return -1;
    }

    bool ret = cmObj->start(*myDevice);

    std::cout << "calling native start " << std::endl ;
    LOGV("JNIControlManager", "Java_OC_Cm_ControlManager_start : END");

    return ret;

}

JNIEXPORT jboolean JNICALL Java_OC_Cm_ControlManager_stop
(JNIEnv *env, jobject thisObj, jlong instance, jboolean isInternalStop)
{
    OC::Cm::ControlManager *cmObj = (OC::Cm::ControlManager *) instance;

    if ( NULL == cmObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ControlManager object is NULL") ;
        return -1;
    }

    return cmObj->stop(isInternalStop == JNI_TRUE);
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_ControlManager_isStarted
(JNIEnv *env, jobject thisObj, jlong instance)
{
    OC::Cm::ControlManager *cmObj = (OC::Cm::ControlManager *) instance;

    if ( NULL == cmObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ControlManager object is NULL") ;
        return -1;
    }

    return cmObj->isStarted();
}

/*  SRIB - NOT Supported

JNIEXPORT jboolean JNICALL Java_OC_Cm_ControlManager_isSCSConnected
(JNIEnv *env, jobject thisObj, jlong instance)
{
#ifdef REMOTE_ACCESS_SUPPORT
    OC::Cm::ControlManager* cmObj = (OC::Cm::ControlManager*) instance;

    if ( NULL == cmObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ControlManager object is NULL") ;
        return -1;
    }

    //return cmObj->isSCSConnected(); // SRIB - NOT Supported
    return false;
#else
    return false;
#endif
}

*/


JNIEXPORT jobject JNICALL Java_OC_Cm_ControlManager_getCMState
(JNIEnv *env, jobject thisObj, jlong instance)
{
    OC::Cm::ControlManager *cmObj = (OC::Cm::ControlManager *) instance;
    if ( NULL == cmObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ControlManager object is NULL") ;
        return NULL;
    }

    jobject cmState =  JNIGlobal::getEnum("OC/Cm/CMStates", "getCMState", "(I)LOC/Cm/CMStates;",
                                          (int)cmObj->getState()) ;
    return cmState;
}

/*  SRIB - NOT Supported

JNIEXPORT jboolean JNICALL Java_OC_Cm_ControlManager_getSimulatorFlag
  (JNIEnv *env, jobject thisObj, jlong instance)
{
    OC::Cm::ControlManager* cmObj = (OC::Cm::ControlManager*) instance;
    if ( NULL == cmObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ControlManager object is NULL") ;
        return JNI_FALSE;
    }

    return cmObj->getSimulatorFlag();  //  SRIB - NOT Supported
}


JNIEXPORT void JNICALL Java_OC_Cm_ControlManager_setSimulatorFlag
  (JNIEnv *env, jobject thisObj, jlong instance, jboolean jFlag)
{
    OC::Cm::ControlManager* cmObj = (OC::Cm::ControlManager*) instance;
    if ( NULL == cmObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ControlManager object is NULL") ;
        return;
    }

    cmObj->setSimulatorFlag(jFlag == JNI_TRUE); // SRIB- Not implemented
}

 SRIB - NOT Supported  */

JNIEXPORT jboolean JNICALL Java_OC_Cm_ControlManager_setConfiguration
(JNIEnv *env, jobject thisObj, jlong instance, jlong config)
{

    OC::Cm::ControlManager *cmObj = (OC::Cm::ControlManager *) instance;

    OC::Cm::Configuration *p_config = (OC::Cm::Configuration *)config;

    if ( NULL == cmObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ControlManager object is NULL") ;
        return -1;
    }

    MyCMResourceHandlerFactory *factory =  (MyCMResourceHandlerFactory *)
                                           p_config->getResourceHandlerFactory();
    if (NULL != factory)
    {
        factory->setContext((jlong) cmObj->getContext());
    }
    return cmObj->setConfiguration(p_config);
}

JNIEXPORT jlong JNICALL Java_OC_Cm_ControlManager_setCMListener
(JNIEnv *env, jobject thisObj, jlong instance, jobject listener)
{
    OC::Cm::ControlManager *cmObj = (OC::Cm::ControlManager *) instance;

    if ( NULL == cmObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ControlManager object is NULL") ;
        return -1;
    }

    CMListener *myCmListener = new CMListener(listener);

    if ( NULL == myCmListener)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ControlManager listener is NULL") ;
        return -1;
    }

    cmObj->setListener(*myCmListener);

    return (jlong)myCmListener ;
}

JNIEXPORT void JNICALL Java_OC_Cm_ControlManager_removeCMListener
(JNIEnv *env, jobject thisObj, jlong instance, jlong listener)
{
    OC::Cm::ControlManager *cmObj = (OC::Cm::ControlManager *) instance;

    if ( NULL == cmObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ControlManager object is NULL") ;
        return;
    }

    CMListener *myCmListener = (CMListener *)listener ;
    if ( NULL == myCmListener)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ControlManager listener is NULL") ;
        return;
    }

    cmObj->removeListener(*myCmListener);

    delete myCmListener ;
}

JNIEXPORT jlong JNICALL Java_OC_Cm_ControlManager_getDeviceFinder
(JNIEnv *env, jobject thisObj, jlong instance)
{

    OC::Cm::ControlManager *cmObj = (OC::Cm::ControlManager *) instance;

    if ( NULL == cmObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ControlManager object is NULL") ;
        return -1;
    }

    return (jlong)cmObj->getDeviceFinder();

}

JNIEXPORT jlong JNICALL Java_OC_Cm_ControlManager_getConfiguration
(JNIEnv *env, jobject thisObj, jlong instance)
{
    OC::Cm::ControlManager *cmObj = (OC::Cm::ControlManager *) instance;

    if ( NULL == cmObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ControlManager object is NULL") ;
        return -1;
    }

    return (jlong)(cmObj->getConfiguration());
}


/*  SRIB - Not supported
JNIEXPORT jint JNICALL Java_OC_Cm_ControlManager_getTimer
(JNIEnv *env, jobject thisObj, jlong instance, jint timerType)
{
#ifdef REMOTE_ACCESS_SUPPORT
    OC::Cm::ControlManager* cmObj = (OC::Cm::ControlManager*) instance;

    if ( NULL == cmObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ControlManager object is NULL") ;
        return -1;
    }

    OC::Cm::TimerType tType = (OC::Cm::TimerType)timerType;

    return (jint)cmObj->getTimer(tType);
#else
    return (jint)0;
#endif
}

 SRIB - Not supported */

/*
 * Class:     OC_Cm_ControlManager
 * Method:    setCMLogLevel
 * Signature: (J)LOC/Cm/CMLogType;
 */
JNIEXPORT jboolean JNICALL Java_OC_Cm_ControlManager_setCMLogLevel
(JNIEnv *env, jobject thisObj, jlong instance, jint cmLogLevel)
{
    OC::Cm::ControlManager *cmObj = (OC::Cm::ControlManager *) instance;

    if ( NULL == cmObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ControlManager object is NULL") ;
        return false;
    }

    OC::Cm::CMLogType logType = (OC::Cm::CMLogType)cmLogLevel;

    return cmObj->setLogLevel(logType);
}

JNIEXPORT jlong JNICALL Java_OC_Cm_ControlManager_getSubscriptionManager
(JNIEnv *env, jobject thisObj, jlong instance)
{
    OC::Cm::ControlManager *cmObj = (OC::Cm::ControlManager *)instance;

    if ( NULL == cmObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ControlManager object is NULL") ;
        return -1;
    }

    //return (jlong)cmObj->getSubscriptionManager(); // getSubscriptionManager() is removed from ControlManager class.
    return -1;

}

JNIEXPORT jlong JNICALL Java_OC_Cm_ControlManager_addNotificationListener
(JNIEnv *env, jobject thisObj, jlong instance, jobject notificationListener)
{

    CM_LOG_DEBUG_A(CM_API, "%s", "Java_OC_Cm_ControlManager_addNotificationListener  START ");
    OC::Cm::Notification::INotificationListener *notificationLstnr = new MyNotificationListener(
        notificationListener) ;
    if ( NULL == notificationLstnr)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Notification Listener object is NULL") ;
        return -1;
    }

    OC::Cm::ControlManager *cmObj = (OC::Cm::ControlManager *)instance;

    if ( NULL == cmObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ControlManager object is NULL") ;
        return -1;
    }


    cmObj->addNotificationListener(*notificationLstnr);

    CM_LOG_DEBUG_A(CM_API, "%s", "Java_OC_Cm_ControlManager_addNotificationListener END ");

    return (jlong)notificationLstnr ;

}

JNIEXPORT void JNICALL Java_OC_Cm_ControlManager_removeNotificationListener
(JNIEnv *env, jobject thisObj, jlong instance, jlong notificationListener)
{
    CM_LOG_DEBUG_A(CM_API, "%s", "Java_OC_Cm_ControlManager_removeNotificationListener  called ");
    MyNotificationListener *notificationLstnr =  (MyNotificationListener *)(notificationListener) ;
    if ( NULL == notificationLstnr)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Notification Listener object is NULL") ;
        return ;
    }

    OC::Cm::ControlManager *cmObj = (OC::Cm::ControlManager *)instance;

    if ( NULL == cmObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ControlManager object is NULL") ;
        return ;
    }

    cmObj->removeNotificationListener(*notificationLstnr) ;
    delete notificationLstnr;
}


JNIEXPORT jboolean JNICALL Java_OC_Cm_ControlManager_validateConfiguration
(JNIEnv *env, jobject thisObj, jlong instance, jobject config )
{

    OC::Cm::ControlManager *cmObj = (OC::Cm::ControlManager *)instance;

    OC::Cm::Configuration *p_config = (OC::Cm::Configuration *)config;

    if ( NULL == cmObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ControlManager object is NULL") ;
        return -1;
    }

    if ( NULL == p_config)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Configuration object is NULL") ;
        return -1;
    }

    //return cmObj->validateConfiguration(p_config) ;
}

JNIEXPORT jboolean JNICALL Java_OC_Cm_ControlManager_validateMyDevice
(JNIEnv *env, jobject thisObj, jlong instance, jobject device)
{

    OC::Cm::ControlManager *cmObj = (OC::Cm::ControlManager *) instance;

    OC::Cm::Device *myDevice = (OC::Cm::Device *)device;

    if ( NULL == cmObj)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "ControlManager object is NULL") ;
        return -1;
    }
    if ( NULL == myDevice)
    {
        jclass mapClass = env->FindClass("java/lang/Exception");
        (env)->ThrowNew(mapClass, "Device object is NULL") ;
        return -1;
    }

    //return cmObj->validateMyDevice(myDevice) ;
}

JNIEXPORT void JNICALL Java_OC_Cm_ControlManager_configureClassLoader
(JNIEnv *env, jclass thisObject, jobject classloader)
{
    if (NULL == JNIGlobal::classLoader)
    {
        JNIGlobal::classLoader = JNIGlobal::getGlobleRef(classloader) ;
    }
}

JNIEXPORT jlong JNICALL Java_OC_Cm_ControlManager_getContext
(JNIEnv *env, jobject thisObject, jlong instance)
{
    OC::Cm::ControlManager *cmObj = (OC::Cm::ControlManager *) instance;
    return (jlong)cmObj->getContext() ;
}

