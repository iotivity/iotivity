//******************************************************************
//
// Copyright 2016 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "pkix_interface.h"
#include "credresource.h"
#include "crlresource.h"
#include "srmresourcestrings.h"
#include "experimental/logger.h"

#define TAG "OIC_SRM_PKIX_INTERFACE"

void GetPkixInfo(PkiInfo_t * inf)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);
    if (NULL == inf)
    {
        OIC_LOG(ERROR, TAG, "NULL passed");
        OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
        return;
    }

    GetOwnCert(&inf->crt, PRIMARY_CERT);
    if (NULL == inf->crt.cert || 0 == inf->crt.cert->len)
    {
        OIC_LOG_V(WARNING, TAG, "%s: empty certificate", __func__);
    }

    GetPrimaryCertKey(&inf->key);
    if (inf->key.len == 0)
    {
        OIC_LOG_V(WARNING, TAG, "%s: empty key", __func__);
    }

    GetCaCert(&inf->ca, TRUST_CA);
    if (NULL == inf->ca.cert || 0 == inf->ca.cert->len)
    {
        OIC_LOG_V(WARNING, TAG, "%s: empty CA cert", __func__);
    }

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
    GetOwnCert(&inf->crt, MF_PRIMARY_CERT);
    if (NULL == inf->crt.cert || 0 == inf->crt.cert->len)
    {
        OIC_LOG_V(WARNING, TAG, "%s: empty certificate", __func__);
    }
    GetDerKey(&inf->key, MF_PRIMARY_CERT);
    GetCaCert(&inf->ca, MF_TRUST_CA);
    if (NULL == inf->ca.cert || 0 == inf->ca.cert->len)
    {
        OIC_LOG_V(WARNING, TAG, "%s: empty CA cert", __func__);
    }
    // CRL not provided
    inf->crl.data = NULL;
    inf->crl.len = 0;
    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
}

void InitCipherSuiteList(bool * list, const char* deviceId)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);
    if (NULL == list)
    {
        OIC_LOG(ERROR, TAG, "NULL passed");
        OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
        return;
    }
    InitCipherSuiteListInternal(list, TRUST_CA, deviceId);
    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
}

void InitManufacturerCipherSuiteList(bool * list, const char* deviceId)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);
    if (NULL == list)
    {
        OIC_LOG(ERROR, TAG, "NULL passed");
        OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
        return;
    }
    InitCipherSuiteListInternal(list, MF_TRUST_CA, deviceId);
    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
}
