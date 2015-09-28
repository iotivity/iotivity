/* *****************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include <OCCertificates.h>
#include <ocstack.h>
#include <gtest/gtest.h>

namespace OCCertificatesTest
{
    using namespace OC;

    const char *JSON_DB_PATH = "CKMInfo.json";

    static FILE* client_open(const char * /*path*/, const char *mode)
    {
        return fopen(JSON_DB_PATH, mode);
    }

    OCPersistentStorage ps {client_open, fread, fwrite, fclose, unlink };

    TEST(init, TestValidPath)
    {
        ASSERT_EQ(OC_STACK_OK, OCRegisterPersistentStorageHandler(&ps));

        EXPECT_EQ(PKI_SUCCESS, SKM::init());
    }

    TEST(setCAInfo, TestValidPath)
    {
        std::string rootName("ca");
        long serial = 100;

        EXPECT_EQ(PKI_SUCCESS, SKM::setCAInfo(serial, rootName));
    }

    TEST(generateKeyPair, TestValidPath)
    {
        std::string privateKey;
        std::string publicKey;

        EXPECT_EQ(PKI_SUCCESS, SKM::generateKeyPair(privateKey, publicKey));
    }

    TEST(generateCAKeyPair, TestValidPath)
    {
        std::string privateKey;
        std::string publicKey;

        EXPECT_EQ(PKI_SUCCESS, SKM::generateCAKeyPair(privateKey, publicKey));
    }

    TEST(issueRootCertificate, TestValidPath)
    {
        std::string rootName("ca");
        long serial = 100;

        EXPECT_EQ(PKI_SUCCESS, SKM::setCAInfo(serial, rootName));

        std::string privateKey;
        std::string publicKey;

        EXPECT_EQ(PKI_SUCCESS, SKM::generateCAKeyPair(privateKey, publicKey));

        std::string notBefore("");
        std::string notAfter("");
        std::string issuedRootCertificate;

        EXPECT_EQ(PKI_SUCCESS, SKM::issueRootCertificate(notBefore, notAfter, issuedRootCertificate));

        std::string notBefore2("140101000001Z");
        std::string notAfter2("170101000001Z");
        std::string issuedRootCertificate2;

        EXPECT_EQ(PKI_SUCCESS, SKM::issueRootCertificate(notBefore2, notAfter2, issuedRootCertificate2));
    }

    TEST(issueDeviceCertificate, TestValidPath)
    {
        std::string rootName("ca");
        long serial = 100;

        EXPECT_EQ(PKI_SUCCESS, SKM::setCAInfo(serial, rootName));

        std::string caPrivateKey;
        std::string caPublicKey;

        EXPECT_EQ(PKI_SUCCESS, SKM::generateCAKeyPair(caPrivateKey, caPublicKey));

        std::string privateKey;
        std::string publicKey;

        EXPECT_EQ(PKI_SUCCESS, SKM::generateKeyPair(privateKey, publicKey));

        std::string subjectName("");
        std::string notBefore("");
        std::string notAfter("");
        std::string deviceCertificate;

        EXPECT_EQ(PKI_SUCCESS, SKM::issueDeviceCertificate(subjectName, notBefore, notAfter, publicKey, deviceCertificate));

        std::string subjectName2("Test");
        std::string notBefore2("140101000001Z");
        std::string notAfter2("170101000001Z");
        std::string deviceCertificate2;

        EXPECT_EQ(PKI_SUCCESS, SKM::issueDeviceCertificate(subjectName2, notBefore2, notAfter2, publicKey, deviceCertificate2));
    }

    TEST(issueCRL, TestValidPath)
    {
        std::string time("");
        std::vector<int> numbers;
        std::vector<std::string> dates;
        std::string crl;

        numbers.push_back(100);
        numbers.push_back(200);

        dates.push_back(std::string("130101000001Z"));
        dates.push_back(std::string("130101000002Z"));

        EXPECT_EQ(PKI_SUCCESS, SKM::issueCRL(time, numbers, dates, crl));
    }

    TEST(revocateCertificate, TestValidPath)
    {
        std::string currentTime("130101000000Z");
        long revokedNumber = 1;

        std::string encodedCRL;

        EXPECT_EQ(PKI_SUCCESS, SKM::revocateCertificate(currentTime, revokedNumber, encodedCRL));
    }

    TEST(destroy, TestValidPath)
    {
        EXPECT_EQ(PKI_SUCCESS, SKM::destroy());
    }
}
