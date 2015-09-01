/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/
/** @file   jni_re_resource_statechange_listener.h
 *
 *   @brief  This file contains JNI resource state change Listener class
 */

#ifndef __JNI_RE_RESOURCE_STATE_CHANGE_LISTENER_H_
#define __JNI_RE_RESOURCE_STATE_CHANGE_LISTENER_H_

#include <jni.h>

#include "RCSRemoteResourceObject.h"

#include "jni_re_jvm.h"
#include "jni_re_utils.h"

class JniRCSRemoteResourceObject;

/**
 * @class   JniResourceStateChangeListener
 * @brief   This class provides functions for handling the Resource state change callback between the Java and native layer
 *
 */
class JniResourceStateChangeListener
{
    public:
        /**
         * @brief constructor
         */
        JniResourceStateChangeListener(JNIEnv *env, jobject jListener,
                                       JniRCSRemoteResourceObject *resource);

        /**
         * @brief destructor
        */
        ~JniResourceStateChangeListener();

        /**
         * @brief callback function that will be passed to Native layer
        */
        void stateChangeCallback(OIC::Service::ResourceState state);

    private:
        jweak m_jwListener;
        JniRCSRemoteResourceObject *m_ownerResource;
        void checkExAndRemoveListener(JNIEnv *env);
};

#endif //__JNI_RE_RESOURCE_STATE_CHANGE_LISTENER_H_
