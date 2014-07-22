//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "OCServer.h"
#include "OCReflect.h"

namespace OC {
	OCServer::OCServer(void)
	{
	}


	OCServer::~OCServer(void)
	{
	}

	void OCServer::setSecurityModel(OCSecurityModel model) {
	}

	void OCServer::registerResource(OCObject *object, std::string url/* TODO: , AccessControl accessControl */) {
	}

	void OCServer::unregisterResource(OCObject *object) {
	}

	void OCServer::start() {
	}

	void OCServer::stop() {
	}

    void OCServer::bind(const OC::OCReflect::method_binding& mb)
    {
        method_bindings[mb.name] = mb;            
    }
}
