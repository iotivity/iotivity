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

#ifndef WEBSERVICE_CREATOR_OBJECT_ID_MANAGER_H_
#define WEBSERVICE_CREATOR_OBJECT_ID_MANAGER_H_

#include <map>
#include <string>

namespace webservice
{

    class IObjectIDManager; /* forward declaration */
    class CreatorObjectIDManager
    {
            typedef IObjectIDManager *(*Creator)();

        public:
            /**
                * Get the instance of this object
                * @return CreatorObjectIDManager&
                */
            static CreatorObjectIDManager &GetInstance(void);

            /**
                * Register object to create
                * @param[in] string
                * @param[in] Creator
                * @return void
                */
            void RegisterObject(std::string object_name, Creator creator);

            /**
                * Create registered object
                * @param[in] HttpContentType
                * @return IObjectIDManager*
                */
            IObjectIDManager *CreateObject(std::string object_name);

        private:
            /* Following is for singleton */
            /* Block multiple creation */
            CreatorObjectIDManager(void) {}
            /* Block cloning */
            CreatorObjectIDManager(const CreatorObjectIDManager &);
            /* Block free each instance */
            virtual ~CreatorObjectIDManager(void) {}
            /* Block cloning using operator = */
            void operator= (const CreatorObjectIDManager &);

        private:
            /* the map for saving each object id manager */
            std::map<std::string, Creator> create_map_;
    };

}
#endif /* WEBSERVICE_CREATOR_OBJECT_ID_MANAGER_H_ */

