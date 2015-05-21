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

/// @file Config.h

/// @brief

#ifndef __CONFIG_H
#define __CONFIG_H
#include <map>
#include <string>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rapidxml.hpp"
#include <fstream>
#include <vector>
#ifdef ANDROID
#include <jni.h>
#endif

using namespace rapidxml;

namespace OIC
{

    enum PMRESULT
    {
        PM_S_OK
        , PM_S_FALSE
        , PM_E_POINTER
        , PM_E_OUTOFMEMORY
        , PM_E_FAIL
        , PM_E_NOINTERFACE
        , PM_E_NOTIMPL
    };
    /**
    * @brief    Configuration class
    *
    *
    */
    class Config
    {
        public:
            /**
            * A function to register pluins in the path.
            * This function will load plugins in plugin manager table.
            *
            * @param path plugin file path to be registered.
            * @return int, 1 is success, 0 is fail.
            *
            * NOTE:
            *
            */
            /**
            *
            * new Singleton pattern instance.
            *
            * @return config pointer Address.
            */
            static Config *Getinstance(void *args = NULL)
            {
                if (NULL == s_configinstance)
                {
                    s_configinstance = new Config(args);
                }

                return s_configinstance;
            }
            std::string  getPluginPath();
            std::string  getVersion();
            std::string  getValue(const std::string key);

        private:
            static Config *s_configinstance;
            typedef std::map<std::string, std::string> configmap;
            configmap m_configurationMap;

            /**
            * Constructor for Config.
            * During construction time, configuration file  will be loaded.
            *
            */
            Config(void *args = NULL);

            /**
            * Virtual destructor
            */
            virtual ~Config(void);

            /**
            * delete Singleton pattern instance.
            */
            static void deleteinstance()
            {
                if (NULL != s_configinstance)
                {
                    delete s_configinstance;
                    s_configinstance = NULL;
                }
            }
            void setValue(const std::string key, const std::string value);
            PMRESULT loadConfigFile(const std::string configfilepath);
            PMRESULT parsing(char *xmlData, xml_document<> *doc);
            PMRESULT getXmlData(  xml_node<> *pluginInfo, std::string key);
    };
}

#endif
