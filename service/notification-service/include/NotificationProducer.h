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

/**
 * @file
 *
 * This file provides Notification Producer APIs to the developers.
 */

#ifndef NOTIFICATION_PRODUCER_H
#define NOTIFICATION_PRODUCER_H


#include <NotificationUtility.h>
#include <ocstack.h>
#include <oic_string.h>
#include <oic_malloc.h>

#include <string.h>
#include <stdbool.h>
#include <pthread.h>

#include <logger.h>
#include <cJSON.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TAG "NotificationProducer"


    /* Structure to represent the observers */
    typedef struct
    {
        OCObservationId observationId;
        bool valid;
        OCResourceHandle resourceHandle;
    } Observers;

    typedef struct
    {
        OCResourceHandle handle;
    } OCNotifyResource;

    typedef enum
    {
        OC_NOTIFY_TEXT = 1,
        OC_NOTIFY_IMAGE,
        OC_NOTIFY_VIDEO,
    } OCNotificationType;


    typedef enum
    {
        OC_NOTIFY_LOW = 1,
        OC_NOTIFY_MODERATE,
        OC_NOTIFY_CRITICAL,
    } OCNotificationPriority;

    typedef struct
    {
        /** Type of the notification. */
        OCNotificationType type;
        /** Priority of the notification. */
        OCNotificationPriority priority;
        /** Notification generated time stamp  */
        char *timeStamp;
        /** Use UUID for universal uniqueness - used to identify different notifictaions. */
        /** Notification data valid interval.*/
        uint32_t ttl;
        /** senderDetails - resource uri data of the notification sender,
         * resource name etc.. */
        OCStringLL *senderDetails;
    } OCNotificationPayload;

    typedef struct
    {
        /** Message needs to be send in text notification. */
        char *textMessage;
        /** This structure holds the basic notification message. */
        OCNotificationPayload *resources;
    } OCNSTextPayload;

    typedef struct
    {
        /** Image Title */
        char *title;
        /** Image URL. */
        char *imageURL;
        /** This structure holds the basic notification message. */
        OCNotificationPayload *resources;
    } OCNSImagePayload;

    typedef struct
    {
        /** Video URL. */
        char *videoURL;

        /** This structure holds the basic notification message. */
        OCNotificationPayload *resources;
    } OCNSVideoPayload;


    void StartNotificationProducer();
    void StopNotificationProducer();
    void SendTextNotification(const OCNSTextPayload *object);
    void SendImageNotification(const OCNSImagePayload *object);
    void SendVideoNotification(const OCNSVideoPayload *object);


    OCNSVideoPayload *CreateVideoNotification(OCNotificationPriority priority,
            OCStringLL *senderDetails, const char *url);
    OCNSImagePayload *CreateImageNotification(OCNotificationPriority priority,
            OCStringLL *senderDetails, const char *title, const char *url);
    OCNSTextPayload *CreateTextNotification(OCNotificationPriority priority, OCStringLL *senderDetails,
                                            const char *textMessage);
    OCStringLL *CreateSenderDetails();
    void AddSenderDetails(OCStringLL *str, const char *val);



#ifdef __cplusplus
}
#endif


#endif  // NOTIFICATION_PRODUCER_H