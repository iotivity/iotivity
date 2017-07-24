/* *****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * *****************************************************************/

#include "octypes.h"
#include "oic_malloc.h"

#include "srmresourcestrings.h"
#include "experimental/securevirtualresourcetypes.h"
#include "psinterface.h"

#include "pstatresource.h"
#include "svrdbeditorpstat.h"

static OicSecPstat_t *g_pstat = NULL;

static void PrintDpm(const OicSecDpm_t dpm);
static void PrintDpom(const OicSecDpom_t dpom);

void DeInitPstat()
{
    DeletePstatBinData(g_pstat);
    g_pstat = NULL;
}

void RefreshPstat()
{
    OicSecPstat_t *tmpPstat = NULL;
    uint8_t *secPayload = NULL;
    size_t payloadSize = 0;
    OCStackResult ocResult = OC_STACK_ERROR;

    ocResult = GetSecureVirtualDatabaseFromPS(OIC_JSON_PSTAT_NAME, &secPayload, &payloadSize);
    if (OC_STACK_OK != ocResult)
    {
        PRINT_WARN("GetSecureVirtualDatabaseFromPS : %d", ocResult);
        return;
    }
    ocResult = CBORPayloadToPstat(secPayload, payloadSize, &tmpPstat);
    if (OC_STACK_OK != ocResult)
    {
        PRINT_ERR("CBORPayloadToPstat : %d", ocResult);
        OICFree(secPayload);
        return;
    }
    OICFree(secPayload);

    if (g_pstat)
    {
        DeInitPstat();
    }
    g_pstat = tmpPstat;
}
static void PrintDpm(const OicSecDpm_t dpm)
{
    PRINT_DATA("%d (", dpm);

    if (NORMAL == dpm)
    {
        PRINT_DATA(" NORMAL ");
    }
    if (dpm & RESET)
    {
        PRINT_DATA(" RESET ");
    }
    if (dpm & TAKE_OWNER)
    {
        PRINT_DATA(" TAKE_OWNER ");
    }
    if (dpm & BOOTSTRAP_SERVICE)
    {
        PRINT_DATA(" BOOTSTRAP_SERVICE ");
    }
    if (dpm & SECURITY_MANAGEMENT_SERVICES)
    {
        PRINT_DATA(" SECURITY_MANAGEMENT_SERVICES ");
    }
    if (dpm & PROVISION_CREDENTIALS)
    {
        PRINT_DATA(" PROVISION_CREDENTIALS ");
    }
    if (dpm & PROVISION_ACLS)
    {
        PRINT_DATA(" PROVISION_ACLS ");
    }
    PRINT_DATA(") \n");
}

static void PrintDpom(const OicSecDpom_t dpom)
{
    PRINT_DATA("%d (", dpom);

    if (dpom & MULTIPLE_SERVICE_SERVER_DRIVEN)
    {
        PRINT_DATA(" MULTIPLE_SERVICE_SERVER_DRIVEN ");
    }
    if (dpom & SINGLE_SERVICE_SERVER_DRIVEN)
    {
        PRINT_DATA(" SINGLE_SERVICE_SERVER_DRIVEN ");
    }
    if (dpom & SINGLE_SERVICE_CLIENT_DRIVEN)
    {
        PRINT_DATA(" SINGLE_SERVICE_CLIENT_DRIVEN ");
    }
    PRINT_DATA(") \n");
}


void PrintPstat()
{
    OicSecPstat_t *pstat = g_pstat;
    PRINT_INFO("\n\n********************* [%-20s] *********************",
               "PSTAT Resource");
    PRINT_PROG("%15s : ", OIC_JSON_ISOP_NAME);
    (pstat->isOp ? PrintString("True") : PrintString("False"));

    PRINT_PROG("%15s : ", OIC_JSON_SM_NAME);
    for (size_t i = 0; i < pstat->smLen; i++)
    {
        PrintDpom(pstat->sm[i]);
    }

    PRINT_PROG("%15s : ", OIC_JSON_OM_NAME);
    PrintDpom(pstat->om);

    PRINT_PROG("%15s : ", OIC_JSON_CM_NAME);
    PrintDpm(pstat->cm);

    PRINT_PROG("%15s : ", OIC_JSON_TM_NAME);
    PrintDpm(pstat->tm);

    PRINT_PROG("%15s : ", OIC_JSON_ROWNERID_NAME);
    PrintUuid(&pstat->rownerID);
    PRINT_INFO("********************* [%-20s] *********************",
               "PSTAT Resource");
}

void HandlePstatOperation(const SubOperationType_t cmd)
{
    (void)cmd;
    //T.B.D
}

