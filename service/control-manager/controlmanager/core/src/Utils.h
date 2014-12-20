//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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



#ifndef UTILS_H_
#define UTILS_H_



#include "string"
#include "list"
#include <map>
#include "CMDataTypes.h"
#include "CMConfig.h"
#include "DeviceDomain.h"
#include "OCPlatform.h"
#include "CMErrorCode.h"

#define UTIL_SUCCESS 0
#define UTIL_FAILURE 1
#define OC_OTHER_ERROR -1
//#ifdef __cplusplus
//extern "C" {
//#endif

CMAPI bool parseUrl( const std::string &url, std::string &protocol, std::string &host,
                     std::string &port, std::string &path, std::string &host_header_value ) ;
CMAPI bool isValidAddress( OC::Cm::DeviceDomain deviceDomain, std::string address );
CMAPI bool isValidIPAddress(std::string address) ;
CMAPI bool isValidPort(std::string &port);
CMAPI std::string getIPAddress (std::string deviceAddress);
CMAPI bool getIPAddrAndPort (std::string deviceAddress, std::string &ipAddr, std::string &port);
CMAPI std::list<std::string> getStringTokens( std::string str, char delimiter);

CMAPI std::string get_next_token(std::string &firstLine, CM_STR_SIZE &start_index,
                                 CM_STR_SIZE &end_index, char delimit, bool trimSpaces, bool forceEnd = false);
CMAPI bool strToBufferSize(const char *pStr, CM_BUFFER_SIZE &result);
CMAPI int generateSessionId();
CMAPI bool isValidUri(std::string url);

CMAPI bool isValidEmailAddress(std::string email);

CMAPI std::string trimString(std::string input);

// OIC utility to map OC and Control Manager Error codes
CMAPI int getCMErrorCodeFromOCResult (OCStackResult result);
// OIC utility to map OC error codes to HTTP error codes to be passed to application
CMAPI int getHTTPErrorCodeFromOCResult (int OCErrorCode);

//#ifdef __cplusplus
//}
//#endif

#endif /* UTILS_H_ */
