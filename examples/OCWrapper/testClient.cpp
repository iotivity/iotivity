//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


#include <WrapperFactory.h>
#include <OCApi.h>
#include <IServerWrapper.h>
#include <IClientWrapper.h>

using namespace OC;

void testClient()
{
	PlatformConfig cfg;
    cfg.ipAddress = "192.168.1.5";
    cfg.port = 8080;
    cfg.mode = ModeType::Client;
    cfg.serviceType = ServiceType::InProc;
	
	IWrapperFactory::Ptr pFactory = std::make_shared<WrapperFactory>();
	IClientWrapper::Ptr pWrapper = pFactory->CreateClientWrapper(cfg);
}

int main()
{
	testClient();
}
