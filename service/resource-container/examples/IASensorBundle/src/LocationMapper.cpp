//******************************************************************
//
// Copyright 2015 Han Joo Chae, Jae Ho Jeon (Seoul National University) All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/**
 * This file contains the exported symbol.
 */
#include "LocationMapper.h"
#include <iostream>

using namespace IASensorName;

LocationMapper::LocationMapper()
{

}

LocationMapper::~LocationMapper()
{

}

std::map<std::string, std::string> LocationMapper::getLocationMap()
{
    std::string loc1 = "05:17:A5:A3:FD:B6,60:14:1E:E7:BB:63,72:43:BB:67:02:A9,3E:17:1B:D5:19:36,82:CB:B0:3E:A3:D5"; // office_cubicle
    std::string loc2 = "BA:98:FD:3B:6D:24,7B:7D:6C:52:1F:9B,4D:55:9C:9C:90:EA,91:75:7A:0F:8D:0A,07:9A:62:BE:38:56"; // office_conference
    std::string loc3 = "CE:22:CD:8D:38:F1,6B:94:E7:1D:C9:04,CC:33:10:32:E7:EA,9D:63:05:5F:C9:D5,A7:59:FD:98:C1:7D"; // office_bathroom
    std::string loc4 = "0A:B4:4D:9F:58:32,5D:76:DD:96:06:39,47:28:95:92:BC:59,8A:76:EC:43:9A:6E,2B:4F:29:79:2A:46"; // office_cafeteria

    std::map<std::string, std::string> map;
    map[loc1] = "office_cubicle";
    map[loc2] = "office_conference";
    map[loc3] = "office_bathroom";
    map[loc4] = "office_cafeteria";

    return map;
}
