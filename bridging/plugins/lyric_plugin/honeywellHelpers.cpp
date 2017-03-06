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

#include "honeywellHelpers.h"
#include "logger.h"
#include <stdlib.h> // realloc
#include <string.h> // memcpy
#include <sstream> // ostringstream
#include <fstream> // ifstream
#include "honeywellDefsLyric.h"

#include <stdio.h> // sprintf

#ifndef LOG_TAG
#define LOG_TAG "HONEYWELL_HELPERS"
#endif

MPMResult LoadFileIntoString(const char *filePath, std::string &fileContents)
{
    MPMResult result = MPM_RESULT_OK;
    if (NULL == filePath)
    {
        OIC_LOG(ERROR, LOG_TAG, "filePath is NULL.");
        result = MPM_RESULT_INVALID_PARAMETER;
    }

    if (MPM_RESULT_OK == result)
    {
        try
        {
            std::ostringstream buffer;
            std::ifstream inputFile(filePath);
            if (!inputFile)
            {
                OIC_LOG_V(ERROR, LOG_TAG, "Couldn't open file %s", filePath);
                result = MPM_RESULT_FILE_NOT_OPEN;
            }
            else
            {
                buffer << inputFile.rdbuf();
                fileContents = buffer.str();
                OIC_LOG_V(INFO, LOG_TAG, "Read %lu bytes from file", (unsigned long) fileContents.size());
            }
        }
        catch (...)
        {
            OIC_LOG(ERROR, LOG_TAG, "caught exception.");
            result = MPM_RESULT_INTERNAL_ERROR;
        }
    }

    return result;
}

MPMResult SaveStringIntoFile(const char *stringData, const char *filePath)
{
    MPMResult result = MPM_RESULT_OK;

    if ((NULL == stringData) || (NULL == filePath))
    {
        OIC_LOG(ERROR, LOG_TAG, "stringData or filePath are NULL");
        result = MPM_RESULT_INVALID_PARAMETER;
        goto cleanUp;
    }

    try
    {
        std::ofstream outFile(filePath, std::ofstream::out);
        if (!outFile)
        {
            OIC_LOG_V(ERROR, LOG_TAG, "Failed to open file %s for output", filePath);
            result = MPM_RESULT_FILE_NOT_OPEN;
            goto cleanUp;
        }
        outFile << stringData;
    }
    catch (...)
    {
        OIC_LOG(ERROR, LOG_TAG, "Caught exception.");
        result = MPM_RESULT_INTERNAL_ERROR;
        goto cleanUp;
    }

    cleanUp:

    return result;
}

MPMResult CopyFile(const char *sourceFilePath, const char *destFilePath, bool binaryFile)
{
    MPMResult result = MPM_RESULT_OK;

    std::ofstream::openmode outMode = std::ofstream::out;
    std::ifstream::openmode inMode = std::ifstream::in;

    if (binaryFile)
    {
        outMode |= std::ofstream::binary;
        inMode |= std::ifstream::binary;
    }

    if ((NULL == sourceFilePath) || (NULL == destFilePath))
    {
        OIC_LOG(ERROR, LOG_TAG, "sourceFilePath or destFilePath are NULL");
        result = MPM_RESULT_INVALID_PARAMETER;
        goto cleanUp;
    }

    try
    {
        std::ofstream outFile(destFilePath, outMode);
        if (!outFile)
        {
            OIC_LOG_V(ERROR, LOG_TAG, "Failed to open file %s for output", destFilePath);
            result = MPM_RESULT_FILE_NOT_OPEN;
            goto cleanUp;
        }

        std::ifstream inFile(sourceFilePath, inMode);
        if (!inFile)
        {
            OIC_LOG_V(ERROR, LOG_TAG, "Failed to open file %s for input", sourceFilePath);
            result = MPM_RESULT_FILE_NOT_OPEN;
            goto cleanUp;
        }

        outFile << inFile.rdbuf();
    }
    catch (...)
    {
        OIC_LOG(ERROR, LOG_TAG, "Caught exception.");
        result = MPM_RESULT_INTERNAL_ERROR;
        goto cleanUp;
    }

    cleanUp:

    return result;
}

std::string GetTokenPath(const char *fileName)
{
    char *tokenPathVar = NULL;
    std::string tokenPath = "/";
    size_t pathLen = 0;

    tokenPathVar = getenv("TOKEN_DIR");
    if (NULL != tokenPathVar)
    {
        // replace default tokenPath with environment variable contents
        tokenPath = tokenPathVar;
    }

    if (NULL != fileName)
    {
        pathLen = tokenPath.length();
        if (0 != pathLen)
        {
            // path has at least one char; make sure it ends with a slash.
            if (tokenPath.at(pathLen - 1) != '/')
            {
                tokenPath += "/";
            }
        }

        // filename should not begin with a slash
        if (fileName[0] == '/')
        {
            fileName++;
        }
        // append passed filename
        tokenPath += fileName;
    }

    OIC_LOG_V(INFO, LOG_TAG, "Token file path: %s", tokenPath.c_str());

    return tokenPath;
}

void computeSetpoints(double targetTemp, double &heatSetpoint, double &coolSetpoint)
{
    // this creates a temperature range around a desired target temperature. Lyric requires
    // at least a 3 degree difference between hot and cool setpoints.
    heatSetpoint = targetTemp - HONEYWELL_SETPOINT_BUFFER;
    coolSetpoint = targetTemp + HONEYWELL_SETPOINT_BUFFER;
    return;
}

double computeTargetTemp(double heatSetpoint, double coolSetpoint)
{
    // we divine a target temperature from honeywell's setpoints by averaging the hot and cool
    // setpoints. the target temperature is right between them.
    return ((heatSetpoint + coolSetpoint) / 2);
}

