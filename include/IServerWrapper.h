//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef _I_SERVER_WRAPPER_H_
#define _I_SERVER_WRAPPER_H_

#include <memory>
#include <string>

#include <OCReflect.h>

using namespace OC::OCReflect;

namespace OC
{
    class IServerWrapper
    {
    public:
        typedef std::shared_ptr<IServerWrapper> Ptr;

        virtual ~IServerWrapper(){};

        virtual void registerResource(  const std::string& resourceURI, 
                                const std::string& resourceTypeName,
                                named_property_binding_vector& properties) = 0; 
    };
}

#endif
