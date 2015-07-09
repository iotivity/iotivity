/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

#ifndef SIMULATOR_ERROR_CODES_H_
#define SIMULATOR_ERROR_CODES_H_

#include <iostream>

typedef enum
{
    SIMULATOR_SUCCESS = 0,
    SIMULATOR_BAD_INPUT,
    SIMULATOR_RESOURCE_NOT_FOUND,
    SIMULATOR_RESOURCE_BUSY,

    // Attribute udpate automation related
    SIMULATOR_AUTOMATION_ALREADY_STARTED,

    SIMULATOR_ERROR = 255
} SimulatorResult;
#endif //SIMULATOR_ERROR_CODES_H_