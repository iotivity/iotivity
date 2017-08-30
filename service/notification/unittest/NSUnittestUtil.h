//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include <stdio.h>
#include "ocpayload.h"
#include "NSCommon.h"

void removeMessage(NSMessage * msg)
{
    if (msg == NULL)
    {
        return;
    }

    msg->messageId = 0;
    if (msg->title != NULL)
    {
        free(msg->title);
    }
    if (msg->contentText != NULL)
    {
        free(msg->contentText);
    }
    if (msg->sourceName != NULL)
    {
        free(msg->sourceName);
    }
    if (msg->dateTime != NULL)
    {
        free(msg->dateTime);
    }
    if (msg->topic != NULL)
    {
        free(msg->topic);
    }

    if (msg->mediaContents != NULL)
    {
        free(msg->mediaContents->iconImage);
        free(msg->mediaContents);
    }

    if (msg->extraInfo != NULL)
    {
        OCRepPayloadDestroy(msg->extraInfo);
        msg->extraInfo = NULL;
    }

    free(msg);
}

void removeTopics(NSTopicLL * topics)
{
    NSTopicLL * iter = topics;
    NSTopicLL * following = NULL;

    while (iter)
    {
        following = iter->next;

        free(iter->topicName);
        iter->next = NULL;

        free(iter);

        iter = following;
    }
}
