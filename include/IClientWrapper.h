//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef _I_CLIENT_WRAPPER_H_
#define _I_CLIENT_WRAPPER_H_

#include <memory>
#include <string>

#include <OCApi.h>
#include <OCResource.h>

namespace OC
{
	class OCResource;
    class IClientWrapper
    {
    public:
        typedef std::shared_ptr<IClientWrapper> Ptr;

        virtual int ListenForResource(const std::string& serviceUrl, const std::string& resourceType,
            std::function<void(OCResource::Ptr)>& callback) = 0;
        virtual ~IClientWrapper(){}

    private:
    };
}

#endif
