/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/

#ifndef BinarySwitchResourceHelper_H_
#define BinarySwitchResourceHelper_H_

#include <memory>
#include <iostream>
#include <stdexcept>
#include <condition_variable>
#include <map>
#include <vector>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <logger.h>
#include "ocstack.h"
#include "ocpayload.h"
#include "oicresourcedirectory.h"
#include "OCApi.h"
#include "OCPlatform.h"
#include "OCPlatform_impl.h"

#include <IotivityTest_Logger.h>
#include <CloudResourceHelper.h>

using namespace OC;
using namespace std;

class BinarySwitchResource: public Resource //oic.r.switch.binary
{
private:
    bool m_value;

public:
    BinarySwitchResource(string uri, vector< string > rt, vector< string > itf);
    void setBinarySwitchRepresentation(OCRepresentation &rep);
    OCEntityHandlerResult entityHandler(std::shared_ptr< OCResourceRequest > request);
};

#endif
