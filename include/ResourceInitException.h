//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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

#ifndef _RESOURCE_INIT_EXCEPTION_H_
#define _RESOURCE_INIT_EXCEPTION_H_

#include <stdexcept>
#include <ocstack.h>

namespace OC
{
    class ResourceInitException : public std::exception
    {
    public:
        ResourceInitException(bool missingUri, bool missingType, bool missingInterface)
		: m_missingUri(missingUri), m_missingType(missingType), m_missingInterface(missingInterface)
        {
        }
		
		bool isUriMissing()
		{
			return m_missingUri;
		}
		
		bool isTypeMissing()
		{
			return m_missingType;
		}
		
		bool isInterfaceMissing()
		{
			return m_missingInterface;
		}
		
		std::string Reason()
		{
			std::string ret;
			
			if(isUriMissing())
			{
				ret += "Missing URI;";
			}
			
			if(isTypeMissing())
			{
				ret += "Missing Resource Type;";
			}
			
			if(isInterfaceMissing())
			{
				ret += "Missing Interface;";
			}
			
			return ret;
		}

    private:
        bool m_missingUri;
		bool m_missingType;
		bool m_missingInterface;
    };
}

#endif