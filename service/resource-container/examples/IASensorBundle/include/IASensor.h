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

#ifndef IASENSOR_H_
#define IASENSOR_H_

#include "WalkingSession.h"
#include "LocationMapper.h"
#include "SimpleJSON.h"

#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <map>
#include <time.h>

#define NOT_WALKING "0"
#define WALKING     "1"
#define MET 2.3 // SLOW WALKING

namespace IASensorName
{
  typedef enum
  { SUCCESS = 0, ERROR } IAResult;

  typedef enum
  { MALE, FEMALE } GENDER;

  class IASensor
  {
    public:
      IASensor();
      ~IASensor();

      int executeIASensorLogic(std::map<std::string, std::string> *pInputData,
          std::string *pOutput);
      IAResult makeIA(void);
      void computePinPlace();
      double get_bmr();
      double calculate_calorie_burn(double time_in_hours);

    private:
      std::string m_motion;
      std::string m_wireless;
      std::string m_IAResult;

      int age;
      int weight;
      int height;
      GENDER gender;

      WalkingSession m_ws;
      std::map<std::string, std::string> m_locationMap;
  };
};

#endif /* IASENSOR_H_ */
