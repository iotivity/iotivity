//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include "ResourceContainerInternal.h"
#include "BundleActivator.h"
#include <dlfcn.h>
#include <iostream>

#include <dlfcn.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>


using namespace std;

static inline std::string trim_both(const std::string &str)
{
    int npos = str.find_first_not_of(" \t\v\n\r");

    if (npos == -1)
    {
        return "";
    }

    unsigned int n = (unsigned int) npos;
    std::string tempString = n == std::string::npos ? str : str.substr(n, str.length());

    n = tempString.find_last_not_of(" \t\v\n\r");

    return n == std::string::npos ? tempString : tempString.substr(0, n + 1);
}

namespace RC {

	ResourceContainerInternal::ResourceContainerInternal() {
		// TODO Auto-generated constructor stub

	}

	ResourceContainerInternal::~ResourceContainerInternal() {
		// TODO Auto-generated destructor stub
	}

	void ResourceContainerInternal::init() {

	}

	// loads the bundle
	void ResourceContainerInternal::registerBundle(BundleInfo* bundleInfo) {

		cout << "\nRegistering bundle: " << bundleInfo->getPath() << "\n";
		char* error;

		typedef void (*activator_t)(ResourceContainerInternal *);
		activator_t bundleActivator = NULL;

		//sstream << bundleInfo.path << std::ends;

		void *hModule = NULL;
		hModule = dlopen(bundleInfo->getPath().c_str(), RTLD_LAZY);

		if (hModule != NULL) {
			bundleActivator = (activator_t) dlsym(hModule,
					"externalActivateBundle");
			if ((error = dlerror()) != NULL) {
				fprintf(stderr, "%s\n", error);
			}
		} else {
			if ((error = dlerror()) != NULL) {
				fprintf(stderr, "%s\n", error);
			}
		}

		if (bundleActivator != NULL) {
			bundleActivator(this);
		} else {
			//Unload module and return error
			printf("load unsuccessful.\n");
		}
	}

	void ResourceContainerInternal::activateBundle(string id) {

	}

	void ResourceContainerInternal::activateBundleByName(string name) {

	}

	void ResourceContainerInternal::deactivateBundle(string id) {

	}

	void ResourceContainerInternal::deactivateBundleByName(string id) {

	}

	vector<Resource*> ResourceContainerInternal::listBundleResources(string id) {

	}

	void ResourceContainerInternal::registerResource(Resource* resource) {

	}

	void ResourceContainerInternal::unregisterResource(Resource* resource) {

	}

	 ConfigParam getConfiguration(string configKey) {}

	    ConfigParam ResourceContainerInternal::getConfiguration(ConfigKey configKey, string id)
	    {
	        string pathXMLFile;
	        string strConfigData;
	        ConfigParam configOutput;

	        rapidxml::xml_document< char > xmlDoc;

	        rapidxml::xml_node< char > *root;
	        rapidxml::xml_node< char > *item;
	        rapidxml::xml_node< char > *subItem;

	        string strKey, strValue;

	        // TODO: temporary path and config file name
	        getCurrentPath(&pathXMLFile);
	        pathXMLFile.append("/");
	        pathXMLFile.append("ResourceContainerConfig.xml");

	        getConfigDocument(pathXMLFile, &strConfigData);

	        try
	        {
	            xmlDoc.parse< 0 >((char *) strConfigData.c_str());

	            // <container>
	            root = xmlDoc.first_node();

	            if (!root)
	            {
	                throw rapidxml::parse_error("No Root Element", 0);
	            }

	            switch (configKey)
	            {
	                case CONFIG_COMMON:
	                    for (item = root->first_node(); item; item = item->next_sibling())
	                    {
	                        strKey = item->name();
	                        strValue = item->value();

	                        if (!strKey.compare("config"))
	                        {
	                            for (subItem = item->first_node(); subItem; subItem = subItem->next_sibling())
	                            {
	                                strKey = subItem->name();
	                                strValue = subItem->value();

	                                map < string, string > mapCommonConfig;
	                                mapCommonConfig[trim_both(strKey)] = trim_both(strValue);

	                                configOutput.push_back(mapCommonConfig);
	                            }
	                            break;
	                        }
	                    }
	                    break;

	                case CONFIG_BUNDLES:
	                    getConfigBundleData(root, id, &configOutput);
	                    break;

	                case CONFIG_RESOURCES:
	                    getConfigResourceData(root, id, &configOutput);
	                    break;

	                default:
	                    assert(0);
	            }
	        }
	        catch (rapidxml::parse_error &e)
	        {
	            cout << "xml parsing failed !!" << endl;
	            cout << e.what() << endl;
	        }

	        return configOutput;
	    }

	    void ResourceContainerInternal::getConfigDocument(std::string pathConfigFile,
	            std::string *pConfigData)
	    {
	        std::basic_ifstream< char > xmlFile(pathConfigFile.c_str());

	        if (!xmlFile.fail())
	        {
	            xmlFile.seekg(0, std::ios::end);
	            unsigned int size = (unsigned int)xmlFile.tellg();
	            xmlFile.seekg(0);

	            std::vector< char > xmlData(size + 1);
	            xmlData[size] = 0;

	            xmlFile.read(&xmlData.front(), (std::streamsize) size);
	            xmlFile.close();
	            *pConfigData = std::string(xmlData.data());
	        }
	        else
	        {
	            std::cout << "Configuration File load failed !!" << std::endl;
	        }
	    }

	    void ResourceContainerInternal::getConfigBundleData(rapidxml::xml_node< char > *rootData,
	            string bundleId, ConfigParam *pConfigOutput)
	    {
	        rapidxml::xml_node< char > *item;
	        rapidxml::xml_node< char > *subItem;

	        string strKey, strValue;

	        try
	        {
	            for (item = rootData->first_node(); item; item = item->next_sibling())
	            {
	                strKey = item->name();
	                strValue = item->value();

	                // <bundle>
	                if (!strKey.compare("bundle"))
	                {
	                    for (subItem = item->first_node(); subItem; subItem = subItem->next_sibling())
	                    {
	                        strKey = subItem->name();
	                        strValue = subItem->value();

	                        if (!bundleId.empty())
	                        {
	                            if (!strKey.compare("bundleID") && strValue.compare(bundleId))
	                                break;
	                        }

	                        // bundle info except resource data
	                        if (strKey.compare("resources"))
	                        {
	                            map < string, string > mapBundleConfig;
	                            mapBundleConfig[trim_both(strKey)] = trim_both(strValue);

	                            pConfigOutput->push_back(mapBundleConfig);
	                        }
	                    }
	                }
	            }
	        }
	        catch (rapidxml::parse_error &e)
	        {
	            cout << "xml parsing failed !!" << endl;
	            cout << e.what() << endl;
	        }
	    }

	    void ResourceContainerInternal::getConfigResourceData(rapidxml::xml_node< char > *rootData,
	            string bundleId, ConfigParam *pConfigOutput)
	    {
	        rapidxml::xml_node< char > *item;
	        rapidxml::xml_node< char > *subItem;
	        rapidxml::xml_node< char > *subItem2;
	        rapidxml::xml_node< char > *subItem3;

	        string strKey, strValue;

	        try
	        {
	            for (item = rootData->first_node(); item; item = item->next_sibling())
	            {
	                strKey = item->name();
	                strValue = item->value();

	                // <bundle>
	                if (!strKey.compare("bundle"))
	                {
	                    for (subItem = item->first_node(); subItem; subItem = subItem->next_sibling())
	                    {
	                        strKey = subItem->name();
	                        strValue = subItem->value();

	                        if (!strKey.compare("bundleID") && strValue.compare(bundleId))
	                            break;

	                        // <resources>
	                        if (!strKey.compare("resources"))
	                        {
	                            for (subItem2 = subItem->first_node(); subItem2; subItem2 = subItem2->next_sibling())
	                            {
	                                strKey = subItem2->name();
	                                strValue = subItem2->value();

	                                // <resourceInfo>
	                                if (!strKey.compare("resourceInfo"))
	                                {
	                                    map <string, string> mapResourceInfo;

	                                    for (subItem3 = subItem2->first_node(); subItem3; subItem3 = subItem3->next_sibling())
	                                    {
	                                        strKey = subItem3->name();
	                                        strValue = subItem3->value();

	                                        mapResourceInfo[trim_both(strKey)] = trim_both(strValue);
	                                    }

	                                    pConfigOutput->push_back(mapResourceInfo);
	                                }
	                            }
	                        }
	                    }
	                }
	            }
	        }
	        catch (rapidxml::parse_error &e)
	        {
	            cout << "xml parsing failed !!" << endl;
	            cout << e.what() << endl;
	        }
	    }

	    void ResourceContainerInternal::getCurrentPath(std::string *path)
	    {
	        char        buffer[2048];
	        char    *strPath = NULL;

	        int length = readlink("/proc/self/exe", buffer, 2047);

	        buffer[length] = '\0';

	        strPath = strrchr(buffer, '/');

	        *strPath = '\0';

	        path->append(buffer);
	    }
}
