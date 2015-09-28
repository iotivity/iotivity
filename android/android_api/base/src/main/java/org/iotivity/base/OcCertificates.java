/*
 * //******************************************************************
 * //
 * // Copyright 2015 Samsung Electronics All Rights Reserved.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base;

import java.util.List;

/**
 * OcCertificates represents functions corresponding to the CKManager
 * resources.
 */
public class OcCertificates {

    /**
     * Method to intialize certificates provisioning.
     *
     * @throws OcException
     */
    public static native void init() throws OcException;

    /**
     * Method to destroy certificates provisioning.
     *
     * @throws OcException
     */
    public static native void destroy() throws OcException;

    /**
     * Method sets CA info and save it in the CA storage.
     * Should be called before certificate generation
     *
     * @param[in] serNum serial number
     * @param[in] rootName root name
     * @throws OcException
     */
    public static native void setCaInfo (long serNum, String rootName) throws OcException;

    /**
     * Method generates key pair for CA.
     * Should be called before certificate generation
     *
     * @param[out] caPrivateKey CA private key
     * @param[out] caPublicKey  CA public key
     * @throws OcException
     */
    public static native void generateCaKeyPair (List<Byte> caPrivateKey, List<Byte> caPublicKey) throws OcException;

    /**
     * Method generates key pair for ordinary device.
     * Should be called before certificate generation
     *
     * @param[out] privateKey CA private key
     * @param[out] publicKey  CA public key
     * @throws OcException
     */
    public static native void generateKeyPair (List<Byte> privateKey, List<Byte> publicKey) throws OcException;

    /**
     * Method issues X.509 root certificate with specified parameters.
     * setCaInfo and generateCaKeyPair should be called before.
     *
     * @param[in] notBefore date from which certificate will be valid (empty string means default value)
     * @param[in] notAfter  date till which certificate will be valid (empty string means default value)
     * @param[out] certificate generated certificate
     * @throws OcException
     */
    public static native void issueRootCertificate (String notBefore, String notAfter,
           List<Byte> certificate) throws OcException;

    /**
     * Method issues X.509 certificate with specified parameters.
     * setCaInfo and generateKeyPair should be called before.
     *
     * @param[in] subjectName subject name (empty string means default value)
     * @param[in] notBefore date from which certificate will be valid (empty string means default value)
     * @param[in] notAfter  date till which certificate will be valid (empty string means default value)
     * @param[in] subjectPublicKey subject's public key
     * @param[out] certificate generated certificate
     * @throws OcException
     */
    public static native void issueDeviceCertificate (String subject, String notBefore, String notAfter,
           List<Byte> subjectPublicKey, List<Byte> certificate) throws OcException;

    /**
     * Method generates certificate revocation list with specified parameters.
     *
     * @param[in] time current time or empty string to use default value
     * @param[in] numbers array with numbers of revoked certificates
     * @param[in] dates array with dates of revoked certificates
     * @param[out] crl DER encoded certificate revocation list
     * @throws OcException
     */
    public static native void issueCrl(String time, List<Long> numbers, List<String> dates, List<Byte> crl)
                  throws OcException;

    /**
     * Method adds specified certificate into certificate revocation list.
     *
     * @param[in] currentTime current time in format YYMMDDHHMMSSZ (empty string means default value)
     * @param[in] revokedNumber certificate to revoke
     * @param[out] crl encoded crl
     * @throws OcException
     */
    public static native void revocateCertificate (String currentTime, long revokedNumber,
           List<Byte> crl) throws OcException;
}
