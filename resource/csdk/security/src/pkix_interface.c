/* *****************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include "pkix_interface.h"
#include "credresource.h"
#include "crlresource.h"
#include "srmresourcestrings.h"
#include "casecurityinterface.h"
#include "logger.h"

#define TAG "OIC_SRM_PKIX_INTERFACE"

static HWPkixContext_t gHwPkixCtx = {
    .getHwKeyContext = NULL,
    .freeHwKeyContext = NULL,
    .getOwnCertCb = NULL,
    .setupPkContextCb = NULL,
    .hwKeyCtx = NULL
};

static bool GetPkixInfoFromHw(PkiInfo_t * inf)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    if (!gHwPkixCtx.hwKeyCtx && gHwPkixCtx.getHwKeyContext)
    {
        gHwPkixCtx.hwKeyCtx = gHwPkixCtx.getHwKeyContext(
                                                        HWKEY_SVC_IOTIVITY,
                                                        HWKEY_USAGE_PRIMARY, NULL);
        if (!gHwPkixCtx.hwKeyCtx)
        {
            OIC_LOG(WARNING, TAG, "gHwPkixCtx.getHwKeyContext return null");
        }
    }

    if (gHwPkixCtx.hwKeyCtx && gHwPkixCtx.getOwnCertCb)
    {
        if (0 != gHwPkixCtx.getOwnCertCb(gHwPkixCtx.hwKeyCtx,
                                         &inf->crt.data, &inf->crt.len))
        {
            OIC_LOG_V(ERROR, TAG, "gHwPkixCtx.getOwnCertCb failed");
            OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
            return false;
        }
        OIC_LOG_V(DEBUG, TAG, "Cert chain length = %d", inf->crt.len);
        OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
        return true;
    }
    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return false;
}

void GetPkixInfo(PkiInfo_t * inf)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);
    if (NULL == inf)
    {
        OIC_LOG(ERROR, TAG, "NULL passed");
        OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
        return;
    }

    if (GetPkixInfoFromHw(inf))
    {
        OIC_LOG(INFO, TAG, "H/W based PKI will be used.");
    }
    else
    {
        GetDerOwnCert(&inf->crt, PRIMARY_CERT);
        GetDerKey(&inf->key, PRIMARY_CERT);
    }
    GetDerCaCert(&inf->ca, TRUST_CA);
    GetDerCrl(&inf->crl);

    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
}

void GetManufacturerPkixInfo(PkiInfo_t * inf)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);
    if (NULL == inf)
    {
        OIC_LOG(ERROR, TAG, "NULL passed");
        OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
        return;
    }

    if (GetPkixInfoFromHw(inf))
    {
        OIC_LOG(INFO, TAG, "H/W based PKI will be used.");
    }
    else
    {
        GetDerOwnCert(&inf->crt, MF_PRIMARY_CERT);
        GetDerKey(&inf->key, MF_PRIMARY_CERT);
    }
    GetDerCaCert(&inf->ca, MF_TRUST_CA);
    // CRL not provided
    inf->crl.data = NULL;
    inf->crl.len = 0;
    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
}

void InitCipherSuiteList(bool * list)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);
    if (NULL == list)
    {
        OIC_LOG(ERROR, TAG, "NULL passed");
        OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
        return;
    }
    InitCipherSuiteListInternal(list, TRUST_CA);
    if (gHwPkixCtx.getOwnCertCb)
    {
        list[1] = true;
    }
    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
}

void InitManufacturerCipherSuiteList(bool * list)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);
    if (NULL == list)
    {
        OIC_LOG(ERROR, TAG, "NULL passed");
        OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
        return;
    }
    InitCipherSuiteListInternal(list, MF_TRUST_CA);
    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
}

int SetupHwPkContext(mbedtls_pk_context* pkCtx)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);
    if (NULL == pkCtx)
    {
        OIC_LOG(ERROR, TAG, "NULL passed");
        OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
        return -1;
    }

    if (gHwPkixCtx.setupPkContextCb)
    {
        OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
        return gHwPkixCtx.setupPkContextCb(pkCtx, gHwPkixCtx.hwKeyCtx);
    }
    else
    {
        OIC_LOG(ERROR, TAG, "gHwPkixCallbacks.setupPkContextCb is NULL");
    }

    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);

    return -1;
}

int SetHwPkixCallbacks(GetHwKeyContext getHwKeyContext,
                        FreeHwKeyContext freeHwKeyContext,
                        GetOwnCertFromHwCallback getOwnCertCb,
                        SetupPkContextFromHwCallback setupPkContextCb)
{
    OIC_LOG_V(INFO, TAG, "In %s", __func__);

    if (NULL == getHwKeyContext || NULL == freeHwKeyContext
        || NULL == getOwnCertCb || NULL == setupPkContextCb)
    {
        OIC_LOG(ERROR, TAG, "NULL Passed");
        OIC_LOG(ERROR, TAG, "Callback function parameters can not be null");
        return -1;
    }

    gHwPkixCtx.getHwKeyContext = getHwKeyContext;
    gHwPkixCtx.freeHwKeyContext = freeHwKeyContext;
    gHwPkixCtx.getOwnCertCb = getOwnCertCb;
    gHwPkixCtx.setupPkContextCb = setupPkContextCb;

    if (gHwPkixCtx.hwKeyCtx)
    {
        gHwPkixCtx.freeHwKeyContext(gHwPkixCtx.hwKeyCtx);
    }
    gHwPkixCtx.hwKeyCtx = NULL;

    // setup pkcontext handler
    CAregisterSetupPkContextHandler(SetupHwPkContext);

    OIC_LOG_V(INFO, TAG, "Out %s", __func__);
    return 0;
}

