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
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/

#include "NSCppUtility.h"

string NSCppUtility::getSyncType(OIC::Service::NSSyncInfo::NSSyncType type)
{
    switch (type) {
        case OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_UNREAD:
            return "UNREAD";

        case OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_READ:
            return "READ";
        case OIC::Service::NSSyncInfo::NSSyncType::NS_SYNC_DELETED:
            return "DELETED";

        default:
            return "UNKNOWN";
    }
}

void NSCppUtility::printSyncInfo(OIC::Service::NSSyncInfo info)
{
    try {
        IOTIVITYTEST_LOG(INFO, "SyncInfo --> ProviderID: %s, MessageID: %d, State: %s",
                info.getProviderId().c_str(), info.getMessageId(), getSyncType(info.getState()).c_str());
    } catch (exception &e) {
        IOTIVITYTEST_LOG(WARNING, "Can't print syncInfo. Exception: %s", e.what());
    }
}

void NSCppUtility::printMessage(OIC::Service::NSMessage message)
{
    string log = "\n\tMessageID: " + to_string(message.getMessageId());
    log += "\n\tTitle = " + message.getTitle();
    log += "\n\tBody = " + message.getContentText();
    log += "\n\tTopic = " + message.getTopic();
    log += "\n\tProviderID = " + message.getProviderId();
    IOTIVITYTEST_LOG(INFO, "The Detailed Message is: %s", log.c_str());
}

OIC::Service::NSTopic NSCppUtility::getDesiredTopic(shared_ptr<OIC::Service::NSTopicsList> topicList, string topicID)
{
    OIC::Service::NSTopic desiredTopic;
    for (OIC::Service::NSTopic topic : topicList->getTopicsList() )
    {
        if (topic.getTopicName().compare(topicID) == 0)
        {
            desiredTopic = topic;
            break;
        }
    }
    return desiredTopic;
}

string NSCppUtility::getResultString(OIC::Service::NSResult result)
{
    switch (result)
    {
        case OIC::Service::NSResult::OK:
            return "OK";

        case OIC::Service::NSResult::ERROR:
            return "ERROR";

        case OIC::Service::NSResult::SUCCESS:
            return "SUCCESS";

        case OIC::Service::NSResult::FAIL:
            return "FAIL";

        default:
            return "UNKNOWN";
    }
}
