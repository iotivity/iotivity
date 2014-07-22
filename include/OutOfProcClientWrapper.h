//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef _OUT_OF_PROC_CLIENT_WRAPPER_H_
#define _OUT_OF_PROC_CLIENT_WRAPPER_H_

#include <OCApi.h>

namespace OC
{
    class OutOfProcClientWrapper : public IClientWrapper
    {
    public:
        OutOfProcClientWrapper(PlatformConfig cfg) { }
	virtual int ListenForResource(const std::string& serviceUrl, const std::string& resourceType, std::function<void(OCResource::Ptr)>& callback) {return 0;}
    };
}

#endif
