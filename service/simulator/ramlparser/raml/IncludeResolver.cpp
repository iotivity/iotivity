/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *		http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include "IncludeResolver.h"

namespace RAML
{

    YAML::Node IncludeResolver::readToYamlNode(const YAML::Node &yamlFile )
    {
        std::string value = yamlFile.as<std::string>();
        YAML::Node yamlInclueNode = YAML::LoadFile(value);
        return yamlInclueNode;
    }

    IncludeResolver::FileType IncludeResolver::getFileType(const YAML::Node &yamlNode )
    {
        IncludeResolver::FileType fileType = IncludeResolver::FileType::NOTAG;
        if (yamlNode.Tag() == "!include")
        {
            std::string value = yamlNode.as<std::string>();
            std::size_t found = value.find_last_of(".");
            if (found > value.length())
            {
                fileType = IncludeResolver::FileType::FILE;
                return fileType;
            }
            std::string extension = value.substr(found + 1);
            if (std::find(Keys::AllowedRamlYamlTypes.begin(), Keys::AllowedRamlYamlTypes.end(),
                          extension) != Keys::AllowedRamlYamlTypes.end())
                fileType = IncludeResolver::FileType::NODE;
            else if (extension == Keys::Json)
                fileType = IncludeResolver::FileType::JSON;
            else
                fileType = IncludeResolver::FileType::FILE;
        }
        return fileType;
    }

    cJSON *IncludeResolver::readToJson(const YAML::Node &jsonFile)
    {
        return cJSON_Parse(readFromFile(jsonFile).c_str());
    }

    std::string IncludeResolver::readFromFile(const YAML::Node &file )
    {
        std::string val = file.as<std::string>();
        std::ifstream fin((m_path + val).c_str());
        if (!fin)
            throw std::runtime_error("Error Include File not present ");
        std::stringstream buffer;
        buffer << fin.rdbuf();
        return buffer.str();
    }

}
