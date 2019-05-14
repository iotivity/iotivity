//******************************************************************
//
// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include <gtest/gtest.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "srmtestcommon.h"
#include "tools.h"
#undef TAG
#include "../src/credresource.c"

#ifdef __cplusplus
}
#endif

#ifdef TAG
#undef TAG
#endif

#define TAG "SRM-CRED-UT"

#define SVR_DB_FILE_NAME TAG".dat"
#define PM_DB_FILE_NAME TAG".db"

class CRED : public ::testing::Test
{
    public:

        static void SetUpTestCase()
        {
            IOT_Init(PM_DB_FILE_NAME);
            //InitCredResource Tests
            EXPECT_EQ(OC_STACK_OK, InitCredResource());
            //CreateCredResource Tests
            EXPECT_EQ(OC_STACK_OK, CreateCredResource());
            //CredEntityHandler Tests
            OCEntityHandlerRequest req = OCEntityHandlerRequest();
            EXPECT_EQ(OC_EH_ERROR,
                CredEntityHandler(OCEntityHandlerFlag::OC_REQUEST_FLAG, &req, NULL));
        }

        static void TearDownTestCase()
        {
            gCred = NULL;
            //DeInitCredResource Tests
            EXPECT_EQ(OC_STACK_OK, DeInitCredResource());
            IOT_DeInit(PM_DB_FILE_NAME);
        }
};

TEST_F(CRED, CredEntityHandlerWithNULLRequest)
{
    EXPECT_EQ(OC_EH_ERROR,
            CredEntityHandler(OCEntityHandlerFlag::OC_REQUEST_FLAG, NULL, NULL));
}

TEST_F(CRED, CredEntityHandlerInvalidFlag)
{
    OCEntityHandlerRequest req = OCEntityHandlerRequest();
    EXPECT_EQ(OC_EH_ERROR,
            CredEntityHandler(OCEntityHandlerFlag::OC_OBSERVE_FLAG, &req, NULL));
}

//Cred DELETE request
TEST_F(CRED, CredEntityHandlerDeleteTest)
{
    const char uuidStr[] = "31313131-3131-3131-3131-313131313131"; //canonical uuid of subject1
    const char uuidTag[] = "subjectuuid=";
    char query[sizeof(uuidTag) + sizeof(uuidStr)] = {0};
    snprintf(query, sizeof(query), "%s%s", uuidTag, uuidStr);

    OicUuid_t rownerId = {{0}};
    OicUuid_t subjectId = {{0}};

    ConvertStrToUuid(uuidStr, &rownerId);
    ConvertStrToUuid(uuidStr, &subjectId);

    uint8_t privateKey[] = "My private Key11";
    OicSecKey_t key = {privateKey, sizeof(privateKey), OIC_ENCODING_RAW};

    OicSecCred_t* subjectCred1 = GenerateCredential(&subjectId, SYMMETRIC_PAIR_WISE_KEY, NULL,
                                                    &key, NULL);

    EXPECT_EQ(OC_STACK_OK, AddCredential(subjectCred1));
    subjectCred1 = GetCredResourceData(&subjectCred1->subject);
    EXPECT_TRUE(NULL != subjectCred1);

    // Create Entity Handler DELETE request
    OCEntityHandlerRequest ehReq =  OCEntityHandlerRequest();
    ehReq.method = OC_REST_DELETE;
    ehReq.query = (char *)OICCalloc(1, strlen(query)+1);

    ASSERT_TRUE(NULL != ehReq.query);
    OICStrcpy(ehReq.query, strlen(query)+1, query);

    OCEntityHandlerResult ehRet = CredEntityHandler(OC_REQUEST_FLAG, &ehReq, NULL);
    EXPECT_EQ(OC_EH_ERROR, ehRet);

    // Verify if SRM has deleted ACE for the subject
    const OicSecCred_t* subjectCred2 = GetCredResourceData(&subjectId);
    EXPECT_TRUE(NULL == subjectCred2);

    // Perform cleanup
    OICFree(ehReq.query);
    OCPayloadDestroy((OCPayload *)ehReq.payload);
}

TEST_F(CRED, CredToCBORPayloadNULL)
{
    int secureFlag = 0;
    OicSecCred_t *cred = getCredList();
    EXPECT_EQ(OC_STACK_INVALID_PARAM, CredToCBORPayload(NULL, NULL, 0, secureFlag));
    size_t size = 0;
    uint8_t *cborPayload = (uint8_t *) OICCalloc(1, 10);
    if (!cborPayload)
    {
        DeleteCredList(cred);
    }
    ASSERT_TRUE(NULL != cborPayload);
    EXPECT_EQ(OC_STACK_INVALID_PARAM, CredToCBORPayload(cred, &cborPayload, &size, secureFlag));
    /* CredToCBORPayload allocates mem if payload is non-NULL, clean up before proceeding */
    OICFree(cborPayload);
    cborPayload = NULL;
    EXPECT_EQ(OC_STACK_OK, CredToCBORPayload(NULL, &cborPayload, &size,secureFlag));
    OICFree(cborPayload);
    cborPayload = NULL;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, CredToCBORPayload(cred, &cborPayload, 0, secureFlag));
    OICFree(cborPayload);
    cborPayload = NULL;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, CredToCBORPayload(cred, NULL, &size, secureFlag));
    DeleteCredList(cred);
}

TEST_F(CRED, CredToCBORPayloadVALID)
{
    uint8_t* payload = NULL;
    size_t size = 0;
    int secureFlag = 0;
    OicSecCred_t *cred = getCredList();

    EXPECT_EQ(OC_STACK_OK, CredToCBORPayload(cred, &payload, &size, secureFlag));
    if (!payload)
    {
        DeleteCredList(cred);
    }
    ASSERT_TRUE(NULL != payload);

    DeleteCredList(cred);
    OICFree(payload);
}

TEST_F(CRED, CBORPayloadToCredVALID)
{
    uint8_t *payload = NULL;
    size_t size = 0;
    int secureFlag = 0;

    OicSecCred_t *cred1 = getCredList();
    memcpy(cred1->roleId.id, "1.3.6.1.4.1.51414.0.0.1.0", ROLEID_LENGTH);         // 2:R:M:N:oic.sec.roletype
    memcpy(cred1->roleId.authority, "samsung", ROLEAUTHORITY_LENGTH);         // 2:R:M:N:oic.sec.roletype

    EXPECT_EQ(OC_STACK_OK, CredToCBORPayload(cred1, &payload, &size, secureFlag));
    DeleteCredList(cred1);
    ASSERT_TRUE(NULL != payload);

    OicSecCred_t *cred2 = NULL;
    OicUuid_t *rownerId = NULL;
    EXPECT_EQ(OC_STACK_OK, CBORPayloadToCred(payload, size, &cred2, &rownerId));
    OICFree(payload);
    ASSERT_TRUE(cred2 != NULL);
    ASSERT_TRUE(rownerId != NULL);
    OICFree(rownerId);
    DeleteCredList(cred2);
}

TEST_F(CRED, CBORPayloadToCredSecureVALID)
{
    OicSecCred_t *cred1 = getCredList();
    ASSERT_TRUE(NULL != cred1);
    cred1->privateData.data = (uint8_t *)OICCalloc(1, OWNER_PSK_LENGTH_128);
    ASSERT_TRUE(NULL != cred1->privateData.data);
    cred1->privateData.len = OWNER_PSK_LENGTH_128;

    uint8_t *payload = NULL;
    size_t size = 0;
    int secureFlag = 1;
    EXPECT_EQ(OC_STACK_OK, CredToCBORPayload(cred1, &payload, &size, secureFlag));

    ASSERT_TRUE(NULL != payload);

    OicSecCred_t *cred2 = NULL;
    OicUuid_t *rownerId = NULL;
    EXPECT_EQ(OC_STACK_OK, CBORPayloadToCred(payload, size, &cred2, &rownerId));
    ASSERT_TRUE(cred2 != NULL);
    ASSERT_TRUE(NULL == cred2->privateData.data);
    ASSERT_TRUE(0 == cred2->privateData.len);

    OICFree(payload);
    OICFree(rownerId);

    DeleteCredList(cred1);
    DeleteCredList(cred2);
}

TEST_F(CRED, CBORPayloadToCredNULL)
{
    OicSecCred_t *cred = NULL;
    OicUuid_t *rownerId = NULL;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, CBORPayloadToCred(NULL, 0, NULL, NULL));
    uint8_t *cborPayload = (uint8_t *) OICCalloc(1, 10);
    ASSERT_TRUE(NULL != cborPayload);
    EXPECT_EQ(OC_STACK_INVALID_PARAM, CBORPayloadToCred(NULL, 0, &cred, &rownerId));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, CBORPayloadToCred(cborPayload, 0, NULL, NULL));
    cred = getCredList();
    EXPECT_EQ(OC_STACK_INVALID_PARAM, CBORPayloadToCred(cborPayload, 0, &cred, &rownerId));
    DeleteCredList(cred);
    OICFree(rownerId);
    OICFree(cborPayload);
}

//GetCredResourceData Test
TEST_F(CRED, GetCredResourceDataNULLSubject)
{
    EXPECT_EQ(NULL, GetCredResourceData(NULL));
}

TEST_F(CRED, GenerateCredentialValidInput)
{
    OicUuid_t rownerID = {{0}};
    OICStrcpy((char *)rownerID.id, sizeof(rownerID.id), "ownersId21");

    OicUuid_t subject = {{0}};
    OICStrcpy((char *)subject.id, sizeof(subject.id), "subject11");

    uint8_t privateKey[] = "My private Key11";
    OicSecKey_t key = {privateKey, sizeof(privateKey), OIC_ENCODING_UNKNOW};

    OicSecCred_t * cred  = NULL;
    cred = GenerateCredential(&subject, SYMMETRIC_PAIR_WISE_KEY, NULL,
                              &key, NULL);
    printCred(cred);

    ASSERT_TRUE(NULL != cred);
    DeleteCredList(cred);
}

TEST_F(CRED, GenerateAndAddCredentialValidInput)
{
    OicUuid_t rownerID = {{0}};
    OICStrcpy((char *)rownerID.id, sizeof(rownerID.id), "ownersId11");

    OicUuid_t subject = {{0}};
    OICStrcpy((char *)subject.id, sizeof(subject.id), "subject11");

    uint8_t privateKey[] = "My private Key11";
    OicSecKey_t key = {privateKey, sizeof(privateKey), OIC_ENCODING_RAW};

    OicSecCred_t *cred1  = NULL;
    OicSecCred_t *headCred = NULL;

    cred1 = GenerateCredential(&subject, SYMMETRIC_PAIR_WISE_KEY, NULL,
                               &key, NULL);

    EXPECT_EQ(OC_STACK_OK, AddCredential(cred1));
    headCred = cred1;

    OICStrcpy((char *)rownerID.id, sizeof(rownerID.id), "ownersId22");
    OICStrcpy((char *)subject.id, sizeof(subject.id), "subject22");
    cred1 = GenerateCredential(&subject, SYMMETRIC_PAIR_WISE_KEY, NULL,
                               &key, NULL);
    EXPECT_EQ(OC_STACK_OK, AddCredential(cred1));

    OICStrcpy((char *)rownerID.id, sizeof(rownerID.id), "ownersId33");
    OICStrcpy((char *)subject.id, sizeof(subject.id), "subject33");
    cred1 = GenerateCredential(&subject, SYMMETRIC_PAIR_WISE_KEY, NULL,
                               &key, NULL);
    EXPECT_EQ(OC_STACK_OK, AddCredential(cred1));

    const OicSecCred_t* credList = GetCredResourceData(&headCred->subject);

    printCred(credList);

    DeleteCredList(headCred);
}

#if 0
TEST_F(CredGetResourceDataTest, GetCredResourceDataValidSubject)
{
    OicSecCred_t* cred = getCredList();
    EXPECT_TRUE(NULL != GetCredResourceData(cred->subject));
}
#endif

#if defined(__WITH_DTLS__) || defined(__WITH_TLS__)
TEST_F(CRED, NullResult)
{
    EXPECT_EQ(-1, GetDtlsPskCredentials(CA_DTLS_PSK_KEY, NULL, 0, NULL, 0));
}

TEST_F(CRED, AddTmpPskWithPIN)
{
    gCred = NULL;
    OicUuid_t* subject = createUuidWith("33333333-3333-3333-3333-222222222222");
    char pin[] = "the_pin";
    size_t pinSize = strlen(pin);
    OicUuid_t *rownerID = createUuidWith("33333333-3333-3333-3333-222222222222");
    OicUuid_t* credSubject = createUuidWith("33333333-3333-3333-3333-222222222222");

    EXPECT_EQ(OC_STACK_INVALID_PARAM, AddTmpPskWithPIN(NULL, SYMMETRIC_PAIR_WISE_KEY,
              NULL, 0, NULL, NULL));
    EXPECT_EQ(OC_STACK_OK, AddTmpPskWithPIN(subject, SYMMETRIC_PAIR_WISE_KEY,
              pin, pinSize,rownerID, credSubject));
}
#endif // __WITH_DTLS__ or __WITH_TLS__
TEST_F(CRED, NullPayload)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, CBORPayloadToCred(NULL, 0, NULL, NULL));
}

TEST_F(CRED, DeleteCredIdList)
{
    CredIdList_t *list = NULL;
    CredIdList_t *cred = (CredIdList_t*)OICCalloc(1, sizeof(CredIdList_t));
    cred->credId = (int16_t)rand();
    LL_APPEND(list, cred);
    DeleteCredIdList(&list);
}

TEST_F(CRED, CheckSubjectOfCertificate)
{
    OicSecCred_t* cred = getSignedAsymKeyCred();
    OicUuid_t deviceID;
    ConvertStrToUuid("33333333-3333-3333-3333-222222222222", &deviceID);

    printCred(cred);
    EXPECT_TRUE(CheckSubjectOfCertificate(cred, deviceID));

    FreeCred(cred);
}

TEST_F(CRED, IsValidCredential)
{
    OicSecCred_t* cred = NULL;

    EXPECT_FALSE(IsValidCredential(cred));

    cred = getSignedAsymKeyCred();
    EXPECT_TRUE(IsValidCredential(cred));
    FreeCred(cred);

    cred = getAsymKeyCred();
    EXPECT_TRUE(IsValidCredential(cred));
    FreeCred(cred);

    cred = getAsymEncKeyCred();
    EXPECT_TRUE(IsValidCredential(cred));
    FreeCred(cred);
}

TEST_F(CRED, IsEmptyCred)
{
    OicSecCred_t* cred = NULL;

    EXPECT_FALSE(IsEmptyCred(cred));

    cred = getSignedAsymKeyCred();
    EXPECT_FALSE(IsEmptyCred(cred));
    FreeCred(cred);

    cred = getAsymKeyCred();
    EXPECT_FALSE(IsEmptyCred(cred));
    FreeCred(cred);

    cred = getAsymEncKeyCred();
    EXPECT_FALSE(IsEmptyCred(cred));
    FreeCred(cred);
}


TEST_F(CRED, SerializeDeserializeSecOpt)
{
    CborEncoder encoder;
    CborEncoder eRootMap;
    uint8_t *payload = (uint8_t *)OICCalloc(1, 2048);
    cbor_encoder_init(&encoder, payload, 2048, 0);
    EXPECT_EQ(CborNoError, cbor_encoder_create_map(&encoder, &eRootMap, 1));
    OicSecOpt_t opt = OIC_DEFAULT_OICSECOPT;
    EXPECT_EQ(CborNoError, SerializeSecOptToCbor(&eRootMap, OIC_JSON_OPTDATA_NAME, &opt));
    EXPECT_EQ(CborNoError, cbor_encoder_close_container(&encoder, &eRootMap));
    size_t size = cbor_encoder_get_buffer_size(&encoder, payload);
    CborParser parser;
    CborValue pRoot = OC_CBOR_VALUE_INIT;
    cbor_parser_init(payload, size, 0, &parser, &pRoot);
    ASSERT_TRUE(cbor_value_is_container(&pRoot));
    OicSecOpt_t *optR = (OicSecOpt_t*)OICCalloc(1,sizeof(OicSecOpt_t));
    EXPECT_EQ(CborNoError, DeserializeSecOptFromCbor(&pRoot, optR));
}

TEST_F(CRED, IsSameSecOpt)
{
    OicSecOpt_t opt1 = OIC_DEFAULT_OICSECOPT;
    OicSecOpt_t opt2 = OIC_DEFAULT_OICSECOPT;
    EXPECT_TRUE(IsSameSecOpt(&opt1, &opt2));
    opt1.revstat = true;
    EXPECT_FALSE(IsSameSecOpt(&opt1, &opt2));
}

TEST_F(CRED, IsSameSecKey)
{
    OicSecKey_t key1 = OIC_DEFAULT_OICSECKEY;
    OicSecKey_t key2 = OIC_DEFAULT_OICSECKEY;
    EXPECT_TRUE(IsSameSecKey(&key1, &key2));
    key1.encoding = OIC_ENCODING_RAW;
    EXPECT_FALSE(IsSameSecKey(&key1, &key2));
}

TEST_F(CRED, RemoveCredential)
{
    RemoveAllCredentials();
    gCred = NULL;
    OicSecCred_t* cred = getSignedAsymKeyCred();
    EXPECT_EQ(OC_STACK_OK,AddCredential(cred));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, RemoveCredentialByCredId(0));
    EXPECT_EQ(OC_STACK_ERROR, RemoveCredentialByCredId((uint16_t)-12));
    EXPECT_EQ(OC_STACK_RESOURCE_DELETED, RemoveCredentialByCredId(cred->credId));
    cred = getSignedAsymKeyCred();
    EXPECT_EQ(OC_STACK_OK,AddCredential(cred));
    CredIdList_t id = {
    #if (__STDC_VERSION__ >= 199901L)
        .credId = cred->credId,
        .next = NULL
    #else
        cred->credId,
        NULL
    #endif
        };
    EXPECT_EQ(OC_STACK_RESOURCE_DELETED, RemoveCredentialByCredIds(&id));
}

TEST_F(CRED, GetCredIdsFromQueryString)
{

    char *query = (char*)OICCalloc(1024,1);
    CredIdList_t *credid = NULL;
#if defined(__unix__)
    snprintf(query, 1024, "%s=%li", OIC_JSON_CREDID_NAME, 13463257457456L);
#endif
    EXPECT_TRUE(GetCredIdsFromQueryString(query, &credid));
    snprintf(query, 1024, "%s=%d", OIC_JSON_CREDID_NAME, 1222);
    EXPECT_TRUE(GetCredIdsFromQueryString(query, &credid));
    OICFree(query);
}

TEST_F(CRED, ConvertCert)
{
    const char *pem = getPemCert();
    size_t pemLen = strlen(pem);
    uint8_t* der = NULL;
    size_t derLen = 0;
    uint8_t *newPem = NULL;
    EXPECT_EQ(0, ConvertPemCertToDer(pem, pemLen, &der, &derLen));
#ifndef NDEBUG
    EXPECT_EQ(0, ConvertDerCertToPem(der, derLen, &newPem));
#endif //NDEBUG
    OICFree((void*)pem);
    OICFree(der);
    OICFree(newPem);
}

TEST_F(CRED, FillPrivateDataOfOwnerPSK)
{
    OicSecCred_t* cred = getSignedAsymKeyCred();
    CAEndpoint_t* ep = createEndpoint();
    OicSecDoxm_t* doxm = createDoxm();

   // SslEndPoint_t * peer = GetSslPeer(ep);
    //u_arraylist_add(g_caSslContext->peerList, (void *) peer);

    EXPECT_FALSE(FillPrivateDataOfOwnerPSK(cred, ep, doxm));

    freeDoxm(doxm);
    freeEndpoint(ep);
    FreeCred(cred);
}

TEST_F(CRED, HandleNewCredential)
{
    OCEntityHandlerRequest ehRequest =  OCEntityHandlerRequest();
    OicSecCred_t* cred = getSignedAsymKeyCred();
    EXPECT_EQ(OC_EH_CHANGED, HandleNewCredential(&ehRequest, cred));
    FreeCred(cred);
}

TEST_F(CRED, GetIdentityHandler)
{
    UuidContext_t ctx;
    ctx.list = NULL;
    unsigned char* crt = NULL;
    size_t crtLen = 0;

    gCred = NULL;

    OicSecCred_t *cred = getSignedAsymKeyCred();
    OICFree(cred->credUsage);
    cred->credUsage = (char*)OICCalloc(strlen(TRUST_CA) + 1, 1);
    sprintf(cred->credUsage,"%s", TRUST_CA);
#ifndef NDEBUG
    LogCred(cred, TAG);
    LogCredResource(cred, TAG, "lab");
#endif //NDEBUG
    LL_APPEND(gCred, cred);
    GetIdentityHandler(&ctx, crt, crtLen);
}

TEST_F(CRED,GetDerKey)
{
    OicSecCred_t *cred = getSignedAsymKeyCred();
    cred->privateData.encoding = OIC_ENCODING_PEM;
    cred->privateData.data = (uint8_t*)getPemKey();
    cred->privateData.len = strlen((const char*)cred->privateData.data);
    LL_APPEND(gCred, cred);

    ByteArray_t* key = NULL;
    GetDerKey(key, PRIMARY_CERT);
    key = (ByteArray_t*)OICCalloc(1, sizeof(ByteArray_t));
    GetDerKey(key, PRIMARY_CERT);
}

TEST_F(CRED, cloneSecKey)
{
    OicSecKey_t *dst = (OicSecKey_t*)OICCalloc(1, sizeof(OicSecKey_t));
    OicSecKey_t *src = (OicSecKey_t*)OICCalloc(1, sizeof(OicSecKey_t));
    EXPECT_EQ(0,cloneSecKey(dst, src));
}

TEST_F(CRED, GetAllRoleCerts)
{
    RoleCertChain_t* output = NULL;
    EXPECT_EQ(OC_STACK_OK, GetAllRoleCerts(&output));
    FreeRoleCertChainList(output);
}
