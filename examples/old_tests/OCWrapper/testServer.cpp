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


#include <WrapperFactory.h>
#include <OCApi.h>
#include <IServerWrapper.h>
#include <IClientWrapper.h>
#include <OCReflect.h>

#include <algorithm>
#include <cstring>
#include <cstdlib>

using namespace OC;
using namespace OC::OCReflect;

// Demo of how to generate OCStack stuff:
void rep_test()
{
	using OC::OCReflect::property_type;
	using OC::OCReflect::named_property_binding;

	named_property_binding_vector sigs {
	       named_property_binding("state", property_type::boolean),
	       named_property_binding("power", property_type::integer),
	};

	using namespace OC::OCReflect::OCStack;

	std::vector<std::string> reps { convert(sigs) };

	for(const auto& r : reps)
	    std::cout << r << '\n';

	char **LEDrep = convert(reps);

	std::for_each(LEDrep, LEDrep + length(LEDrep), [](const char *s) { std::cout << s << '\n'; });




    OCEntityHandler entityHandler;
    OCResourceHandle resourceHandle;

    OCCreateResource(   &resourceHandle, // OCResourceHandle *handl
                        "core.led", // const char * resourceTypeName
                        "state:oc.bt.b;power:oc.bt.i", //const char * resourceTypeRepresentation
                        "core.rw", //const char * resourceInterfaceName
                        OC_REST_GET | OC_REST_PUT, // uint8_t allowedMethods
                        "/a/led", // const char * uri
                        entityHandler, // OCEntityHandler entityHandler
                        OC_DISCOVERABLE | OC_OBSERVABLE // uint8_t resourceProperties
                    );
                                        


 release(LEDrep);
}

void testServer()
{
    PlatformConfig cfg;
    cfg.ipAddress = "192.168.1.5";
    cfg.port = 8080;
    cfg.mode = ModeType::Server;
    cfg.serviceType = ServiceType::InProc;

    IWrapperFactory::Ptr pFactory= std::make_shared<WrapperFactory>();

    IServerWrapper::Ptr pServer = pFactory->CreateServerWrapper(cfg);

    //pServer->bindTo
}

int main()
{
    rep_test();
	testServer();
}
