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

/// @file Plugin.h

/// @brief


#ifndef __Plugin_H__
#define __Plugin_H__

#define TRUE 1
#define FALSE 0

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#ifdef HAVE_GETTEXT
#include <locale.h>
#endif
#include <assert.h>
#include <cpluff.h>

#include <string>
#include <map>
#include <vector>

namespace OIC
{

    typedef std::string AttributeValue;


    /**
    *   @brief  This class provides information of each plugin installed.
    *
    *
    */
    class Plugin
    {
        public:
            /**
            * Constructor for Plugin.
            *
            */
            Plugin(void);


            /**
            * Virtual destructor.
            */
            virtual ~Plugin(void);

            /**
            * Get unique ID of the plug-in
            *
            * @return unique id string of the pluin. if ID is not exist, it return ""
            */
            std::string getID(void);


            /**
            * Get version of the plug-in
            *
            * @return version information string of the plugin. if Version is not exist, it return ""
            *
            */
            std::string getVersion(void);


            /**
            * Get name of the plug-in
            *
            * @return name string of the plugin. if Name is not exist, it return ""
            */
            std::string getName(void);


            /**
            * Get provider name of the plug-in
            *
            * @return provider name of the plugin. if ProviderName is not exist, it return ""
            *
            */
            std::string getProviderName(void);


            /**
            * Get value for the attributes
            *
            * @param attribute attriubute string to get value
            * @return value of the attribute. if "attribute" is not exist, it return ""
            *
            */
            std::string getValueByAttribute(const std::string attribute);

            /**
            * Check whether the plugin same or not.
            *
            * @param plugin
            * @return true if same, else return false
            *
            */
            bool operator==(Plugin &plugin);

        private:
            friend class CpluffAdapter;
            friend class FelixAdapter;
              /**
            * Set key, value. key is attribute name.
            *
            * @param key is atrribute name.
            * @param value for the attribute.
            * @return void
            *
            */
            void setValue(const std::string key, const std::string value);

            std::map<std::string, AttributeValue> m_attributeMap;
            std::vector<std::string> m_supportedType;
    };

}


#endif //__Plugin_H__
