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



#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <string.h>
#include "oic_string.h"
#include "mpmErrorCode.h"
#include "hue_file.h"
#include "cJSON.h"
#include <pthread.h>
#include <map>
#include "experimental/logger.h"

#define TAG "HUE_FILE"

/**
 * Parse the authorized bridge array
 * @param[in] fileBuffer
 * @param[in] fileBufferSize
 * @return true if parsed else false
 */
static bool parseAuthorizedBridgeArray(char *fileBuffer, uint32_t fileBufferSize);

/**
 * Parse each authorized bridge to get client and Mac address
 * @param [in] object
 * @return true if parsed else false
 */
static bool parseAuthorizedBridge(cJSON *object);

std::map<std::string, std::string> file_map;

bool readAuthorizedBridgeFile()
{
    bool parsedOk = false;
    long lSize;
    char *buffer = NULL;
    size_t fileResult;
    FILE *pFile = NULL;

    pFile = fopen(HUE_AUTHORIZATION_FILE, "r");
    if (pFile == NULL)
    {
        OIC_LOG_V(INFO, TAG, "File %s not present",
                  HUE_AUTHORIZATION_FILE );
    }
    else
    {
        OIC_LOG_V(INFO, TAG, "Reading auth file @  %s",
                  HUE_AUTHORIZATION_FILE);
        // obtain file size:
        fseek(pFile, 0, SEEK_END);
        lSize = ftell(pFile);
        rewind(pFile);

        // allocate memory to contain the whole file:
        buffer = (char *) malloc(sizeof(char) * (lSize + 1));
        if ((buffer != NULL) && (lSize > 1))
        {
            // copy the file into the buffer:
            fileResult = fread(buffer, 1, lSize, pFile);
            if (fileResult == (size_t) lSize)
            {
                buffer[lSize] = '\0';
                OIC_LOG_V(INFO, TAG, "Auth file contents = \n%s\n", buffer);
                parsedOk = parseAuthorizedBridgeArray(buffer, lSize);
            }
        }
    }
    if (NULL != buffer)
    {
        free(buffer);
    }
    if (NULL != pFile)
    {
        fclose(pFile);
    }
    return (parsedOk);
}

bool findAuthorizedBridge(const char *macAddrString, const char *clientID, hueFile &bridgeCtx)
{
    if ((macAddrString != NULL) && (clientID == NULL))
    {
        if (file_map.find(macAddrString) != file_map.end())
        {
            std::string clientid = file_map[macAddrString];
            OICStrcpy(bridgeCtx.clientID, MAX_STRING - 1, clientid.c_str());
            return true;
        }
    }
    else if ((macAddrString == NULL) && (clientID != NULL))
    {
        if (file_map.find(clientID) != file_map.end())
        {
            std::string macAddress = file_map[clientID];
            OICStrcpy(bridgeCtx.macAddrString, MAX_STRING - 1, macAddress.c_str());
            return true;
        }
    }
    else if ((macAddrString != NULL) && (clientID != NULL))
    {
        OICStrcpy(bridgeCtx.macAddrString, MAX_STRING - 1, macAddrString);
        OICStrcpy(bridgeCtx.clientID, MAX_STRING - 1, clientID);
        return true;
    }
    else
    {
        OIC_LOG(ERROR, TAG, "Both mac and client id is NULL");
        return false;
    }
    OIC_LOG(ERROR, TAG, "Bridge is not Authorized...........");
    return false;
}

static bool parseAuthorizedBridgeArray(char *fileBuffer, uint32_t fileBufferSize)
{
    bool parsedOk = false;
    cJSON *object = NULL;
    cJSON *array = NULL;
    int32_t numBridges = 0;
    int32_t index;

    if ((fileBuffer != NULL) && (fileBufferSize > 0))
    {
        array = cJSON_Parse((char *) fileBuffer);
        if (array != NULL)
        {
            numBridges = cJSON_GetArraySize(array);
            parsedOk = true;
            for (index = 0; index < numBridges; index++)
            {
                object = cJSON_GetArrayItem(array, index);
                parsedOk = parseAuthorizedBridge(object);
                if (parsedOk == false)
                {
                    OIC_LOG(ERROR, TAG, "Parsing one of the bridge lines in file failed");
                }
            }
            cJSON_Delete(array);
        }
        else
        {
            OIC_LOG(ERROR, TAG, "array returned from call to cJSON_Parse is NULL.");
        }
    }
    return (parsedOk);
}

bool addAuthorizedBridge(const char *mac, const char *clientId)
{

    if (mac == NULL || clientId == NULL)
    {
        OIC_LOG(ERROR, TAG, "Failed to add the bridge Details to the authorization list");
        return false;
    }
    file_map[mac] = clientId;

    return true;
}

static bool parseAuthorizedBridge(cJSON *object)
{
    char *macAddrString = NULL;
    char *clientID = NULL;

    if (object != NULL)
    {
        if (cJSON_GetObjectItem(object, "id") != NULL)
        {
            macAddrString = cJSON_GetObjectItem(object, "id")->valuestring;
        }
        if (cJSON_GetObjectItem(object, "username") != NULL)
        {
            clientID = cJSON_GetObjectItem(object, "username")->valuestring;
        }

        addAuthorizedBridge(macAddrString, clientID);

    }
    return (true);
}

bool collectAuthorizedClients(const char *macAddrString, char *clientArray, uint32_t *numClients)
{
    if (macAddrString == NULL)
    {
        OIC_LOG_V(ERROR, TAG, "Mac id is NULL");
        return false;
    }
    for (std::map<std::string, std::string>::iterator  itr = file_map.begin(); itr != file_map.end();
         ++itr)
    {
        if (strcmp(macAddrString, (*itr).first.c_str()) == 0)
        {
            std::string clientID = (*itr).second;
            if (!clientID.empty())
            {
                OICStrcpy(clientArray, MAX_STRING, clientID.c_str());
            }
            *numClients = 1;
            return true;
        }
    }
    return (false);
}

void clearBridgeDetails()
{
    file_map.clear();
}

