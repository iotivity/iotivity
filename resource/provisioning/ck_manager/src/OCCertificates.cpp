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

#include "OCCertificates.h"
#include "OCPlatform_impl.h"

namespace OC
{
    void SKM::convertStringToByteArray(std::string &in, ByteArray &out)
    {
        out.data = (uint8_t*)in.c_str();
        out.len = in.length();
    }

    void SKM::convertByteArrayToString(ByteArray &in, std::string &out)
    {
        out.assign((char*)in.data, in.len);
    }

    void SKM::convertStringToCharArray(std::string &in, uint8_t **out)
    {
        *out = (uint8_t*)in.c_str();
    }

    PKIError SKM::init()
    {
        PKIError result = PKI_UNKNOWN_ERROR;

        auto cLock = OCPlatform_impl::Instance().csdkLock().lock();

        if (cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);

            result = InitCKMInfo();
        }

        return result;
    }

    PKIError SKM::destroy()
    {
        PKIError result = PKI_UNKNOWN_ERROR;

        auto cLock = OCPlatform_impl::Instance().csdkLock().lock();

        if (cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);

            result = CloseCKMInfo();
        }

        return result;
    }

    PKIError SKM::setCAInfo (const long serNum, std::string &_rootName)
    {
        PKIError  result = PKI_UNKNOWN_ERROR;
        ByteArray rootName;

        auto cLock = OCPlatform_impl::Instance().csdkLock().lock();

        if (cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);

            convertStringToByteArray(_rootName, rootName);
            result = CKMSetCAInfo(serNum, rootName);
        }

        return result;
    }

    PKIError SKM::generateCAKeyPair (std::string &caPrivateKey, std::string &caPublicKey)
    {
        PKIError  result = PKI_UNKNOWN_ERROR;
        ByteArray privateKey = {0, 0};
        ByteArray publicKey  = {0, 0};

        auto cLock = OCPlatform_impl::Instance().csdkLock().lock();

        if (cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);

            try
            {
                std::unique_ptr<uint8_t[]> ptr1(new uint8_t[PUBLIC_KEY_SIZE]);
                std::unique_ptr<uint8_t[]> ptr2(new uint8_t[PRIVATE_KEY_SIZE]);

                publicKey.data  = ptr1.get();
                privateKey.data = ptr2.get();

                result = GenerateCAKeyPair(&privateKey, &publicKey);

                if (PKI_SUCCESS == result)
                {
                    convertByteArrayToString(privateKey, caPrivateKey);
                    convertByteArrayToString(publicKey, caPublicKey);
                }
            }
            catch (std::bad_alloc)
            {
                result = PKI_MEMORY_ALLOC_FAILED;
            }
        }

        return result;
    }

    PKIError SKM::generateKeyPair (std::string &_privateKey, std::string &_publicKey)
    {
        PKIError  result = PKI_UNKNOWN_ERROR;
        ByteArray privateKey = {0, 0};
        ByteArray publicKey  = {0, 0};

        auto cLock = OCPlatform_impl::Instance().csdkLock().lock();

        if (cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);

            try
            {
                std::unique_ptr<uint8_t[]> ptr1(new uint8_t[PUBLIC_KEY_SIZE]);
                std::unique_ptr<uint8_t[]> ptr2(new uint8_t[PRIVATE_KEY_SIZE]);

                publicKey.data  = ptr1.get();
                privateKey.data = ptr2.get();

                result = GenerateCAKeyPair(&privateKey, &publicKey);

                if (PKI_SUCCESS == result)
                {
                    convertByteArrayToString(privateKey, _privateKey);
                    convertByteArrayToString(publicKey, _publicKey);
                }
            }
            catch (std::bad_alloc)
            {
                result = PKI_MEMORY_ALLOC_FAILED;
            }
        }

        return result;
    }

    PKIError SKM::issueRootCertificate (std::string &notBefore,
                         std::string &notAfter, std::string &issuedRootCertificate)
    {
        PKIError result = PKI_UNKNOWN_ERROR;
        ByteArray cert;
        uint8_t *dateBefore = NULL;
        uint8_t *dateAfter  = NULL;

        auto cLock = OCPlatform_impl::Instance().csdkLock().lock();

        if (cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);

            try
            {
                cert.data = new uint8_t[ISSUER_MAX_CERT_SIZE];
                cert.len  = ISSUER_MAX_CERT_SIZE;
            }
            catch (std::bad_alloc)
            {
                return PKI_MEMORY_ALLOC_FAILED;
            }

            if (!notBefore.empty())
            {
                convertStringToCharArray(notBefore, &dateBefore);
            }

            if (!notAfter.empty())
            {
                convertStringToCharArray(notAfter, &dateAfter);
            }

            result = CKMIssueRootCertificate(dateBefore, dateAfter, &cert);

            if (PKI_SUCCESS == result)
            {
                convertByteArrayToString(cert, issuedRootCertificate);
            }

            delete[] cert.data;
        }

        return result;
    }

    PKIError SKM::issueDeviceCertificate (std::string &subjectName,
                         std::string &notBefore, std::string &notAfter,
                         std::string &subjectPublicKey, std::string &issuedCertificate)
    {
        PKIError result = PKI_UNKNOWN_ERROR;
        ByteArray cert;
        uint8_t *subject    = NULL;
        uint8_t *dateBefore = NULL;
        uint8_t *dateAfter  = NULL;
        uint8_t *publicKey  = NULL;

        auto cLock = OCPlatform_impl::Instance().csdkLock().lock();

        if (cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);

            try
            {
                cert.data = new uint8_t[ISSUER_MAX_CERT_SIZE];
                cert.len  = ISSUER_MAX_CERT_SIZE;
            }
            catch (std::bad_alloc)
            {
                return PKI_MEMORY_ALLOC_FAILED;
            }

            convertStringToCharArray(subjectPublicKey, &publicKey);

            if (!notBefore.empty())
            {
                convertStringToCharArray(notBefore, &dateBefore);
            }

            if (!notAfter.empty())
            {
                convertStringToCharArray(notAfter, &dateAfter);
            }

            if (!subjectName.empty())
            {
                convertStringToCharArray(subjectName, &subject);
            }

            result = CKMIssueDeviceCertificate(subject, dateBefore, dateAfter, publicKey, &cert);

            if (PKI_SUCCESS == result)
            {
                convertByteArrayToString(cert, issuedCertificate);
            }

            delete[] cert.data;
        }

        return result;
    }

    PKIError SKM::issueCRL(std::string &issuingCrlTime,
                           std::vector<int> &revokedNumbers,
                           std::vector<std::string> &revocationDates,
                           std::string &encodedCRL)
    {
        PKIError result = PKI_UNKNOWN_ERROR;
        ByteArray crl;
        uint8_t *time     = NULL;
        uint32_t count    = (uint32_t) revokedNumbers.size();
        uint32_t *numbers = (uint32_t*)revokedNumbers.data();
        uint8_t **dates   = NULL;

        auto cLock = OCPlatform_impl::Instance().csdkLock().lock();

        if (cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);

            if (!issuingCrlTime.empty())
            {
                convertStringToCharArray(issuingCrlTime, &time);
            }

            crl.len = CRL_MIN_SIZE + count * (sizeof(CertificateRevocationInfo_t) + 4);

            try
            {
                std::unique_ptr<uint8_t[]> ptr1(new uint8_t[crl.len]);
                std::unique_ptr<uint8_t*[]> ptr2(new uint8_t*[count]);

                crl.data = ptr1.get();
                dates    = ptr2.get();

                for (uint32_t i = 0; i < count; i++)
                {
                    dates[i] = (uint8_t*)revocationDates[i].c_str();
                }

                result = CKMIssueCRL(time, count, numbers, (const uint8_t**)dates, &crl);

                if (PKI_SUCCESS == result)
                {
                    convertByteArrayToString(crl, encodedCRL);
                }
            }
            catch (std::bad_alloc)
            {
                return PKI_MEMORY_ALLOC_FAILED;
            }
        }

        return result;
    }

    PKIError SKM::revocateCertificate (std::string &issuingCrlTime,
                         const long revokedNumber, std::string &encodedCRL)
    {
        PKIError result = PKI_UNKNOWN_ERROR;
        ByteArray crl;
        uint8_t *time = NULL;
        long cnt = 0;

        auto cLock = OCPlatform_impl::Instance().csdkLock().lock();

        if (cLock)
        {
            std::lock_guard<std::recursive_mutex> lock(*cLock);

            result = GetNumberOfRevoked(&cnt);
            if (result != PKI_SUCCESS)
            {
                return result;
            }

            long size = CRL_MIN_SIZE + (cnt + 1) * (sizeof(CertificateRevocationInfo_t) + 4);

            try
            {
                crl.data = new uint8_t[size];
                crl.len  = size;
            }
            catch (std::bad_alloc)
            {
                return PKI_MEMORY_ALLOC_FAILED;
            }

            if (!issuingCrlTime.empty())
            {
                convertStringToCharArray(issuingCrlTime, &time);
            }

            result = CKMRevocateCertificate(time, revokedNumber, &crl);

            if (PKI_SUCCESS == result)
            {
                convertByteArrayToString(crl, encodedCRL);
            }

            delete[] crl.data;
        }

        return result;
    }
}
