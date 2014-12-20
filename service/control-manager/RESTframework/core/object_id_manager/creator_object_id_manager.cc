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

#include "object_id_manager/creator_object_id_manager.h"

namespace webservice
{

    /**
      * Get the instance of this object
      * @return CreatorObjectIDManager&
      */
    CreatorObjectIDManager &CreatorObjectIDManager::GetInstance(void)
    {
        static CreatorObjectIDManager instance;
        return instance;
    }

    /**
      * Register object to create
      * @param[in] string
      * @param[in] Creator
      * @return void
      */
    void CreatorObjectIDManager::RegisterObject(std::string object_name, Creator creator)
    {
        create_map_[object_name] = creator;
    }

    /**
      * Create registered object
      * @param[in] string
      * @return IObjectIDManager*
      */
    IObjectIDManager *CreatorObjectIDManager::CreateObject(std::string object_name)
    {
        IObjectIDManager *p = 0;
        if (create_map_[object_name]) p = create_map_[object_name]();
        return p;
    }

}
