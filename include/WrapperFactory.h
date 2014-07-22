//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef _WRAPPER_FACTORY_H_
#define _WRAPPER_FACTORY_H_

#include <memory>
#include <OCApi.h>
#include "IClientWrapper.h"
#include "IServerWrapper.h"
#include <OutOfProcClientWrapper.h>
#include <InProcClientWrapper.h>
#include <OutOfProcServerWrapper.h>
#include <InProcServerWrapper.h>

namespace OC
{
    // Interface to permit easier mocking/unit testing later
    class IWrapperFactory
    {
    public:
        typedef std::shared_ptr<IWrapperFactory> Ptr;

        virtual IClientWrapper::Ptr CreateClientWrapper(PlatformConfig cfg) =0;
        virtual IServerWrapper::Ptr CreateServerWrapper(PlatformConfig cfg) =0;
        virtual ~IWrapperFactory(){}
    };

    // Class to create the client/server object!
    class WrapperFactory : public IWrapperFactory
    {
    public:
        WrapperFactory(){}

        virtual IClientWrapper::Ptr CreateClientWrapper(PlatformConfig cfg)
        {
            switch(cfg.serviceType)
            {
            case ServiceType::InProc:
                return std::make_shared<InProcClientWrapper>(cfg);
                break;
            case ServiceType::OutOfProc:
                return std::make_shared<OutOfProcClientWrapper>(cfg);
                break;
            }
			return nullptr;
        }

        virtual IServerWrapper::Ptr CreateServerWrapper(PlatformConfig cfg)
        {
            switch(cfg.serviceType)
            {
            case ServiceType::InProc:
                return std::make_shared<InProcServerWrapper>(cfg);
                break;
            case ServiceType::OutOfProc:
              //  return std::make_shared<OutOfProcServerWrapper>(cfg);
                break;
            }
			return nullptr;
        }

        virtual ~WrapperFactory(){}
    };
}

#endif
