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

#include "JniOcCertificates.h"
#include "JniOcStack.h"
#include "OCCertificates.h"

using namespace OC;

void convertJStringToStdString(JNIEnv *env, jstring in, std::string &out)
{
    if (!in)
    {
        out.clear();
        return;
    }

    const char *s = env->GetStringUTFChars(in,NULL);
    out = s;
    env->ReleaseStringUTFChars(in,s);
}

void convertListLongsToStdVectorInt(JNIEnv *env, jobject in, std::vector<int> &out)
{
    if (!in)
    {
        out.clear();
        return;
    }

    jboolean *isCopy = NULL;
    jint      mode   = 0;

    jlongArray array = static_cast<jlongArray>(env->CallObjectMethod(in, g_mid_List_toArray));

    jlong *body = env->GetLongArrayElements(array, isCopy);
    jsize len   = env->GetArrayLength(array);

    for (int i = 0; i < len; i++)
    {
        jlong value = body[i];
        out.push_back(static_cast<int>(value));
    }
    env->ReleaseLongArrayElements(array, body, mode);
}

void convertListStringsToStdVectorString(JNIEnv *env, jobject in, std::vector<std::string> &out)
{
    if (!in)
    {
        out.clear();
        return;
    }

    jobjectArray array = static_cast<jobjectArray>(env->CallObjectMethod(in, g_mid_List_toArray));

    for (int i = 0; i< env->GetArrayLength(array); i++)
    {
        std::string str;
        jstring jstr = static_cast<jstring>(env->GetObjectArrayElement(array, i));

        convertJStringToStdString(env, jstr, str);
        out.push_back(str);
    }
}

void convertListBytesToStdString(JNIEnv *env, jobject in, std::string &out)
{
    if (!in)
    {
        out.clear();
        return;
    }

    jobjectArray array = static_cast<jobjectArray>(env->CallObjectMethod(in, g_mid_List_toArray));

    int len = env->GetArrayLength(array);

    out.resize(len);

    for (int i = 0; i < len; i++)
    {
        jobject obj = env->GetObjectArrayElement(array, i);

        jbyte b = env->CallByteMethod(obj, g_mid_Byte_byteValue);

        out[i] = (char)b;
    }
}

jobject convertStdStringToListBytes(JNIEnv *env, std::string &in)
{
    jobject ret = env->NewObject(g_cls_List, g_mid_List_ctor);

    for (int i = 0; i < in.size(); i++)
    {
        jobject data = env->NewObject(g_cls_Byte, g_mid_Byte_ctor, (jbyte)in[i]);

        env->CallVoidMethod(ret, g_mid_List_add_object, data);
    }
}

/*
 * Class:     org_iotivity_base_OcCertificates
 * Method:    init
 * Signature: ()I
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcCertificates_init
  (JNIEnv *env, jclass clazz)
{
    LOGI("OcCertificates_Init");
    PKIError result = PKI_UNKNOWN_ERROR;

    try
    {
        result = SKM::init();

        if (PKI_SUCCESS != result)
        {
            ThrowOcException(result, "Failed to Init CKManager");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
 * Class:     org_iotivity_base_OcCertificates
 * Method:    destroy
 * Signature: ()I
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcCertificates_destroy
  (JNIEnv *env, jclass clazz)
{
    LOGI("OcCertificates_Destroy");
    PKIError result = PKI_UNKNOWN_ERROR;

    try
    {
        result = SKM::destroy();

        if (PKI_SUCCESS != result)
        {
            ThrowOcException(result, "Failed to Destroy CKManager");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
 * Class:     org_iotivity_base_OcCertificates
 * Method:    setCaInfo
 * Signature: (JLjava/lang/String;)I
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcCertificates_setCaInfo
  (JNIEnv *env, jclass clazz, jlong serial, jstring rootName)
{
    LOGI("OcCertificates_SetCaInfo");
    PKIError result = PKI_UNKNOWN_ERROR;

    try
    {
        if (!rootName)
        {
            ThrowOcException(OC_STACK_INVALID_PARAM, "rootName cannot be null");
            return;
        }

        std::string name;

        convertJStringToStdString(env, rootName, name);

        result = SKM::setCAInfo(serial, name);

        if (PKI_SUCCESS != result)
        {
            ThrowOcException(result, "Failed to set CKManager's CA Info");
            return;
        }
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
 * Class:     org_iotivity_base_OcCertificates
 * Method:    generateCaKeyPair
 * Signature: (Ljava/util/List;Ljava/util/List;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcCertificates_generateCaKeyPair
  (JNIEnv *env, jclass clazz, jobject caPrivateKey, jobject caPublicKey)
{
    LOGI("OcCertificates_generateCaKeyPair");
    PKIError result = PKI_UNKNOWN_ERROR;

    try
    {
        if (!caPrivateKey || !caPublicKey)
        {
            ThrowOcException(OC_STACK_INVALID_PARAM, "caPrivateKey/caPublicKey cannot be null");
            return;
        }

        std::string privateKey;
        std::string publicKey;

        result = SKM::generateCAKeyPair(privateKey, publicKey);

        if (PKI_SUCCESS != result)
        {
            ThrowOcException(result, "Failed to generate CA key pair");
            return;
        }

        caPublicKey  = convertStdStringToListBytes(env, publicKey );
        caPrivateKey = convertStdStringToListBytes(env, privateKey);
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
 * Class:     org_iotivity_base_OcCertificates
 * Method:    generateKeyPair
 * Signature: (Ljava/util/List;Ljava/util/List;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcCertificates_generateKeyPair
  (JNIEnv *env, jclass clazz, jobject privateKey, jobject publicKey)
{
    LOGI("OcCertificates_generateKeyPair");
    PKIError result = PKI_UNKNOWN_ERROR;

    try
    {
        if (!privateKey || !publicKey)
        {
            ThrowOcException(OC_STACK_INVALID_PARAM, "privateKey/publicKey cannot be null");
            return;
        }

        std::string _privateKey;
        std::string _publicKey;

        result = SKM::generateKeyPair(_privateKey, _publicKey);

        if (PKI_SUCCESS != result)
        {
            ThrowOcException(result, "Failed to generate key pair");
            return;
        }

        publicKey  = convertStdStringToListBytes(env, _publicKey );
        privateKey = convertStdStringToListBytes(env, _privateKey);
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
 * Class:     org_iotivity_base_OcCertificates
 * Method:    issueRootCertificate
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/util/List;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcCertificates_issueRootCertificate
  (JNIEnv *env, jclass clazz, jstring dateBefore, jstring dateAfter, jobject certificate)
{
    LOGI("OcCertificates_issueRootCertificate");
    PKIError result = PKI_UNKNOWN_ERROR;

    try
    {
        if (!certificate)
        {
            ThrowOcException(OC_STACK_INVALID_PARAM, "certificate cannot be null");
            return;
        }

        std::string before;
        std::string after;
        std::string cert;

        convertJStringToStdString(env, dateBefore, before);
        convertJStringToStdString(env, dateAfter,  after );

        result = SKM::issueRootCertificate(before, after, cert);

        if (PKI_SUCCESS != result)
        {
            ThrowOcException(result, "Failed to issue root certificate");
            return;
        }

        certificate = convertStdStringToListBytes(env, cert);
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
 * Class:     org_iotivity_base_OcCertificates
 * Method:    issueDeviceCertificate
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/util/List;Ljava/util/List;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcCertificates_issueDeviceCertificate
  (JNIEnv *env, jclass clazz, jstring subjectName, jstring dateBefore, jstring dateAfter,
   jobject subjectPublicKey, jobject certificate)
{
    LOGI("OcCertificates_issueDeviceCertificate");
    PKIError result = PKI_UNKNOWN_ERROR;

    try
    {
        if (!subjectPublicKey || !certificate)
        {
            ThrowOcException(OC_STACK_INVALID_PARAM, "subjectPublicKey/certificate can't be null");
            return;
        }

        std::string name;
        std::string before;
        std::string after;
        std::string publicKey;
        std::string cert;

        convertJStringToStdString(env, subjectName,  name);
        convertJStringToStdString(env, dateBefore, before);
        convertJStringToStdString(env, dateAfter,  after );
        convertListBytesToStdString(env, subjectPublicKey, publicKey);

        result = SKM::issueDeviceCertificate(name, before, after, publicKey, cert);

        if (PKI_SUCCESS != result)
        {
            ThrowOcException(result, "Failed to issue Device certificate");
            return;
        }

        certificate = convertStdStringToListBytes(env, cert);
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
 * Class:     org_iotivity_base_OcCertificates
 * Method:    issueCRL
 * Signature: (Ljava/lang/String;Ljava/util/List;Ljava/util/List;Ljava/util/List;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcCertificates_issueCrl
  (JNIEnv *env, jclass clazz, jstring currentTime, jobject numbers, jobject dates,
          jobject crl)
{
    LOGI("OcCertificates_issueCRL");
    PKIError result = PKI_UNKNOWN_ERROR;

    try
    {
        if (!crl)
        {
            ThrowOcException(OC_STACK_INVALID_PARAM, "crl cannot be null");
            return;
        }

        if (!numbers || !dates)
        {
            ThrowOcException(OC_STACK_INVALID_PARAM, "numbers/dates cannot be null");
            return;
        }

        std::string time;
        std::string _crl;
        std::vector<int> _numbers;
        std::vector<std::string> _dates;

        convertJStringToStdString(env, currentTime, time);
        convertListLongsToStdVectorInt(env, numbers, _numbers);
        convertListStringsToStdVectorString(env, dates, _dates);

        if (_numbers.size() != _dates.size())
        {
            ThrowOcException(OC_STACK_INVALID_PARAM, "numbers & dates should have same length");
            return;
        }

        result = SKM::issueCRL(time, _numbers, _dates, _crl);

        if (PKI_SUCCESS != result)
        {
            ThrowOcException(result, "Failed to issue crl");
            return;
        }

        crl = convertStdStringToListBytes(env, _crl);
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}

/*
 * Class:     org_iotivity_base_OcCertificates
 * Method:    revocateCertificate
 * Signature: (Ljava/lang/String;JLjava/util/List;)V
 */
JNIEXPORT void JNICALL Java_org_iotivity_base_OcCertificates_revocateCertificate
  (JNIEnv *env, jclass clazz, jstring currentTime, jlong revokedNumber, jobject crl)
{
    LOGI("OcCertificates_revocateCertificate");
    PKIError result = PKI_UNKNOWN_ERROR;

    try
    {
        if (!crl)
        {
            ThrowOcException(OC_STACK_INVALID_PARAM, "crl cannot be null");
            return;
        }

        std::string time;
        std::string _crl;

        convertJStringToStdString(env, currentTime, time);

        result = SKM::revocateCertificate(time, revokedNumber, _crl);

        if (PKI_SUCCESS != result)
        {
            ThrowOcException(result, "Failed to revocate certificate");
            return;
        }

        crl = convertStdStringToListBytes(env, _crl);
    }
    catch (OCException& e)
    {
        LOGE("%s", e.reason().c_str());
        ThrowOcException(OC_STACK_ERROR, e.reason().c_str());
    }
}
