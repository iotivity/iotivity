//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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