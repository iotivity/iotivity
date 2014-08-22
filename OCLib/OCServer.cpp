//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

#include "OCServer.h"
#include "OCReflect.h"
#include "OCProperties.h"

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

    // Method and property wrangling:
    void OCServer::bind(const OC::OCReflect::method_binding& mb)
    {
        method_bindings[mb.name] = mb;           
    }

    void OCServer::bind(const OC::OCReflect::property_binding& pb)
    {
std::cout << "OCServer::bind() property " << std::get<0>(pb) << '\n';
        property_bindings[std::get<0>(pb)] = pb;
    }
}
