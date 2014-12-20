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

#ifndef WEBSERVICE_CREATOR_IDENTIFIED_OBJECT_H_
#define WEBSERVICE_CREATOR_IDENTIFIED_OBJECT_H_

#include <map>
#include <string>

namespace webservice
{

    class BaseIdentifiedObject; /* forward declaration */
    class CreatorIdentifiedObject
    {
            typedef BaseIdentifiedObject *(*Creator)(std::string);

        public:
            /**
              * Get the instance of this object
              * @return CreatorIdentifiedObject&
              */
            static CreatorIdentifiedObject &GetInstance(void);

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
              * @param[in] id : id of the object
              * @return BaseIdentifiedObject*
              */
            BaseIdentifiedObject *CreateObject(std::string name, std::string id);

        private:
            /* Following is for singleton */
            /* Block multiple creation */
            CreatorIdentifiedObject(void) {}
            /* Block cloning */
            CreatorIdentifiedObject(const CreatorIdentifiedObject &);
            /* Block free each instance */
            virtual ~CreatorIdentifiedObject(void) {}
            /* Block cloning using operator = */
            void operator= (const CreatorIdentifiedObject &);

        private:
            /* the map for saving each identified object */
            std::map<std::string, Creator> create_map_;
    };

}
#endif /* WEBSERVICE_CREATOR_IDENTIFIED_OBJECT_H_ */

