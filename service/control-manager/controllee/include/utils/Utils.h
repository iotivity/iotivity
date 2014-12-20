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
#include "DeviceDomain.h"


#define UTIL_SUCCESS 0
#define UTIL_FAILURE 1


bool parseUrl( const std::string &url, std::string &protocol, std::string &host, std::string &port, std::string &path, std::string &host_header_value ) ;
bool isValidAddress( OC::Cm::DeviceDomain deviceDomain, std::string address );
bool isValidIPAddress(std::string address) ;
bool isValidPort(std::string &port);

std::list<std::string> getStringTokens( std::string str, char delimiter);


bool isValidUri(std::string url);

std::string trimString(std::string input);
bool stringToUI32(const char *pStr, uint32_t &result);




#endif /* UTILS_H_ */
