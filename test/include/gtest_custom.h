/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
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

#ifndef __GTEST_CUSTOM_H__
#define __GTEST_CUSTOM_H__

//extern void WriteTestingLog(const char* format, ...);

#define SET_FAILURE(message) do {\
	char buffer[15];\
	time_t t = time(0);\
	struct tm * now = localtime(&t);\
	strftime(buffer, 15, "%I:%M:%S", now);\
	ADD_FAILURE_AT(__FILE__, __LINE__) << "[" << buffer << "] " << message; \
}while(0)

#endif //__GTEST_CUSTOM_H__
