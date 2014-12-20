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

#include "web_cache/web_cache_manager.h"

namespace webservice
{
    typedef Singleton<WebCacheManager, LeakySingletonTraits<WebCacheManager> > WebCacheManagerSingleton;
    WebCacheManager *WebCacheManager::web_cache_manager_instance_ = NULL;

// constructor
    WebCacheManager::WebCacheManager()
    {
        web_cache_storage_instance_ = WebCacheStorage::GetInstance();
        if (web_cache_storage_instance_)
        {
            web_cache_storage_instance_->Init();
        }
    }

// destructor
    WebCacheManager::~WebCacheManager()
    {
        if (web_cache_storage_instance_)
        {
            web_cache_storage_instance_->DestroyInstance();
            web_cache_storage_instance_ = NULL;
        }
    }

// returns singleton instance
    WebCacheManager *WebCacheManager::GetInstance()
    {
        DLOG(INFO) << __FUNCTION__ << "Enter";
        if (!web_cache_manager_instance_)
        {
            web_cache_manager_instance_ = WebCacheManagerSingleton::get();
        }
        DLOG(INFO) << __FUNCTION__ << "Exit";
        return web_cache_manager_instance_;
    }

// deletes singleton instance
    void WebCacheManager::DestroyInstance()
    {
        DLOG(INFO) << __FUNCTION__ << "Enter";
        static base::Lock destroy_lock;
        base::AutoLock auto_lock(destroy_lock);
        if (web_cache_manager_instance_)
        {
            delete web_cache_manager_instance_;
            web_cache_manager_instance_ = NULL;
        }
    }

    bool WebCacheManager::CheckIfURIandETagExists(const std::string &uri, const std::string &etag,
            bool *is_exists) const
    {
        DLOG(INFO) << __FUNCTION__ << "  uri:: " << uri << "  etag::  " << etag;
        if (web_cache_storage_instance_)
        {
            std::string etag_from_storage("");
            // check whether the given uri is present in storage, if exists fill the eTag value.
            if (web_cache_storage_instance_->GetETagByURI(uri, etag_from_storage))
            {
                // the given URI is present in storage
                *is_exists = true;
                if (!etag.compare(etag_from_storage))
                {
                    // etag received from request matched the value present in storage.
                    DLOG(INFO) << __FUNCTION__ << "Exit :: Success";
                    return true;
                }
            }
            else
            {
                // uri is not present in storage
                *is_exists = false;
            }
        }
        LOG(ERROR) << __FUNCTION__ << "Exit:: Failure";
        // return false in other cases
        return false;
    }

    bool WebCacheManager::UpdateCache(const std::string &uri)
    {
        DLOG(INFO) << __FUNCTION__ << "  uri:: " << uri;
        if (web_cache_storage_instance_)
        {
            if (web_cache_storage_instance_->UpdateStorage(uri))
            {
                // storage updated successfully
                DLOG(INFO) << __FUNCTION__ << "Exit Success";
                return true;
            }
        }
        LOG(ERROR) << __FUNCTION__ << "Exit Failure";
        return false;
    }

    bool WebCacheManager::AddUriToCache(const std::string &uri)
    {
        DLOG(INFO) << __FUNCTION__ << "  uri:   " << uri;
        if (web_cache_storage_instance_)
        {
            if (web_cache_storage_instance_->AddEntryToStorage(uri))
            {
                // URI added to storage  successfully
                DLOG(INFO) << __FUNCTION__ << "Exit Success";
                return true;
            }
        }
        LOG(ERROR) << __FUNCTION__ << "Exit web_cache_storage_instance_ is NULL";
        return false;
    }

    bool WebCacheManager::DeleteUriFromCache(const std::string &uri)
    {
        DLOG(INFO) << __FUNCTION__ << "  uri:   " << uri;
        if (web_cache_storage_instance_)
        {
            if (web_cache_storage_instance_->DeleteUriFromStorage(uri))
            {
                // URI deleted from storage
                DLOG(INFO) << __FUNCTION__ << "Exit Success";
                return true;
            }
        }
        LOG(ERROR) << __FUNCTION__ << " Exit";
        return false;
    }

    bool WebCacheManager::GetETagByURI(const std::string &uri, std::string &etag) const
    {
        DLOG(INFO) << __FUNCTION__ <<  "  uri:: " << uri;
        // Initialize the etag value
        etag = "";
        if (web_cache_storage_instance_)
        {
            if (web_cache_storage_instance_->GetETagByURI(uri, etag))
            {
                return true;
            }
        }
        // returns failure when GetETagByURI fails, or web_cache_storage_instance_ is NULL
        return false;
    }
}  // namespace webservice
