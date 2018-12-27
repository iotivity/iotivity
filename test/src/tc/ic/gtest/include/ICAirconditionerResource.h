/******************************************************************
*
* Copyright 2017 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#ifndef ICAirconditionerResource_H_
#define ICAirconditionerResource_H_

#include <memory>
#include <iostream>
#include <stdexcept>
#include <condition_variable>
#include <map>
#include <vector>
#include <string>
#include <unistd.h>

#include "ocstack.h"
#include "ocpayload.h"
#include "rd_client.h"
#include "rd_server.h"
#include "rd_database.h"

#include <OCApi.h>
#include <OCPlatform.h>
#include "ICHelper.h"

using namespace OC;
using namespace std;

class ICAirconditionerResource : public ICResource //oic.d.airconditioner
{
public:
    ICAirconditionerResource(string uri, vector<string> rt, vector<string> itf);
    OCEntityHandlerResult entityHandler(shared_ptr<OCResourceRequest> request);
    static void foundAirconditionerResource(shared_ptr<OC::OCResource> resource);
    static void ICAirconditionerResource::onObserve(const HeaderOptions /*headerOptions*/, const OCRepresentation &rep,
                   const int &eCode, const int &sequenceNumber);
};
#endif
