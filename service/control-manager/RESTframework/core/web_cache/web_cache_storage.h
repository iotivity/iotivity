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

#ifndef WEBSERVICE_SRC_WEB_CACHE_WEB_CACHE_STORAGE_H_
#define WEBSERVICE_SRC_WEB_CACHE_WEB_CACHE_STORAGE_H_

#include <math.h>

#include <map>
#include <string>

#include "base/memory/singleton.h"
#include "base/synchronization/lock.h"
#include "base/threading/thread.h"
#include "base/time.h"
#include "base/timer.h"

namespace webservice
{

    typedef struct
    {
        std::string uri_;
        std::string etag_;
        base::Time storage_time_;
        bool is_marked_for_deletion;
    } webCacheStruct;

// This class is Singleton and thread safe
    class WebCacheStorage
    {
        public:
            // constructor
            WebCacheStorage();
            // destructor
            ~WebCacheStorage();
            // returns singleton instance
            static WebCacheStorage *GetInstance();
            // deletes singleton instance
            void DestroyInstance();
            /**
            * Returns the Etag value for the given uri
            * @param[in] std::string uri
            * @param[out] std::string eTag
            * @return bool
            */
            bool GetETagByURI(const std::string &uri, std::string &eTag);
            /**
            * Adds the given uri to the web cache storage
            * @param[in] std::string uri
            * @return bool
            */
            bool AddEntryToStorage(const std::string &uri);
            /**
            * Update the etag value for the given uri in web cache storage
            * @param[in] std::string uri
            * @return bool
            */
            bool UpdateStorage(const std::string &uri);
            /**
            * Initializes the web cache storage
            * @return bool
            */
            bool Init(void);
            /**
            * Stops the cleanup timer thread
            * @return void
            */
            void StopTimer(void);
            /**
            * Deletes the given uri from the web cache storage (including iits child uri's)
            * @param[in] std::string uri
            * @return bool
            */
            bool DeleteUriFromStorage(const std::string &uri);

            class CleanUpThread :  public base::Thread
            {
                public:
                    // constructor
                    CleanUpThread();
                    // destructor
                    ~CleanUpThread();
                    /** posts a task to the thread
                     *
                     * @param tracked_objects::Location, base::Closure
                     * @return void
                     */
                    void PostTask(const tracked_objects::Location &from_here, const base::Closure &task) const;
                    /**
                     * Starts the repeating timer to clean the timed out URI's
                     * @param void
                     * @return void
                     */
                    void StartCleanUpTimer(int timer_interval);
                    /**
                    * Stops the cleanup timer thread
                    * @return void
                    */
                    void StopCleanUpTimer();

                private:
                    // Repeating timer
                    base::RepeatingTimer<WebCacheStorage> *timer_;
            };

        private:
            // singleton instance
            static WebCacheStorage *web_cache_storage_instance_;
            base::Lock lock_;
            // map to store the uri and etag values
            std::map<std::string, webCacheStruct> uri_etag_map_;
            // thread object
            CleanUpThread *cleanup_thread_;
            // counter value, this will be added at the end of system time to generate unique ETag.
            int counter_;
            // repeating timer interval, in seconds
            // CleanUpTimer will be called whenever the timer expires
            int cleanup_timer_interval_;
            // how long the URI can be present in the storage, without any modifications on it
            int caching_duration_for_uri_;
            // window size to clear timed out uri's
            int cleanup_thread_window_size_;
            // Generates a unique ETag
            std::string GenerateUniqueETag();
            // XOR's the given string, and generates an encrypted string
            void XORETag(const std::string &uuid, std::string &encrypt);
            /** Cleans the timed out URI's from storage
             *
             * @param void
             * @return void
             */
            void CleanTimedOutUris();
    };
}  // namespace webservice
#endif  // WEBSERVICE_SRC_WEB_CACHE_WEB_CACHE_STORAGE_H_

