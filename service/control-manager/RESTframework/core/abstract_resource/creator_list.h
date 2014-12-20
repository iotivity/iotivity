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

#ifndef WEBSERVICE_CREATOR_LIST_H_
#define WEBSERVICE_CREATOR_LIST_H_

#include <map>
#include <string>

namespace webservice
{

    class BaseList; /* forward declaration */
    class CreatorList
    {
            typedef BaseList *(*Creator)();

        public:
            /**
              * Get the instance of this object
              * @return CreatorList&
              */
            static CreatorList &GetInstance(void);

            /**
              * Register object to create
              * @param[in] name : the object name
              * @param[in] Creator : function pointer to create the object
              * @return void
              */
            void RegisterObject(std::string name, Creator creator);

            /**
              * Create registered object
              * @param[in] name : the object name
              * @return BaseList*
              */
            BaseList *CreateObject(std::string name);

        private:
            /* Following is for singleton */
            /* Block multiple creation */
            CreatorList(void) {}
            /* Block cloning */
            CreatorList(const CreatorList &);
            /* Block free each instance */
            virtual ~CreatorList(void) {}
            /* Block cloning using operator = */
            void operator= (const CreatorList &);

        private:
            /* the map for saving each list object */
            std::map<std::string, Creator> create_map_;
    };

}
#endif /* WEBSERVICE_CREATOR_LIST_H_ */

