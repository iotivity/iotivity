//******************************************************************
// Copyright 2018 Cable Television Laboratories, Inc.
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
// See the License for the specific lan guage governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include <string.h>
#include <time.h>

#include <mbedtls/error.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/oid.h>

#include "cacommonutil.h"
#include "cacertprofile.h"
#include "experimental/logger.h"

#define TAG "OIC_CC_CERT_PROFILE"

#if defined(__WITH_DTLS__) || defined (__WITH_TLS__)
// OCF Compliant ID Cert profiles
static const mbedtls_x509_crt_profile s_certProfile = {
    MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA256),        // MD algorithms
    MBEDTLS_X509_ID_FLAG(MBEDTLS_PK_ECKEY) |        // Allowed key types
    MBEDTLS_X509_ID_FLAG(MBEDTLS_PK_ECDSA),
    MBEDTLS_X509_ID_FLAG(MBEDTLS_ECP_DP_SECP256R1), // EC curves
    0                                               // RSA minimum key length - not used because we only use EC key pairs
};

// OID for ID certificates (1.3.6.1.4.1.44924.1.6) suitable for mbedTLS check
static const char s_ekuIdOid[] = MBEDTLS_OID_ISO_IDENTIFIED_ORG "\x06\x01\x04\x01\x82\xDE\x7C\x01\x06";


static const unsigned int s_eeKeyUsage = MBEDTLS_X509_KU_DIGITAL_SIGNATURE |
                                         MBEDTLS_X509_KU_KEY_AGREEMENT;

static const unsigned int s_eeNonKeyUsage = MBEDTLS_X509_KU_NON_REPUDIATION |
                                            MBEDTLS_X509_KU_DATA_ENCIPHERMENT |
                                            MBEDTLS_X509_KU_KEY_ENCIPHERMENT |
                                            MBEDTLS_X509_KU_KEY_CERT_SIGN |
                                            MBEDTLS_X509_KU_CRL_SIGN |
                                            MBEDTLS_X509_KU_ENCIPHER_ONLY |
                                            MBEDTLS_X509_KU_DECIPHER_ONLY;

static const unsigned int s_caKeyUsage = MBEDTLS_X509_KU_KEY_CERT_SIGN |
                                         MBEDTLS_X509_KU_CRL_SIGN;

static const unsigned int s_caNonKeyUsage = MBEDTLS_X509_KU_NON_REPUDIATION  |
                                            MBEDTLS_X509_KU_KEY_ENCIPHERMENT |
                                            MBEDTLS_X509_KU_DATA_ENCIPHERMENT |
                                            MBEDTLS_X509_KU_KEY_AGREEMENT |
                                            MBEDTLS_X509_KU_ENCIPHER_ONLY |
                                            MBEDTLS_X509_KU_DECIPHER_ONLY;

static CertProfileResult FindEndEntityCert(const mbedtls_x509_crt *certChain, mbedtls_x509_crt const **eeCert)
{
    *eeCert = NULL;

    const mbedtls_x509_crt* curCert = certChain;
    while (NULL != curCert)
    {
        if (0 == curCert->ca_istrue)
        {
            // first EE
            if (NULL == *eeCert)
            {
                *eeCert = curCert;
            }
            // more than 1 EE is an error condition
            else
            {
                *eeCert = NULL;
                OIC_LOG(ERROR, TAG, "More than 1 end entity cert in chain");
                return CP_MUL_EE_CERTS;
            }
        }
        curCert = curCert->next;
    }

    if (NULL == *eeCert)
    {
        OIC_LOG(WARNING, TAG, "No end entity cert in chain");
        return CP_NO_EE_CERT;
    }

    return CP_STATUS_OK;
}

static CertProfileResult FindRootCACert(const mbedtls_x509_crt *certChain, mbedtls_x509_crt const **caCert)
{
    *caCert = NULL;

    const mbedtls_x509_crt* curCert = certChain;
    while (NULL != curCert)
    {
        if ((1 == curCert->ca_istrue) &&
            (curCert->issuer_raw.len == curCert->subject_raw.len) &&
            (0 == memcmp(curCert->issuer_raw.p, curCert->subject_raw.p, curCert->issuer_raw.len)))
        {
            // first CA
            if (NULL == *caCert)
            {
                *caCert = curCert;
            }
            // more than 1 CA
            else
            {
                *caCert = NULL;
                OIC_LOG(ERROR, TAG, "More than 1 root CA cert in chain");
                return CP_MUL_CA_CERTS;
            }
        }
        curCert = curCert->next;
    }

    if (NULL == *caCert)
    {
        OIC_LOG(INFO, TAG, "No root CA cert in chain");
        return CP_NO_CA_CERT;
    }

    return CP_STATUS_OK;
}

static CertProfileResult CheckMdAlgorithm(const mbedtls_x509_crt_profile *profile, mbedtls_md_type_t mdAlgorithm)
{
    if ((MBEDTLS_X509_ID_FLAG(mdAlgorithm) & profile->allowed_mds) != 0)
    {
        return CP_STATUS_OK;
    }
    return CP_STATUS_FAILED;
}

static CertProfileResult CheckPubKeyAlgorithm(const mbedtls_x509_crt_profile *profile, mbedtls_pk_type_t pkAlgorithm)
{
    if ((MBEDTLS_X509_ID_FLAG(pkAlgorithm) & profile->allowed_pks) != 0)
    {
        return CP_STATUS_OK;
    }
    return CP_STATUS_FAILED;
}


// returns CP_STATUS_OK if pathlen is valid for cert type, othewise returns CP_STATUS_FAILED
static CertProfileResult CheckPathLen( CertType certType, int mbedMaxPathLen) {

    // mbedtls max_pathlen behaviour:
    // CA Cert: Expects RFC5280_val as encoding input
    //          Provides RFC5280_val+1 as decoding output, where 0 = not present
    // EE Cert: Does not encode
    //          Provides 0 as decoding output

    switch(certType)
    {
        case CERT_CA_ROOT:
            return (0 == mbedMaxPathLen) ? CP_STATUS_OK : CP_STATUS_FAILED;
        case CERT_CA_INT:
            return (1 == mbedMaxPathLen) ? CP_STATUS_OK : CP_STATUS_FAILED;
        case CERT_EE:
            return (0 == mbedMaxPathLen) ? CP_STATUS_OK : CP_STATUS_FAILED;
        default:
        {
            OIC_LOG(WARNING, TAG, "Unkown cert type for pathlen check");
            return CP_STATUS_FAILED;
        }
    }
}

// Check all cert entries that are common across root-ca, intermediate-ca, and ee certs
static CertProfileViolations ValidateCommonCertProfileEntries(const mbedtls_x509_crt *cert) {

    CertProfileResult cpResult = CP_STATUS_OK;
    CertProfileViolations profileViolations = CP_NO_VIOLATIONS;

    // Start with time windows
    // notBefore: Required
    // notAfter: Required
    profileViolations = ValidateCertTimeWindow(cert);
    if (CP_NO_VIOLATIONS != profileViolations)
    {
        OIC_LOG(ERROR, TAG, "Cert has expired or is not yet valid");
    }

    // signatureAlgorithm
    // ecdsa-with-SHA256 (OID: 1.2.840.10045.4.3.2)
    cpResult = CheckMdAlgorithm(&s_certProfile, cert->sig_md);
    if (CP_STATUS_OK != cpResult)
    {
        OIC_LOG(ERROR, TAG, "Cert signature algorithm must be SHA256");
        profileViolations |= CP_INVALID_SIG_ALGORITHM;
    }

    // Version: v3
    // mbedTLS version 3 = x509 v3
    if (3 != cert->version)
    {
        OIC_LOG(ERROR, TAG, "Cert is not x509 v3");
        profileViolations |= CP_INVALID_VERSION;
    }

    // Subject Public Key Info
    // id-ecPublicKey (OID: 1.2.840.10045.2.1) secp256r1 (OID:1.2.840.10045.3.1.7)
    cpResult = CheckPubKeyAlgorithm(&s_certProfile, cert->sig_pk);
    if (CP_STATUS_OK != cpResult)
    {
        OIC_LOG(ERROR, TAG, "Cert public key algorithm must be ECDSA");
        profileViolations |= CP_INVALID_PUB_KEY_ALGORITHM;
    }

    // SerialNumber: SHALL be a positive integer, unique among all certificates issued by Root CA
    // Not possible to validate SN uniqueness

    // Issuer: SHALL match the Subject field of the issuing Root CA
    // mbedTLS will check proper chaining during DTLS handshake

    return profileViolations;
}


CertProfileViolations ValidateEndEntityCertProfile(const mbedtls_x509_crt *cert)
{

    // OCF requirements exist for the following extensions, but w/o mbedTLS support
    // * check for certificate policies, if present must be 1.3.6.1.4.1.51414.0.1.2
    // * cRL Distribution Points

    if (NULL == cert)
    {
        return CP_INVALID_CERT_INPUT;
    }

    int mbedRet = 0;
    CertProfileResult cpResult = CP_STATUS_OK;
    CertProfileViolations profileViolations = CP_NO_VIOLATIONS;

    // Check all common entries first
    profileViolations = ValidateCommonCertProfileEntries(cert);

    // keyUsage (REQUIRED/Critical)
    // digitalSignature (0) and keyAgreement(4) bits SHALL be the only bits enabled
    mbedRet = mbedtls_x509_crt_check_key_usage(cert, s_eeKeyUsage);
    if (0 != mbedRet)
    {
        OIC_LOG(ERROR, TAG, "End entity cert key usage must include digitalSignature & keyAgreement");
        profileViolations |= CP_INVALID_KEY_USAGE_MISSING;
    }

    if (0 != (cert->key_usage & s_eeNonKeyUsage))
    {
        OIC_LOG(ERROR, TAG, "End entity cert key usage must not include usages other than digitalSignature & keyAgreement");
        profileViolations |= CP_INVALID_KEY_USAGE_EXTRA;
    }

    // basicConstraints (OPTIONAL/Critical)
    // cA = FALSE
    if (1 == cert->ca_istrue)
    {
        OIC_LOG(ERROR, TAG, "End entity cert marked as CA cert");
        profileViolations |= CP_INVALID_BASIC_CONSTRAINTS_CA;
    }

    // pathLenConstraint: not present
    cpResult = CheckPathLen(CERT_EE, cert->max_pathlen);
    if (CP_STATUS_OK != cpResult)
    {
        OIC_LOG(ERROR, TAG, "Invalid End entity max pathlen");
        profileViolations |= CP_INVALID_BASIC_CONSTRAINTS_PATH_LEN;
    }

    // extendedKeyUsage (REQUIRED/Non-critical)
    // Mandatory: serverAuthentication - 1.3.6.1.5.5.7.3.1
    mbedRet = mbedtls_x509_crt_check_extended_key_usage(cert,
                                                        MBEDTLS_OID_SERVER_AUTH,
                                                        MBEDTLS_OID_SIZE(MBEDTLS_OID_SERVER_AUTH));
    if (0 != mbedRet)
    {
        OIC_LOG(ERROR, TAG, "End entity cert extended key usage must include serverAuthentication");
        profileViolations |= CP_INVALID_EKU_NO_SERVER_AUTH;
    }

    // extendedKeyUsage (REQUIRED/Non-critical)
    // Mandatory: clientAuthentication - 1.3.6.1.5.5.7.3.2
    mbedRet = mbedtls_x509_crt_check_extended_key_usage(cert,
                                                        MBEDTLS_OID_CLIENT_AUTH,
                                                        MBEDTLS_OID_SIZE(MBEDTLS_OID_CLIENT_AUTH));
    if (0 != mbedRet)
    {
        OIC_LOG(ERROR, TAG, "End entity cert extended key usage must include clientAuthentication");
        profileViolations  |= CP_INVALID_EKU_NO_CLIENT_AUTH;
    }

    // extendedKeyUsage (REQUIRED/Non-critical)
    // Mandatory: OCF Identity certificate - 1.3.6.1.4.1.44924.1.6
    mbedRet = mbedtls_x509_crt_check_extended_key_usage(cert, s_ekuIdOid, MBEDTLS_OID_SIZE(s_ekuIdOid));
    if (0 != mbedRet)
    {
        OIC_LOG(ERROR, TAG, "End entity cert extended key usage must include OCF ID OID (1.3.6.1.4.1.44924.1.6");
        profileViolations |= CP_INVALID_EKU_NO_OCF_ID_OID;
    }

    // extendedKeyUsage (REQUIRED/Non-critical)
    // CAs SHALL NOT issue certificates with the anyExtendedKeyUsage OID (2.5.29.37.0)
    mbedRet = mbedtls_x509_crt_check_extended_key_usage(cert,
                                                        MBEDTLS_OID_ANY_EXTENDED_KEY_USAGE,
                                                        MBEDTLS_OID_SIZE(MBEDTLS_OID_ANY_EXTENDED_KEY_USAGE));
    if (0 == mbedRet)
    {
        OIC_LOG(ERROR, TAG, "End entity cert extended key usage must not include anyExtendedKeyUsage");
        profileViolations |= CP_INVALID_EKU_INCLUDES_ANY;
    }

    // subjectAlternativeName: No requirements for ID certs

    return profileViolations;
}

CertProfileViolations ValidateIntermediateCACertProfile(const mbedtls_x509_crt *cert)
{

    // OCF requirements exist for the following extensions, but w/o mbedTLS support
    // * cRL Distribution Points

    if (NULL == cert)
    {
        return CP_INVALID_CERT_INPUT;
    }

    int mbedRet = 0;
    CertProfileResult cpResult = CP_STATUS_OK;
    CertProfileViolations profileViolations = CP_NO_VIOLATIONS;

    // Check all common entries first
    profileViolations = ValidateCommonCertProfileEntries(cert);

    // keyUsage: REQUIRED/Critical
    // keyCertSign (5) & cRLSign (6) bits SHALL be the only bits enabled
    mbedRet = mbedtls_x509_crt_check_key_usage(cert, s_caKeyUsage);
    if (0 != mbedRet)
    {
        OIC_LOG(ERROR, TAG, "Intermediate CA cert key usage must include keyCertSign & cRLSign");
        profileViolations |= CP_INVALID_KEY_USAGE_MISSING;
    }

    if (0 != (cert->key_usage & s_caNonKeyUsage))
    {
        OIC_LOG(ERROR, TAG, "Intermediate CA cert key usage must not include usages other than keyCertSign & cRLSign");
        profileViolations |= CP_INVALID_KEY_USAGE_EXTRA;
    }

    // basicConstraints REQUIRED/Critical
    // cA = TRUE
    if (1 != cert->ca_istrue)
    {
        OIC_LOG(ERROR, TAG, "Intermediate CA cert not marked as CA cert");
        profileViolations |= CP_INVALID_BASIC_CONSTRAINTS_CA;
    }

    // pathLenConstraint
    // 0 - can only sign end-entity certs
    cpResult = CheckPathLen(CERT_CA_INT, cert->max_pathlen);
    if (CP_STATUS_OK != cpResult)
    {
        OIC_LOG(ERROR, TAG, "Invalid Intermediate CA max pathlen");
        profileViolations |= CP_INVALID_BASIC_CONSTRAINTS_PATH_LEN;
    }
    return profileViolations;
}

CertProfileViolations ValidateRootCACertProfile(const mbedtls_x509_crt *cert)
{
    if (NULL == cert)
    {
        return CP_INVALID_CERT_INPUT;
    }

    int mbedRet = 0;
    CertProfileResult cpResult = CP_STATUS_OK;
    CertProfileViolations profileViolations = CP_NO_VIOLATIONS;

    // Check all common entries first
    profileViolations = ValidateCommonCertProfileEntries(cert);

    // Issuer: SHALL match the Subject field
    // Subject: SHALL match the Issuer field
    if ( (cert->issuer_raw.len != cert->subject_raw.len) ||
         (0 != memcmp(cert->issuer_raw.p, cert->subject_raw.p, cert->issuer_raw.len)))
    {
        OIC_LOG(ERROR, TAG, "Root CA cert subject must be the same as issuer");
        profileViolations |= CP_INVALID_ISSUER_SUBJ_MISMATCH;

    }

    // keyUsage: REQUIRED/Critical
    // keyCertSign (5) & cRLSign (6) bits SHALL be the only bits enabled
    mbedRet = mbedtls_x509_crt_check_key_usage(cert, s_caKeyUsage);
    if (0 != mbedRet)
    {
        OIC_LOG(ERROR, TAG, "Root CA cert key usage must include keyCertSign & cRLSign");
        profileViolations |= CP_INVALID_KEY_USAGE_MISSING;
    }
    if (0 != (cert->key_usage & s_caNonKeyUsage))
    {
        OIC_LOG(ERROR, TAG, "Root CA cert key usage must not include usages other than keyCertSign & cRLSign");
        profileViolations |= CP_INVALID_KEY_USAGE_EXTRA;
    }

    // basicConstraints REQUIRED Critical cA = TRUE
    if (1 != cert->ca_istrue)
    {
        OIC_LOG(ERROR, TAG, "Root CA cert not marked as CA cert");
        profileViolations |= CP_INVALID_BASIC_CONSTRAINTS_CA;
    }

    // pathLenConstraint = not present (unlimited)
    cpResult = CheckPathLen(CERT_CA_ROOT, cert->max_pathlen);
    if (CP_STATUS_OK != cpResult)
    {
        OIC_LOG(ERROR, TAG, "Invalid Root CA max pathlen");
        profileViolations |= CP_INVALID_BASIC_CONSTRAINTS_PATH_LEN;
    }

    return profileViolations;
}

int ValidateAuthCertChainProfiles(const mbedtls_x509_crt *certChain)
{
    int numViolations = 0;
    CertProfileResult cpResult = CP_STATUS_OK;
    CertProfileViolations profileViolations = CP_NO_VIOLATIONS;

    const mbedtls_x509_crt* eeCert = NULL;
    cpResult = FindEndEntityCert(certChain, &eeCert);
    if ((CP_STATUS_OK != cpResult) || (NULL == eeCert))
    {
        return CP_INVALID_CERT_CHAIN;
    }

    const mbedtls_x509_crt* caCert = NULL;
    cpResult = FindRootCACert(certChain, &caCert);

    const mbedtls_x509_crt* curCert = certChain;
    while (NULL != curCert)
    {
        if (curCert == eeCert)
        {
            profileViolations = ValidateEndEntityCertProfile(curCert);
        }
        else if (curCert == caCert)
        {
            profileViolations = ValidateRootCACertProfile(curCert);
        }
        else
        {
            profileViolations = ValidateIntermediateCACertProfile(curCert);
        }
        if (CP_NO_VIOLATIONS != profileViolations)
        {
           numViolations++;
        }
        curCert = curCert->next;
    }
    return numViolations;
}

int ValidateRootCACertListProfiles(const mbedtls_x509_crt *certList)
{
    int numViolations = 0;
    CertProfileViolations profileViolations = CP_NO_VIOLATIONS;

    const mbedtls_x509_crt* curCert = certList;
    while (NULL != curCert)
    {
        profileViolations = ValidateRootCACertProfile(curCert);
        if (CP_NO_VIOLATIONS != profileViolations)
        {
           numViolations++;
        }
        curCert = curCert->next;
    }
    return numViolations;
}

CertProfileViolations ValidateCertTimeWindow(const mbedtls_x509_crt *cert)
{
     CertProfileViolations profileViolations = CP_NO_VIOLATIONS;

    if (mbedtls_x509_time_is_future(&cert->valid_from))
    {
        OIC_LOG(ERROR, TAG, "Certificate is not yet valid");
        profileViolations |= CP_NOT_YET_VALID;
    }
    if (mbedtls_x509_time_is_past(&cert->valid_to))
    {
        OIC_LOG(ERROR, TAG, "Certificate is no longer valid");
        profileViolations |= CP_EXPIRED;
    }
    return profileViolations;
}

int CheckCertListTimeWindows(const mbedtls_x509_crt *certList)
{
    int numInvalid = 0;
    CertProfileViolations profileViolations = CP_NO_VIOLATIONS;

    const mbedtls_x509_crt* curCert = certList;
    while (NULL != curCert)
    {
        profileViolations = ValidateCertTimeWindow(curCert);
        if (CP_NO_VIOLATIONS != profileViolations)
        {
            numInvalid++;
        }
        curCert = curCert->next;
    }
    return numInvalid;
}

#endif // (__WITH_DTLS__) || defined (__WITH_TLS__)
