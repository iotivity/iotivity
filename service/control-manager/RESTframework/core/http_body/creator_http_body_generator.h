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

#ifndef WEBSERVICE_CREATOR_HTTP_BODY_GENERATOR_H_
#define WEBSERVICE_CREATOR_HTTP_BODY_GENERATOR_H_

#include <map>
#include "http_header/http_header_defined.h"
#include "rest_engine_export.h"

namespace webservice
{

    class IHTTPBodyGenerator;
    class REST_ENGINE_EXPORT CreatorHTTPBodyGenerator
    {
            typedef IHTTPBodyGenerator *(*Creator)();

        public:
            /**
                * Get the instance of this object
                * @return CreatorHTTPBodyGenerator&
                */
            static CreatorHTTPBodyGenerator &GetInstance(void);

            /**
                * Register object to create
                * @param[in] HttpContentType
                * @param[in] Creator
                * @return void
                */
            void RegisterObject(HttpContentType content_type, Creator creator);

            /**
                * Create registered object
                * @param[in] HttpContentType
                * @return IHTTPBodyGenerator*
                */
            IHTTPBodyGenerator *CreateObject(HttpContentType content_type);

            /**
                * Find registered object
                * @param[in] HttpContentType
                * @return bool
                */
            bool IsRegisteredObject(HttpContentType content_type);

        private:
            /* Following is for singleton */
            /* Block multiple creation */
            CreatorHTTPBodyGenerator(void) {}
            /* Block cloning */
            CreatorHTTPBodyGenerator(const CreatorHTTPBodyGenerator &);
            /* Block free each instance */
            virtual ~CreatorHTTPBodyGenerator(void) {}
            /* Block cloning using operator = */
            void operator= (const CreatorHTTPBodyGenerator &);

        private:
            std::map<HttpContentType, Creator> create_map_;
    };

}

#endif /* WEBSERVICE_CREATOR_HTTP_BODY_GENERATOR_H_ */

