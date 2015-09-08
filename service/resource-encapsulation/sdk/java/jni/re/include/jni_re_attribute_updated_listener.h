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

/** @file   jni_re_attribute_updated_listener.h
 *
 *   @brief  This file contains JNI attribute update Listener class
 */

#ifndef __JNI_RE_ATTRIBUTE_UPDATED_LISTENER_H_
#define __JNI_RE_ATTRIBUTE_UPDATED_LISTENER_H_

#include <jni.h>

#include "jni_re_jvm.h"
#include "jni_re_utils.h"
#include "jni_re_resource_attributes.h"


class JniRCSResourceObject;

/**
 * @class   JniAttributeUpdatedListener
 * @brief   This class provides functions for handling the attribute updated callback between the Java and Native layer
 *
 */
class JniAttributeUpdatedListener
{
    public:
        /**
         * @brief constructor
         */
        JniAttributeUpdatedListener(JNIEnv *env, jobject jListener,
                                    JniRCSResourceObject *resource);

        /**
         * @brief destructor
        */
        ~JniAttributeUpdatedListener();

        /**
         * @brief callback function that will be passed to Native layer
        */
        void onAttributeUpdatedCallback(OIC::Service::RCSResourceAttributes::Value oldValue,
                                        OIC::Service::RCSResourceAttributes::Value newValue);

    private:
        jweak m_jwListener;
        JniRCSResourceObject *m_ownerResource;
        void checkExAndRemoveListener(JNIEnv *env);
};

#endif //__JNI_RE_ATTRIBUTE_UPDATED_LISTENER_H_