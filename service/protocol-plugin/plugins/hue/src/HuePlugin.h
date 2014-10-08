//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

/// @file HuePlugin.h

/// @brief

#ifndef __HUEPlUGIN_H__
#define __HUEPlUGIN_H__
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "OCResourceDesc.h"

namespace OIC
{
    class HuePlugin
    {
        public:
            HuePlugin();
            virtual ~HuePlugin();

            /**
            * Search Resources by philips hue protocol.
            *
            *@return Return a vector, which members are ResourceDesc type
            *
            */
            std::vector<OCResourceDesc> SearchDeviceResources(std::vector<string> &pool);

            /**
            * Read config file to fetch the Hue bridge search info
            *
            *@return Return a vector, which members are hue url type
            *
            */
//           std::vector<string> ReadConfig(const char* filepath);

            /**
            *  convert received json object to we defined resource info struct
            */
            void ConvertOcRes(string &hueRes,  std::map<string, string> &dictmap);

            /**
            *  create a local xml file to mark the resources information
            *@resources        Resources info
            *@template_file    the xml file
            *
            */
            void CreateResourcesFile(std::vector<OCResourceDesc> &resources, string &template_file);

            /**
            *  fetch Resources change information
            *@param oldR  the vector for the last search result
             @param newR  the vector for the current search result
             @param comm  the vector for both oldR and newR contain
             @return   the map for changed resrouces
            */
            std::map<string, string> ResourcesChange(std::vector<string> &oldR, std::vector<string> &newR,
                    std::vector<string> &comm);
        private:
            std::vector<string> res_pool;
    };
}
#endif //__HUEPlUGIN_H__
