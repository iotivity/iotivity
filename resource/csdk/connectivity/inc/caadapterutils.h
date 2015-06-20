/* ****************************************************************
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
 *
 * This file contains common utility function for CA transport adaptors.
 */

#ifndef CA_ADAPTER_UTILS_H_
#define CA_ADAPTER_UTILS_H_

#include <stdbool.h>
#ifdef __ANDROID__
#include <jni.h>
#endif

#ifndef WITH_ARDUINO
#include <sys/socket.h>
#endif

#include "cacommon.h"
#include "logger.h"
#include "pdu.h"
#include "uarraylist.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @def VERIFY_NON_NULL
 * @brief Macro to verify the validity of input argument
 */
#define VERIFY_NON_NULL(arg, log_tag, log_message) \
    if (NULL == arg ){ \
        OIC_LOG_V(ERROR, log_tag, "Invalid input:%s", log_message); \
        return CA_STATUS_INVALID_PARAM; \
    } \

/**
 * @def VERIFY_NON_NULL_RET
 * @brief Macro to verify the validity of input argument
 */
#define VERIFY_NON_NULL_RET(arg, log_tag, log_message,ret) \
    if (NULL == arg ){ \
        OIC_LOG_V(ERROR, log_tag, "Invalid input:%s", log_message); \
        return ret; \
    } \

/**
 * @def VERIFY_NON_NULL_VOID
 * @brief Macro to verify the validity of input argument
 */
#define VERIFY_NON_NULL_VOID(arg, log_tag, log_message) \
    if (NULL == arg ){ \
        OIC_LOG_V(ERROR, log_tag, "Invalid input:%s", log_message); \
        return; \
    } \

/**
 * @brief Length of network interface name.
 */
#define CA_INTERFACE_NAME_SIZE 16

/**
 * @def IPV4_ADDR_ONE_OCTECT_LEN
 * @brief Macro to allocate memory for ipv4 address in the form of uint8_t.
 */
#define IPV4_ADDR_ONE_OCTECT_LEN 4

#ifdef SINGLE_THREAD
/**
 * @brief Network Interface Information. Only needed for Arduino.
 */
typedef struct
{
    char ipAddress[CA_IPADDR_SIZE];             /**< Address of the interface  **/
    char subnetMask[CA_IPADDR_SIZE];            /**< Maintains interface subnetmask **/
    char interfaceName[CA_INTERFACE_NAME_SIZE]; /**< Interface  name**/
} CANetInfo_t;
#endif

/**
 * @brief unicast and multicast server information.
 */
typedef struct
{
    int socketFd;                               /**< Socket decriptor **/
    CAEndpoint_t endpoint;                      /**< endpoint description **/
    bool isServerStarted;                       /**< Indicates server started **/
    bool isMulticastServer;                     /**< Indicates multicast server **/
    char ifAddr[CA_IPADDR_SIZE];                /**< Address of the multicast interface  **/
    char interfaceName[CA_INTERFACE_NAME_SIZE]; /**< Interface Name **/
    char subNetMask[CA_IPADDR_SIZE];            /**< Subnet Mask **/
} CAServerInfo_t;

/**
 * @brief To log the PDU data
 */
void CALogPDUData(coap_pdu_t *pdu);

/**
 * @fn CAParseIPv4AddressInternal
 * @brief   To parse the IP address and port from "ipaddress:port"
 * @param   ipAddrStr   [IN]   IP address to be parsed
 * @param   ipAddr      [OUT]  Parsed IP address
 * @param   ipAddr      [IN]   Buffer length for parsed IP address
 * @param   port        [OUT]  Parsed Port number
 * @return  #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAParseIPv4AddressInternal(const char *ipAddrStr, uint8_t *ipAddr,
                                      size_t ipAddrLen, uint16_t *port);

/**
 * @fn CAAdapterIsSameSubnet
 * @brief Check if two ip address belong to same subnet.
 * @param   ipAddress1   [IN]   IP address to be checked
 * @param   ipAddress2   [IN]   IP address to be checked
 * @param   netMask      [IN]   Subnet mask
 * @return  true if same subnet and false if not same subnet
 */
bool CAAdapterIsSameSubnet(const char *ipAddress1, const char *ipAddress2,
                           const char *netMask);
/**
 * @brief  Used to check the multicast server is running or not.
 *
 * @param   serverInfoList    [IN] Server information list.
 * @param   ipAddress         [IN] Interface address of the server.
 * @param   multicastAddress  [IN] Multicast address of the server.
 * @param   port              [IN] Port number of the server.
 *
 * @return  true or false.
 */
bool CAIsMulticastServerStarted(const u_arraylist_t *serverInfoList, const char *ipAddress,
                                const char *multicastAddress, uint16_t port);

/**
 * @brief  Used to check the unicast server is running or not.
 *
 * @param   serverInfoList  [IN] Server information list.
 * @param   ipAddress       [IN] Ip address of the server.
 * @param   port            [IN] Port number of the server.
 *
 * @return  true or false.
 */
bool CAIsUnicastServerStarted(const u_arraylist_t *serverInfoList, const char *ipAddress,
                              uint16_t port);

/**
 * @brief  Used to get the port number based on given information.
 *
 * @param   serverInfoList  [IN] Server information list.
 * @param   ipAddress       [IN] Ip address of the server.
 * @param   isSecured       [IN] specifies whether to get secured or normal unicast server port.
 *
 * @return  positive value on success and 0 on error.
 */
uint16_t CAGetServerPort(const u_arraylist_t *serverInfoList, const char *ipAddress,
                         bool isSecured);

/**
 * @brief  Used to get the socket fd for given server information.
 *
 * @param   serverInfoList  [IN] Server information list.
 * @param   isMulticast     [IN] To check whether it is multicast server or not.
 * @param   endpoint        [IN] network address

 * @return  positive value on success and -1 on error.
 */
int CAGetSocketFdForUnicastServer(const u_arraylist_t *serverInfoList,
                         bool isMulticast, const CAEndpoint_t *endpoint);

/**
 * @brief  Used to add the server information into serverinfo list
 *
 * @param   serverInfoList     [INOUT] server information list.
 * @param   info               [IN] server informations like ip, port.
 *
 * @return  #CA_STATUS_OK or Appropriate error code
 * @retval  #CA_STATUS_OK  Successful
 * @retval  #CA_STATUS_INVALID_PARAM Invalid input data
 * @retval  #CA_STATUS_FAILED Initialization failed
 */
CAResult_t CAAddServerInfo(u_arraylist_t *serverInfoList, CAServerInfo_t *info);

/**
 * @brief  Used to remove the server information based on socket fd from server info list.
 *
 * @param   serverInfoList  [INOUT] server information list.
 * @param   sockFd          [IN] Socket descriptor.
 *
 * @return  None
 */
void CARemoveServerInfo(u_arraylist_t *serverInfoList, int sockFd);

/**
 * @brief  Used to clear the memory of network inteface list
 *         Memory pointed by infoList will become invalid after this function call.
 *
 * @param   infoList  [IN] Network interface list.
 *
 * @return  None
 */
void CAClearNetInterfaceInfoList(u_arraylist_t *infoList);

/**
 * @brief  Used to clear the memory of server info list.
 *         Memory pointed by serverInfoList will become invalid after this function call.
 *
 * @param   infoList  [IN] Server information list.
 *
 * @return  None
 */
void CAClearServerInfoList(u_arraylist_t *serverInfoList);

/**
 * @brief   Convert address from binary to string
 * @param   ipaddr  [IN] IP address info
 * @param   host    [OUT] address string (must be CA_IPADDR_SIZE)
 * @param   port    [OUT] host order port number
 */
void CAConvertAddrToName(const struct sockaddr_storage *sockaddr, char *host, uint16_t *port);

/**
 * @brief   Convert address from string to binary
 * @param   host    [IN] address string
 * @param   port    [IN] host order port number
 * @param   ipaddr  [OUT] IP address info
 */
void CAConvertNameToAddr(const char *host, uint16_t port, struct sockaddr_storage *sockaddr);

#ifdef __ANDROID__
/**
 * @fn CANativeJNISetContext
 * @brief   To set context of JNI Application
 *          This must be called by the Android API before CA Initialization
 * @param   env         [IN] JNI interface pointer
 * @param   context     [IN] context object
 * @return  None
 */
void CANativeJNISetContext(JNIEnv *env, jobject context);

/**
 * @fn CANativeJNISetJavaVM
 * @brief   To set jvm object
 *          This must be called by the Android API before CA Initialization
 * @param   jvm         [IN] jvm object
 * @return  None
 */
void CANativeJNISetJavaVM(JavaVM *jvm);

/**
 * @fn CANativeJNISetContext
 * @brief   To get context
 *          Called by adapters to get Application context
 * @return  context object
 */
jobject CANativeJNIGetContext();

/**
 * @fn CANativeJNIGetJavaVM
 * @brief   To get JVM object
 *          Called from adapters to get JavaVM object
 * @return  JVM object
 */
JavaVM *CANativeJNIGetJavaVM();
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif  /* CA_ADAPTER_UTILS_H_ */

