//******************************************************************
// Copyright 2018 Cable Television Laboratories, Inc.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 s(the "License");
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
///
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Warning disabled globally but VS2013 ignores the /wd4200 option in C++ files.
#if defined(_MSC_VER) && _MSC_VER < 1900
#pragma warning(disable : 4200)
#endif

#include <gtest/gtest.h>

#include <mbedtls/error.h>
#include <mbedtls/entropy.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/oid.h>
#include <mbedtls/pem.h>
#include <mbedtls/asn1write.h>

#include "oic_platform.h"
#include "oic_malloc.h"
#include "octypes.h"
#include "ocrandom.h"
#include "cacommonutil.h"
#include "cacertprofile.h"
#include "occertutility.h"

#define TAG  "CA_CERT_PROFILE_UNIT_TEST"

#define PERSONALIZATION_STRING "IOTIVITY_CERT_TEST"

typedef char ValidityTime[50];

#define ARRAY_LEN(a) (sizeof(a) / sizeof(*a))

// helper functions
static CertProfileResult GenerateTestCert(CertType certType, CertProfileViolations violations, mbedtls_x509_crt *outCert);
static CertProfileResult GenerateInternalCert(CertType certType, mbedtls_x509_crt *issuerCert, mbedtls_x509_crt *outCert);
static void InitTestCert(mbedtls_x509_crt *cert);
static void FreeTestCert(mbedtls_x509_crt *cert);
static void removeCertChaining(mbedtls_x509_crt* rootCert, mbedtls_x509_crt* intCert, mbedtls_x509_crt* eeCert);
static CertProfileResult SetNotBefore(ValidityTime notBefore, bool invalid);
static CertProfileResult SetNotAfter(ValidityTime notAfter, bool invalid);
static int OCWriteBasicConstraints( mbedtls_x509write_cert *ctx, int is_ca, int max_pathlen, int critical);

class CACertProfileTests : public testing::Test {
    protected:
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

//*****************************************************************************
// Test Data
//*****************************************************************************

static const mbedtls_x509_crt_profile s_certProfile = {
    MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA256),        // MD algorithms
    MBEDTLS_X509_ID_FLAG(MBEDTLS_PK_ECKEY) |        // Allowed key types
    MBEDTLS_X509_ID_FLAG(MBEDTLS_PK_ECDSA),
    MBEDTLS_X509_ID_FLAG(MBEDTLS_ECP_DP_SECP256R1), // EC curves
    0                                               // RSA minimum key length - not used because we only use EC key pairs
};
                                      //    total len of eku byte string, will need to be written into final buff
                                      //    |
static uint8_t s_ekuHeaderBytes[] = { 0x30, 0x00 };

// serverAuth (1.3.6.1.5.5.7.3.1)             tag   len   1.3.  6.    1.    5.    5.    7.    3.    1
static uint8_t s_ekuServerAuthOidBytes[]  = { 0x06, 0x08, 0x2B, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x01 };

// clientAuth (1.3.6.1.5.5.7.3.2)             tag   len   1.3.  6.    1.    5.    5.    7.    3.    2
static uint8_t s_ekuClientAuthOidBytes[]  = { 0x06, 0x08, 0x2B, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x02 };


// OCF ID OID (1.3.6.1.4.1.44924.1.6)         tag   len   1.3.  6.    1.    4.    1.    44924.            1.    6
static uint8_t s_ekuOcfIdentityOidBytes[] = { 0x06, 0x0A, 0x2B, 0x06, 0x01, 0x04, 0x01, 0x82, 0xDE, 0x7C, 0x01, 0x06 };

// any (2.5.29.37.0)                  tag   len   2.5.  29.   37.   0
static uint8_t s_ekuAnyOidBytes[] = { 0x06, 0x04, 0x55, 0x1D, 0x25, 0x00 };

static const OCByteString s_ekuHeader = { s_ekuHeaderBytes, sizeof(s_ekuHeaderBytes) };
static const OCByteString s_ekuServerAuthOid = { s_ekuServerAuthOidBytes, sizeof(s_ekuServerAuthOidBytes) };
static const OCByteString s_ekuClientAuthOid = { s_ekuClientAuthOidBytes, sizeof(s_ekuClientAuthOidBytes) };
static const OCByteString s_ekuOcfIdentityOid = { s_ekuOcfIdentityOidBytes, sizeof(s_ekuOcfIdentityOidBytes) };
static const OCByteString s_ekuAnyOid = { s_ekuAnyOidBytes, sizeof(s_ekuAnyOidBytes) };


static const char s_ComplianceExtOid[] = MBEDTLS_OID_ISO_IDENTIFIED_ORG MBEDTLS_OID_ORG_DOD "\x01\x04\x01\x83\x91\x56\x01\x00"; // 1.3.6.1.4.1.51414.1.0
static const uint8_t s_ComplianceExtBytes[] = {

  0x30, 0x81, 0x94, // compliance extension sequence

  0x30, 0x09, // version sequence (9 bytes)
  0x02, 0x01, 0x02, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, // [2, 0, 0]

  0x30, 0x6C, // security profile sequence (108 bytes)

    0x0C, 0x19, // utf8_string, 25 bytes long, '1.3.6.1.4.1.51414.0.0.1.0' (baseline)
    0x31, 0x2E, 0x33, 0x2E, 0x36, 0x2E, 0x31, 0x2E, 0x34, 0x2E,
    0x31, 0x2E, 0x35, 0x31, 0x34, 0x31, 0x34, 0x2E, 0x30, 0x2E, 0x30, 0x2E, 0x31, 0x2E, 0x30,

    0x0C, 0x19, // utf8_string, 25 bytes long, '1.3.6.1.4.1.51414.0.0.2.0' (black)
    0x31, 0x2E, 0x33, 0x2E, 0x36, 0x2E, 0x31, 0x2E, 0x34, 0x2E,
    0x31, 0x2E, 0x35, 0x31, 0x34, 0x31, 0x34, 0x2E, 0x30, 0x2E, 0x30, 0x2E, 0x32, 0x2E, 0x30,

    0x0C, 0x19, // utf8_string, 25 bytes long, '1.3.6.1.4.1.51414.0.0.3.0' (blue)
    0x31, 0x2E, 0x33, 0x2E, 0x36, 0x2E, 0x31, 0x2E, 0x34, 0x2E,
    0x31, 0x2E, 0x35, 0x31, 0x34, 0x31, 0x34, 0x2E, 0x30, 0x2E, 0x30, 0x2E, 0x33, 0x2E, 0x30,

    0x0C, 0x19, // utf8_string, 25 bytes long, '1.3.6.1.4.1.51414.0.0.4.0' (purple)
    0x31, 0x2E, 0x33, 0x2E, 0x36, 0x2E, 0x31, 0x2E, 0x34, 0x2E,
    0x31, 0x2E, 0x35, 0x31, 0x34, 0x31, 0x34, 0x2E, 0x30, 0x2E, 0x30, 0x2E, 0x34, 0x2E, 0x30,

  0x0C, 0x0F, // urf8_string 15 bytes long (device name)
  0x49, 0x6F, 0x54, 0x69, 0x76, 0x69, 0x74, 0x79, 0x20, 0x53, // 'IoTivity Server'
  0x65, 0x72, 0x76, 0x65, 0x72,

  0x0C, 0x08, // urf8_string 8 bytes long (device manufacturer)
  0x49, 0x6F, 0x54, 0x69, 0x76, 0x69, 0x74, 0x79 // 'IoTivity'
};


static const char s_cplSecurityClaimsExtOid[] = MBEDTLS_OID_ISO_IDENTIFIED_ORG MBEDTLS_OID_ORG_DOD "\x01\x04\x01\x83\x91\x56\x01\x01"; // 1.3.6.1.4.1.51414.1.1
static const uint8_t s_cplSecurityClaimsExtBytes[] = {
    0x30, 0x1A, // sequence of length 26
    0x06, 0x0B, 0x2B, 0x06, 0x01, 0x04, 0x01, 0x83, 0x91, 0x56, 0x01, 0x01, 0x00, // OID 1.3.6.1.4.1.51414.1.1.0 (claim secure boot)
    0x06, 0x0B, 0x2B, 0x06, 0x01, 0x04, 0x01, 0x83, 0x91, 0x56, 0x01, 0x01, 0x01  // OID 1.3.6.1.4.1.51414.1.1.1 (claim hw backed credential)
};

static const char s_cplAttributesExtOid[] = MBEDTLS_OID_ISO_IDENTIFIED_ORG MBEDTLS_OID_ORG_DOD "\x01\x04\x01\x83\x91\x56\x01\x02"; // 1.3.6.1.4.1.51414.1.2
static const uint8_t s_cplAttributesExtBytes[] = {
    0x30, 0x20, // sequence of length 32

    0x0C, 0x0E, // utf8_string, 14 bytes long
    0X31, 0X2E, 0X33, 0X2E, 0X36, 0X2E, 0X31, 0X2E, 0X34, 0X2E, 0X31, 0X2E, 0X37, 0X31,  // '1.3.6.1.4.1.71'

    0x0C, 0x09, // utf8_string, 9 bytes long
    0X44, 0X69, 0X73, 0X63, 0X6F, 0X76, 0X65, 0X72, 0X79, // 'Discovery'

    0x0C, 0x03, // utf8_string, 3 bytes long
    0X31, 0X2E, 0X30 // '1.0'
};


// The key pairs below are OCF compliant ellliptic curve key pairs
// (RFC 5480 secp256r1 keys) generated by the IoTivity`certgenerator` utility.
// They are used as the public and signing keys for certificates
// that are generated for unit tests in this module.

static const char* s_rootCaPubKey = "-----BEGIN PUBLIC KEY-----\n"
"MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAET0k0tJJu5fnSXa5Wik7VPxpbiWeT\n"
"rWnvGY8Jv2/xsODb6A7SZPvpu4svwBkO2Wy0gCDwRtwi08ZLemgrtA7w7g==\n"
"-----END PUBLIC KEY-----\n";

static const char* s_rootCaPrivKey = "-----BEGIN EC PRIVATE KEY-----\n"
"MHcCAQEEIGrNaXfihmU41VIYHdXACpy6/X9X0MB9g0vtb/ujV403oAoGCCqGSM49\n"
"AwEHoUQDQgAET0k0tJJu5fnSXa5Wik7VPxpbiWeTrWnvGY8Jv2/xsODb6A7SZPvp\n"
"u4svwBkO2Wy0gCDwRtwi08ZLemgrtA7w7g==\n"
"-----END EC PRIVATE KEY-----\n";

static const char* s_intCaPubKey = "-----BEGIN PUBLIC KEY-----\n"
"MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEEQQ9DVoBQlnYFjBBvV1sligvG60T\n"
"vF0O1kXWjUOOFaVHVooHOnxUv1jQFNhH0wERkIYdHGA0DrkbLiWNe0qjDg==\n"
"-----END PUBLIC KEY-----\n";

static const char* s_intCaPrivKey = "-----BEGIN EC PRIVATE KEY-----\n"
"MHgCAQEEIQCtAr1LI2tSUGDkaANJOaII9I9RuNl/smpx61ZL1mvm1aAKBggqhkjO\n"
"PQMBB6FEA0IABBEEPQ1aAUJZ2BYwQb1dbJYoLxutE7xdDtZF1o1DjhWlR1aKBzp8\n"
"VL9Y0BTYR9MBEZCGHRxgNA65Gy4ljXtKow4=\n"
"-----END EC PRIVATE KEY-----\n";

static const char* s_eePubKey = "-----BEGIN PUBLIC KEY-----\n"
"MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEK+y7B9e0o7+6SeH2o7YnJKSpY5eO\n"
"cw29uqb5kTvGj7hOoBK/Ulr492YKlxouF+j1pXUvZYRy2aMoUnm2sGNd4Q==\n"
"-----END PUBLIC KEY-----\n";

static const char* s_eePrivKey = "-----BEGIN EC PRIVATE KEY-----\n"
"MHcCAQEEIFiqzuEt5rOCRB4D3fHW+L22vsM7mvnK+Np8vLDl3BwCoAoGCCqGSM49\n"
"AwEHoUQDQgAEK+y7B9e0o7+6SeH2o7YnJKSpY5eOcw29uqb5kTvGj7hOoBK/Ulr4\n"
"92YKlxouF+j1pXUvZYRy2aMoUnm2sGNd4Q==\n"
"-----END EC PRIVATE KEY-----\n";


// The key pair below are NON OCF compliant RSA keys generated using
// openssl.  The pair is used in negative testing to create
// certificates that violate the OCF crypto type requirements

static const char* s_nonEccPubKey = "-----BEGIN PUBLIC KEY-----\n"
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAwwInaid7Q3LSM+rvmI0a\n"
"46/IWxWkSbTP91W2UQ2jt5/iwZQUEaKRxnGUtW+TdigKb1XTEHLoOa2vBnO6OCzW\n"
"TfeA4aJSCO0cc/oW6inZqnp+3aAMoE34Y1PjOM69enY1WpbQGxoYxXpHjjuQ8M7i\n"
"xw2xnb03HRgvy/Yg8dJ2+QZTopuxqh4Z9qHBALFZW04iS2e4GPj1NQCoc/MBsX4e\n"
"WQV1UkeuYhbj1w3KDMDV6hcfNFp/A0XEmPh+ZcXTVRT0EHHY9sLGiVkjYX4b0i1u\n"
"3lOv2imbIu3mPnhSUaFFBENTL+MQcvnsaHS/WNZL7tlaTWCHStlJnAlKke1rmdBB\n"
"2wIDAQAB\n"
"-----END PUBLIC KEY-----\n";

static const char* s_nonEccPrivKey = "-----BEGIN RSA PRIVATE KEY-----\n"
"MIIEpQIBAAKCAQEAwwInaid7Q3LSM+rvmI0a46/IWxWkSbTP91W2UQ2jt5/iwZQU\n"
"EaKRxnGUtW+TdigKb1XTEHLoOa2vBnO6OCzWTfeA4aJSCO0cc/oW6inZqnp+3aAM\n"
"oE34Y1PjOM69enY1WpbQGxoYxXpHjjuQ8M7ixw2xnb03HRgvy/Yg8dJ2+QZTopux\n"
"qh4Z9qHBALFZW04iS2e4GPj1NQCoc/MBsX4eWQV1UkeuYhbj1w3KDMDV6hcfNFp/\n"
"A0XEmPh+ZcXTVRT0EHHY9sLGiVkjYX4b0i1u3lOv2imbIu3mPnhSUaFFBENTL+MQ\n"
"cvnsaHS/WNZL7tlaTWCHStlJnAlKke1rmdBB2wIDAQABAoIBADfxeT8x3mHGIdl1\n"
"D5EzAcDJ3++JcN0YOwUYmLn98Lav7oxcH7nDapCEawX14VbnO78e3vL8E/TgB7MR\n"
"Jh4RdtVdKKMSPviClUQMPeClauVGSPjpflXKqHROT4QvgxoLqVi4pTDxNqQIZVi/\n"
"a+A5acNzAT5gVtxI4QVhMahOGYVsszaTrCf/9/rKleqKOnFyNq4QNtPtdUv+RT9B\n"
"Sk5XruOJ3hR6Piivw5uLBYahiFbafXieQv4WAgTwZY0Hx1X5nhW3xbA2LXX4PuPv\n"
"+DPBPtS2qGVxEc487UhhMbqBGygvOPycIYyXXUPdlN8hQkvA/vk9bEDg6Koro/MQ\n"
"GbflZxkCgYEA+fLnwwyo6Yy4t5Gss8me0F5SA3praoGUfuN2xpzYy99puLlXazSi\n"
"FZ2PrbiufeUJ1Fuf2AVu8/SdKKuwoinYT5MMUhbCm+ywMlGhf5aACV3XQxATU+si\n"
"/YF8KDxRV6amBxqhmZahadKWmpV/nO7Xs1E/sIUuIIRWYa+qcnG/vJUCgYEAx7q/\n"
"Sq3nkPhbNqbUUp9/FuHP2vz241Y5j5kHbCwiUQ5Qs9ZibShWbHEXz3uV2bWvUmb0\n"
"eO+/Ch38SVSKpClwrubX2ABJZjxXnQiGRHdZ+Z7bepTTzNEdonlVL68D5f7Mix16\n"
"db96+ZM6cD36k8Duiekuv4LrVyDRUDHZIwNW+K8CgYEA7nQxbSCXyPAqjV6Zz4/L\n"
"Rmilf5H8BkkHHSLufr7GCYHjv4p6ftlgxJRjrU1iktK+VOzwistsNaspf9KEGcEo\n"
"D4pvF6WFFfJxih/cnfc7wTPYmK8ucUIDSKyOgWhCBggDnwVmJDvTVDzG10VG0LX/\n"
"7J/XzhTQk88NBfbr+nDQhh0CgYEAmfUz+4buF2u6M/iJdl8IAkVyieWxEjCnEi/t\n"
"7s5ebYjRYK8DFA2iANl0dzVWn4lTaft+u7Ji2pOeo5ANhtmCyO+cth4SdkJRbvTh\n"
"Oumda7Ouj2Elmp4fb9yDc8w+/w1X8mx4t0cwri11QIwmGbsApUIgDPQIIRNlBaJH\n"
"NJwgcHUCgYEAzjkPOMoS4idZk7e1f/ymprGT6nsbwo2yyD18yYzaNHpSnIkZpJm0\n"
"LXOoB+APM8Y9Hl3xHUtWV8WN1CmuWFyK7SvCPeqf6xm+fAFyRvFhX7Qm9ZMJLCif\n"
"KCYeMZY1ax+OqnwZ79+8UB2gbjQ5efbdb8jVrZDEABCRp3Ko+/tGVYg=\n"
"-----END RSA PRIVATE KEY-----\n";

static const char* s_rootCaSerial = "244240106504148688103282289603386694025638582413";
static const char* s_intCaSerial = "398222190774066044346258430305847925228418308044";
static const char* s_eeSerial = "611138144226495869526348350957100882548422091813";

static const char* s_rootCaIssuer = "O=OCF, OU=P256 Test Infrastructure, CN=Root Test CA";
static const char* s_rootCaSubject = "O=OCF, OU=P256 Test Infrastructure, CN=Root Test CA";

static const char* s_intCaIssuer = "O=OCF, OU=P256 Test Infrastructure, CN=Root Test CA";
static const char* s_intCaSubject = "O=OCF, OU=P256 Test Infrastructure, CN=Intermediate Test CA";

static const char* s_eeIssuer = "O=OCF, OU=P256 Test Infrastructure, CN=Intermediate Test CA";
static const char* s_eeSubject = "O=OCF, OU=P256 Test Infrastructure, CN=11111111-1111-1111-1111-111111111111";
static const char* s_eeUuid = "11111111-1111-1111-1111-111111111111";

//*****************************************************************************
// Tests
//*****************************************************************************


TEST_F(CACertProfileTests, RootCAProfiles)
{
    CertProfileResult cpResult = CP_STATUS_OK;
    CertProfileViolations violations = CP_NO_VIOLATIONS;
    CertProfileViolations expectedViolations = CP_NO_VIOLATIONS;
    mbedtls_x509_crt cert;

    CertProfileViolations violationsToTest[] =
    {
        CP_NO_VIOLATIONS,
        CP_NOT_YET_VALID,
        CP_EXPIRED,
        CP_INVALID_SIG_ALGORITHM,
        CP_INVALID_VERSION,
        CP_INVALID_PUB_KEY_ALGORITHM,
        CP_INVALID_ISSUER_SUBJ_MISMATCH,
        CP_INVALID_KEY_USAGE_MISSING,
        CP_INVALID_KEY_USAGE_EXTRA,
        CP_INVALID_BASIC_CONSTRAINTS_CA,
        CP_INVALID_BASIC_CONSTRAINTS_PATH_LEN
    };

    for (unsigned int i = 0; i < ARRAY_LEN(violationsToTest); i++)
    {
        InitTestCert(&cert);

        cpResult = GenerateTestCert(CERT_CA_ROOT, violationsToTest[i], &cert);
        EXPECT_TRUE(CP_STATUS_OK == cpResult);
        if (CP_STATUS_OK == cpResult)
        {
            expectedViolations = violationsToTest[i];
            violations = ValidateRootCACertProfile(&cert);
            EXPECT_TRUE(violations == expectedViolations);
        }
        else
        {
            OIC_LOG(ERROR, TAG, "Could not generate root CA test cert");
        }

        FreeTestCert(&cert);
    }
}

TEST_F(CACertProfileTests, IntermediateCAProfiles)
{
    CertProfileResult cpResult = CP_STATUS_OK;
    CertProfileViolations violations = CP_NO_VIOLATIONS;
    CertProfileViolations expectedViolations = CP_NO_VIOLATIONS;
    mbedtls_x509_crt cert;

    CertProfileViolations violationsToTest[] =
    {
        CP_NO_VIOLATIONS,
        CP_NOT_YET_VALID,
        CP_EXPIRED,
        CP_INVALID_SIG_ALGORITHM,
        CP_INVALID_VERSION,
        CP_INVALID_PUB_KEY_ALGORITHM,
        CP_INVALID_KEY_USAGE_MISSING,
        CP_INVALID_KEY_USAGE_EXTRA,
        CP_INVALID_BASIC_CONSTRAINTS_CA,
        CP_INVALID_BASIC_CONSTRAINTS_PATH_LEN
    };

    for (unsigned int i = 0; i < ARRAY_LEN(violationsToTest); i++)
    {
        InitTestCert(&cert);

        cpResult = GenerateTestCert(CERT_CA_INT, violationsToTest[i], &cert);
        EXPECT_TRUE(CP_STATUS_OK == cpResult);
        if (CP_STATUS_OK == cpResult)
        {
            expectedViolations = violationsToTest[i];
            if (CP_INVALID_BASIC_CONSTRAINTS_CA == violationsToTest[i])
            {
                // Because we are faking non-CA cert, mbed will not encode the max_pathlen
                // and we will get a pathlen violation as well
                expectedViolations |= CP_INVALID_BASIC_CONSTRAINTS_PATH_LEN;
            }
            violations = ValidateIntermediateCACertProfile(&cert);
            EXPECT_TRUE(violations == expectedViolations);
        }
        else
        {
            OIC_LOG(ERROR, TAG, "Could not generate root CA test cert");
        }

        FreeTestCert(&cert);
    }
}

TEST_F(CACertProfileTests, EndEntityProfiles)
{
    CertProfileResult cpResult = CP_STATUS_OK;
    CertProfileViolations violations = CP_NO_VIOLATIONS;
    CertProfileViolations expectedViolations = CP_NO_VIOLATIONS;
    mbedtls_x509_crt cert;

    CertProfileViolations violationsToTest[] =
    {
        CP_NO_VIOLATIONS,
        CP_NOT_YET_VALID,
        CP_EXPIRED,
        CP_INVALID_SIG_ALGORITHM,
        CP_INVALID_VERSION,
        CP_INVALID_PUB_KEY_ALGORITHM,
        CP_INVALID_KEY_USAGE_MISSING,
        CP_INVALID_KEY_USAGE_EXTRA,
        CP_INVALID_BASIC_CONSTRAINTS_CA,
        CP_INVALID_BASIC_CONSTRAINTS_PATH_LEN,
        CP_INVALID_EKU_NO_SERVER_AUTH,
        CP_INVALID_EKU_NO_CLIENT_AUTH,
        CP_INVALID_EKU_NO_OCF_ID_OID,
        CP_INVALID_EKU_INCLUDES_ANY
    };

    for (unsigned int i = 0; i < ARRAY_LEN(violationsToTest); i++)
    {
        InitTestCert(&cert);

        cpResult = GenerateTestCert(CERT_EE, violationsToTest[i], &cert);
        EXPECT_TRUE(CP_STATUS_OK == cpResult);
        if (CP_STATUS_OK == cpResult)
        {
            expectedViolations = violationsToTest[i];
            if (CP_INVALID_BASIC_CONSTRAINTS_PATH_LEN == violationsToTest[i])
            {
                // Because mbed does not support max_pathlen for EE certs,
                // it is returned as zero, no matter what was specified at encoding,
                // so this test will errantly pass
                expectedViolations = CP_NO_VIOLATIONS;
            }
            violations = ValidateEndEntityCertProfile(&cert);
            EXPECT_TRUE(violations == expectedViolations);
        }
        else
        {
            OIC_LOG(ERROR, TAG, "Could not generate End Entity test cert");
        }

        FreeTestCert(&cert);
    }
}

TEST_F(CACertProfileTests, InternalRootCACert)
{
    CertProfileResult cpResult = CP_STATUS_OK;
    CertProfileViolations violations = CP_NO_VIOLATIONS;

    mbedtls_x509_crt rootCert;

    InitTestCert(&rootCert);

    cpResult = GenerateInternalCert(CERT_CA_ROOT, NULL, &rootCert);
    EXPECT_TRUE(CP_STATUS_OK == cpResult);
    if (CP_STATUS_OK == cpResult)
    {
        violations = ValidateRootCACertProfile(&rootCert);
        EXPECT_TRUE(violations == CP_NO_VIOLATIONS);
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Could not generate internal CA test cert");
    }

    FreeTestCert(&rootCert);
}

TEST_F(CACertProfileTests, InternalIntermediateCert)
{
    CertProfileResult cpResult = CP_STATUS_OK;
    CertProfileViolations violations = CP_NO_VIOLATIONS;

    mbedtls_x509_crt rootCert;
    mbedtls_x509_crt intCert;

    InitTestCert(&rootCert);
    InitTestCert(&intCert);

    cpResult = GenerateInternalCert(CERT_CA_ROOT, NULL, &rootCert);
    EXPECT_TRUE(CP_STATUS_OK == cpResult);
    VERIFY_SUCCESS_OR_RETURN(TAG, (CP_STATUS_OK == cpResult), "Could not generate issuing Root CA cert", ERROR);
    violations = ValidateRootCACertProfile(&rootCert);
    EXPECT_TRUE(violations == CP_NO_VIOLATIONS);
    VERIFY_SUCCESS_OR_RETURN(TAG, (violations == CP_NO_VIOLATIONS), "Invalid Internal Root CA cert generated ", ERROR);

    cpResult = GenerateInternalCert(CERT_CA_INT , &rootCert, &intCert);
    EXPECT_TRUE(CP_STATUS_OK == cpResult);
    if (CP_STATUS_OK == cpResult)
    {
        violations = ValidateIntermediateCACertProfile(&intCert);
        EXPECT_TRUE(violations == CP_NO_VIOLATIONS);
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Could not generate internal CA test cert");
    }

    FreeTestCert(&rootCert);
    FreeTestCert(&intCert);
}

TEST_F(CACertProfileTests, InternalEndEntityCert)
{
    CertProfileResult cpResult = CP_STATUS_OK;
    CertProfileViolations violations = CP_NO_VIOLATIONS;

    mbedtls_x509_crt rootCert;
    mbedtls_x509_crt intCert;
    mbedtls_x509_crt eeCert;

    InitTestCert(&rootCert);
    InitTestCert(&intCert);
    InitTestCert(&eeCert);

    cpResult = GenerateInternalCert(CERT_CA_ROOT, NULL, &rootCert);
    EXPECT_TRUE(CP_STATUS_OK == cpResult);
    VERIFY_SUCCESS_OR_RETURN(TAG, (CP_STATUS_OK == cpResult), "Could not generate issuing Root CA cert", ERROR);
    violations = ValidateRootCACertProfile(&rootCert);
    EXPECT_TRUE(violations == CP_NO_VIOLATIONS);
    VERIFY_SUCCESS_OR_RETURN(TAG, (violations == CP_NO_VIOLATIONS), "Invalid Internal Root CA cert generated ", ERROR);

    cpResult = GenerateInternalCert(CERT_CA_INT, &rootCert, &intCert);
    EXPECT_TRUE(CP_STATUS_OK == cpResult);
    VERIFY_SUCCESS_OR_RETURN(TAG, (CP_STATUS_OK == cpResult), "Could not generate issuing Intermediate CA cert", ERROR);
    violations = ValidateIntermediateCACertProfile(&intCert);
    EXPECT_TRUE(violations == CP_NO_VIOLATIONS);
    VERIFY_SUCCESS_OR_RETURN(TAG, (violations == CP_NO_VIOLATIONS), "Invalid Internal Intermediate CA cert generated ", ERROR);

    cpResult = GenerateInternalCert(CERT_EE, &intCert, &eeCert);
    EXPECT_TRUE(CP_STATUS_OK == cpResult);
    if (CP_STATUS_OK == cpResult)
    {
        violations = ValidateEndEntityCertProfile(&eeCert);
        EXPECT_TRUE(violations == CP_NO_VIOLATIONS);
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Could not generate internal CA test cert");
    }

    FreeTestCert(&rootCert);
    FreeTestCert(&intCert);
    FreeTestCert(&eeCert);
}

TEST_F(CACertProfileTests, VerifyInternalCertChain)
{
    CertProfileResult cpResult = CP_STATUS_OK;
    CertProfileViolations violations = CP_NO_VIOLATIONS;
    int mbedRet = 0;

    mbedtls_x509_crt rootCert;
    mbedtls_x509_crt intCert;
    mbedtls_x509_crt eeCert;
    mbedtls_x509_crt *certChain;

    InitTestCert(&rootCert);
    InitTestCert(&intCert);
    InitTestCert(&eeCert);

    cpResult = GenerateInternalCert(CERT_CA_ROOT, NULL, &rootCert);
    EXPECT_TRUE(CP_STATUS_OK == cpResult);
    VERIFY_SUCCESS_OR_RETURN(TAG, (CP_STATUS_OK == cpResult), "Could not generate issuing Root CA cert", ERROR);
    violations = ValidateRootCACertProfile(&rootCert);
    EXPECT_TRUE(violations == CP_NO_VIOLATIONS);
    VERIFY_SUCCESS_OR_RETURN(TAG, (violations == CP_NO_VIOLATIONS), "Invalid Internal Root CA cert generated ", ERROR);

    cpResult = GenerateInternalCert(CERT_CA_INT, &rootCert, &intCert);
    EXPECT_TRUE(CP_STATUS_OK == cpResult);
    VERIFY_SUCCESS_OR_RETURN(TAG, (CP_STATUS_OK == cpResult), "Could not generate issuing Intermediate CA cert", ERROR);
    violations = ValidateIntermediateCACertProfile(&intCert);
    EXPECT_TRUE(violations == CP_NO_VIOLATIONS);
    VERIFY_SUCCESS_OR_RETURN(TAG, (violations == CP_NO_VIOLATIONS), "Invalid Internal Intermediate CA cert generated ", ERROR);

    cpResult = GenerateInternalCert(CERT_EE, &intCert, &eeCert);
    EXPECT_TRUE(CP_STATUS_OK == cpResult);
    VERIFY_SUCCESS_OR_RETURN(TAG, (CP_STATUS_OK == cpResult), "Could not generate End Entity CA cert", ERROR);
    EXPECT_TRUE(violations == CP_NO_VIOLATIONS);
    VERIFY_SUCCESS_OR_RETURN(TAG, (violations == CP_NO_VIOLATIONS), "Invalid Internal End Entity cert generated ", ERROR);

    uint32_t flags;
    certChain = &eeCert;
    certChain->next = &intCert;
    mbedRet = mbedtls_x509_crt_verify_with_profile(
        certChain, &rootCert, NULL,
        &s_certProfile, NULL,
        &flags, NULL, NULL);
    EXPECT_TRUE(0 == mbedRet);

    FreeTestCert(&rootCert);
    FreeTestCert(&intCert);
    InitTestCert(&eeCert);
}

TEST_F(CACertProfileTests, ChainProfileValidation)
{
    CertProfileResult cpResult = CP_STATUS_OK;
    CertProfileViolations violations = CP_NO_VIOLATIONS;
    CertProfileViolations violationsToGen = CP_NO_VIOLATIONS;
    int ret = 0;

    mbedtls_x509_crt validRootCert;
    mbedtls_x509_crt validIntCert;
    mbedtls_x509_crt validEeCert;

    mbedtls_x509_crt badRootCert;
    mbedtls_x509_crt badIntCert;
    mbedtls_x509_crt badEeCert;

    mbedtls_x509_crt *certChain = NULL;

    InitTestCert(&validRootCert);
    InitTestCert(&validIntCert);
    InitTestCert(&validEeCert);

    InitTestCert(&badRootCert);
    InitTestCert(&badIntCert);
    InitTestCert(&badEeCert);

    //
    // Valid chains
    //

    violationsToGen = CP_NO_VIOLATIONS;
    cpResult = GenerateTestCert(CERT_CA_ROOT, violationsToGen, &validRootCert);
    violations = ValidateRootCACertProfile(&validRootCert);
    EXPECT_TRUE(CP_STATUS_OK == cpResult);
    EXPECT_TRUE(violations == violationsToGen);
    VERIFY_SUCCESS_OR_RETURN(TAG, ((CP_STATUS_OK == cpResult) && (violations == violationsToGen)),
                             "Could not generate Root CA cert for chain test", ERROR);

    violationsToGen = CP_NO_VIOLATIONS;
    cpResult = GenerateTestCert(CERT_CA_INT, violationsToGen, &validIntCert);
    violations = ValidateIntermediateCACertProfile(&validIntCert);
    EXPECT_TRUE(CP_STATUS_OK == cpResult);
    EXPECT_TRUE(violations == violationsToGen);
    VERIFY_SUCCESS_OR_RETURN(TAG, ((CP_STATUS_OK == cpResult) && (violations == violationsToGen)),
                             "Could not generate Intermediate CA cert for chain test", ERROR);


    violationsToGen = CP_NO_VIOLATIONS;
    cpResult = GenerateTestCert(CERT_EE, violationsToGen, &validEeCert);
    violations = ValidateEndEntityCertProfile(&validEeCert);
    EXPECT_TRUE(CP_STATUS_OK == cpResult);
    EXPECT_TRUE(violations == violationsToGen);
    VERIFY_SUCCESS_OR_RETURN(TAG, ((CP_STATUS_OK == cpResult) && (violations == violationsToGen)),
                             "Could not generate End Entity cert for chain test", ERROR);

    certChain = &validIntCert;
    certChain->next = &validEeCert;
    certChain->next->next = NULL;
    ret = ValidateAuthCertChainProfiles(certChain);
    EXPECT_TRUE(0 == ret);
    removeCertChaining(&validRootCert, &validIntCert, &validEeCert);

    certChain = &validRootCert;
    ret = ValidateRootCACertListProfiles(certChain);
    EXPECT_TRUE(0 == ret);

    //
    // Invalid chains
    //

    violationsToGen = CP_NOT_YET_VALID | CP_INVALID_KEY_USAGE_EXTRA | CP_INVALID_KEY_USAGE_MISSING;
    cpResult = GenerateTestCert(CERT_CA_ROOT, violationsToGen, &badRootCert);
    violations = ValidateRootCACertProfile(&badRootCert);
    EXPECT_TRUE(CP_STATUS_OK == cpResult);
    EXPECT_TRUE(violations == violationsToGen);
    VERIFY_SUCCESS_OR_RETURN(TAG, ((CP_STATUS_OK == cpResult) && (violations == violationsToGen)),
                             "Could not generate Root CA cert for chain test", ERROR);

    violationsToGen = CP_EXPIRED | CP_INVALID_SIG_ALGORITHM;
    cpResult = GenerateTestCert(CERT_CA_INT, violationsToGen, &badIntCert);
    violations = ValidateIntermediateCACertProfile(&badIntCert);
    EXPECT_TRUE(CP_STATUS_OK == cpResult);
    EXPECT_TRUE(violations == violationsToGen);
    VERIFY_SUCCESS_OR_RETURN(TAG, ((CP_STATUS_OK == cpResult) && (violations == violationsToGen)),
                             "Could not generate Intermediate CA cert for chain test", ERROR);


    violationsToGen = CP_INVALID_EKU_NO_SERVER_AUTH | CP_INVALID_VERSION | CP_INVALID_EKU_NO_OCF_ID_OID;
    cpResult = GenerateTestCert(CERT_EE, violationsToGen, &badEeCert);
    violations = ValidateEndEntityCertProfile(&badEeCert);
    EXPECT_TRUE(CP_STATUS_OK == cpResult);
    EXPECT_TRUE(violations == violationsToGen);
    VERIFY_SUCCESS_OR_RETURN(TAG, ((CP_STATUS_OK == cpResult) && (violations == violationsToGen)),
                             "Could not generate End Entity cert for chain test", ERROR);

    // no EE cert in auth chain
    certChain = &validRootCert;
    certChain->next = &validIntCert;
    certChain->next->next = NULL;
    ret = ValidateAuthCertChainProfiles(certChain);
    EXPECT_TRUE(CP_INVALID_CERT_CHAIN == ret);
    removeCertChaining(&validRootCert, &validIntCert, &validEeCert);

    // multiple EE certs in auth chain
    certChain = &validEeCert;
    certChain->next = &badEeCert;
    certChain->next->next = NULL;
    ret = ValidateAuthCertChainProfiles(certChain);
    EXPECT_TRUE(CP_INVALID_CERT_CHAIN == ret);
    removeCertChaining(&validRootCert, &validIntCert, &validEeCert);
    removeCertChaining(&badRootCert, &badIntCert, &badEeCert);

    // Auth cert profile errors
    certChain = &badIntCert;
    certChain->next = &validEeCert;
    certChain->next->next = NULL;
    ret = ValidateAuthCertChainProfiles(certChain);
    EXPECT_TRUE(1 == ret);
    certChain->next = &badEeCert;
    certChain->next->next = NULL;
    ret = ValidateAuthCertChainProfiles(certChain);
    EXPECT_TRUE(2 == ret);
    removeCertChaining(&validRootCert, &validIntCert, &validEeCert);
    removeCertChaining(&badRootCert, &badIntCert, &badEeCert);

    // Time window errors
    certChain = &badRootCert;
    certChain->next = &badIntCert;
    certChain->next->next = NULL;
    ret = CheckCertListTimeWindows(certChain);
    EXPECT_TRUE(2 == ret);
    removeCertChaining(&validRootCert, &validIntCert, &validEeCert);
    removeCertChaining(&badRootCert, &badIntCert, &badEeCert);

    FreeTestCert(&validRootCert);
    FreeTestCert(&validIntCert);
    FreeTestCert(&validEeCert);

    FreeTestCert(&badRootCert);
    FreeTestCert(&badIntCert);
    FreeTestCert(&badEeCert);
}
//*****************************************************************************
// Helper fxns
//*****************************************************************************

static void removeCertChaining(mbedtls_x509_crt* rootCert, mbedtls_x509_crt* intCert, mbedtls_x509_crt* eeCert)
{
    rootCert->next = NULL;
    intCert->next = NULL;
    eeCert->next = NULL;
}

static CertProfileResult SetNotBefore(ValidityTime notBefore, bool invalid)
{
    time_t nowTimeT = time(NULL);
    struct tm *now = gmtime(&nowTimeT);

    if (invalid)
    {
        now->tm_year += 10;
    }

    if (0 == strftime(notBefore, sizeof(ValidityTime), "%Y%m%d%H%M%S", now))
    {
        return CP_DATE_ERROR;
    }
    return CP_STATUS_OK;
}

static CertProfileResult SetNotAfter(ValidityTime notAfter, bool invalid)
{
    time_t nowTimeT = time(NULL);
    struct tm *now = gmtime(&nowTimeT);

    if (invalid)
    {
        now->tm_year -= 10;
    }
    else
    {
        now->tm_year += 10;
    }

    if (0 == strftime(notAfter, sizeof(ValidityTime), "%Y%m%d%H%M%S", now))
    {
        return CP_DATE_ERROR;
    }
    return CP_STATUS_OK;
}

// returns length of constructed byte stream, or -1 on error
static int constructEku( unsigned int numOids, const OCByteString **oidList, unsigned char *buf, unsigned int buflen)
{
    size_t encodingLen = 0;
    unsigned char *curPtr = buf;
    unsigned char *bufEnd = buf+buflen;
    if (0 == numOids )
    {
        return 0;
    }

    // add header
    if ( (curPtr + s_ekuHeader.len > bufEnd) ||
         (encodingLen + s_ekuHeader.len) > 127)
    {
        return -1;
    }
    memcpy(curPtr, s_ekuHeader.bytes, s_ekuHeader.len);
    encodingLen += s_ekuHeader.len;
    curPtr += s_ekuHeader.len;

     // add supplied OIDs
    for ( unsigned int i = 0; i < numOids; i++ )
    {
        if ( (curPtr + oidList[i]->len > bufEnd) ||
             (encodingLen + s_ekuHeader.len) > 127)
        {
            return -1;
        }
        memcpy(curPtr, oidList[i]->bytes, oidList[i]->len);
        encodingLen += oidList[i]->len;
        curPtr += oidList[i]->len;
    }

    // write the length into the encoding (2nd byte), only supporting short form
    if ( encodingLen > 127 )
    {
        return -1;
    }
    *(buf+1) = (unsigned char)(encodingLen - s_ekuHeader.len);

    return (int)encodingLen;
}

// must call InitTestCert before calling, and FreeTestCert when done
static CertProfileResult GenerateTestCert(CertType certType, CertProfileViolations violations, mbedtls_x509_crt *outCert)
{
    OC_UNUSED(s_eePrivKey);

    int mbedRet = 0;
    CertProfileResult cpResult = CP_STATUS_OK;

    char buf[2048] = "";
    char mbedErrBuf[256] = "";

    int isCa = 0;
    int maxPathLen = 0;
    int critical = 0;
    unsigned int usage = 0;
    bool makeDateInvalid = false;

    mbedtls_x509write_cert outCertCtx;
    mbedtls_pk_context subjectPubKeyCtx;
    mbedtls_pk_context issuerPrivKeyCtx;
    mbedtls_mpi serialMpi;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;

    mbedtls_x509write_crt_init(&outCertCtx);
    mbedtls_pk_init(&subjectPubKeyCtx);
    mbedtls_pk_init(&issuerPrivKeyCtx);
    mbedtls_mpi_init(&serialMpi);
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    const char *subjectPubKey = NULL;
    const char *issuerPrivKey = NULL;
    const char *serial = NULL;
    const char *subject = NULL;
    const char *issuer = NULL;

    switch ( certType )
    {
    case CERT_CA_ROOT:
        subjectPubKey = (violations & CP_INVALID_PUB_KEY_ALGORITHM) ? s_nonEccPubKey : s_rootCaPubKey;
        issuerPrivKey = (violations & CP_INVALID_PUB_KEY_ALGORITHM) ? s_nonEccPrivKey : s_rootCaPrivKey;
        serial = s_rootCaSerial;
        subject = s_rootCaSubject;
        issuer = (violations & CP_INVALID_ISSUER_SUBJ_MISMATCH) ? s_eeIssuer : s_rootCaIssuer;
        break;
    case CERT_CA_INT:
        subjectPubKey = (violations & CP_INVALID_PUB_KEY_ALGORITHM) ? s_nonEccPubKey : s_intCaPubKey;
        issuerPrivKey = (violations & CP_INVALID_PUB_KEY_ALGORITHM) ? s_nonEccPrivKey : s_rootCaPrivKey;
        serial = s_intCaSerial;
        subject = s_intCaSubject;
        issuer = s_intCaIssuer;
        break;
    case CERT_EE:
        subjectPubKey =  (violations & CP_INVALID_PUB_KEY_ALGORITHM) ? s_nonEccPubKey : s_eePubKey;
        issuerPrivKey = (violations & CP_INVALID_PUB_KEY_ALGORITHM) ? s_nonEccPrivKey : s_intCaPrivKey;
        serial = s_eeSerial;
        subject = s_eeSubject;
        issuer = s_eeIssuer;
        break;
    default:
        OIC_LOG(ERROR, TAG, "Unknown cert type for internal cert generation");
        return CP_STATUS_FAILED;
    };

    // Set up

    mbedRet = mbedtls_mpi_read_string(&serialMpi, 10, serial);
    CP_LOG_MBED_ERROR(TAG, mbedRet, mbedErrBuf, sizeof(mbedErrBuf), ERROR);
    cpResult = (0 == mbedRet) ? CP_STATUS_OK : CP_STATUS_FAILED;
    VERIFY_SUCCESS_OR_EXIT(TAG, (0 == mbedRet), "Problem reading serial number", ERROR);

    mbedRet = mbedtls_pk_parse_public_key(&subjectPubKeyCtx, (const uint8_t *)subjectPubKey, strlen(subjectPubKey) + 1);
    CP_LOG_MBED_ERROR(TAG, mbedRet, mbedErrBuf, sizeof(mbedErrBuf), ERROR);
    cpResult = (0 == mbedRet) ? CP_STATUS_OK : CP_STATUS_FAILED;
    VERIFY_SUCCESS_OR_EXIT(TAG, (0 == mbedRet), "Problem parsing public key", ERROR);

    mbedRet = mbedtls_pk_parse_key(&issuerPrivKeyCtx, (const uint8_t *)issuerPrivKey, strlen(issuerPrivKey) + 1, NULL, 0);
    CP_LOG_MBED_ERROR(TAG, mbedRet, mbedErrBuf, sizeof(mbedErrBuf), ERROR);
    cpResult = (0 == mbedRet) ? CP_STATUS_OK : CP_STATUS_FAILED;
    VERIFY_SUCCESS_OR_EXIT(TAG, (0 == mbedRet), "Problem parsing private key", ERROR);

    mbedRet = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                (const uint8_t *)PERSONALIZATION_STRING,
                                sizeof(PERSONALIZATION_STRING));
    CP_LOG_MBED_ERROR(TAG, mbedRet, mbedErrBuf, sizeof(mbedErrBuf), ERROR);
    cpResult = (0 == mbedRet) ? CP_STATUS_OK : CP_STATUS_FAILED;
    VERIFY_SUCCESS_OR_EXIT(TAG, (0 == mbedRet), "Problem setting  seed", ERROR);

    mbedtls_ctr_drbg_set_prediction_resistance(&ctr_drbg, MBEDTLS_CTR_DRBG_PR_ON);

    // Validity window

    ValidityTime notValidBefore;
    makeDateInvalid = (violations & CP_NOT_YET_VALID) ? true : false;
    cpResult = SetNotBefore(notValidBefore, makeDateInvalid);
    VERIFY_SUCCESS_OR_EXIT(TAG, (CP_STATUS_OK == cpResult), "Problem setting not before date", ERROR);

    ValidityTime notValidAfter;
    makeDateInvalid = (violations & CP_EXPIRED) ? true : false;
    cpResult = SetNotAfter(notValidAfter, makeDateInvalid);
    VERIFY_SUCCESS_OR_EXIT(TAG, (CP_STATUS_OK == cpResult), "Problem setting not after date", ERROR);

    mbedRet = mbedtls_x509write_crt_set_validity(&outCertCtx, notValidBefore, notValidAfter);
    CP_LOG_MBED_ERROR(TAG, mbedRet, mbedErrBuf, sizeof(mbedErrBuf), ERROR);
    cpResult = (0 == mbedRet) ? CP_STATUS_OK : CP_STATUS_FAILED;
    VERIFY_SUCCESS_OR_EXIT(TAG, (0 == mbedRet), "Problem setting certificate time window", ERROR);

    // Version

    // mbedtls only encodes V3, will fake violations below
    mbedtls_x509write_crt_set_version(&outCertCtx, MBEDTLS_X509_CRT_VERSION_3);

    // Sig Type

    if (violations & CP_INVALID_SIG_ALGORITHM)
    {
        mbedtls_x509write_crt_set_md_alg(&outCertCtx, MBEDTLS_MD_SHA224);
    }
    else
    {
        mbedtls_x509write_crt_set_md_alg(&outCertCtx, MBEDTLS_MD_SHA256);
    }

    // Serial Number

    mbedRet = mbedtls_x509write_crt_set_serial(&outCertCtx, &serialMpi);
    CP_LOG_MBED_ERROR(TAG, mbedRet, mbedErrBuf, sizeof(mbedErrBuf), ERROR);
    cpResult = (0 == mbedRet) ? CP_STATUS_OK : CP_STATUS_FAILED;
    VERIFY_SUCCESS_OR_EXIT(TAG, 0 == mbedRet, "Problem writing certificate serial number", ERROR);

    // Subject

    mbedRet = mbedtls_x509write_crt_set_subject_name(&outCertCtx, subject);
    CP_LOG_MBED_ERROR(TAG, mbedRet, mbedErrBuf, sizeof(mbedErrBuf), ERROR);
    cpResult = (0 == mbedRet) ? CP_STATUS_OK : CP_STATUS_FAILED;
    VERIFY_SUCCESS_OR_EXIT(TAG, 0 == mbedRet, "Problem writing certificate subject", ERROR);

    // Issuer

    mbedRet = mbedtls_x509write_crt_set_issuer_name(&outCertCtx, issuer);
    CP_LOG_MBED_ERROR(TAG, mbedRet, mbedErrBuf, sizeof(mbedErrBuf), ERROR);
    cpResult = (0 == mbedRet) ? CP_STATUS_OK : CP_STATUS_FAILED;
    VERIFY_SUCCESS_OR_EXIT(TAG, 0 == mbedRet, "Problem writing certificate issuer", ERROR);

    // Keys

    mbedtls_x509write_crt_set_subject_key(&outCertCtx, &subjectPubKeyCtx);
    mbedtls_x509write_crt_set_issuer_key(&outCertCtx, &issuerPrivKeyCtx);

    // Basic Constraints

    // mbedtls max_pathlen behaviour:
    // CA Cert: Expects RFC5280_val as encoding input
    //          Provides RFC5280_val+1 as decoding output, where 0 = not present
    // EE Cert: Does not encode
    //          Provides 0 as decoding output

    switch (certType)
    {
    case CERT_CA_ROOT:
        isCa = (violations & CP_INVALID_BASIC_CONSTRAINTS_CA) ? 0 : 1;
        maxPathLen = (violations & CP_INVALID_BASIC_CONSTRAINTS_PATH_LEN) ? 2 :-1;
        critical = 1;
        break;
    case CERT_CA_INT:
        isCa = (violations & CP_INVALID_BASIC_CONSTRAINTS_CA) ? 0 : 1;
        maxPathLen = (violations & CP_INVALID_BASIC_CONSTRAINTS_PATH_LEN) ? -1 : 0;
        critical = 1;
        break;
    case CERT_EE:
        isCa = (violations & CP_INVALID_BASIC_CONSTRAINTS_CA) ? 1 : 0;
        maxPathLen = (violations & CP_INVALID_BASIC_CONSTRAINTS_PATH_LEN) ? 10 : -1;
        critical = 0;
        break;
    };

    mbedRet = OCWriteBasicConstraints(&outCertCtx, isCa, maxPathLen, critical  );
    CP_LOG_MBED_ERROR(TAG, mbedRet, mbedErrBuf, sizeof(mbedErrBuf), ERROR);
    cpResult = (0 == mbedRet) ? CP_STATUS_OK : CP_STATUS_FAILED;
    VERIFY_SUCCESS_OR_EXIT(TAG, 0 == mbedRet, "Problem writing basic constraints", ERROR);

    // Key Usage

    switch (certType)
    {
    case CERT_CA_ROOT:
        usage = MBEDTLS_X509_KU_KEY_CERT_SIGN | MBEDTLS_X509_KU_CRL_SIGN;
        if (violations & CP_INVALID_KEY_USAGE_MISSING) {
            usage &= ~MBEDTLS_X509_KU_CRL_SIGN;
        }
        if (violations & CP_INVALID_KEY_USAGE_EXTRA) {
            usage |= MBEDTLS_X509_KU_DIGITAL_SIGNATURE;
        }
        break;
    case CERT_CA_INT:
        usage = MBEDTLS_X509_KU_KEY_CERT_SIGN | MBEDTLS_X509_KU_CRL_SIGN;
        if (violations & CP_INVALID_KEY_USAGE_MISSING) {
            usage &= ~MBEDTLS_X509_KU_KEY_CERT_SIGN;
        }
        if (violations & CP_INVALID_KEY_USAGE_EXTRA) {
            usage |= MBEDTLS_X509_KU_KEY_ENCIPHERMENT;
        }
        break;
    case CERT_EE:
        usage = MBEDTLS_X509_KU_DIGITAL_SIGNATURE | MBEDTLS_X509_KU_KEY_AGREEMENT;
        if ( violations & CP_INVALID_KEY_USAGE_MISSING ) {
            usage &= ~MBEDTLS_X509_KU_KEY_AGREEMENT;
        }
        if (violations & CP_INVALID_KEY_USAGE_EXTRA) {
            usage |= MBEDTLS_X509_KU_NON_REPUDIATION;
        }
        break;
    };

    mbedRet = mbedtls_x509write_crt_set_key_usage(&outCertCtx, usage);
    cpResult = (0 == mbedRet) ? CP_STATUS_OK : CP_STATUS_FAILED;
    CP_LOG_MBED_ERROR(TAG, mbedRet, mbedErrBuf, sizeof(mbedErrBuf), ERROR);
    VERIFY_SUCCESS_OR_EXIT(TAG, (0 == mbedRet), "Problem writing key usages", ERROR);

    // Extended Key Usage

    if (CERT_EE == certType)
    {
        const OCByteString *oidList[4];

        unsigned int numOids = 0;
        if (!(violations & CP_INVALID_EKU_NO_SERVER_AUTH))
        {
            oidList[numOids] = &s_ekuServerAuthOid;
            numOids++;
        }

        if (!(violations & CP_INVALID_EKU_NO_CLIENT_AUTH))
        {
            oidList[numOids] = &s_ekuClientAuthOid;
            numOids++;
        }

        if (!(violations & CP_INVALID_EKU_NO_OCF_ID_OID))
        {
            oidList[numOids] = &s_ekuOcfIdentityOid;
            numOids++;
        }

        if (violations & CP_INVALID_EKU_INCLUDES_ANY)
        {
            oidList[numOids] = &s_ekuAnyOid;
            numOids++;
        }

        if ( numOids > 0 )
        {
            int encodingLen = constructEku(numOids, oidList, (unsigned char*)buf, sizeof(buf));
            cpResult = (0 < encodingLen) ? CP_STATUS_OK : CP_STATUS_FAILED;
            VERIFY_SUCCESS_OR_EXIT(TAG, (0 < encodingLen), "Insufficient buffer size for EKU encoding", ERROR);


            mbedRet = mbedtls_x509write_crt_set_extension(
                        &outCertCtx,
                        MBEDTLS_OID_EXTENDED_KEY_USAGE,
                        MBEDTLS_OID_SIZE(MBEDTLS_OID_EXTENDED_KEY_USAGE), 0,
                        (const unsigned char*)buf, encodingLen);
            CP_LOG_MBED_ERROR(TAG, mbedRet, mbedErrBuf, sizeof(mbedErrBuf), ERROR);
            cpResult = (0 == mbedRet) ? CP_STATUS_OK : CP_STATUS_FAILED;
            VERIFY_SUCCESS_OR_EXIT(TAG, (0 == mbedRet), "Problem writing eku OCF OID", ERROR);
        }
    }

    // Optional extensions

    if (CERT_EE == certType)
    {

        // FILE *fp = fopen("./cpl.der", "wb");
        // fwrite(s_cplAttributesExtBytes, sizeof(s_cplAttributesExtBytes), 1, fp );
        // fclose(fp);

        mbedRet = mbedtls_x509write_crt_set_extension(
                    &outCertCtx,
                    s_cplAttributesExtOid,
                    MBEDTLS_OID_SIZE(s_cplAttributesExtOid), 0,
                    (const unsigned char*)s_cplAttributesExtBytes, sizeof(s_cplAttributesExtBytes));
        CP_LOG_MBED_ERROR(TAG, mbedRet, mbedErrBuf, sizeof(mbedErrBuf), ERROR);
        cpResult = (0 == mbedRet) ? CP_STATUS_OK : CP_STATUS_FAILED;
        VERIFY_SUCCESS_OR_EXIT(TAG, (0 == mbedRet), "Problem writing certified product list extension", ERROR);

        // fp = fopen("./claims.der", "wb");
        // fwrite(s_cplSecurityClaimsExtBytes, sizeof(s_cplSecurityClaimsExtBytes), 1, fp );
        // fclose(fp);

        mbedRet = mbedtls_x509write_crt_set_extension(
                    &outCertCtx,
                    s_cplSecurityClaimsExtOid,
                    MBEDTLS_OID_SIZE(s_cplSecurityClaimsExtOid), 0,
                    (const unsigned char*)s_cplSecurityClaimsExtBytes, sizeof(s_cplSecurityClaimsExtBytes));
        CP_LOG_MBED_ERROR(TAG, mbedRet, mbedErrBuf, sizeof(mbedErrBuf), ERROR);
        cpResult = (0 == mbedRet) ? CP_STATUS_OK : CP_STATUS_FAILED;
        VERIFY_SUCCESS_OR_EXIT(TAG, (0 == mbedRet), "Problem writing certified product list extension", ERROR);

        // fp = fopen("./compliance.der", "wb");
        // fwrite(s_ComplianceExtBytes, sizeof(s_ComplianceExtBytes), 1, fp );
        // fclose(fp);

        mbedRet = mbedtls_x509write_crt_set_extension(
                    &outCertCtx,
                    s_ComplianceExtOid,
                    MBEDTLS_OID_SIZE(s_ComplianceExtOid), 0,
                    (const unsigned char*)s_ComplianceExtBytes, sizeof(s_ComplianceExtBytes));
        CP_LOG_MBED_ERROR(TAG, mbedRet, mbedErrBuf, sizeof(mbedErrBuf), ERROR);
        cpResult = (0 == mbedRet) ? CP_STATUS_OK : CP_STATUS_FAILED;
        VERIFY_SUCCESS_OR_EXIT(TAG, (0 == mbedRet), "Problem writing certified product list extension", ERROR);
    }

    // Create the cert

    mbedRet = mbedtls_x509write_crt_pem(&outCertCtx, (uint8_t *)buf, sizeof(buf), mbedtls_ctr_drbg_random, &ctr_drbg);
    CP_LOG_MBED_ERROR(TAG, mbedRet, mbedErrBuf, sizeof(mbedErrBuf), ERROR);
    cpResult = (0 == mbedRet) ? CP_STATUS_OK : CP_STATUS_FAILED;
    VERIFY_SUCCESS_OR_EXIT(TAG, (0 == mbedRet), "Problem writing certificate", ERROR);

    mbedRet = mbedtls_x509_crt_parse(outCert, (const unsigned char*)buf, sizeof(buf));
    CP_LOG_MBED_ERROR(TAG, mbedRet, mbedErrBuf, sizeof(mbedErrBuf), ERROR);
    cpResult = (0 == mbedRet) ? CP_STATUS_OK : CP_STATUS_FAILED;
    VERIFY_SUCCESS_OR_EXIT(TAG, (0 == mbedRet), "Problem parsing cert buffer", ERROR);

    if (violations & CP_INVALID_VERSION )
    {
        outCert->version = 1;
    }

exit:
    mbedtls_mpi_free(&serialMpi);
    mbedtls_pk_free(&subjectPubKeyCtx);
    mbedtls_pk_free(&issuerPrivKeyCtx);
    mbedtls_x509write_crt_free(&outCertCtx);
    mbedtls_entropy_free(&entropy);
    mbedtls_ctr_drbg_free(&ctr_drbg);

    return cpResult;
}

// must call InitTestCert before calling, and FreeTestCert when done
// pass issuerCert as NULL for root CA
static CertProfileResult GenerateInternalCert(CertType certType, mbedtls_x509_crt *issuerCert, mbedtls_x509_crt *outCert)
{
    char pemBuf[1024];
    char mbedErrBuf[256];

    CertProfileResult cpResult = CP_STATUS_OK;
    OCStackResult stackResult = OC_STACK_OK;
    int mbedRet = 0;

    char *certificate = NULL;
    size_t certificateLen = 0;

    const char *subjectPubKey = NULL;
    const char *issuerPrivKey = NULL;
    const char *serial = NULL;
    const char *subject = NULL;
    const char *issuerCertPem = NULL;
    OicUuid_t subjectUuid;

    switch ( certType )
    {
    case CERT_CA_ROOT:
        subjectPubKey = s_rootCaPubKey;
        issuerPrivKey = s_rootCaPrivKey;
        serial = s_rootCaSerial;
        subject = s_rootCaSubject;
        break;
    case CERT_CA_INT:
        subjectPubKey = s_intCaPubKey;
        issuerPrivKey = s_rootCaPrivKey;
        serial = s_intCaSerial;
        subject = s_intCaSubject;
        break;
    case CERT_EE:
        subjectPubKey = s_eePubKey;
        issuerPrivKey = s_intCaPrivKey;
        serial = s_eeSerial;
        if (!OCConvertStringToUuid(s_eeUuid, subjectUuid.id))
        {
            OIC_LOG(ERROR, TAG, "Could not generate end entity cert UUID");
            cpResult = CP_STATUS_FAILED;
            goto exit;
        }
        break;
    default:
        OIC_LOG(ERROR, TAG, "Unknown cert type for internal cert generation");
        cpResult = CP_STATUS_FAILED;
        goto exit;
    };

    ValidityTime notValidBefore;
    cpResult = SetNotBefore(notValidBefore, false);
    VERIFY_SUCCESS_OR_EXIT(TAG, (CP_STATUS_OK == cpResult), "Problem setting not before date", ERROR);

    ValidityTime notValidAfter;
    cpResult = SetNotAfter(notValidAfter, false);
    VERIFY_SUCCESS_OR_EXIT(TAG, (CP_STATUS_OK == cpResult), "Problem setting not after date", ERROR);

    // need to convert issuer cert to PEM
    if ( (CERT_CA_ROOT != certType) && (NULL != issuerCert) )
    {
        size_t pemLen = 0;
        mbedRet = mbedtls_pem_write_buffer( "-----BEGIN CERTIFICATE-----\n",
                                            "-----END CERTIFICATE-----\n",
                                            issuerCert->raw.p, issuerCert->raw.len,
                                            (unsigned char*)pemBuf, sizeof(pemBuf), &pemLen );
        CP_LOG_MBED_ERROR(TAG, mbedRet, mbedErrBuf, sizeof(mbedErrBuf), ERROR);
        cpResult = (0 == mbedRet) ? CP_STATUS_OK : CP_STATUS_FAILED;
        VERIFY_SUCCESS_OR_EXIT(TAG, 0 == mbedRet, "Problem generating PEM from issuer cert", ERROR);
        issuerCertPem = pemBuf;
    }

    switch ( certType )
    {
    case CERT_CA_ROOT:
        stackResult = OCGenerateRootCACertificate(
            subject, subjectPubKey,
            issuerCertPem, issuerPrivKey,
            serial,
            notValidBefore, notValidAfter,
            &certificate, &certificateLen);
        break;
    case CERT_CA_INT:
        stackResult = OCGenerateIntermediateCACertificate(
            subject, subjectPubKey,
            issuerCertPem, issuerPrivKey,
            serial,
            notValidBefore, notValidAfter,
            &certificate, &certificateLen);
        break;
    case CERT_EE:
        stackResult = OCGenerateIdentityCertificate(
            &subjectUuid, subjectPubKey,
            issuerCertPem, issuerPrivKey,
            serial,
            notValidBefore, notValidAfter,
            &certificate, &certificateLen);
        break;
    default:
        OIC_LOG(ERROR, TAG, "Unknown cert type for internal cert generation");
        cpResult = CP_STATUS_FAILED;
        goto exit;
    };
    VERIFY_SUCCESS_OR_EXIT(TAG, (OC_STACK_OK == stackResult), "Problem generating internal certificate", ERROR);

    mbedRet = mbedtls_x509_crt_parse(outCert, (const unsigned char*)certificate, certificateLen);
    CP_LOG_MBED_ERROR(TAG, mbedRet, mbedErrBuf, sizeof(mbedErrBuf), ERROR);
    cpResult = (0 == mbedRet) ? CP_STATUS_OK : CP_STATUS_FAILED;
    VERIFY_SUCCESS_OR_EXIT(TAG, (0 == mbedRet), "Problem converting internal cert to mbed format", ERROR);

exit:
    OICFree(certificate);
    return cpResult;
    };

static void InitTestCert(mbedtls_x509_crt *cert)
{
    mbedtls_x509_crt_init(cert);
}


// write basic constraints to a cert
// Same as mbedtls_x509write_crt_set_basic_constraints, with the added ability to set `critical` flag
// returns mbedtls error
static int OCWriteBasicConstraints( mbedtls_x509write_cert *ctx, int is_ca, int max_pathlen, int critical)
{

    int ret;
    unsigned char buf[9];
    unsigned char *c = buf + sizeof(buf);
    size_t len = 0;

    memset( buf, 0, sizeof(buf) );

    if( is_ca && max_pathlen > 127 )
        return( MBEDTLS_ERR_X509_BAD_INPUT_DATA );

    if( is_ca )
    {
        if( max_pathlen >= 0 )
        {
            MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_int( &c, buf, max_pathlen ) );
        }
        MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_bool( &c, buf, 1 ) );
    }

    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_len( &c, buf, len ) );
    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_tag( &c, buf, MBEDTLS_ASN1_CONSTRUCTED |
                                                       MBEDTLS_ASN1_SEQUENCE ) );

    return mbedtls_x509write_crt_set_extension( ctx, MBEDTLS_OID_BASIC_CONSTRAINTS,
                                                 MBEDTLS_OID_SIZE( MBEDTLS_OID_BASIC_CONSTRAINTS ),
                                                   critical, buf + sizeof(buf) - len, len );
}



static void FreeTestCert(mbedtls_x509_crt *cert)
{
    mbedtls_x509_crt_free(cert);
}

