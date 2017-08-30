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

#ifndef __HONEYWELLHELPERS_H__
#define __HONEYWELLHELPERS_H__

#include <string>
#include "mpmErrorCode.h"

// HELPER FUNCS

/// Loads a text file into a string.
///
/// @param filePath - (input) Path/filename to load.
/// @param fileContents - (output) Returned file contents.
///
/// @return MPM_RESULT_OK on success, error code otherwise.
MPMResult LoadFileIntoString(const std::string &filePath, std::string &fileContents);

/// Saves a string into a text file.
///
/// @param stringData - (input) String to save to file.
/// @param filePath - (input) Path/filename to load.
///
/// @return MPM_RESULT_OK on success, error code otherwise.
MPMResult SaveStringIntoFile(const std::string &stringData, const std::string &filePath);

/// Copies a file.
///
/// @param sourceFilePath - (input) source file/path to copy
/// @param destFilePath - (input) target file/path
/// @param binaryFile - (input) true if source is binary file, false if text file.
///
/// @return MPM_RESULT_OK on success, error code otherwise.
MPMResult CopyFile(const std::string &sourceFilePath, const std::string &destFilePath, bool binaryFile);

/// builds a path string to a given token file of given filename.
/// Uses the contents of the TOKEN_DIR environment variable as the
/// directory, if set, otherwise uses "/" (which is the "IntelCE/root" folder
/// on Ubuntu host in tftpboot scenario). If you don't specify a valid filename
/// this function returns the currently selected token directory.
///
/// @param fileName - (input) filename only
///
/// @return Full path/filename of token file.
std::string GetTokenPath(const std::string &fileName);

/// Calculates setpoints to achieve a desired target temperature. Honeywell thermostats don't have
/// a single target temp like Nest. Instead you need to set an upper and lower threshold using
/// setpoints. This computes a setpoint range around the desired target temperature.
///
/// @param targetTemp - (input) desired target temperature
/// @param heatSetpoint - (output) computed heat threshold
/// @param coolSetpoint - (output) computed cool threshold
void computeSetpoints(double targetTemp, double &heatSetpoint, double &coolSetpoint);

/// Calculates an average target temperature based on current theshold temperatures.
///
/// @param heatSetpoint - (input) current heat threshold
/// @param coolSetpoint - (input) current cool threshold
///
/// @return computed target temperature
double computeTargetTemp(double heatSetpoint, double coolSetpoint);

#endif // __HONEYWELLHELPERS_H__
