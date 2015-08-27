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

#ifndef RAML_PARSER_H
#define RAML_PARSER_H

#include "yaml-cpp/yaml.h"
#include "Raml.h"
#include "Utils.h"
#include "RequestResponseBody.h"
#include "RamlResource.h"
#include "Action.h"
#include "Response.h"
#include <map>

namespace RAML
{
    class RamlParser
    {
        private:
            void setDataFromRoot();
            void setBodyDefaultMediaType(std::map<std::string, RamlResource> resource);
            void setBodySchema(std::map<std::string, RamlResource> resource);
            void setTypes(std::map<std::string, RamlResource> resource);
            void setTraits(std::map<std::string, RamlResource> resource);

        public:
            virtual RamlPtr build();
            virtual RamlPtr getRamlPtr();
            RamlParser(): m_ramlPtr(new Raml()) {}
            RamlParser(std::string &fileLocation,
                       std::string &ramlName): m_ramlPtr(new Raml(fileLocation)) ,
                m_fileLocation(fileLocation) , m_ramlName(ramlName) {}
        private:

            RamlPtr m_ramlPtr;
            std::string m_fileLocation;
            std::string m_ramlName;
    };

}
#endif
