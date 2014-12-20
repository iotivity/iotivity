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

#ifndef WEBSERVICE_CREATOR_SIMPLE_OBJECT_STRATEGY_H_
#define WEBSERVICE_CREATOR_SIMPLE_OBJECT_STRATEGY_H_

#include <string>

#include "base/values.h"
#include "http_header/http_request_header.h"
#include "http_start_line/http_request_line.h"

namespace webservice
{

    class ISimpleObjectStrategy;  /* forward declaration */
    class CreatorSimpleObjectStrategy
    {
        public:
            /**
              * Get the instance of this object
              * @return CreatorSimpleObjectStrategy&
              */
            static CreatorSimpleObjectStrategy &GetInstance(void);

            /**
              * Set policy Child of simple object
              * @param[in] request_line
              * @param[in] request_header
              * @param[in] request_body
              * @param[in] current_uri
              * @return ISimpleObjectStrategy*
              */
            ISimpleObjectStrategy *SetChildPolicy(const HTTPRequestLine &request_line,
                                                  const HTTPRequestHeader &request_header, base::Value *request_body, const std::string &current_uri);

            /**
              * Release child strategy object
              * @param[in] child_strategy
              * @return void
              */
            void ReleaseChildStrategyObject(ISimpleObjectStrategy *child_strategy);

        private:
            /**
              * Check requested URI whether it is devices/{id} or not
              * @param[in] current_uri
              * @return bool
              */
            bool IsDeviceObject(const std::string &current_uri);
            /* Following is for singleton */
            /* Block multiple creation */
            CreatorSimpleObjectStrategy(void) {}
            /* Block cloning */
            CreatorSimpleObjectStrategy(const CreatorSimpleObjectStrategy &);
            /* Block free each instance */
            virtual ~CreatorSimpleObjectStrategy(void) {}
            /* Block cloning using operator = */
            void operator= (const CreatorSimpleObjectStrategy &);
    };

}
#endif /* WEBSERVICE_CREATOR_SIMPLE_OBJECT_STRATEGY_H_ */

