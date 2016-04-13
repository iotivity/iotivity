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

#include<stdio.h>

#include <NotificationProducer.h>

pthread_t threadId_process;

int gResourceUnderObservation = 0;
uint8_t gNumberOfIds=0;

Observers interestedObservers[SAMPLE_MAX_NUM_OBSERVATIONS];
OCObservationId gObsIdList[SAMPLE_MAX_NUM_OBSERVATIONS];

OCQualityOfService gQos;

OCNotifyResource notifyResource;
bool beExit = false;


// Entity handler should check for resourceTypeName and ResourceInterface in order to GET
// the existence of a known resource
const char *resourceURI = "/oic/notify";
const char *resourceTypeName = "oic.r.notify";
const char *resourceInterface = OC_RSRVD_INTERFACE_DEFAULT;


void ProcessObserveRegister(OCEntityHandlerRequest *ehRequest)
{
    for (uint8_t i = 0; i < SAMPLE_MAX_NUM_OBSERVATIONS; i++)
    {
        if (interestedObservers[i].valid == false)
        {
            interestedObservers[i].observationId = ehRequest->obsInfo.obsId;
            interestedObservers[i].valid = true;
            gResourceUnderObservation = 1;
            break;
        }
    }
}

void ProcessObserveDeregister(OCEntityHandlerRequest *ehRequest)
{
    bool clientStillObserving = false;

    for (uint8_t i = 0; i < SAMPLE_MAX_NUM_OBSERVATIONS; i++)
    {
        if (interestedObservers[i].observationId == ehRequest->obsInfo.obsId)
        {
            interestedObservers[i].valid = false;
        }

        if (interestedObservers[i].valid == true)
        {
            // Even if there is one single client observing we continue notifying entity handler
            clientStillObserving = true;
        }
    }

    if (clientStillObserving == false)
        gResourceUnderObservation = 0;
}


OCEntityHandlerResult OCEntityHandlerCb (OCEntityHandlerFlag flag,
OCEntityHandlerRequest *entityHandlerRequest, void* param)
{
    OIC_LOG_V (INFO, TAG, "Inside entity handler - flags: 0x%x", flag);

    OCEntityHandlerResult ehResult = OC_EH_OK;

      // Validate pointer
    if (!entityHandlerRequest)
    {
        OIC_LOG (ERROR, TAG, "Invalid request pointer");
        return OC_EH_ERROR;
    }

    if (flag & OC_OBSERVE_FLAG)
    {
        OIC_LOG (INFO, TAG,  "Flag includes OC_OBSERVE_FLAG");

        if (OC_OBSERVE_REGISTER == entityHandlerRequest->obsInfo.action)
        {
                OIC_LOG (INFO, TAG, "Received OC_OBSERVE_REGISTER from client");
                ProcessObserveRegister (entityHandlerRequest);
        }
        else if (OC_OBSERVE_DEREGISTER == entityHandlerRequest->obsInfo.action)
        {
                OIC_LOG (INFO, TAG, "Received OC_OBSERVE_DEREGISTER from client");
                ProcessObserveDeregister (entityHandlerRequest);
        }

    }

    return ehResult;
}

void process()
{
    while (!beExit)
    {
        if (OCProcess() != OC_STACK_OK)
        {
            OIC_LOG(ERROR, TAG, "OCStack process error");
            return;
        }
    }
}

OCStackResult createNotifyResource()
{
    OIC_LOG_V(INFO, TAG, "Creating Notify resource");

    OCStackResult res = OCCreateResource(&notifyResource.handle,
                                    resourceTypeName,
                                    resourceInterface,
                                    resourceURI,
                                    OCEntityHandlerCb,
                                    NULL,
                                    OC_DISCOVERABLE|OC_OBSERVABLE);

    beExit = false;
    pthread_create(&threadId_process, NULL, process, (void *)NULL);

    return res;

}
void StartNotificationProducer()
{
    // Initialize observations data structure for the resource
    for (uint8_t i = 0; i < SAMPLE_MAX_NUM_OBSERVATIONS; i++)
    {
            interestedObservers[i].valid = false;
    }

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
            OIC_LOG(ERROR, TAG, "OCStack init error");
            return ;
    }

    OCStackResult res = createNotifyResource();

    if(res == OC_STACK_OK)
    {
            OIC_LOG_V(INFO, TAG, "Notify resource is Created");
    }
    else
    {
            OIC_LOG_V(INFO, TAG, "Notify resource not created");
    }

}


void setQos(OCNotificationPriority priority)
{
    if(priority == OC_NOTIFY_LOW)
    {
        gQos = OC_LOW_QOS;
    }
    else
    if(priority == OC_NOTIFY_MODERATE)
    {
        gQos = OC_MEDIUM_QOS;
    }
    else
    if(priority == OC_NOTIFY_CRITICAL)
    {
        gQos = OC_HIGH_QOS;
    }
    else
    {
        gQos = OC_NA_QOS;
    }

}


unsigned int GenerateId()
{
    srand(time(NULL));
    unsigned int id = rand();

    return(id);
}

OCRepPayload* getPayload(const char* details, const char* payload)
{
    OCRepPayload* ocPayload = OCRepPayloadCreate();
    if(!ocPayload)
    {
        OIC_LOG(ERROR, TAG, PCF("Failed to allocate OCRepPayload"));
    }

    OCRepPayloadSetUri(ocPayload,resourceURI);
    OCRepPayloadAddInterface(ocPayload, resourceInterface);
    OCRepPayloadAddResourceType(ocPayload, resourceTypeName);

    unsigned int notificationId = GenerateId();
    OCRepPayloadSetPropInt(ocPayload, NOTIFICATION_ID, notificationId);

    OCRepPayloadSetPropInt(ocPayload, ACK_ID, DEFAULT);

    OCRepPayloadSetPropString(ocPayload, DETAILS, details);

    OCRepPayloadSetPropString(ocPayload, PAYLOAD, payload);

    return ocPayload;
}

void SendNotification(const char* details, const char* payload)
{
    OCRepPayload* ocPayload= getPayload(details, payload);

    gNumberOfIds=0;

    for (uint8_t i = 0; i < SAMPLE_MAX_NUM_OBSERVATIONS; i++)
    {
        if (interestedObservers[i].valid == true)
        {
            gObsIdList[gNumberOfIds]=interestedObservers[i].observationId;
            gNumberOfIds++;
            break;
        }
    }


    OCStackResult res = OCNotifyListOfObservers(notifyResource.handle,
                                                    gObsIdList,
                                                    gNumberOfIds,
                                                    ocPayload,
                                                    gQos);


    if(res == OC_STACK_OK)
    {
        OIC_LOG_V(INFO, TAG, "Notify resource is sent");
    }
    else
    {
        OIC_LOG_V(INFO, TAG, "Notify resource not sent");
    }

}

void CreateSenderJson(cJSON* senderJson, OCStringLL* senderDetails)
{
    OCStringLL* str = senderDetails;
    if(str->value)
    {
        cJSON_AddItemToArray(senderJson, cJSON_CreateString(str->value));
    }
    while(str->next)
    {
        str = str->next;
        if(str->value)
        {
            cJSON_AddItemToArray(senderJson, cJSON_CreateString(str->value));
        }
    }
}

void CreateDetailsJson(cJSON* detailsJson, const OCNotificationPayload* notifyObj)
{
    setQos(notifyObj->priority);

    OCNotificationType notifyType = notifyObj->type;
    cJSON_AddNumberToObject(detailsJson, TYPE, notifyType);

    OCNotificationPriority notifyPriority = notifyObj->type;
    cJSON_AddNumberToObject(detailsJson, PRIORITY, notifyPriority);

    char *notifyTimestamp = notifyObj->timeStamp;
    cJSON_AddStringToObject(detailsJson, TIMESTAMP, notifyTimestamp);

    uint32_t notifyTtl = notifyObj->ttl;
    cJSON_AddNumberToObject(detailsJson, TTL, notifyTtl);

    cJSON *senderJson = cJSON_CreateArray();
    CreateSenderJson(senderJson, notifyObj->senderDetails);
    cJSON_AddItemToObject(detailsJson, SENDER, senderJson);

}

void SendTextNotification(const OCNSTextPayload* object)
{
    OCNotificationPayload* notifyObj;
    notifyObj = object->resources;

    cJSON *detailsJson = cJSON_CreateObject();
    CreateDetailsJson(detailsJson, notifyObj);
    char* details;
    details = cJSON_Print(detailsJson);


    cJSON *textJson = cJSON_CreateObject();
    cJSON_AddStringToObject(textJson, MESSAGE, object->textMessage);
    char* payload;
    payload = cJSON_Print(textJson);

    SendNotification(details, payload);
}

void SendImageNotification(const OCNSImagePayload* object)
{
    OCNotificationPayload* notifyObj;
    notifyObj = object->resources;

    cJSON *detailsJson = cJSON_CreateObject();
    CreateDetailsJson(detailsJson, notifyObj);
    char* details;
    details = cJSON_Print(detailsJson);

    cJSON *imageJson = cJSON_CreateObject();
    cJSON_AddStringToObject(imageJson, TITLE, object->title);
    cJSON_AddStringToObject(imageJson, IMAGE_URL, object->imageURL);
    char* payload;
    payload = cJSON_Print(imageJson);

    SendNotification(details, payload);
}

void SendVideoNotification(const OCNSVideoPayload* object)
{
    OCNotificationPayload* notifyObj;
    notifyObj = object->resources;

    cJSON *detailsJson = cJSON_CreateObject();
    CreateDetailsJson(detailsJson, notifyObj);
    char* details;
    details = cJSON_Print(detailsJson);

    cJSON *videoJson = cJSON_CreateObject();
    cJSON_AddStringToObject(videoJson, VIDEO_URL, object->videoURL);
    char* payload;
    payload = cJSON_Print(videoJson);

    SendNotification(details, payload);
}


void GenerateTimeStamp(OCNotificationPayload* notifyObj)
{
    time_t now = time(0);
    char *dt = ctime(&now);
    notifyObj->timeStamp = dt;
}

void GenerateTtl(OCNotificationPayload* notifyObj)
{
    notifyObj->ttl = TTL_VAlUE;
}

OCNSTextPayload* CreateTextNotification(OCNotificationPriority priority, OCStringLL* senderDetails, const char* textMessage)
{
    OCNSTextPayload* textPayload = (OCNSTextPayload*)OICMalloc(sizeof(OCNSTextPayload));
    OCNotificationPayload* notifyPayload = (OCNotificationPayload*)OICMalloc(sizeof(OCNotificationPayload));

    notifyPayload->priority = priority;
    notifyPayload->type = OC_NOTIFY_TEXT;
    GenerateTimeStamp(notifyPayload);
    GenerateTtl(notifyPayload);
    notifyPayload->senderDetails = senderDetails;

    textPayload->resources = notifyPayload;
    textPayload->textMessage =OICStrdup(textMessage);

    return(textPayload);
}

OCNSImagePayload* CreateImageNotification(OCNotificationPriority priority,OCStringLL* senderDetails,const char* title, const char* url)
{
    OCNSImagePayload* imagePayload = (OCNSImagePayload*)OICMalloc(sizeof(OCNSImagePayload));
    OCNotificationPayload* notifyPayload = (OCNotificationPayload*)OICMalloc(sizeof(OCNotificationPayload));

    notifyPayload->priority = priority;
    notifyPayload->type = OC_NOTIFY_IMAGE;
    GenerateTimeStamp(notifyPayload);
    GenerateTtl(notifyPayload);
    notifyPayload->senderDetails = senderDetails;

    imagePayload->resources = notifyPayload;
    imagePayload->title = OICStrdup(title);
    imagePayload->imageURL = OICStrdup(url);

    return(imagePayload);
}

OCNSVideoPayload* CreateVideoNotification(OCNotificationPriority priority, OCStringLL* senderDetails, const char* url)
{
    OCNSVideoPayload* videoPayload = (OCNSVideoPayload*)OICMalloc(sizeof(OCNSVideoPayload));
    OCNotificationPayload* notifyPayload = (OCNotificationPayload*)OICMalloc(sizeof(OCNotificationPayload));

    notifyPayload->priority = priority;
    notifyPayload->type = OC_NOTIFY_VIDEO;
    GenerateTimeStamp(notifyPayload);
    GenerateTtl(notifyPayload);
    notifyPayload->senderDetails = senderDetails;

    videoPayload->resources = notifyPayload;
    videoPayload->videoURL = OICStrdup(url);

    return(videoPayload);
}

OCStringLL* CreateSenderDetails()
{
    OCStringLL* str =  (OCStringLL*)OICMalloc(sizeof(OCStringLL));
    str->value = NULL;
    str->next = NULL;

    return(str);
}

void AddSenderDetails(OCStringLL* sender, const char* val)
{
    OCStringLL* str = sender;
    if(str->value)
    {
        while(str->next)
        {
            str = str->next;
        }

        if(str->value)
        {
            str->next = (OCStringLL*)OICMalloc(sizeof(OCStringLL));
            str = str->next;
            str->value = OICStrdup(val);
            str->next = NULL;
        }
        else
        {
            str->value = OICStrdup(val);
        }
    }
    else
    {
        str->value = OICStrdup(val);
    }
}

void StopNotificationProducer()
{
    beExit = true;

    if (OCStop() != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack stop error");
        return 0;
    }

}

