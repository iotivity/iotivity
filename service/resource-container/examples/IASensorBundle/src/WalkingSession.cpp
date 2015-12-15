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
#include "WalkingSession.h"
#include <iostream>

//using namespace IASensorName;

WalkingSession::WalkingSession()
{
    start = -1;
    stop = -1;
    orientation = "";
    destination = "";
}

WalkingSession::~WalkingSession()
{

}

bool WalkingSession::isComplete()
{
    return !(start < 0 || stop < 0 || 
        orientation.empty() || destination.empty());
}

bool WalkingSession::isWalking()
{
    return start >= 0 && !orientation.empty();
}

void WalkingSession::reset()
{
   start = -1;
   stop = -1;
   orientation = "";
   destination = ""; 
}

void WalkingSession::printStatus()
{
    std::cout << "start_time = " << start << std::endl; 
    std::cout << "orient = " << orientation << std::endl; 
    std::cout << "stop_time = " << stop << std::endl; 
    std::cout << "dest = " << destination << std::endl; 
}
