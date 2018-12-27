/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_


#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

const string COMMON_CONFIG_FILE = "config.properties";
const string TCP_SERVER_ETHERNET_IP_KEY = "ci_server_ethernet_ip";
const string TCP_SERVER_WIFI_IP_KEY = "ci_server_wifi_ip";
const string TCP_SERVER_PORT_KEY = "tcp_server_port";

class Configuration
{
    std::map<string,string> properties;
    string filename;
    std::fstream propFile;
    const string EQUAL_SIGN = "=";

public:
    Configuration(string filePath);

    void open(string filePath);
    void setProperties(string key, string value);
    string getProperties(string key);
    void save();
    void printContent();
};

#endif /* _CONFIGURATION_H_ */
