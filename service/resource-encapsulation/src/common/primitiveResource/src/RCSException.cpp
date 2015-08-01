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

#include <RCSException.h>

#include <OCException.h>

namespace OIC
{
    namespace Service
    {

        RCSException::RCSException()
        {
        }

        RCSException::RCSException(const std::string& what) :
                m_what{ what }
        {
        }

        RCSException::RCSException(std::string&& what) :
                m_what{ std::move(what) }
        {
        }

        RCSException::~RCSException() noexcept
        {
        }

        const char* RCSException::what() const noexcept
        {
            return m_what.c_str();
        }


        PlatformException::PlatformException(OCStackResult reason) :
                RCSException{ "Failed : " + OC::OCException::reason(reason) },
                m_reason { reason }
        {
        }

        OCStackResult PlatformException::getReasonCode() const
        {
            return m_reason;
        }

        std::string PlatformException::getReason() const
        {
            return  OC::OCException::reason(m_reason);
        }


        BadRequestException::BadRequestException(const std::string& what) :
                RCSException{ what }
        {
        }

        BadRequestException::BadRequestException(std::string&& what) :
                RCSException{ std::move(what) }
        {
        }


        InvalidParameterException::InvalidParameterException(const std::string& what) :
                RCSException{ what }
        {
        }

        InvalidParameterException::InvalidParameterException(std::string&& what) :
                RCSException{ std::move(what) }
        {
        }


        BadGetException::BadGetException(const std::string& what) :
                RCSException{ what }
        {
        }

        BadGetException::BadGetException(std::string&& what) :
                RCSException{ std::move(what) }
        {
        }


        InvalidKeyException::InvalidKeyException(const std::string& what) :
                RCSException{ what }
        {
        }

        InvalidKeyException::InvalidKeyException(std::string&& what) :
                RCSException{ std::move(what) }
        {
        }
    }
}
