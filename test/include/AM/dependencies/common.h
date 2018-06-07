/******************************************************************
*
* Copyright 2018 Open Connectivity Foundation All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#ifndef OCSAMPLE_COMMON_H_
#define OCSAMPLE_COMMON_H_

#define USE_HW 0
#define IS_SECURE_MODE 1
#include "ocstack.h"


/* Get the result in string format. */
const char *getResult(OCStackResult result);

/* Removes the new line character from a NULL terminated C string. */
void StripNewLineChar(char* str);

void getCurrentTime(char * buf);
void setUserTime(char * buf);

#endif //OCSAMPLE_COMMON_H_


