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

#include "web_cache/web_cache_storage.h"

#include <utility>

#include "base/base64.h"
#include "base/logging.h"
#include "base/sha1.h"
#include "base/threading/platform_thread.h"

namespace webservice
{
    typedef Singleton<WebCacheStorage, LeakySingletonTraits<WebCacheStorage> > WebCacheStorageSingleton;
    WebCacheStorage *WebCacheStorage::web_cache_storage_instance_ = NULL;
// TODO as of now hard coding the values, but these should set by the application
    const int cleanup_timer_interval = 10;
    const int caching_duration_for_uri = 5;
    const int cleanup_thread_window_size = 10;

// constructor
    WebCacheStorage::WebCacheStorage()
        : cleanup_thread_(NULL),
          counter_(0),
          cleanup_timer_interval_(0),
          caching_duration_for_uri_(0),
          cleanup_thread_window_size_(0)
    {
        // cleanup_thread_ is created in Init
    }

// destructor
    WebCacheStorage::~WebCacheStorage()
    {
        if (uri_etag_map_.size())
            uri_etag_map_.clear();
        if (cleanup_thread_)
        {
            cleanup_thread_->Stop();
            delete cleanup_thread_;
        }
    }

// returns singleton instance
    WebCacheStorage *WebCacheStorage::GetInstance()
    {
        DLOG(INFO) << __FUNCTION__ << "Enter";
        if (!web_cache_storage_instance_)
        {
            web_cache_storage_instance_ = WebCacheStorageSingleton::get();
        }
        DLOG(INFO) << __FUNCTION__ << "Exit";
        return web_cache_storage_instance_;
    }

// deletes singleton instance
    void WebCacheStorage::DestroyInstance()
    {
        DLOG(INFO) << __FUNCTION__ << "Enter";
        static base::Lock destroy_lock;
        base::AutoLock auto_lock(destroy_lock);
        if (web_cache_storage_instance_)
        {
            StopTimer();
            delete web_cache_storage_instance_;
            web_cache_storage_instance_ = NULL;
        }
    }

    bool WebCacheStorage::Init(void)
    {
        DLOG(INFO) << __FUNCTION__ << "Enter";
        uri_etag_map_.clear();

        cleanup_timer_interval_ = cleanup_timer_interval;
        caching_duration_for_uri_ = caching_duration_for_uri;
        cleanup_thread_window_size_ = cleanup_thread_window_size;

        // Create the clkeanup thread
        cleanup_thread_ = new CleanUpThread();
        if (NULL == cleanup_thread_)
        {
            LOG(ERROR) << "CleanUp thread creation failed";
            return false;
        }
        // Start the thread
        if (false == cleanup_thread_->Start())
        {
            LOG(ERROR) << "CleanUp thread starting failed";
            return false;
        }
        cleanup_thread_->PostTask(FROM_HERE, base::Bind(&WebCacheStorage::CleanUpThread::StartCleanUpTimer,
                                  base::Unretained(cleanup_thread_), cleanup_timer_interval_));
        return true;
    }

    std::string WebCacheStorage::GenerateUniqueETag()
    {
        std::string time("");
        base::Time now;
        now = base::Time::Now();

        int64 time_in_micro = now.ToInternalValue();
        std::stringstream stream;

#if defined(ENABLE_ENCODED_ETAG)
        stream << time_in_micro << counter_++;
        time = stream.str();

        std::string sha1_encrypted = base::SHA1HashString(time);
        std::string encrypted(ceil(sha1_encrypted.size() / 2) + 1, '\0');
        XORETag(sha1_encrypted, encrypted);

        std::string encoded("");
        base::Base64Encode(encrypted, &encoded);
        return encoded;
#else
        stream << time_in_micro;
        time = stream.str();
        return time;
#endif /* ENABLE_ENCODED_ETAG */
    }

    void WebCacheStorage::XORETag(const std::string &uuid, std::string &encrypt)
    {
        char str1[20] = {'\0'}, str2[20] = {'\0'};
        int len = ceil(uuid.length() / 2);
        uuid.copy(str1, len, 0);
        uuid.copy(str2, len, len - 1);
        int i = 0;
        for (; i < len; i++)
        {
            encrypt[i] = str1[i] ^ str2[i];
        }
    }

    bool WebCacheStorage::GetETagByURI(const std::string &uri, std::string &etag)
    {
        DLOG(INFO) << __FUNCTION__ << "  uri  :  " << uri << "  etag::  " << etag << std::endl;
        lock_.Acquire();
        std::map<std::string, webCacheStruct>::iterator it = uri_etag_map_.find(uri);
        if (it != uri_etag_map_.end() && (!it->second.is_marked_for_deletion))
        {
            etag = (it->second).etag_;
            DLOG(INFO) << __FUNCTION__ << "Exit:: Success";
            lock_.Release();
            return true;
        }
        LOG(ERROR) << __FUNCTION__ << "Exit:: Failure";
        lock_.Release();
        return false;
    }

    bool WebCacheStorage::AddEntryToStorage(const std::string &uri)
    {
        DLOG(INFO) << __FUNCTION__ << "  uri:: " << uri;
        lock_.Acquire();
        std::string temp_uri = uri;
        size_t end;
        if (temp_uri.length())
        {
            // add the following URI's to storage
            // given URI
            // all the parent URI's of the given URI

            // Reason to add parent Uri's to storage:
            /* Whenever there is any state change in child URI, the parent URI also should get updated.
               For example,
                  1. GET /devices/{device-id} will return 2xx with ETag (First time request)
                  2. Now change the state of the device using /devices/{device-id}/operation
                  3. GET /devices/{device-id} will return 304 (wrong response)
                  Adding the parent URI also to storage makes sure that the client will the correct response from SLEP.
            */
            std::string etag = GenerateUniqueETag();
            do
            {
                std::map<std::string, webCacheStruct>::iterator it = uri_etag_map_.find(temp_uri);
                if (it == uri_etag_map_.end())
                {
                    webCacheStruct temp_cache;
                    temp_cache.uri_ = temp_uri;
                    temp_cache.etag_ = etag;
                    temp_cache.storage_time_ = base::Time::Now();
                    temp_cache.is_marked_for_deletion = false;
                    uri_etag_map_.insert(std::pair<std::string, webCacheStruct> (temp_uri, temp_cache));
                }
                else if (it->second.is_marked_for_deletion)
                {
                    // Uri already marked for deletion, but client has added this URI again, and sent a GET request.
                    // So this URI has to be updated with the latest values
                    DLOG(INFO) << "Deleted URI cached again";
                    it->second.etag_ = etag;
                    it->second.storage_time_ = base::Time::Now();
                    it->second.is_marked_for_deletion = false;
                }
                end = temp_uri.find_last_of("/");
                temp_uri = temp_uri.substr(0, end);
            }
            while (end != std::string::npos);

            DLOG(INFO) << __FUNCTION__ << "Exit:: Success";
            lock_.Release();
            return true;
        }
        LOG(ERROR) << __FUNCTION__ << "Exit:: Invalid input strings";
        lock_.Release();
        return false;
    }

    bool WebCacheStorage::UpdateStorage(const std::string &uri)
    {
        DLOG(INFO) << __FUNCTION__ << "  uri::  " << uri;
        lock_.Acquire();
        std::string temp_uri = uri;
        size_t end;
        bool isUriPresentInCache = true;

        if (uri_etag_map_.size() && uri.length())
        {
            if (temp_uri[0] == '/')
            {
                temp_uri.erase(0, 1);
            }

            std::map<std::string, webCacheStruct>::iterator it = uri_etag_map_.find(temp_uri);
            if (it == uri_etag_map_.end())
            {
                DLOG(INFO) << "URI not found in storage:  " << temp_uri;
                isUriPresentInCache = false;
            }

            // Update the following URI's to storage
            // given URI
            // If the URI is present, then add/update all the parent URI's of the given URI in storage
            // Incase if the given URI is not found in storage, search for it's parent URI's existence and update.
            std::string etag = GenerateUniqueETag();
            do
            {
                std::map<std::string, webCacheStruct>::iterator it = uri_etag_map_.find(temp_uri);
                if (it != uri_etag_map_.end())
                {
                    (it->second).etag_ = etag;
                }
                else if (isUriPresentInCache)
                {
                    // the uri is present in storage, so add all its parent URI's to the cache
                    DLOG(INFO) << "URI not found in storage:  " << temp_uri;
                    webCacheStruct temp_cache;
                    temp_cache.uri_ = temp_uri;
                    temp_cache.etag_ = etag;
                    temp_cache.storage_time_ = base::Time::Now();
                    temp_cache.is_marked_for_deletion = false;
                    uri_etag_map_.insert(std::pair<std::string, webCacheStruct> (temp_uri, temp_cache));
                }
                end = temp_uri.find_last_of("/");
                temp_uri = temp_uri.substr(0, end);
            }
            while (end != std::string::npos);
        }
        DLOG(INFO) << __FUNCTION__ << "Exit:: success";
        lock_.Release();
        return true;
    }

    bool WebCacheStorage::DeleteUriFromStorage(const std::string &uri)
    {
        DLOG(INFO) << __FUNCTION__ << "  uri::  " << uri;
        lock_.Acquire();
        std::string temp_uri = uri;

        if (uri_etag_map_.size() && temp_uri.length())
        {
            std::map<std::string, webCacheStruct>::iterator it = uri_etag_map_.find(temp_uri);
            if (it == uri_etag_map_.end())
            {
                LOG(ERROR) << "URI not found in storage:  " << temp_uri;
                lock_.Release();
                return false;
            }

            // delete all the child uri's
            it = uri_etag_map_.begin();
            for (; it != uri_etag_map_.end(); it++)
            {
                if (!strncmp(it->first.c_str(), temp_uri.c_str(), strlen(temp_uri.c_str())))
                {
                    DLOG(INFO) << "Child Uri found.. deleting  " << it->first << "  from cache";
                    // child uri of the given uri
                    it->second.is_marked_for_deletion = true;
                }
            }
        }
        LOG(ERROR) << __FUNCTION__ << "Exit:: success";
        lock_.Release();
        return true;
    }

    void WebCacheStorage::StopTimer()
    {
        cleanup_thread_->PostTask(FROM_HERE, base::Bind(&WebCacheStorage::CleanUpThread::StopCleanUpTimer,
                                  base::Unretained(cleanup_thread_)));
        return;
    }

// constructor
    WebCacheStorage::CleanUpThread::CleanUpThread() : base::Thread("Cleanupthread")
    {
        timer_ =  new base::RepeatingTimer<WebCacheStorage>();
    }

    WebCacheStorage::CleanUpThread::~CleanUpThread()
    {
        // timer_ should be deleted from the thread which had STARTED it, so
        // the task of deleting timer is posted to the cleanup_thread_
        // NOTE: Static code analysis tools may raise a defect in this destructor as the timer_ is not being freed.
        //           Please ignore any such defect as timer_ is actually being freed by the thread which had started it. (Check function WebCacheStorage::CleanUpThread::StopCleanUpTimer())
        Stop();
    }

// posts a task to the thread
    void WebCacheStorage::CleanUpThread::PostTask(const tracked_objects::Location &from_here,
            const base::Closure &task) const
    {
        if (this->message_loop())
        {
            return (this->message_loop()->PostTask(from_here, task));
        }
        else
        {
            LOG(ERROR) << "CleanUpThread's message loop is Null.. The thread ia already stopped";
        }
        return;
    }

    void WebCacheStorage::CleanUpThread::StopCleanUpTimer()
    {
        if (timer_)
        {
            timer_->Stop();
            delete timer_;
            timer_ = NULL;
        }
        return;
    }

    void WebCacheStorage::CleanUpThread::StartCleanUpTimer(int timer_interval)
    {
        if (timer_interval > 0)
            timer_->Start(FROM_HERE, base::TimeDelta::FromSeconds(timer_interval),
                          WebCacheStorage::GetInstance(), &WebCacheStorage::CleanTimedOutUris);
    }

    void WebCacheStorage::CleanTimedOutUris()
    {
        // cleanup thread, to clean the Timed out URI's
        DLOG(INFO) << __FUNCTION__ << "Enter list size :: " << uri_etag_map_.size();
        const base::TimeDelta timeout_value = base::TimeDelta::FromMinutes(
                caching_duration_for_uri_);  // in minutes

        base::Time current_time = base::Time::Now();
        std::map<std::string, webCacheStruct>::iterator it = uri_etag_map_.begin();

        // If the uri_etag_map_ size is big, this function blocks all other add/modify/delete api's from accessing uri_etag_map_.
        // To avoid this, this function checks a few uri's validity at a time and release lock for other api's.
        for (; it != uri_etag_map_.end();)
        {
            int counter = cleanup_thread_window_size_;
            lock_.Acquire();
            while (counter && it != uri_etag_map_.end())
            {
                base::TimeDelta time_diff;
                time_diff = current_time - (it->second).storage_time_;
                if (time_diff >= timeout_value || it->second.is_marked_for_deletion)
                {
                    std::map<std::string, webCacheStruct>::iterator item_to_delete = it;
                    DLOG(INFO) << "Timeout happened or the URI deleted from SLEP.... so deleting uri from map::  " <<
                               it->first;
                    // first increment the iterator and erase the uri from map, otherwise android platform is not incrementing the iterator properly
                    it++;
                    uri_etag_map_.erase(item_to_delete);
                }
                else
                {
                    // it should be incremented in both 'if' and 'else' case
                    it++;
                }
                counter--;
            }
            lock_.Release();
        }
        DLOG(INFO) << __FUNCTION__ << "Exit";
    }
}  // namespace webservice

