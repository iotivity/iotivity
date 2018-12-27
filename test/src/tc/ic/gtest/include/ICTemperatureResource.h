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

#ifndef SRC_TC_IC_GTEST_INCLUDE_ICTemperatureResource_H_
#define SRC_TC_IC_GTEST_INCLUDE_ICTemperatureResource_H_

#include "ICHelper.h"

using namespace OC;
using namespace std;

class ICTemperatureResource : public ICResource //oic.r.temperature
{
private:
        int m_temperature;
        string m_range;
        string m_units;

public:
    ICTemperatureResource(string uri, vector<string> rt, vector<string> itf);
    void setTemperatureRepresentation(OCRepresentation &rep);
    OCEntityHandlerResult entityHandler(shared_ptr<OCResourceRequest> request);
};
#endif
