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
#include "experimental/logger.h"
#include <stdlib.h> // realloc
#include <string.h> // memcpy
#include <sstream> // ostringstream
#include <fstream> // ifstream
#include "honeywellDefsLyric.h"

#include <stdio.h> // sprintf

#ifndef LOG_TAG
#define LOG_TAG "HONEYWELL_HELPERS"
#endif

MPMResult LoadFileIntoString(const std::string &filePath, std::string &fileContents)
{
    MPMResult result = MPM_RESULT_OK;
    if (filePath.empty())
    {
        OIC_LOG(ERROR, LOG_TAG, "filePath is empty.");
        return MPM_RESULT_INVALID_PARAMETER;
    }

    try
    {
        std::ostringstream buffer;
        std::ifstream inputFile(filePath.c_str());
        if (!inputFile)
        {
            OIC_LOG_V(ERROR, LOG_TAG, "Couldn't open file %s", filePath.c_str());
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

    return result;
}

MPMResult SaveStringIntoFile(const std::string &stringData, const std::string &filePath)
{
    MPMResult result = MPM_RESULT_OK;

    if ((stringData.empty()) || (filePath.empty()))
    {
        OIC_LOG(ERROR, LOG_TAG, "stringData or filePath is empty");
        result = MPM_RESULT_INVALID_PARAMETER;
        goto cleanUp;
    }

    try
    {
        std::ofstream outFile(filePath.c_str(), std::ofstream::out);
        if (!outFile)
        {
            OIC_LOG_V(ERROR, LOG_TAG, "Failed to open file %s for output", filePath.c_str());
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

MPMResult CopyFile(const std::string &sourceFilePath, const std::string &destFilePath, bool binaryFile)
{
    MPMResult result = MPM_RESULT_OK;

    std::ofstream::openmode outMode = std::ofstream::out;
    std::ifstream::openmode inMode = std::ifstream::in;

    if (binaryFile)
    {
        outMode |= std::ofstream::binary;
        inMode |= std::ifstream::binary;
    }

    if ((sourceFilePath.empty()) || (destFilePath.empty()))
    {
        OIC_LOG(ERROR, LOG_TAG, "sourceFilePath or destFilePath is empty");
        result = MPM_RESULT_INVALID_PARAMETER;
        goto cleanUp;
    }

    try
    {
        std::ofstream outFile(destFilePath.c_str(), outMode);
        if (!outFile)
        {
            OIC_LOG_V(ERROR, LOG_TAG, "Failed to open file %s for output", destFilePath.c_str());
            result = MPM_RESULT_FILE_NOT_OPEN;
            goto cleanUp;
        }

        std::ifstream inFile(sourceFilePath.c_str(), inMode);
        if (!inFile)
        {
            OIC_LOG_V(ERROR, LOG_TAG, "Failed to open file %s for input", sourceFilePath.c_str());
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

std::string GetTokenPath(const std::string &fileName)
{
    std::string tokenPath = "./";

    if (!fileName.empty())
    {
        // filename should not begin with a slash
        if (fileName.at(0) == '/')
        {
            tokenPath += fileName.substr(1);
        }
        else
        {
            tokenPath += fileName;
        }
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

