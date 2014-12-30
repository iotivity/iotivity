//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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
#ifndef OICPLATFORMCONFIG_H_
#define OICPLATFORMCONFIG_H_

#include "OCApi.h"
#include "OCPlatform.h"
#include "OCResource.h"
#include "OCResourceRequest.h"
#include "OCResourceResponse.h"
#include "ocstack.h"

using namespace OC;
using namespace OCPlatform;

class OICPlatformConfig
{
    private:

        OICPlatformConfig();
        ~OICPlatformConfig();

        static OICPlatformConfig *s_instance;
        static mutex s_mutexForCreation;
        static PlatformConfig s_cfg;

    public:

        void initialize();
        static OICPlatformConfig *getInstance();
        void setIP(std::string ipaddress);
};

#endif /* OICPLATFORMCONFIG_H_ */
