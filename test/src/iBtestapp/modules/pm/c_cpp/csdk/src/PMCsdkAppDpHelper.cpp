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
#include "../include/PMCsdkAppHelper.h"
#include "../include/PMCsdkAppDpHelper.h"
#define TAG "Direct Pairing"

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

void pairingReqCBF(void *ctx, OCDPDev_t* peer, OCStackResult result)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCsdkDpHelper] pairingReqCB IN");

    (void) ctx;

    if (g_expectedCbResult != result)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMCsdkDpHelper] pairingReqCB returns = %s", getOCStackResult(result));
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

bool discoverDirectPairingDevices(unsigned short waittime)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCsdkDpHelper] DiscoverDirectPairingDevices IN");
    g_pDiscoveredDevs = OCDiscoverDirectPairingDevices(DP_DISCOVERY_TIMEOUT);

    IOTIVITYTEST_LOG(DEBUG, "[PMCsdkDpHelper] DiscoverDirectPairingDevices OUT");
    return true;
}

bool doDirectPairing(void *ctx, int peerDevInstance, OCPrm_t pmSel, char *pinNumber,
        OCDirectPairingCB resultCallback, OCStackResult expectedApiResult,
        OCStackResult expectedCbResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCsdkDpHelper] DoDirectPairing IN");

    g_isDpCbInvoked = CALLBACK_NOT_INVOKED;
    g_expectedCbResult = expectedCbResult;
    OCDPDev_t *peer1 = getDeviceInstance(g_pDiscoveredDevs, (uint32_t) peerDevInstance);
    OCStackResult res = OCDoDirectPairing(NULL, peer1, pmSel, pinNumber, resultCallback);
    IOTIVITYTEST_LOG(DEBUG, "[PMCsdkDpHelper] DoDirectPairing returns = %s", getOCStackResult(res));

    if (expectedApiResult != res)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper]  Expected Error : %s Actual Error : %s\n",
                getOCStackResult(expectedApiResult), getOCStackResult(res));
        return false;
    }

    if (OC_STACK_OK == res)
    {
        int nCallbackResult = waitDpCallbackRet();
        if (CALLBACK_NOT_INVOKED == nCallbackResult || CALLBACK_RES_WRONG == nCallbackResult)
        {
            IOTIVITYTEST_LOG(ERROR, "Callback Not Invoked");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCsdkDpHelper] DoDirectPairing OUT");
    return true;
}

bool getDirectPairedDevices()
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCsdkDpHelper] GetDirectPairedDevices IN");
    g_pPairedDevs = OCGetDirectPairedDevices();

    if (NULL == g_pPairedDevs)
    {
        IOTIVITYTEST_LOG(ERROR, "Device List Empty");
        return false;
    }

    printDevList(g_pPairedDevs);
    IOTIVITYTEST_LOG(DEBUG, "[PMCsdkDpHelper] GetDirectPairedDevices OUT");
    return true;
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

        sleep(DELAY_SHORT);

        if (OC_STACK_OK != OCProcess())
        {
            IOTIVITYTEST_LOG(ERROR, "OCStack process error");
            return CALLBACK_NOT_INVOKED;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "waitDpCallbackRet OUT");
    return CALLBACK_NOT_INVOKED;
}
