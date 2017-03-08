/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/

#ifndef PMCppCertHelper_H_
#define PMCppCertHelper_H_

#include "PMCppUtilityHelper.h"

class PMCppCertHelper
{
private:
    std::string m_failureMessage;

public:
    static OCPersistentStorage s_ps;
    static int s_cbInvoked;

    static ByteArray_t s_trustCertChainArray;
    static uint16_t s_credId;

    PMCppCertHelper();

    static FILE* clientOpen(const char *UNUSED_PARAM, const char *mode);

    bool provisionInit(const std::string& dbPath = CLIENT_DATABASE);

    bool saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize,
            OicEncodingType_t encodingType, uint16_t *credId, OCStackResult expectedResult);

    bool readTrustCertChain(uint16_t credId, uint8_t **trustCertChain, size_t *chainSize, OCStackResult expectedResult);

    bool registerTrustCertChangeNotifier(CertChainCallBack Callback, OCStackResult expectedResult);

    bool removeTrustCertChangeNotifier();

    bool provisionTrustCertChain(DeviceList_t& deviceList, OicSecCredType_t type, uint16_t credId,
            ResultCallBack resultCallback, OCStackResult expectedResult);

    static void provisionCB(PMResultList_t *result, int hasError);

    static void certChainCB(uint16_t credId, uint8_t *trustCertChain, size_t chainSize);

    std::string getFailureMessage();

    int waitCallbackRet();

};

#endif
