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
#include "SimpleJSON.h"
#include <iostream>
#include <sstream>
#include <boost/property_tree/json_parser.hpp> 

std::string toJSON(WalkingSession & ws, double calorie_burn)
{ 
  boost::property_tree::ptree root;
  boost::property_tree::ptree walking_session;
  boost::property_tree::ptree energy_consumption;

  walking_session.put("start", getFormattedTime(ws.start));
  walking_session.put("stop", getFormattedTime(ws.stop));
  walking_session.put("orientation", ws.orientation);
  walking_session.put("destination", ws.destination);

  energy_consumption.put("calorie_burn", calorie_burn);

  root.add_child("walking_session", walking_session); 
  root.add_child("energy_consumption", energy_consumption); 

  std::ostringstream json;
  boost::property_tree::json_parser::write_json(json, root);
  //std::cout << json.str() << std::endl; 

  return json.str();
}

std::string getFormattedTime(time_t & raw_time)
{
  struct tm * timeinfo;
  char buffer[30];

  timeinfo = localtime(&raw_time);
  strftime(buffer,30,"%F %T",timeinfo);

  return std::string(buffer);
}
