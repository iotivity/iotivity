/* *****************************************************************
 *
 * Copyright 2019 Samsung Electronics All Rights Reserved.
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
#ifndef __UNITTEST_TOOLS__
#define __UNITTEST_TOOLS__

#ifdef __cplusplus
extern "C" {
#endif
#include "securevirtualresourcetypes.h"
#include "octypes.h"
#include "pmtypes.h"
#include "ownershiptransfermanager.h"
#include "cacommon.h"
//#include "byte_array.h"
#ifdef __cplusplus
}
#endif

#define UUID_SRV1 "11111111-1234-1234-1234-123456789011"
#define UUID_SRV2 "11111111-1234-1234-1234-123456789012"

#ifndef OIC_DEFAULT_OICSECOPT
#   if (__STDC_VERSION__ >= 199901L)
#       define OIC_DEFAULT_OICSECOPT\
        { \
        .data = keyData(),\
        .len = keyDataLen(),\
        .encoding = OIC_ENCODING_DER,\
        .revstat = false \
        }
#   else
#       define OIC_DEFAULT_OICSECOPT\
        { \
        keyData(),\
        keyDataLen(),\
        OIC_ENCODING_DER,\
        false \
        }
#   endif
#endif

#ifndef OIC_DEFAULT_OICSECKEY
#   if (__STDC_VERSION__ >= 199901L)
#       define OIC_DEFAULT_OICSECKEY\
        { \
        .data = keyData(),\
        .len = keyDataLen(),\
        .encoding = OIC_ENCODING_DER\
        }
#   else
#       define OIC_DEFAULT_OICSECKEY\
        { \
        keyData(),\
        keyDataLen(),\
        OIC_ENCODING_DER\
        }
#   endif
#endif

bool AddSslPeer(const CAEndpoint_t *endpoint, uint8_t *pk, size_t pklen);

void RmSslPeer(const CAEndpoint_t *peer);

FILE *fopen_config(const char *path, const char *mode);

OicSecDoxm_t *createDoxm();

void freeDoxm(OicSecDoxm_t *doxm);

OCDevAddr *setAddr(const char *ip, uint16_t port, OCTransportAdapter adapter);

OCProvisionDev_t *createProvisionDev();

void freeProvisionDev(OCProvisionDev_t *pd);

OTMContext_t *createOTMContext();

void freeOTMContext(OTMContext_t *otmCtx);

CAEndpoint_t *createEndpoint();

void freeEndpoint(CAEndpoint_t *endpoint);

OicUuid_t *createUuid();

OicUuid_t *createUuidWith(const char *uuidStr);

void freeUuid(OicUuid_t *uuid);

uint8_t *certData();

uint8_t *keyData();

const char *getPemKey();

const char *getPemCert();

const char *getCertKey();

size_t keyDataLen();

size_t certDataLen();

OicSecCred_t *createCred();

OicSecSp_t *getSpDefault();

OicSecPstat_t *createPstat();

void IOT_Init(const char *pDbName);

void IOT_DeInit(const char *pDbName);

int readFile(const char *path, uint8_t **buf, size_t *size);

int readTextFile(const char *path, const char **buf);

ByteArrayLL_t *getCerts();

void clearCerts(ByteArrayLL_t *list);

OicSecCred_t * getCredList();

void printCred(const OicSecCred_t * cred);

OicSecCred_t *getAsymKeyCred();

OicSecCred_t *getSignedAsymKeyCred();

OicSecCred_t *getSymPairCred();

OicSecCred_t *getAsymEncKeyCred();

#endif //__UNITTEST_TOOLS__
