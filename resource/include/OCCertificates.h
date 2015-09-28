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

#ifndef _OCCERTIFICATES_CXX_H
#define _OCCERTIFICATES_CXX_H

#include <string>
#include <vector>
#include "ck_manager.h"

namespace OC
{
    class SKM
    {
        public:

            /**
             * Initializes CA storage from CA_STORAGE_FILE.
             *
             * @return PKI_SUCCESS if success, error code otherwise
             */
            static PKIError init(void);

            /**
             * Free CA storage memory.
             *
             * @return PKI_SUCCESS if success, error code otherwise
             */
            static PKIError destroy(void);

            /**
             * Set CA info and save it in the CA storage.
             *
             * Should be called before certificate generation
             *
             * @param[in] serNum certificate serial number to be set
             * @param[in] rootName CA common name to be set
             * @return PKI_SUCCESS if success, error code otherwise
             */
            static PKIError setCAInfo (const long serNum, std::string &rootName);

            /**
             * Generate key pair for CA.
             * Should be called before certificate generation
             *
             * @param[out] CAPrivateKey CA private key
             * @param[out] CAPublicKey  CA public key
             * @return PKI_SUCCESS if success, error code otherwise
             */
            static PKIError generateCAKeyPair (std::string &caPrivateKey, std::string &caPublicKey);

            /**
             * Generate key pair for ordinary device.
             * Should be called before certificate generation
             *
             * @param[out] privateKey private key
             * @param[out] publicKey  public key
             * @return PKI_SUCCESS if success, error code otherwise
             */
            static PKIError generateKeyPair (std::string &privateKey, std::string &publicKey);

            /**
             * Issues X.509 root certificate with specified parameters.
             *
             * setCAInfo and generateCAKeyPair should be called before.
             *
             * @param[in] notBefore pointer to string with certificate validity limit
             *                             or 0 to use default value
             * @param[in] notAfter pointer to string with certificate validity limit
             *                            or 0 to use default value
             * @param[out] issuedCertificate string for DER encoded certificate
             * @return PKI_SUCCESS if success, error code otherwise
             */

             static PKIError issueRootCertificate (std::string &notBefore,
                     std::string &notAfter, std::string &issuedRootCertificate);

             /**
              * Issues X.509 certificate with specified parameters.
              *
              * setCAInfo and generateCAKeyPair should be called before.
              *
              * @param[in] subjectName string with serial subject's common name
              *                               or empty string to use default value
              * @param[in] notBefore date from certificate is valid
              *                               or empty string to use default value
              * @param[in] notAfter date until certificate is valid
              *                               or empty string to use default value
              * @param[in] subjectPublicKey subject's public key to be signed
              * @param[out] issuedCertificate DER encoded certificate
              * @return PKI_SUCCESS if success, error code otherwise
              */
             static PKIError issueDeviceCertificate (std::string &subjectName,
                     std::string &notBefore, std::string &notAfter,
                     std::string &subjectPublicKey, std::string &issuedCertificate);

             /**
              * Generate certificate revocation list with specified parameters.
              *
              * @param[in] issuingCrlTime current time or empty string to use default value
              * @param[in] revokedNumbers array with numbers of revoked certificates
              * @param[in] revocationDates array with dates of revoked certificates
              * @param[out] encodedCRL DER encoded certificate revocation list
              * @return PKI_SUCCESS if success, error code otherwise
              */
             static PKIError issueCRL(std::string &issuingCrlTime,
                                   std::vector<int> &revokedNumbers,
                                   std::vector<std::string> &revocationDates,
                                   std::string &encodedCRL);

             /**
              * Add specified certificate into certificate revocation list.
              *
              * @param[in] issuingCrlTime current time in format YYMMDDHHMMSSZ
              *                               or empty string to use default value
              * @param[in] revokedNumber number of certificate to revoke
              * @param[out] encodedCRL DER encoded certificate revocation list
              * @return PKI_SUCCESS if success, error code otherwise
              */
             static PKIError revocateCertificate (std::string &issuingCrlTime,
                     const long revokedNumber, std::string &encodedCRL);

        private:

             /**
              * Convert String to ByteArray
              *
              * Does NOT allocate memory, uses string::c_str() pointer
              *
              * @param[in] in String to be converted
              * @param[out] out ByteArray to store converted string
              */
             static void convertStringToByteArray(std::string &in, ByteArray &out);

             /**
              * Convert ByteArray to String
              *
              * Allocate memory for new string using string::assign()
              *
              * @param[in] in ByteArray to be converted
              * @param[out] out String to store
              */
             static void convertByteArrayToString(ByteArray &in, std::string &out);

             /**
              * Convert String to c-style string
              *
              * Does NOT allocate memory, uses string::c_str() pointer
              *
              * @param[in] in String to be converted
              * @param[out] out c-style string
              */
             static void convertStringToCharArray(std::string &in, uint8_t **out);

    };

}
#endif //_OCCERTIFICATES_CXX_H
