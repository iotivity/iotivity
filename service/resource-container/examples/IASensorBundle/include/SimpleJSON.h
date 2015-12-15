//******************************************************************
////
//// Copyright 2015 Han Joo Chae, Jae Ho Jeon (Seoul National University) All Rights Reserved.
////
////-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
////
//// Licensed under the Apache License, Version 2.0 (the "License");
//// you may not use this file except in compliance with the License.
//// You may obtain a copy of the License at
////
////      http://www.apache.org/licenses/LICENSE-2.0
////
//// Unless required by applicable law or agreed to in writing, software
//// distributed under the License is distributed on an "AS IS" BASIS,
//// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//// See the License for the specific language governing permissions and
//// limitations under the License.
////
////-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef SIMPLEJSON_H_
#define SIMPLEJSON_H_

#include "WalkingSession.h"

#include <string>
#include <map>
#include <time.h>
#include <iostream>
#include <fstream>

std::string toJSON(WalkingSession & ws, double calorie_burn);
std::string getFormattedTime(time_t & raw_time);
void addVariable(std::string & json, std::string key, std::string value, bool isLast);
bool WriteToFile(std::string & filename, std::string & json_string);
bool ReadFromFile(std::string & filename);

#endif /* SIMPLEJSON_H_ */
