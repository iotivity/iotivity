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

#ifndef RES_MANIPULATION_PRIMITIVEEXCEPTION_H
#define RES_MANIPULATION_PRIMITIVEEXCEPTION_H

#include <string>

#include <octypes.h>

namespace OIC
{
    namespace Service
    {

        class PrimitiveException: public std::exception
        {
        public:
            PrimitiveException() {}
            explicit PrimitiveException(const std::string& what) : m_what{ what } {}
            explicit PrimitiveException(std::string&& what) : m_what{ std::move(what) } {}

            virtual ~PrimitiveException() noexcept {}

            virtual const char* what() const noexcept
            {
                return m_what.c_str();
            }

        private:
            std::string m_what;
        };

        class PlatformException: public PrimitiveException
        {
        public:
            explicit PlatformException(OCStackResult reason);

            OCStackResult getReasonCode() const;
            std::string getReason() const;

        private:
            OCStackResult m_reason;
        };
    }
}

#endif // RES_MANIPULATION_PRIMITIVEEXCEPTION_H
