/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
#include <iostream>
#include <limits>

#include <string.h>
#include <pthread.h>

#include <ocstack.h>
#include <logger.h>
#include <cJSON.h>
#include <ocpayload.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <NotificationProducer.h>

#ifdef __cplusplus
}
#endif

constexpr int CORRECT_INPUT = 99;
constexpr int INCORRECT_INPUT = 100;

pthread_t threadId_loop;
bool beExit = false;

typedef struct LIGHTRESOURCE
{
    OCResourceHandle handle;
    int power;
    bool state;
} LightResource;

static LightResource Light;

std::string space;


enum Menu
{
    START_NOTIFICATION_PRODUCER = 1,
    SEND_NOTIFICATION_POWER,
    SEND_NOTIFICATION_STATE,
    STOP_NOTIFICATION_PRODUCER,
    QUIT,
    END_OF_MENU
};


void displayMenu()
{
    std::cout << "====================================================" << std::endl;
    std::cout << "1 :: Start Notification Producer" << std::endl;
    std::cout << "2 :: Change light Power" << std::endl;
    std::cout << "3 :: Change light state" << std::endl;
    std::cout << "4 :: Stop Notification Producer" << std::endl;
    std::cout << "5 :: Quit Sample Application" << std::endl;
    std::cout << "====================================================" << std::endl;
}

int processUserInput()
{
    int userInput;
    std::cin >> userInput;
    if (std::cin.fail())
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1;
    }
    return userInput;
}


void sampleStartNotificationProducer()
{

    StartNotificationProducer();
    std::cout << "------------------NOTIFICATION_PRODUCER_STARTED---------------------" << std::endl;
}

void sampleSendNotification(int changeId)
{
    int messageTypeInput;
    int messagePriorityInput;

    OCNotificationPriority notificationPriority;

    char *notificationMessage;
    std::string notificationImageUrl;
    std::string notificationVideoUrl;

    if (changeId == 1)
        notificationPriority = OC_NOTIFY_CRITICAL;
    else if (changeId == 2)
        notificationPriority = OC_NOTIFY_LOW;

    std::string state;
    if (Light.state == true)
        state = "On";
    else
        state = "Off";

    cJSON *msg = cJSON_CreateObject();
    cJSON_AddNumberToObject(msg, "Power", Light.power);
    cJSON_AddBoolToObject(msg, "State", Light.state);

    notificationMessage = cJSON_Print(msg);

    OCStringLL *senderDetails = CreateSenderDetails();
    AddSenderDetails(senderDetails, "/oic/nlight");

    OCNSTextPayload *textNotification = CreateTextNotification(notificationPriority, senderDetails,
                                        notificationMessage);
    SendTextNotification(textNotification);


}


void sampleStopNotificationProducer()
{
    StopNotificationProducer();
}

int selectProviderMenu()
{
    switch (processUserInput())
    {
            // Start Provider
        case Menu::START_NOTIFICATION_PRODUCER :
            std::getline(std::cin, space);
            std::cout << "START_NOTIFICATION_PRODUCER" << std::endl;
            sampleStartNotificationProducer();
            return CORRECT_INPUT;

            // Send Notification
        case Menu::SEND_NOTIFICATION_POWER:
            std::cout << "Enter Power" << std::endl;
            int pow;
            std::cin >> pow;
            Light.power = pow ;
            std::cout << "SEND_NOTIFICATION" << std::endl;
            sampleSendNotification(1);
            return CORRECT_INPUT;

        case Menu::SEND_NOTIFICATION_STATE:
            std::cout << "Enter State" << std::endl;
            std::cout << " 1. ON" << std::endl;
            std::cout << "2. Off" << std::endl;
            int state;
            std::cin >> state;
            if (state = 1)
                Light.state = true ;
            else if (state = 2)
                Light.state = false ;
            std::cout << "SEND_NOTIFICATION" << std::endl;
            sampleSendNotification(2);
            return CORRECT_INPUT;

            // Stop Provider
        case Menu::STOP_NOTIFICATION_PRODUCER :
            std::cout << "STOP_NOTIFICATION_PRODUCER" << std::endl;
            sampleStopNotificationProducer();
            return CORRECT_INPUT;

            // Exit
        case Menu::QUIT :
            std::cout << "QUIT" << std::endl;
            return QUIT;

        default :
            std::cout << "Invalid input, please try again" << std::endl;
            return INCORRECT_INPUT;
    }
}

void processData()
{
    while (true)
    {
        displayMenu();
        int ret = selectProviderMenu();

        if (ret == QUIT)
            break;
    }
}

OCRepPayload *getPayload(const char *uri, int64_t power, bool state)
{
    OCRepPayload *payload = OCRepPayloadCreate();
    if (!payload)
    {
        OIC_LOG(ERROR, TAG, PCF("Failed to allocate Payload"));
        return nullptr;
    }

    OCRepPayloadSetUri(payload, uri);
    OCRepPayloadSetPropBool(payload, "state", state);
    OCRepPayloadSetPropInt(payload, "power", power);

    return payload;
}

OCRepPayload *constructResponse(OCEntityHandlerRequest *ehRequest)
{
    if (ehRequest->payload && ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        OIC_LOG(ERROR, TAG, PCF("Incoming payload not a representation"));
        return nullptr;
    }

    OCRepPayload *input = reinterpret_cast<OCRepPayload *>(ehRequest->payload);

    LightResource *currLightResource = &Light;

    if (OC_REST_PUT == ehRequest->method || OC_REST_POST == ehRequest->method)
    {
        // Get pointer to query
        int64_t pow;
        if (OCRepPayloadGetPropInt(input, "power", &pow))
        {
            currLightResource->power = pow;
        }

        bool state;
        if (OCRepPayloadGetPropBool(input, "state", &state))
        {
            currLightResource->state = state;
        }

        sampleSendNotification(1);
    }

    return getPayload("/oic/nlight", currLightResource->power, currLightResource->state);
}


OCEntityHandlerResult ProcessGetRequest (OCEntityHandlerRequest *ehRequest,
        OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult;

    OCRepPayload *getResp = constructResponse(ehRequest);
    if (!getResp)
    {
        std::cout << "\nconstructResponse failed";
        return OC_EH_ERROR;
    }

    *payload = getResp;
    ehResult = OC_EH_OK;


    return ehResult;
}

OCEntityHandlerResult ProcessPostRequest (OCEntityHandlerRequest *ehRequest,
        OCEntityHandlerResponse *response, OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult = OC_EH_OK;
    OCRepPayload *respPLPost_light = nullptr;

    respPLPost_light = constructResponse(ehRequest);

    if ((respPLPost_light != NULL))
    {
        *payload = respPLPost_light;
    }
    else
    {
        OIC_LOG(INFO, TAG, "Payload was NULL");
        ehResult = OC_EH_ERROR;
    }

    return ehResult;
}

OCEntityHandlerResult ProcessPutRequest (OCEntityHandlerRequest *ehRequest,
        OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult;
    OCRepPayload *putResp = constructResponse(ehRequest);

    if (!putResp)
    {
        std::cout << "\nFailed to construct response";
        return OC_EH_ERROR;
    }

    *payload = putResp;
    ehResult = OC_EH_OK;

    return ehResult;
}

OCEntityHandlerResult ProcessDeleteRequest (OCEntityHandlerRequest *ehRequest)
{
    if (ehRequest == NULL)
    {
        OIC_LOG(INFO, TAG, "The ehRequest is NULL");
        return OC_EH_ERROR;
    }
}

OCEntityHandlerResult OCEntityHandlerCb (OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest, void *callback)
{
    std::cout << "\nInside entity handler - flags: " << flag;

    OCEntityHandlerResult ehResult = OC_EH_OK;
    OCEntityHandlerResponse response;

    // Validate pointer
    if (!entityHandlerRequest)
    {
        std::cout << "\nInvalid request pointer";
        return OC_EH_ERROR;
    }

    // Initialize certain response fields
    response.numSendVendorSpecificHeaderOptions = 0;
    memset(response.sendVendorSpecificHeaderOptions,
           0, sizeof response.sendVendorSpecificHeaderOptions);
    memset(response.resourceUri, 0, sizeof response.resourceUri);
    OCRepPayload *payload = nullptr;

    if (flag & OC_REQUEST_FLAG)
    {
        std::cout << "\n=================================\n";
        std::cout << "\nFlag includes OC_REQUEST_FLAG\n";

        if (OC_REST_GET == entityHandlerRequest->method)
        {
            std::cout << "\nReceived OC_REST_GET from client";
            ehResult = ProcessGetRequest (entityHandlerRequest, &payload);
            std::cout << "\n=================================\n";
        }
        else if (OC_REST_PUT == entityHandlerRequest->method)
        {
            std::cout << "\nReceived OC_REST_PUT from client";
            ehResult = ProcessPutRequest (entityHandlerRequest, &payload);
            std::cout << "\n=================================\n";
        }
        else if (OC_REST_POST == entityHandlerRequest->method)
        {
            std::cout << "\nReceived OC_REST_POST from client";
            ehResult = ProcessPostRequest (entityHandlerRequest, &response, &payload);
            std::cout << "\n=================================\n";
        }
        else if (OC_REST_DELETE == entityHandlerRequest->method)
        {
            std::cout << "\nReceived OC_REST_DELETE from client";
            ehResult = ProcessDeleteRequest (entityHandlerRequest);
            std::cout << "\n=================================\n";
        }
        else
        {
            std::cout << "\nReceived unsupported method " << entityHandlerRequest->method << " from client";
            ehResult = OC_EH_ERROR;
        }

        if (!((ehResult == OC_EH_ERROR) || (ehResult == OC_EH_FORBIDDEN)))
        {
            // Format the response.  Note this requires some info about the request
            response.requestHandle = entityHandlerRequest->requestHandle;
            response.resourceHandle = entityHandlerRequest->resource;
            response.ehResult = ehResult;
            response.payload = reinterpret_cast<OCPayload *>(payload);

            // Indicate that response is NOT in a persistent buffer
            response.persistentBufferFlag = 0;

            // Send the response
            if (OCDoResponse(&response) != OC_STACK_OK)
            {
                OIC_LOG(ERROR, TAG, "Error sending response");
                ehResult = OC_EH_ERROR;
            }
        }
    }
    return ehResult;
}

void *loop(void *param)
{
    while (!beExit)
    {
        if (OCProcess() != OC_STACK_OK)
        {
            OIC_LOG(ERROR, TAG, "OCStack process error");
        }
    }
}

OCStackResult createLightResource()
{
    Light.power = 0;
    OCStackResult res = OCCreateResource(&Light.handle,
                                         "oic.r.nlight",
                                         OC_RSRVD_INTERFACE_DEFAULT,
                                         "/oic/nlight",
                                         OCEntityHandlerCb,
                                         NULL,
                                         OC_DISCOVERABLE | OC_OBSERVABLE);

    beExit = false;
    pthread_create(&threadId_loop, NULL, loop, (void *)NULL);

    return res;
}
int main()
{
    std::cout << "Starting Notification Producer Sample" << std::endl;

    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack init error");
        return 0;
    }
    if (createLightResource() != OC_STACK_OK)
    {
        std::cout << "OCStack cannot create resource..." << std::endl;
        OIC_LOG(ERROR, TAG, "OCStack cannot create resource...");
    }


    try
    {
        processData();
    }
    catch (const std::exception &e)
    {
        std::cout << "main exception  : " << e.what() << std::endl;
    }

    std::cout << "Terminating Notification Producer Sample" << std::endl;
}
