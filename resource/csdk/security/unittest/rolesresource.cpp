/******************************************************************
*
* Copyright 2019 Samsung Electronics All Rights Reserved.
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
#include <gtest/gtest.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "tools.h"
#include "occertutility.h"
#include "ocserverrequest.h"
#undef TAG
#include "../src/rolesresource.c"
#undef TAG
#undef UUID_LENGTH
#include "../connectivity/src/adapter_util/ca_adapter_net_ssl.c"
#ifdef __cplusplus
}
#endif

#ifdef TAG
#undef TAG
#endif

#define TAG  "RR"

#define SVR_DB_FILE_NAME TAG".dat"
#define PM_DB_FILE_NAME TAG".db"

#define UUID_003 "44444442-4444-4444-5555-666666666666"

#define ISSUER_CERT "resource/csdk/security/unittest/issuer/cert.pem"
#define ISSUER_KEY "resource/csdk/security/unittest/issuer/key.pem"

static CAEndpoint_t *endpoint = NULL;
static mbedtls_pk_context keyPair;
static uint8_t *pubKey = NULL;
static size_t pubKeyLen = 0;
static OicUuid_t *subjectUuid = NULL;
static const char *issuerCert = NULL;
static const char *issuerPrivateKey = NULL;
static const char *publicKey = NULL;
static RoleCertChain_t *roleCertChain = NULL;

class RR : public ::testing::Test
{
    public:
        static void SetUpTestCase()
        {
            IOT_Init(TAG);

            endpoint = createEndpoint();
            ASSERT_TRUE(AddSslPeer(endpoint, keyData(),keyDataLen()));

            ASSERT_EQ(0, readTextFile(ISSUER_CERT, &issuerCert));
            ASSERT_NE(nullptr, issuerCert);
            ASSERT_EQ(0, readTextFile(ISSUER_KEY, &issuerPrivateKey));
            ASSERT_NE(nullptr, issuerPrivateKey);

            subjectUuid = createUuid();
            ASSERT_NE(nullptr, subjectUuid);

            mbedtls_pk_init(&keyPair);
            EXPECT_GE(OCInternalGenerateKeyPair(&keyPair), 0);

            int ret = 0;
            size_t len = 1024;
            uint8_t *buf = (uint8_t*)OICCalloc(len, 1);
            ASSERT_NE(buf, nullptr);

            ret = 0;
            ret = mbedtls_pk_write_pubkey_der(&keyPair, buf, len);
            ASSERT_GE(ret, 0);
            pubKey = (uint8_t*)OICCalloc(ret, 1);
            memcpy(pubKey, buf + len - ret, ret);
            pubKeyLen = ret;

            ret = 0;
            ret = mbedtls_pk_write_pubkey_pem(&keyPair, buf, len);
            ASSERT_EQ(ret, 0);
            ret = (int)strlen((const char*)buf);
            publicKey = (char*)OICCalloc(ret + 1, 1);
            ASSERT_NE(publicKey, nullptr);
            memcpy((char*)publicKey, buf, ret);
/*
            ret = 0;
            ret = mbedtls_pk_write_key_pem(&keyPair, buf, len);
            ASSERT_GE(ret, 0);
            privateKey = (uint8_t*)OICCalloc(ret, 1);
            ASSERT_NE(privateKey, nullptr);
            memcpy(privateKey, buf, ret);
*/
            OICFree(buf);

            const char *serial = NULL;
            size_t serialLen = 0;
            ASSERT_EQ(OC_STACK_OK,  OCGenerateRandomSerialNumber((char**)&serial, &serialLen));

            roleCertChain = (RoleCertChain_t*)OICCalloc(1, sizeof(RoleCertChain_t));
            roleCertChain->credId = rand();
            roleCertChain->next = NULL;

            EXPECT_EQ(OC_STACK_OK, OCGenerateRoleCertificate(
                subjectUuid, publicKey, issuerCert, issuerPrivateKey,
                serial, "20190101235959", "20210101235959", "1.3.6.1.4.1.44924.1.7", "samsung",
                (char**)&roleCertChain->certificate.data, &roleCertChain->certificate.len
            ));
            ASSERT_NE(nullptr, roleCertChain->certificate.data);
            ASSERT_TRUE(roleCertChain->certificate.len > 0);
            roleCertChain->certificate.encoding = OIC_ENCODING_DER;
        }
        static void TearDownTestCase()
        {
            RmSslPeer(endpoint);
            OICFree(subjectUuid);
//            OICFree(privateKey);
            OICFree((void*)pubKey);
            OICFree((void*)publicKey);
            mbedtls_pk_free(&keyPair);
            IOT_DeInit(TAG);
        }
};

extern SslContext_t* g_caSslContext;

void RmSslPeer(const CAEndpoint_t* _endpoint)
{
    size_t listLength = u_arraylist_length(g_caSslContext->peerList);
    for (size_t listIndex = 0; listIndex < listLength; listIndex++)
    {
        SslEndPoint_t * tep = (SslEndPoint_t *)u_arraylist_get(g_caSslContext->peerList, listIndex);
        if (NULL == tep)
        {
            continue;
        }
        if(0 == strncmp(_endpoint->addr, tep->sep.endpoint.addr, MAX_ADDR_STR_SIZE_CA)
                && (_endpoint->port == tep->sep.endpoint.port))
        {
            u_arraylist_remove(g_caSslContext->peerList, listIndex);
            return;
        }
    }
}

bool AddSslPeer(const CAEndpoint_t* _endpoint, uint8_t *pk, size_t pklen)
{
    RmSslPeer(endpoint);
    mbedtls_ssl_config * config = (_endpoint->adapter == CA_ADAPTER_IP ||
                                   _endpoint->adapter == CA_ADAPTER_GATT_BTLE ?
                                   &g_caSslContext->serverDtlsConf : &g_caSslContext->serverTlsConf);
    SslEndPoint_t* peer = NewSslEndPoint(_endpoint, config);
    if (NULL == peer)
    {
            OIC_LOG(ERROR, NET_SSL_TAG, "Malloc failed!");
            return NULL;
    }
    memcpy(peer->sep.publicKey, pk, pklen);
    peer->sep.publicKeyLength = pklen;

    return u_arraylist_add(g_caSslContext->peerList, (void *) peer);
}
TEST_F(RR, AddRoleCertificate)
{
    EXPECT_EQ(OC_STACK_OK, AddRoleCertificate(roleCertChain, pubKey, pubKeyLen));
    EXPECT_EQ(OC_STACK_OK, AddRoleCertificate(roleCertChain, pubKey, pubKeyLen));
}

TEST_F(RR, RegisterSymmetricCredentialRole)
{
    OicSecCred_t *cred = createCred();
    EXPECT_EQ(OC_STACK_OK, RegisterSymmetricCredentialRole(cred));
    ASSERT_EQ(OC_STACK_OK, RegisterSymmetricCredentialRole(cred));
}

TEST_F(RR, GetEndpointRoles)
{
    OicSecRole_t *roles = NULL;
    size_t roleCount = 0;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, GetEndpointRoles(NULL, &roles, &roleCount));

    OicSecCred_t *cred = getSignedAsymKeyCred();
    OICFree(cred->credUsage);
    cred->credUsage = (char*)OICCalloc(strlen(TRUST_CA) + 1, 1);
    strcpy(cred->credUsage, TRUST_CA);
    memset(cred->roleId.id, 0, ROLEID_LENGTH);
    memset(cred->roleId.authority, 0, ROLEAUTHORITY_LENGTH);
    EXPECT_EQ(OC_STACK_OK, AddCredential(cred));

    cred = createCred();
    memset(cred->roleId.id, 0, ROLEID_LENGTH);
    memset(cred->roleId.authority, 0, ROLEAUTHORITY_LENGTH);
    ASSERT_EQ(OC_STACK_OK, RegisterSymmetricCredentialRole(cred));

    uint8_t *pk = NULL;
    size_t pkLen = 0;
    EXPECT_EQ(OC_STACK_OK, GetPeerPublicKeyFromEndpoint(endpoint, &pk, &pkLen));
    EXPECT_EQ(OC_STACK_OK, AddRoleCertificate(roleCertChain, pk, pkLen));

    EXPECT_EQ(OC_STACK_NO_MEMORY, GetEndpointRoles(endpoint, &roles, &roleCount));

    CAEndpoint_t *endpointX = createEndpoint();
    endpointX->port = 1111;
    sprintf(endpointX->addr,"192.168.7.7");
    EXPECT_EQ(OC_STACK_OK, GetEndpointRoles(endpointX, &roles, &roleCount));
}

static OCStackResult responseHandler(OCEntityHandlerResponse * ehResponse)
{
    OC_UNUSED(ehResponse);
    OIC_LOG_V(DEBUG, TAG, "%s run", __func__);
    return OC_STACK_OK;
}

TEST_F(RR, HandleGetRequest)
{
    OCServerRequest *srvReq = (OCServerRequest*)OICCalloc(1, sizeof(OCServerRequest));
    srvReq->ehResponseHandler = responseHandler;
    OCEntityHandlerRequest req = OCEntityHandlerRequest();
    req.method = OC_REST_GET;
    req.requestHandle = (OCRequestHandle) srvReq;
    req.devAddr.port = endpoint->port;
    strncpy(req.devAddr.addr, endpoint->addr, MAX_ADDR_STR_SIZE);
    req.devAddr.flags = (OCTransportFlags)endpoint->flags;
    req.devAddr.adapter = (OCTransportAdapter)endpoint->adapter;

    EXPECT_EQ(OC_EH_OK, HandleGetRequest(&req));
}

TEST_F(RR, HandlePostRequest)
{
    OCServerRequest *srvReq = (OCServerRequest*)OICCalloc(1, sizeof(OCServerRequest));
    srvReq->ehResponseHandler = responseHandler;
    OCEntityHandlerRequest req = OCEntityHandlerRequest();
    req.method = OC_REST_POST;
    req.requestHandle = (OCRequestHandle) srvReq;
    req.devAddr.port = endpoint->port;
    strncpy(req.devAddr.addr, endpoint->addr, MAX_ADDR_STR_SIZE);
    req.devAddr.flags = (OCTransportFlags)endpoint->flags;
    req.devAddr.adapter = (OCTransportAdapter)endpoint->adapter;

    uint8_t *payload = NULL;
    size_t size = 0;

    ASSERT_EQ(OC_STACK_OK, RolesToCBORPayload(roleCertChain, &payload, &size));
    OCSecurityPayload *securityPayload = OCSecurityPayloadCreate(payload, size);
    ASSERT_TRUE(NULL != securityPayload);
    req.payload = (OCPayload *) securityPayload;

    EXPECT_EQ(OC_EH_OK, HandlePostRequest(&req));
}

TEST_F(RR, HandleDeleteRequest)
{
    OCServerRequest *srvReq = (OCServerRequest*)OICCalloc(1, sizeof(OCServerRequest));
    srvReq->ehResponseHandler = responseHandler;
    OCEntityHandlerRequest req = OCEntityHandlerRequest();
    req.method = OC_REST_POST;
    req.requestHandle = (OCRequestHandle) srvReq;
    req.devAddr.port = endpoint->port;
    strncpy(req.devAddr.addr, endpoint->addr, MAX_ADDR_STR_SIZE);
    req.devAddr.flags = (OCTransportFlags)endpoint->flags;
    req.devAddr.adapter = (OCTransportAdapter)endpoint->adapter;
    req.query = (char*)OICCalloc(512, 1);
    snprintf(req.query, 512, "%s=3322", OIC_JSON_CREDID_NAME);

    uint8_t *pk = NULL;
    size_t pkLen = 0;
    EXPECT_EQ(OC_STACK_OK, GetPeerPublicKey(&req.devAddr, &pk, &pkLen));
    EXPECT_EQ(OC_STACK_OK, AddRoleCertificate(roleCertChain, pk, pkLen));
    EXPECT_EQ(OC_EH_OK, HandleDeleteRequest(&req));
}


TEST_F(RR, IsBefore)
{
    struct tm before;
    struct tm after;

    memset(&before, 0, sizeof(struct tm));
    memset(&after, 0, sizeof(struct tm));

    EXPECT_FALSE(IsBefore(&before, &after));
}

TEST_F(RR, RolesEntityHandler)
{
    OCEntityHandlerFlag flag = OC_REQUEST_FLAG;
    OCEntityHandlerRequest req = OCEntityHandlerRequest();
    req.method = OC_REST_POST;

    EXPECT_EQ(OC_EH_ERROR, RolesEntityHandler(flag, NULL, NULL));
    EXPECT_EQ(OC_EH_ERROR, RolesEntityHandler(flag, &req, NULL));
    req.method = OC_REST_GET;
    EXPECT_EQ(OC_EH_ERROR, RolesEntityHandler(flag, &req, NULL));
    req.method = OC_REST_DELETE;
    EXPECT_EQ(OC_EH_ERROR, RolesEntityHandler(flag, &req, NULL));
    req.method = OC_REST_NOMETHOD;
    EXPECT_EQ(OC_EH_ERROR, RolesEntityHandler(flag, &req, NULL));
}

TEST_F(RR, DeleteRolesCB)
{
    SetDoxmIsOwned(true);
    DeleteRolesCB(NULL, 0);
}

