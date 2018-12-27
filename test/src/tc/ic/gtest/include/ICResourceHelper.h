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

#ifndef SRC_TC_IC_GTEST_INCLUDE_ICRESOURCEHELPER_H_
#define SRC_TC_IC_GTEST_INCLUDE_ICRESOURCEHELPER_H_

#include "ICHelper.h"

class ICResourceHelper
{
private:
    static ICResourceHelper* s_ICResourceHelper;

public:
    ICResourceHelper(void);
    ~ICResourceHelper(void);
    bool initCloudClient();
    static std::mutex s_mutex;
    static bool s_isCreateTopicSuccess;
    static bool s_isDiscoverTopicSuccess;
    static bool s_isPublishTopicSuccess;
    static bool s_isrequestTopicPublishSuccess;
    static bool s_isSubscribeSuccess;
    static bool s_isUnsubscribeSuccess;

    static ICResourceHelper* getInstance(void);
    static vector< shared_ptr< OC::OCResource > > s_TopicList;
    static void createTopicCB(const int ecode, const string &originUri,
            shared_ptr< OC::OCResource > topic);
    static void discoverTopicCB(const int ecode, const string &brokerUri,
            shared_ptr< OC::OCResource > topic);
    static void requestMQPublishCB(const HeaderOptions&, const OCRepresentation&, const int);
    static void publishMessageCB(const HeaderOptions &, const OCRepresentation &, const int eCode);
    static void subscribeCB(const HeaderOptions &, const OCRepresentation &rep, const int eCode,
            const int sequenceNumber);
};
#endif
