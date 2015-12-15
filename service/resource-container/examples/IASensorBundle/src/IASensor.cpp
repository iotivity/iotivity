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
#include "IASensor.h" 
#include <thread>
#include <cctype>
#include "web++.hpp"

#ifdef __ANDROID__
#include "OCAndroid.h"
#endif

#define MAX_PATH 2048

using namespace IASensorName; 
std::string m_json_string;

void get_json(WPP::Request* req, WPP::Response* res) {
  std::cout << req->method << " " << req->path << std::endl; 
  std::cout << "Headers:" << std::endl;

  std::map<std::string, std::string>::iterator iter;
  for (iter = req->headers.begin(); iter != req->headers.end(); ++iter) {
    std::cout << iter->first << " = " << iter->second << std::endl; 
  }   

  std::cout << "Query:" << std::endl;

  for (iter = req->query.begin(); iter != req->query.end(); ++iter) {
    std::cout << iter->first << " = " << iter->second << std::endl; 
  }   

  std::cout << "Cookies: " << req->cookies.size() << std::endl;

  for (iter = req->cookies.begin(); iter != req->cookies.end(); ++iter) {
    std::cout << iter->first << " = " << iter->second << std::endl; 
  }   

  res->body << m_json_string; 
}

void start_server() {
  std::cout << "Listening on port 5000" << std::endl; 
  WPP::Server server;

  server.get("/pinplace.json", &get_json);
  server.get("/jquery.js", "./examples/IASensorBundle/IAContainer/jquery.js");
  server.get("/script.js", "./examples/IASensorBundle/IAContainer/script.js");
  server.get("/", "./examples/IASensorBundle/IAContainer/index.html");
  server.all("/dir", "./");
  server.start(5000); 
} 

double IASensor::get_bmr()
{
  if (gender == MALE)
    return 66 + 13.75 * weight + 5 * height - 6.8 * age;
  else
    return 655 + 9.56 * weight + 5 * height - 6.8 * age; 
}


double IASensor::calculate_calorie_burn(double time_in_hours)
{
  return get_bmr() / 24 * MET * time_in_hours; 
} 

IASensor::IASensor()
{
  m_motion = "";
  m_wireless = "";
  m_IAResult = "";
  m_ws = WalkingSession();
  m_locationMap = LocationMapper().getLocationMap();

  std::thread server_thread(start_server); 
  server_thread.detach(); 

  //char gender_input;

  //std::cout << "Input gender (M/F): ";
  //std::cin >> gender_input;
  //gender = toupper(gender_input) == 'M' ? MALE : FEMALE;
  //std::cout << std::endl;

  //std::cout << "Input age: ";
  //std::cin >> age;
  //std::cout << std::endl;

  //std::cout << "Input weight(kg): ";
  //std::cin >> weight;
  //std::cout << std::endl;

  //std::cout << "Input height(cm): ";
  //std::cin >> height;
  //std::cout << std::endl; 

  age = 25;
  weight = 70;
  height = 180;
  gender = MALE; 
}

IASensor::~IASensor()
{

}

int IASensor::executeIASensorLogic(std::map<std::string, std::string> *pInputData, std::string *pOutput)
{
  IAResult result;

  if (pInputData->find("motion") != pInputData->end())
  {
    m_motion = pInputData->at("motion");
  }

  if (pInputData->find("wireless") != pInputData->end())
  {
    m_wireless = pInputData->at("wireless");
  }

  if ((result = makeIA()) != SUCCESS)
  {
    return -1;
  }

  (*pOutput) = m_IAResult;

  return 0;
}

void remove_quotes(std::string & s) 
{
  if(s[0] == '\"' && s[s.size() - 1] == '\"') {
    s.erase(0, 1);
    s.erase(s.size() - 1); 
  }
}

std::vector<std::string> parse_bssid_string(std::string str) {                                                      
  std::vector<std::string> res;
  boost::split(res, str, boost::is_any_of(","));
  return res;
}

void getCurrentPath(std::string *pPath)
{
  char buffer[MAX_PATH];

#if defined(__linux__)
  char *strPath = NULL;
  int length = readlink("/proc/self/exe", buffer, MAX_PATH - 1); 

  if (length != -1) 
  {   
    buffer[length] = '\0';
    strPath = strrchr(buffer, '/');

    if (strPath != NULL)
      *strPath = '\0';
  }   
#endif
  pPath->append(buffer);

}

/**
 * PinPlace  
 */
void IASensor::computePinPlace()
{
  std::string curr_location = m_wireless;;
  time_t curr_time = time(0);

  if (m_motion == NOT_WALKING)
  {
    if (m_ws.isWalking())
    {
      // just stopped walking
      m_ws.destination = m_locationMap[curr_location];
      m_ws.stop = curr_time;
      return;
    }
    else
    {
      // not walking
      m_ws.orientation = m_locationMap[curr_location];
    }
  } 
  else
  {   
    if (!m_ws.isWalking())
    {
      // just start walking
      m_ws.start = curr_time; 
    }
    else
    {
      // keep walking
    }
  }   
}


/**
 * Calculation of IA with Motion&Wireless
 */
IAResult IASensor::makeIA(void)
{
  WalkingSession ws;
  if (!m_motion.empty() && !m_wireless.empty())
  {
    remove_quotes(m_motion);
    remove_quotes(m_wireless);

    computePinPlace();

    if (m_ws.isComplete())
    {
      std::string json_data;
      struct tm * timeinfo;
      char buffer[30];
      struct tm * timeinfo2;
      char buffer2[30];

      timeinfo = localtime(&m_ws.start);
      strftime(buffer,30,"%F %T",timeinfo);
      timeinfo2 = localtime(&m_ws.stop);
      strftime(buffer2,30,"%F %T",timeinfo2);

      // WalkingSession and EnergyConsumption to JSON string
      int time_difference = m_ws.stop - m_ws.start; 
      double time_in_hours = time_difference / 3600.0; 

      json_data = toJSON(m_ws, calculate_calorie_burn(time_in_hours));
      std::cout << json_data << std::endl; 
      m_json_string = json_data; 
      m_ws.reset();
    }

    return SUCCESS;
  }

  return ERROR;
}

