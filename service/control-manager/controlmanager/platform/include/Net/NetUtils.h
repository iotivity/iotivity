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



#ifndef NETUTILS_H_
#define NETUTILS_H_

#include "CMPALConfig.h"
#include "CMDataTypes.h"

CMPAL_API uint32_t pal_ntoh32(uint32_t);

CMPAL_API uint32_t pal_hton32(uint32_t);

CMPAL_API bool pal_validateIPAddress( std::string ipAddress );

CMPAL_API bool pal_validateEmail(const std::string &email);

#endif /* NETUTILS_H_ */
