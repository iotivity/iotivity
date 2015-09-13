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

#ifndef RAML_EXCEPTIONS_H_
#define RAML_EXCEPTIONS_H_

#include <exception>
#include "RamlErrorCodes.h"
#include "yaml-cpp/exceptions.h"

namespace RAML
{
    class RamlException : public std::exception
    {
        public:
            RamlException(const std::string &message) : m_message(message) {}
            RamlException(const YAML::Mark &mark, const std::string &message): m_message(message),
                m_mark(mark) {}
            virtual const char *what() const noexcept;
            virtual ~RamlException() throw() {}

        private:
            std::string m_message;
            YAML::Mark m_mark;
    };

    class RamlParserException : public RamlException
    {
        public:
            RamlParserException(const std::string &message): RamlException(message) {}
            RamlParserException(const YAML::Mark &mark, const std::string &message): RamlException(mark,
                        message) {}
    };

    class RamlRepresentationException : public RamlException
    {
        public:
            RamlRepresentationException(const std::string &message): RamlException(message) {}
            RamlRepresentationException(const YAML::Mark &mark, const std::string &message): RamlException(mark,
                        message) {}
    };

    class RamlBadFile : public RamlException
    {
        public:
            RamlBadFile(const std::string &message) : RamlException(message) {}
            RamlBadFile(const YAML::Mark &mark, const std::string &message): RamlException(mark, message) {}
    };
    class JsonException : public RamlException
    {
        public:
            JsonException(const std::string &message) : RamlException(message) {}

    };
}
#endif
