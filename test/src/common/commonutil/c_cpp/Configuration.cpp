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

#include <Configuration.h>

Configuration::Configuration(string filePath)
{
    cout << "Configuration" << endl;
    filename = filePath;

    std::string line;
    ifstream myfile(filename);
    if (myfile)
    {
        while (getline(myfile, line))
        {
            size_t index = line.find(EQUAL_SIGN);

            string key = line.substr(0, index);
            string value = line.substr(index + 1, line.length());

            properties[key] = value;

        }

        printContent();

        myfile.close();
    }
    else
    {
        cout << "Unable to Open " << filename << endl;
    }
}

void Configuration::open(string filePath)
{
    cout << "open" << endl;
}

void Configuration::save()
{
    cout << "saving " << filename << "..." << endl;

    propFile.open(filename, std::ofstream::out | std::ofstream::trunc);
    if (!propFile.is_open())
    {
        propFile.open(filename, std::ofstream::out | std::ofstream::trunc);
    }

    for (std::map< string, string >::iterator it = properties.begin(); it != properties.end(); ++it)
    {
        propFile << it->first << "=" << it->second << '\n';
    }

    propFile.close();
}

void Configuration::setProperties(string key, string value)
{
    properties[key] = value;
}

string Configuration::getProperties(string key)
{
    string value = "NULL";

    if (properties.count(key) > 0)
    {
        value = properties[key];
    }
    cout << "{ " <<key << " : " << value << " }"<< endl;
    return value;
}

void Configuration::printContent()
{
    cout << filename << " contains:" << endl;
    for (std::map< string, string >::iterator it = properties.begin(); it != properties.end(); ++it)
    {
        cout << "{ " <<it->first << " : " << it->second << " }"<< endl;
    }
}
