/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

/*
 * SSMTestApp.h
 */

#ifndef SSMTESTAPP_H_
#define SSMTESTAPP_H_

#include <string>

#include "OCResource.h"
#include "OCPlatform.h"
#include "SSMClient.h"
#include "ISSMClientListener.h"

namespace APPMenu
{
    typedef enum
    {
        NONE = 0, REGISTER, UNREGISTER, DISCOMFORT_SAMPLE, EXIT = 9
    } APPMenu;
}
;

typedef enum
{
    ALL_DISCOMPORT = 2, HALF_DISCOMPORT, LITTLE_DISCOMPORT, ALL_COMPORT
} DIResult;

class SSMTestApp: public ISSMClientListener
{
private:
    SSMClient m_SSMClient;

public:

    SSMTestApp();

    void displayMenu();
	void registerQuery(std::string queryString);
    void unregisterQuery();

    /* operations from listener interface */
    void onRegisterQuery(const AttributeMap& attributeMap, SSMReturn& eCode);

};

#endif /* SSMTESTAPP_H_ */
