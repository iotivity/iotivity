//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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

#ifndef _INTEL_OCUTILITIES_H_
#define _INTEL_OCUTILITIES_H_
#endif

#include <map>
#include <vector>
#include <memory>
#include <exception>

namespace OC {
    namespace Utilities {

        typedef std::map<std::string, std::string> QueryParamsKeyVal;
        /*
         * @brief Helper function to get query parameter from a URI
         * @remarks      Its okay to return a copy of the container.\
         *               The size is not expected to be huge.
         * @remarks      Temporary: The URI must strictly have\
         *               coap as the protocol in the fully qualified URI\
         *               e.g., coap://1.2.3.4:5657/foo?bar=0)
         * @remarks      If a separate class for URI parser is needed,\
         *               please talk to Erich Keane.
         * @todo         If more URI elements need to be parsed,\
         *               please move the common parsing logic to a
         *               different function
         */
        QueryParamsKeyVal getQueryParams(const std::string& uri);

    }
}
