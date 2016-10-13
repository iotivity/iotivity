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

#ifndef CloudResourceHelper_H_
#define CloudResourceHelper_H_
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
#include "rd_client.h"

#include "OCApi.h"
#include "OCPlatform.h"
#include "OCPlatform_impl.h"

#include "CommonUtil.h"
#include "IotivityTest_Logger.h"

using namespace OC;
using namespace std;

const int TCP_RESPONSE_OK = 200;

class Resource
{
public:
    OCResourceHandle m_handle;
    Resource(string uri, vector< string > rt, vector< string > itf);
    string getResourceUri();
    vector< string > getResourceType();
    vector< string > getInterfaces();
    OCRepresentation getRepresentation();
    OCStackResult addChildResource(Resource *childResource);
    OCStackResult sendRepresentation(std::shared_ptr< OCResourceRequest > pRequest);
    OCStackResult propagate();
    virtual OCEntityHandlerResult entityHandler(std::shared_ptr< OCResourceRequest > request) = 0;

protected:
    OCRepresentation m_representation;
    vector< Resource * > m_childResources;
    ObservationIds m_interestedObservers;
};

#endif
