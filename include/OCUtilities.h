//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

#include <map>
#include <vector>
#include <memory>
#include <utility>
#include <exception>

#include "OCException.h"

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

/* The C++11 standard unfortunately forgot to provide make_unique<>! However, if we're
using C++14 or later, we want to take the standard library's implementation: */
#if defined(__cplusplus) && __cplusplus < 201300
namespace OC {

    template<typename T, typename ...XS>
    std::unique_ptr<T> make_unique(XS&& ...xs)
    {
        return std::unique_ptr<T>(new T(std::forward<XS>(xs)...));
    }

} // namespace OC
#else
    using std::make_unique;
#endif

namespace OC {

    using OC::make_unique;

    /* Examine an OCStackResult, and either forward its value or raise an exception: */
    OCStackResult result_guard(const OCStackResult r);

    /* Check for a nullptr, and throw an exception if we see one; otherwise, return the
    result of the function call: */
    template <typename PtrT, typename FnT, typename ...ParamTs>
    auto nil_guard(PtrT&& p, FnT&& fn, ParamTs&& ...params) -> OCStackResult
    {
        if(nullptr == p)
        {
            throw OCException(OC::Exception::NIL_GUARD_NULL, OC_STACK_INVALID_PARAM);
        }

        // Note that although parameters are being forwarded, std::bind() will make a single copy:
        return std::bind(fn, p, std::forward<ParamTs>(params)...)();
    }

    /* Check for nullptr and forward the result of an OC function call on success; raise
    an exception on failure or exceptional result: */
    template <typename PtrT, typename FnT, typename ...ParamTs>
    auto checked_guard(PtrT&& p, FnT&& fn, ParamTs&& ...params) -> OCStackResult
    {
        return result_guard(nil_guard(p, fn, std::forward<ParamTs>(params)...));
    }

} // namespace OC

#endif
