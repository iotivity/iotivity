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

#ifndef WEBSERVICE_SRC_WEB_CACHE_WEB_CACHE_MANAGER_H_
#define WEBSERVICE_SRC_WEB_CACHE_WEB_CACHE_MANAGER_H_

#include <string>
#include "base/memory/singleton.h"
#include "web_cache/web_cache_storage.h"
#include "rest_engine_export.h"

namespace webservice
{

// This class is Singleton and thread safe
    class REST_ENGINE_EXPORT WebCacheManager
    {
        public:
            // returns singleton instance
            static WebCacheManager *GetInstance();
            // deletes singleton instance
            void DestroyInstance();
            /**
            * Checks whether the given URI, eTag pair exists in the web cache storage
            * @param[out] bool is_exists : this will be set to true if the given <URI, ETag> pair exists
            * @param[in] std::string uri
            * @param[in] std::string etag
            * @return bool
            */
            bool CheckIfURIandETagExists(const std::string &uri, const std::string &etag,
                                         bool *is_exists) const;
            /**
            * Update the etag value for the given uri in web cache storage
            * @param[in] std::string uri
            * @return bool
            */
            bool UpdateCache(const std::string &uri);
            /**
            * Adds the given uri to the web cache storage
            * @param[in] std::string uri
            * @return bool
            */
            bool AddUriToCache(const std::string &uri);
            /**
            * Returns the Etag value for the given uri
            * @param[in] std::string uri
            * @return std::string
            */
            bool GetETagByURI(const std::string &uri, std::string &etag) const;
            /**
            * Deletes the given uri from the web cache storage
            * @param[in] std::string uri
            * @return bool
            */
            bool DeleteUriFromCache(const std::string &uri);

        private:
            // singleton instance
            static WebCacheManager *web_cache_manager_instance_;
            WebCacheStorage *web_cache_storage_instance_;
            // constructor
            WebCacheManager();
            // destructor
            ~WebCacheManager();
            // generate unique id
            std::string GenerateUniqueETag();
            // friend member, it creates WebCacheManager object
            friend struct DefaultSingletonTraits<webservice::WebCacheManager>;
    };
}  // namespace webservice
#endif  // WEBSERVICE_SRC_WEB_CACHE_WEB_CACHE_MANAGER_H_

