/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
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

/**
 * @file
 * @brief This file contains the APIs for BT EDR communications.
 */
#ifndef CA_EDR_SERVER_H_
#define CA_EDR_SERVER_H_

#include <stdbool.h>

#include "cacommon.h"
#include "cathreadpool.h"
#include "jni.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*CAPacketReceiveCallback)(const char *address, const char *data);

/**
 * @brief   Initialize JNI object
 * @return  None
 */
void CAEDRServerJniInit();

/**
 * @brief   Initialize server for EDR
 * @param   handle           [IN] thread pool handle object
 * @return  None
 */
void CAEDRServerInitialize(ca_thread_pool_t handle);

/*
 * @brief   Start Accept Thread
 * @return  None
 */
void CAEDRServerStartAcceptThread();
/**
 * @brief   Start unicast server
 * @param   isSecured       [IN] unicast server type
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAEDRStartUnicastServer(bool isSecured);

/**
 * @brief   Start multicast server
 * @param   isSecured       [IN] multicst server type
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAEDRStartMulticastServer(bool isSecured);

/**
 * @brief   Stop unicast server
 * @param   serverID        [IN] unicast server id
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAEDRStopUnicastServer(int32_t serverID);

/**
 * @brief   Stop multicast server
 * @param   serverID        [IN] multicast server id
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAEDRStopMulticastServer(int32_t serverID);

/**
 * EDR Method
 */

/**
 * @brief  This function will read the data from remote device.
 * @param  env              [IN] JNI interface pointer
 * @param  id               [IN] index of remote address
 * @param  type             [IN] EDR server type
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAEDRNativeReadData(JNIEnv *env, uint32_t id, CAAdapterServerType_t type);

/*
 * @brief   Start Listen Task
 * @param   env             [IN] JNI interface pointer
 * @return  None
 */
void CANativeStartListenTask(JNIEnv *env);

/**
 * @brief  This function will listen the connection from remote device.
 * @param  env              [IN] JNI interface pointer
 * @return server socket object or NULL
 */
jobject CAEDRNativeListen(JNIEnv *env);

/**
 * @brief  This function will listen the connection from remote device.
 * @param  env              [IN] JNI interface pointer
 * @param  socket           [IN] server socket object
 * @return JNI_TRUE or JNI_FALSE
 */
jboolean CAEDRIsConnectedForSocket(JNIEnv *env, jobject socket);

/**
 * @brief  This function will accept the connection from remote device.
 * @param  env                  [IN] JNI interface pointer
 * @param  severSocketObject    [IN] server socket object
 * @return None
 */
void CAEDRNativeAccept(JNIEnv *env, jobject severSocketObject);

/**
 * @brief   Remove all device objects in the list
 * @param   env    [IN] JNI interface pointer
 * @return  None
 */
void CAEDRNatvieCloseServerTask(JNIEnv* env);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CA_EDR_SERVER_H_ */

