/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include "JniEsUtils.h"

void throwESException(JNIEnv *env, std::string reason)
{
    jobject ex = env->NewObject(g_cls_ESException, g_mid_ESException_ctor,
                                env->NewStringUTF(reason.c_str()));
    if (!ex)
    {
        LOGI("throwException : jobject is NULL");
    }
    env->Throw((jthrowable)ex);
}

OCConnectivityType  getOCConnectivityTypeFromInt(int connectivityType)
{
    switch (connectivityType)
    {
        case CT_DEFAULT:
            return CT_DEFAULT;
        case CT_ADAPTER_IP:
            return CT_ADAPTER_IP;

            //May be need to add more types, if easy setup service support more transport.
    }
    return CT_DEFAULT;
}

int convertNativeProvisionStateToInt(EasySetupState nativeState)
{
    switch (nativeState)
    {
        case EasySetupState::DEVICE_PROVISIONED:
            return 0;
        case EasySetupState::DEVICE_NOT_PROVISIONED:
            return 1;
        case EasySetupState::DEVICE_OWNED:
            return 2;
        case EasySetupState::DEVICE_NOT_OWNED:
            return 3;
    }
    return -1;
}