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
#ifndef CA_EDR_CLIENT_H_
#define CA_EDR_CLIENT_H_

#include <stdbool.h>

#include "cacommon.h"
#include "cathreadpool.h"
#include "jni.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Initialize context of application
 * @return  None
 */
void CAEDRJniInitContext();

/**
 * @brief   Create JNI Object
 * @param   context          [IN] context of application
 * @return  #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAEDRCreateJNIInterfaceObject(jobject context);

/**
 * @brief   Initialize client for EDR
 * @param   handle           [IN] thread pool handle object
 * @return  None
 */
void CAEDRInitialize(ca_thread_pool_t handle);

/**
 * @brief   Terminate server for EDR
 * @return  None
 */
void CAEDRTerminate();

/**
 * @brief   Initialize JNI object
 * @return  None
 */
void CAEDRCoreJniInit();

/**
 * @brief   Send data for unicast
 * @param   address         [IN] remote address
 * @param   data            [IN] data for transmission
 * @param   dataLen         [IN] data length
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAEDRSendUnicastMessage(const char *address, const char *data, uint32_t dataLen);

/**
 * @brief   Send data for multicast
 * @param   data            [IN] data for transmission
 * @param   dataLen         [IN] data length
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAEDRSendMulticastMessage(const char *data, uint32_t dataLen);

/**
 * @brief   Get Local EDR Address
 * @param   address         [OUT] local address
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAEDRGetInterfaceInfo(char **address);

/**
 * @brief   Get address from a local device
 * @param   address         [OUT] local address
 * @return  None
 */
void CAEDRGetLocalAddress(char **address);

/**
 * @brief   Send data for unicast (implement)
 * @param   address         [IN] remote address
 * @param   data            [IN] data for transmission
 * @param   dataLen         [IN] data length
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAEDRSendUnicastMessageImpl(const char *address, const char *data, uint32_t dataLen);

/**
 * @brief   Send data for multicast (implement)
 * @param   env             [IN] JNI interface pointer
 * @param   data            [IN] data for transmission
 * @param   dataLen         [IN] data length
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_FAILED Operation failed
 */
CAResult_t CAEDRSendMulticastMessageImpl(JNIEnv *env, const char *data, uint32_t dataLen);

/**
 * EDR Method
 */

/**
 * @brief  This function will send the data to remote device.
 * @param  env              [IN] JNI interface pointer
 * @param  address          [IN] Remote Address
 * @param  data             [IN] Data to be transmitted from EDR
 * @param  dataLength       [IN] Length of data
 * @param  id               [IN] index of remote address
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAEDRNativeSendData(JNIEnv *env, const char* address, const char* data,
                               uint32_t dataLength, uint32_t id);

/**
 * @brief  This function will connect to remote device.
 * @param  env              [IN] JNI interface pointer
 * @param  address          [IN] Remote Address
 * @param  id               [IN] index of remote address
 * @return #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAEDRNativeConnect(JNIEnv *env, const char *address, uint32_t id);

/**
 * @brief  This function will close socket.
 * @param  env              [IN] JNI interface pointer
 * @param  address          [IN] Remote Address
 * @param  id               [IN] index of remote address
 * @return None
 */
void CAEDRNativeSocketClose(JNIEnv *env, const char *address, uint32_t id);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CA_EDR_CLIENT_H_ */

