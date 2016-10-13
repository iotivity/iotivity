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
 *      LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/
#if defined(__DIRECTPAIRING__)
#include "PMCsdkDpHelper.h"
#include "PMCsdkUtilityHelper.h"

int g_isDpCbInvoked = CALLBACK_NOT_INVOKED;
OCStackResult g_expectedCbResult = OC_STACK_ERROR;
const OCDPDev_t *g_pDiscoveredDevs = NULL;
const OCDPDev_t *g_pPairedDevs = NULL;

FILE* clientFopenDP(const char *path, const char *mode)
{
    (void) path;
    return fopen(JUSTWORKS_SERVER_DP_C_DAT, mode);
}

int printDevList(const OCDPDev_t* pList)
{
    if (!pList)
    {
        printf("Device List is Empty..\n\n");
        return 0;
    }

    printf("Printing Device List...\n");
    const OCDPDev_t* lst = pList;
    int lst_cnt = 0;
    for (; lst;)
    {
        printf("     [%d] ", ++lst_cnt);
        for (int i = 0; i < UUID_IDENTITY_SIZE; i++)
        {
            fprintf(stdout, "%c", (char) lst->deviceID.id[i]);
        }
        printf("\n");
        lst = lst->next;
    }
    printf("\n");

    return lst_cnt;
}

OCDPDev_t* getDeviceInstance(const OCDPDev_t* pList, const uint32_t dev_num)
{
    if (NULL == pList)
    {
        printf("     Device List is Empty..\n");
        return NULL;
    }

    OCDPDev_t* lst = (OCDPDev_t*) pList;
    for (size_t i = 0; lst;)
    {
        if (dev_num == ++i)
        {
            return lst;
        }
        lst = lst->next;
    }

    return NULL;
}

void PMCsdkDpHelper::pairingReqCBF(void *ctx, OCDPDev_t* peer, OCStackResult result)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCsdkDpHelper] pairingReqCB IN");

    (void) ctx;

    if (g_expectedCbResult != result)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMCsdkDpHelper] pairingReqCB returns = %s", getDPResult(result));
        g_isDpCbInvoked = CALLBACK_RES_WRONG;
    }
    else
    {
        g_isDpCbInvoked = CALLBACK_INVOKED;
    }

    if (OC_STACK_OK == result)
    {
        IOTIVITYTEST_LOG(DEBUG,
                "[PMCsdkDpHelper] Direct-Pairing PASSED with Host Address : %s and Port : %d",
                peer->endpoint.addr, peer->endpoint.port);
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCsdkDpHelper] Direct-Pairing FAILED");
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCsdkDpHelper] pairingReqCB OUT");
}

PMCsdkDpHelper::PMCsdkDpHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCsdkDpHelper] Constructor Called");
}

bool PMCsdkDpHelper::discoverDirectPairingDevices(unsigned short waittime)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCsdkDpHelper] discoverDirectPairingDevices IN");
    g_pDiscoveredDevs = OCDiscoverDirectPairingDevices(DP_DISCOVERY_TIMEOUT);

    IOTIVITYTEST_LOG(DEBUG, "[PMCsdkDpHelper] discoverDirectPairingDevices OUT");
    return true;
}

bool PMCsdkDpHelper::doDirectPairing(void *ctx, int peerDevInstance, OCPrm_t pmSel, char *pinNumber,
        OCDirectPairingCB resultCallback, OCStackResult expectedApiResult,
        OCStackResult expectedCbResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCsdkDpHelper] doDirectPairing IN");

    g_isDpCbInvoked = CALLBACK_NOT_INVOKED;
    g_expectedCbResult = expectedCbResult;
    OCDPDev_t *peer1 = getDeviceInstance(g_pDiscoveredDevs, (uint32_t) peerDevInstance);
    OCStackResult res = OCDoDirectPairing(NULL, peer1, pmSel, pinNumber, resultCallback);
    IOTIVITYTEST_LOG(DEBUG, "[PMCsdkDpHelper] doDirectPairing returns = %s", getDPResult(res));

    if (expectedApiResult != res)
    {
        m_failureMessage = setFailureMessage(expectedApiResult, res);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        int nCallbackResult = waitDpCallbackRet();
        if (CALLBACK_NOT_INVOKED == nCallbackResult || CALLBACK_RES_WRONG == nCallbackResult)
        {
            m_failureMessage = setFailureMessage("Callback Not Invoke or rovided wrong Result");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCsdkDpHelper] doDirectPairing OUT");
    return true;
}

bool PMCsdkDpHelper::getDirectPairedDevices()
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCsdkDpHelper] getDirectPairedDevices IN");
    g_pPairedDevs = OCGetDirectPairedDevices();

    if (NULL == g_pPairedDevs)
    {
        m_failureMessage = setFailureMessage("Device List is Null");
        return false;
    }

    printDevList(g_pPairedDevs);
    IOTIVITYTEST_LOG(DEBUG, "[PMCsdkDpHelper] getDirectPairedDevices OUT");
    return true;
}

std::string PMCsdkDpHelper::getFailureMessage()
{
    return m_failureMessage;
}

/**
 * Function to set failure message
 */
std::string PMCsdkDpHelper::setFailureMessage(OCStackResult actualResult,
        OCStackResult expectedResult)
{
    std::string errorMessage("\033[1;31m[Error] Expected : ");
    errorMessage.append(getDPResult(expectedResult));
    errorMessage.append("\033[0m  \033[1;31mActual : ");
    errorMessage.append(getDPResult(actualResult));
    errorMessage.append("\033[0m");
    return errorMessage;
}

/**
 * Function to set failure message
 */
std::string PMCsdkDpHelper::setFailureMessage(std::string errorMessage)
{
    std::string retErrorMessage("\033[1;31m[Error] Expected : ");
    retErrorMessage.append(errorMessage);
    retErrorMessage.append("\033[0m");

    return retErrorMessage;
}

int waitDpCallbackRet()
{
    IOTIVITYTEST_LOG(DEBUG, "waitDpCallbackRet IN");

    for (int i = 0; CALLBACK_TIMEOUT > i; ++i)
    {
        IOTIVITYTEST_LOG(DEBUG, "waitDpCallbackRet Loop = %d", i);

        if (CALLBACK_INVOKED == g_isDpCbInvoked)
        {
            return CALLBACK_INVOKED;
        }

        if (CALLBACK_RES_WRONG == g_isDpCbInvoked)
        {
            return CALLBACK_RES_WRONG;
        }

        CommonUtil::waitInSecond(DELAY_SHORT);

        if (OC_STACK_OK != OCProcess())
        {
            IOTIVITYTEST_LOG(ERROR, "OCStack process error");
            return CALLBACK_NOT_INVOKED;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "waitDpCallbackRet OUT");
    return CALLBACK_NOT_INVOKED;
}

const char *getDPResult(OCStackResult result)
{
    switch (result)
    {
        case OC_STACK_OK:
        return "OC_STACK_OK";
        case OC_STACK_RESOURCE_CREATED:
        return "OC_STACK_RESOURCE_CREATED";
        case OC_STACK_RESOURCE_DELETED:
        return "OC_STACK_RESOURCE_DELETED";
        case OC_STACK_INVALID_URI:
        return "OC_STACK_INVALID_URI";
        case OC_STACK_INVALID_QUERY:
        return "OC_STACK_INVALID_QUERY";
        case OC_STACK_INVALID_IP:
        return "OC_STACK_INVALID_IP";
        case OC_STACK_INVALID_PORT:
        return "OC_STACK_INVALID_PORT";
        case OC_STACK_INVALID_CALLBACK:
        return "OC_STACK_INVALID_CALLBACK";
        case OC_STACK_INVALID_METHOD:
        return "OC_STACK_INVALID_METHOD";
        case OC_STACK_NO_MEMORY:
        return "OC_STACK_NO_MEMORY";
        case OC_STACK_COMM_ERROR:
        return "OC_STACK_COMM_ERROR";
        case OC_STACK_INVALID_PARAM:
        return "OC_STACK_INVALID_PARAM";
        case OC_STACK_NOTIMPL:
        return "OC_STACK_NOTIMPL";
        case OC_STACK_NO_RESOURCE:
        return "OC_STACK_NO_RESOURCE";
        case OC_STACK_RESOURCE_ERROR:
        return "OC_STACK_RESOURCE_ERROR";
        case OC_STACK_SLOW_RESOURCE:
        return "OC_STACK_SLOW_RESOURCE";
        case OC_STACK_NO_OBSERVERS:
        return "OC_STACK_NO_OBSERVERS";
        case OC_STACK_AUTHENTICATION_FAILURE:
        return "OC_STACK_AUTHENTICATION_FAILURE";
#ifdef WITH_PRESENCE
        case OC_STACK_PRESENCE_STOPPED:
        return "OC_STACK_PRESENCE_STOPPED";
#endif
        case OC_STACK_ERROR:
        return "OC_STACK_ERROR";
        default:
        return "UNKNOWN";
    }
}

#endif /*#if defined(__DIRECTPAIRING__)*/
