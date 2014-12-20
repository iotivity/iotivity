/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include <string>

#include "http_header/http_response_header.h"
#include "http_start_line/http_request_line.h"
#include "http_start_line/http_status_code_defined.h"
#include "http_header.h"
#include "web_cache/web_cache_manager.h"
#include "web_cache/web_cache_wrapper_functions.h"
#include "base/logging.h"

namespace webservice
{

    /**
       * Process web cach
       * @param[in] request_uri : request uri which is defined to string
       * @param[in] etag
       * @param[in] p_is_etag_present
       * @param[in] p_is_uri_present_in_cache
       * @return bool
       */
    bool WebCacheWrapperFunctions::ProcessWebCache(const HTTPRequestLine &request_line,
            const std::string &etag,
            bool *p_is_etag_present, bool *p_is_uri_present_in_cache)
    {
#if defined(ENABLE_WEB_CACHE)
        if (NULL == p_is_etag_present || NULL == p_is_uri_present_in_cache)
        {
            LOG(ERROR) <<
                       "Allocation fail for input parameter(p_is_etag_present or p_is_uri_present_in_cache).";
            return false;
        }

        // Web Cache is allowed only for GET method
        if (web_util::HTTP_METHOD_GET != request_line.GetHTTPMethod()) return false;

        // Get request received, check the cache
        if (0 == etag.size()) return false;

        // eTag is present, validate it with the cache entry
        *p_is_etag_present = true;
        if (ValidateETagField(request_line.GetOriginUri(), etag, p_is_uri_present_in_cache)) return true;
#endif /* ENABLE_WEB_CACHE */

        // ETAG mismatch.. Send the request to SLEP
        return false;
    }

    /**
       * This is to check "ETag" existence in request information.
       * @param[in] request_uri : request uri which is defined to string
       * @param[in] etag : etag string
       * @param[in] is_uri_present_in_cache
       * @return bool
       */
    bool WebCacheWrapperFunctions::ValidateETagField(const std::string &request_uri,
            const std::string &etag, bool *is_uri_present_in_cache)
    {
#if defined(ENABLE_WEB_CACHE)
        WebCacheManager *web_cache_inst = WebCacheManager::GetInstance();
        if (web_cache_inst)
        {
            if (web_cache_inst->CheckIfURIandETagExists(request_uri, etag, is_uri_present_in_cache))
            {
                DLOG(INFO) << "URI and ETag matches";
                return true;
            }
        }
#endif /* ENABLE_WEB_CACHE */
        return false;
    }

    /**
       * This is to update cache
       * @param[in] request_uri : request uri which is defined to string
       * @param[in] request_method : request method for checking "Get" method, web cache is valid only for "Get" method.
       * @param[in] is_etag_present
       * @param[in] is_uri_present_in_cache
       * @param[in] response_code : http status code
       * @param[out] response_header : to set new ETag
       * @return bool
       */
    void WebCacheWrapperFunctions::UpdateCache(const std::string &request_uri, const int request_method,
            bool is_etag_present, bool is_uri_present_in_cache, HTTPResponseType response_code,
            HTTPResponseHeader &response_header)
    {
#if defined(ENABLE_WEB_CACHE)
        // When the response code for the GET URI is between kHttpStatusCodeSuccessOk and kHttpStatusCodeRedirectSpecialResponse then,
        // 1. Cache will be updated with the latest information (New entries could be added)
        // 2. ETag header will be added in the response
        // cache update required only incase of GET URI's
        std::string eTag = "";
        WebCacheManager *web_cache_inst = WebCacheManager::GetInstance();
        if (NULL == web_cache_inst)
        {
            return;
        }
        if (web_util::HTTP_METHOD_GET != request_method || (response_code < RT_200_START
                || response_code >= RT_300_START))
        {
            // Delete request for a particular Uri is received, so all the information related to this Uri should be deleted from cache.
            // For Ex. When DELETE /devices/{device-id} request is received, all the data related to '/devices/{device-id}' Uri should be deleted from cache.
            if (web_util::HTTP_METHOD_DELETE == request_method)
            {
                LOG(INFO) << "Deleting URI from Cache";
                if (!web_cache_inst->DeleteUriFromCache(request_uri))
                {
                    LOG(INFO) << "Deleting URI from cache failed";
                }
            }
            return;
        }
        DLOG(INFO) << "is_etag_present :: " << is_etag_present << " is_uri_present_in_cache::  " <<
                   is_uri_present_in_cache;
        if (!is_etag_present || (is_etag_present && !is_uri_present_in_cache))
        {
            /* A new URI should be added to the cache in 2 different scenario's
                 1. No ETag header is present in the http request (Request received for the first time)
                 2. ETag header is present in request, but the requested URI is not present in cache (CleanupThread would have deleted the URI from cache after time out)
               */
            LOG(INFO) << "Adding URI to Cache";
            if (!web_cache_inst->AddUriToCache(request_uri))
            {
                LOG(INFO) << "Deleting URI from cache failed";
                return;
            }
        }
        // Add the ETag header in response
        if (web_cache_inst->GetETagByURI(request_uri, eTag))
        {
            // eTag Validation check
            if (!eTag.empty())
            {
                response_header.SetETag(eTag);
            }
        }
#endif /* ENABLE_WEB_CACHE */
    }

}

