/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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

#include "parsechain.h"
#include "utlist.h"
#include "caadapterutils.h"
#include "oic_malloc.h"

#define PARSE_CHAIN_TAG "OIC_PARSE_CHAIN"

int ParseChain(mbedtls_x509_crt *crt, const ByteArrayLL_t *certs, int *errNum)
{
    OIC_LOG_V(DEBUG, PARSE_CHAIN_TAG, "In %s", __func__);
    VERIFY_NON_NULL_RET(crt, PARSE_CHAIN_TAG, "Param crt is NULL", -1);
    VERIFY_NON_NULL_RET(certs, PARSE_CHAIN_TAG, "Param certs is NULL", -1);
    VERIFY_NON_NULL_RET(errNum, PARSE_CHAIN_TAG, "Param errNum is NULL", -1);
    VERIFY_NON_NULL_RET(certs->cert, PARSE_CHAIN_TAG, "certs->cert is NULL", -1);

    int count = 0;
    int ret = 0;
    *errNum = 0;

    const ByteArrayLL_t *temp = certs;
    LL_FOREACH(certs, temp)
    {
        ret = mbedtls_x509_crt_parse(crt, temp->cert->data, temp->cert->len);
        if (0 == ret)
        {
            count++;
        }
        else
        {
            (*errNum)++;
            OIC_LOG_V(ERROR, PARSE_CHAIN_TAG, "mbedtls_x509_crt_parse returned -0x%04x\n", -(ret));
            return -1;
        }
    }
    OIC_LOG_V(DEBUG, PARSE_CHAIN_TAG, "%s successfully parsed %d certificates", __func__, count);
    OIC_LOG_V(DEBUG, PARSE_CHAIN_TAG, "Out %s", __func__);
    return count;
}

void FreeCertChain(ByteArrayLL_t *certs)
{
    ByteArrayLL_t *tmp0 = certs, *tmp1 = NULL, *tmp2 = NULL;
    LL_FOREACH_SAFE(tmp0, tmp1, tmp2)
    {
        LL_DELETE(tmp0, tmp1);
        if (NULL != tmp1->cert) {
            if (NULL != tmp1->cert->data)
            {
                OICFree(tmp1->cert->data);
            }
            OICFree(tmp1->cert);
        }
        tmp1 = NULL;
    }
}
