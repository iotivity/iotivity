//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef _OUT_OF_PROC_SERVER_WRAPPER_H_
#define _OUT_OF_PROC_SERVER_WRAPPER_H_

#include <OCApi.h>

#include <OCReflect.h>

using namespace OC::OCReflect;

namespace OC
{
    class OutOfProcServerWrapper : public IServerWrapper
    {
    public:
        OutOfProcServerWrapper(PlatformConfig cfg) {};

        void registerResource( const std::string& resourceURI,
                                                    const std::string& resourceTypeName,
                                                    named_property_binding_vector properties)
        {
        }

    };
}

#endif
