//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

/**
 * @file
 *
 * This file contains caching API, it binds the caching call with report policy object.
 */

#ifndef REPORT_POLICY_PROXY_H_
#define REPORT_POLICY_PROXY_H_

#include<iostream>
#include<string>
#include<functional>

#include "CacheTypes.h"
#include "PrimitiveResource.h"

/**
 * @class   ReportPolicyProxy
 * @brief   This class provides method for initiating caching with given caching method.
 *         Caching methods are defined in REPORT_FREQUENCY.
 *
 * NOTE: REPORT_FREQUENCY is enum class having values as the following:
 *          NONE(default), UPDATE and PERIODIC.
*/
class ReportPolicyProxy
{
    public:

        /**
        *  Typedef to bind ProxyFunc to CacheCB method of PrimitiveResource.
        */
        typedef std::function<CacheID (std::shared_ptr<PrimitiveResource>  , CacheCB)> ProxyFunc;

        ReportPolicyProxy(ReportPolicyProxy &&reportPolicyProxy) = default;

        /**
         * Constructor for ReportPolicyProxy
         *
         * @param func - Caching method so as to bind ReportPolicyProxy object to
         *                      corresponding method of the PrimitiveResource class.
         *
         */

        ReportPolicyProxy(ProxyFunc func) : m_proxyFunc(func)
        {
        }

        /**
         * API for initiating caching on given resource and given caching method.
         *         Caching methods are defined in REPORT_FREQUENCY.
         *
         * @param res - resource to start caching for
         * @param cb - callback to obtain caching data
         *
         * @return CacheID
         */
        CacheID startProxyCaching(std::shared_ptr<PrimitiveResource> &res, CacheCB cb);

    private:

        /**
         *  proxy_binded binded to CacheCB method of PrimitiveResource.
         */
        ProxyFunc m_proxyFunc;
};
#endif //REPORT_POLICY_PROXY_H_