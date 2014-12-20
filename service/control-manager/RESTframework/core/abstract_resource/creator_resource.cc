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

#include "abstract_resource/creator_resource.h"

namespace webservice
{

    /**
      * Get the instance of this object
      * @return CreatorResource&
      */
    CreatorResource &CreatorResource::GetInstance()
    {
        static CreatorResource instance;
        return instance;
    }

    /**
      * Register object to create
      * @param[in] name : the object name
      * @param[in] Creator : function pointer to create the object
      * @return void
      */
    void CreatorResource::RegisterObject(std::string name, Creator creator)
    {
        this->create_map_[name] = creator;
    }

    /**
      * Create registered object
      * @param[in] name : the object name
      * @return BaseResource*
      */
    BaseResource *CreatorResource::CreateObject(std::string name)
    {
        BaseResource *p = 0;
        if (create_map_[name]) p = create_map_[name]();
        return p;
    }

}

