//******************************************************************
//
// Copyright 2017 Intel Mobile Communications GmbH All Rights Reserved.
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
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//

#include <time.h>
#include <stdio.h>
#include <iostream>
#include "honeywell.h"
#include "curlClient.h"

#include "honeywellDefsLyric.h"
#include "oic_string.h"

#include "rapidjson.h"
#include "document.h"
#include "stringbuffer.h"
#include "writer.h"
#include "experimental/logger.h"
#include <math.h>  // for fmin
#include <sstream> // for stringstream
#include <map>
using namespace rapidjson;

#define LOG_TAG "HONEYWELL"

// EXTERNS
extern std::map<std::string, bool> g_deviceChangedMap;

// MEMBER FUNCTIONS

Honeywell::Honeywell(const ACCESS_TOKEN &accessToken, const CLIENT_ID_SECRET &clientIdSecret)
        : m_accessToken(accessToken), m_clientIdAndSecret(clientIdSecret), m_isAuthorized(false)
{
    // for curl support
    manageMutexes(true);
    m_getInProgress = false;
}

Honeywell::Honeywell() : m_isAuthorized(false)
{
    initializeAccessToken();
    initializeClientIdSecret();
    // for curl support
    manageMutexes(true);
    m_getInProgress = false;

    return;
}

Honeywell::~Honeywell()
{
    manageMutexes(false);
}

void Honeywell::initializeAccessToken()
{
        ::memset(&m_accessToken.accessToken, 0, HONEYWELL_ACCESS_TOKEN_BUFSIZE);
        ::memset(&m_accessToken.expires, 0, HONEYWELL_ACCESS_TOKEN_EXPIRY);
        m_accessToken.acquiredTime = 0;
        m_accessToken.grantTime = 0;
        m_accessToken.userId = 0;
}

void Honeywell::initializeClientIdSecret()
{
        ::memset(&m_clientIdAndSecret.honeywell_clientId, 0, HONEYWELL_CLIENT_ID_BUFFSIZE);
        ::memset(&m_clientIdAndSecret.honeywell_client_secret, 0, HONEYWELL_CLIENT_AND_SECRET_64_BUFFSIZE);
}

MPMResult Honeywell::manageMutexes(bool initialize)
{
    MPMResult result = MPM_RESULT_OK;
    m_accessTokenMutexInitialized = false;
    m_deviceChangeMutexInitialized = false;
    int intResult = 0;

    if (initialize)
    {
        // caller wants to init
        intResult = pthread_mutex_init(&m_cloudAccessMutex, NULL);
        if (0 != intResult)
        {
            OIC_LOG_V(ERROR, LOG_TAG, "pthread_mutex_init m_cloudAccessMutex failed with error %d", intResult);
            result = MPM_RESULT_INTERNAL_ERROR;
        }
        else
        {
            m_accessTokenMutexInitialized = true;
        }
    }
    else
    {
        // caller wants to destroy
        if (m_accessTokenMutexInitialized)
        {
            intResult = pthread_mutex_destroy(&m_cloudAccessMutex);
            if (0 != intResult)
            {
                OIC_LOG_V(ERROR, LOG_TAG, "pthread_mutex_destroy m_cloudAccessMutex failed with error %d", intResult);
                result = MPM_RESULT_INTERNAL_ERROR;
            }
            // always set to false
            m_accessTokenMutexInitialized = false;
        }
        // don't do anything if m_accessTokenMutexInitialized already false
    }

    return result;
}

void Honeywell::dumpResponseString(const std::string &stringData, const std::string &fileName)
{
    char logBuffer[MAX_LOG_STRING + 1];

    if (stringData.empty())
    {
        OIC_LOG_V(ERROR, LOG_TAG, "stringData is NULL or zero len");
        return;
    }

    OICStrcpy(logBuffer, sizeof(logBuffer), stringData.c_str());
    // display LENGTH of full response string, but only output the safe/truncated string
    if (!fileName.empty())
    {
        if (MPM_RESULT_OK != SaveStringIntoFile(stringData, fileName))
        {
            OIC_LOG_V(ERROR, LOG_TAG, "Error saving file %s", fileName.c_str());
        }
    }
}

void Honeywell::setAccessToken(const ACCESS_TOKEN &token)
{
    m_accessToken = token;
}

void Honeywell::setClientIdAndSecret(const CLIENT_ID_SECRET &clientIdAndSecret)
{
    m_clientIdAndSecret = clientIdAndSecret;
}

bool Honeywell::lockCloudAccess()
{
    bool mutexLocked = false;
    int intResult = pthread_mutex_lock(&m_cloudAccessMutex);
    if (0 != intResult)
    {
        OIC_LOG_V(ERROR, LOG_TAG, "pthread_mutex_lock failed with %d", intResult);
    }
    else
    {
        mutexLocked = true;
    }
    return mutexLocked;
}

bool Honeywell::unlockCloudAccess()
{
    bool success = true;
    int intResult = pthread_mutex_unlock(&m_cloudAccessMutex);
    if (0 != intResult)
    {
        OIC_LOG_V(ERROR, LOG_TAG, "pthread_mutex_unlock failed with %d", intResult);
        success = false;
    }
    return success;
}

int Honeywell::deauthorizateToken()
{
    int result = (int) MPM_RESULT_OK;
    initializeAccessToken();
    initializeClientIdSecret();
    m_isAuthorized = false;

    return result;
}

