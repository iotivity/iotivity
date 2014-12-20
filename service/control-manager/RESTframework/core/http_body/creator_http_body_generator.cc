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

#include "http_body/creator_http_body_generator.h"
#include "http_header/http_header_defined.h"

namespace webservice
{

    /**
      * Get the instance of this object
      * @return CreatorHTTPBodyGenerator&
      */
    CreatorHTTPBodyGenerator &CreatorHTTPBodyGenerator::GetInstance()
    {
        static CreatorHTTPBodyGenerator instance;
        return instance;
    }

    /**
      * Register object to create
      * @param[in] HttpContentType
      * @param[in] Creator
      * @return void
      */
    void CreatorHTTPBodyGenerator::RegisterObject(HttpContentType content_type, Creator creator)
    {
        this->create_map_[content_type] = creator;
    }

    /**
      * Create registered object
      * @param[in] HttpContentType
      * @return IHTTPBodyGenerator*
      */
    IHTTPBodyGenerator *CreatorHTTPBodyGenerator::CreateObject(HttpContentType content_type)
    {
        IHTTPBodyGenerator *p = 0;
        if (this->create_map_[content_type])
        {
            p = this->create_map_[content_type]();
        }
        return p;
    }

    /**
      * Find registered object
      * @param[in] HttpContentType
      * @return bool
      */
    bool CreatorHTTPBodyGenerator::IsRegisteredObject(HttpContentType content_type)
    {
        std::map<HttpContentType, Creator>::iterator it;
        it = this->create_map_.find(content_type);
        if (this->create_map_.end() != it) return true;
        else return false;
    }

}
