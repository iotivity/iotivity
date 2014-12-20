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

#ifndef WEBSERVICE_SRC_NEW_RESOURCE_HANDLER_GENERATED_RESOURCE_LINKED_CHILD_COLOR_H_
#define WEBSERVICE_SRC_NEW_RESOURCE_HANDLER_GENERATED_RESOURCE_LINKED_CHILD_COLOR_H_

#include "base_resource/base_linked_child.h"
#include "data_types/simple_data_type.h"
#include "data_types/simple_array_data_type.h"

namespace webservice
{

    class LinkedChildColor : public BaseLinkedChild
    {
        public:
            LinkedChildColor(void);
            virtual ~LinkedChildColor(void);
            /**
              * Create this class.
              * @return BaseLinkedChild*
              */
            static BaseLinkedChild *Create(void);

        public:
            /**
              * Register function to create this class.
              * @param[in] string
              * @param[in] BaseLinkedChild*
              * @return static RegisterLinkedChild
              */
            static RegisterLinkedChild regChildObject;

        private:
            UInt8 *hue_;
            Int16 *relativeHue_;
            UInt8 *saturation_;
            Int16 *relativeSaturation_;
            UInt16 *chromaticityX_;
            UInt16 *chromaticityY_;
            UInt16 *colorTemperature_;
    };

}

#endif /* WEBSERVICE_SRC_NEW_RESOURCE_HANDLER_GENERATED_RESOURCE_LINKED_CHILD_COLOR_H_ */
