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
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/


#ifndef UTILS_H
#define UTILS_H

#include "yaml-cpp/yaml.h"
#include "ActionType.h"

namespace RAML
{
    namespace Keys
    {
        const std::string Title = "title";
        const std::string Version = "version";
        const std::string BaseUri = "baseUri";
        const std::string Protocols = "protocols";
        const std::string MediaType = "mediaType";
        const std::string Schemas = "schemas";
        const std::string ResourceTypes = "resourceTypes";
        const std::string Traits = "traits";
        const std::string IsTrait = "is";

        const std::string Resource = "/";
        const std::vector<std::string> ActionType = {"get", "post", "put", "delete", "head", "patch", "options", "trace"};

        const std::string Responses = "responses";
        const std::string Body = "body";
        const std::string Schema = "schema";
        const std::string Example = "example";

        const std::string BaseUriParameters = "baseUriParameters";
        const std::string UriParameters = "uriParameters";
        const std::string Headers = "headers";
        const std::string QueryParameters = "queryParameters";
        const std::string FormParameters = "formParameters";
        const std::string DisplayName = "displayName";
        const std::string Description = "description";
        const std::string Type = "type";
        const std::string Enum = "enum";
        const std::string Pattern = "pattern";
        const std::string MinLength = "minLength";
        const std::string MaxLength = "maxLength";
        const std::string Minimum = "minimum";
        const std::string Maximum = "maximum";
        const std::string Repeat = "repeat";
        const std::string Required = "required";
        const std::string Default = "default";

        const std::string Documentation = "documentation";
        const std::string Content = "content";

        const std::string Json = "json";
        const std::vector<std::string> AllowedRamlYamlTypes = {"raml", "yaml", "yml"};

    }


#define READ_NODE_AS_STRING(yamlNode)                   \
({                                                      \
(yamlNode).as<std::string>();                           \
})

#define READ_NODE_AS_INT(yamlNode)                      \
({                                                      \
    (yamlNode).as<int>();                               \
})

#define READ_NODE_AS_LONG(yamlNode)                     \
({                                                      \
    (yamlNode).as<long>();                              \
})
#define READ_NODE_AS_BOOL(yamlNode)                     \
({                                                      \
        (yamlNode).as<bool>();                          \
})

#define GET_ACTION_TYPE(key)                            \
({                                                      \
    ActionType actionType;                              \
    if (key == "get" )                                  \
        actionType = ActionType::GET;                               \
    else if (key == "post" )                            \
        actionType = ActionType::POST;                              \
    else if (key == "put" )                             \
        actionType = ActionType::PUT;                               \
    else if (key == "delete" )                          \
        actionType = ActionType::DELETE;                            \
    else if (key == "head" )                            \
        actionType = ActionType::HEAD;                              \
    else if (key == "patch" )                           \
        actionType = ActionType::PATCH;                             \
    else if (key == "options" )                         \
        actionType = ActionType::OPTIONS;                           \
    else if (key == "trace" )                           \
        actionType = ActionType::TRACE;                             \
    actionType;                                         \
})

}
#endif
