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

#ifndef WEBSERVICE_CREATOR_HTTP_BODY_PARSER_H_
#define WEBSERVICE_CREATOR_HTTP_BODY_PARSER_H_

#include <map>
#include "http_header/http_header_defined.h"
#include "rest_engine_export.h"

namespace webservice
{

    class IHTTPBodyParser;
    class REST_ENGINE_EXPORT CreatorHTTPBodyParser
    {
            typedef IHTTPBodyParser *(*Creator)();

        public:
            /**
                * Get the instance of this object
                * @return CreatorHTTPBodyGenerator&
                */
            static CreatorHTTPBodyParser &GetInstance(void);

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
            IHTTPBodyParser *CreateHTTPBodyParser(HttpContentType content_type);

        private:
            /* Following is for singleton */
            CreatorHTTPBodyParser(void) {}
            CreatorHTTPBodyParser(const CreatorHTTPBodyParser &);
            virtual ~CreatorHTTPBodyParser(void) {}
            void operator= (const CreatorHTTPBodyParser &);

        private:
            std::map<HttpContentType, Creator> create_map_;
    };

}

#endif /* WEBSERVICE_CREATOR_HTTP_BODY_PARSER_H_ */

