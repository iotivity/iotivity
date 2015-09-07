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

/** @file   jni_re_discover_resource_listener.h
 *
 *   @brief  This file contains JNI Discover Resource Listener class
 */

#ifndef __JNI_RE_DISCOVER_RESOURCE_LISTENER_H_
#define __JNI_RE_DISCOVER_RESOURCE_LISTENER_H_

#include <jni.h>

#include "RCSRemoteResourceObject.h"

#include "jni_re_jvm.h"
#include "jni_re_rcs_remoteresourceobject.h"

typedef void(*RemoveListenerCallback)(JNIEnv *env, jobject jListener);

class RCSRemoteResourceObject;

/**
 * @class   JniDiscoverResourceListener
 * @brief   This class provides functions for handling the Discover Resource Callback between the Java and Native layer
 *
 */
class JniDiscoverResourceListener
{
    public:
        /**
         * @brief constructor
         */
        JniDiscoverResourceListener(JNIEnv *env, jobject jListener,
                                    RemoveListenerCallback removeListenerCallback);

        /**
         * @brief destructor
         */
        ~JniDiscoverResourceListener();

        /**
         * @brief callback function that will be passed to Native layer
         */
        void discoverResourceCallback(std::shared_ptr<OIC::Service::RCSRemoteResourceObject> resource);

    private:
        RemoveListenerCallback m_removeListenerCallback;
        jweak m_jwListener;
        void checkExAndRemoveListener(JNIEnv *env);
};
#endif //__JNI_RE_DISCOVER_RESOURCE_LISTENER_H_
