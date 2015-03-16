//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

#define _POSIX_C_SOURCE 200112L
#include <string.h>

#include "cJSON.h"
#include "ocmalloc.h"
#include "oicgroup.h"
#include "ocresource.h"
#include "occollection.h"
#include "logger.h"

#define TAG PCF("OICGROUP")

#define DESC_DELIMITER          "\""
#define ACTION_DELIMITER        "*"
#define ATTR_DELIMITER          "|"
#define ATTR_ASSIGN             "="

typedef struct aggregatehandleinfo
{
    OCServerRequest *ehRequest;
    OCDoHandle required;
    OCResource *collResource;

    struct aggregatehandleinfo *next;
} ClientRequestInfo;

static ClientRequestInfo *clientRequestList = NULL;

void AddClientRequestInfo(ClientRequestInfo **head, ClientRequestInfo* add)
{
    ClientRequestInfo *tmp = NULL;

    if (*head != NULL)
    {
        tmp = *head;

        while (tmp->next)
        {
            tmp = tmp->next;
        }
        tmp->next = add;
    }
    else
    {
        *head = add;
    }
}

ClientRequestInfo* GetClientRequestInfo(ClientRequestInfo *head, OCDoHandle handle)
{
    ClientRequestInfo *tmp = NULL;

    tmp = head;

    if (tmp)
    {
        while (tmp)
        {
            if (tmp->required == handle)
            {
                break;
            }

            tmp = tmp->next;
        }

        return tmp;
    }
    return NULL;
}

void RemoveClientRequestInfo(ClientRequestInfo **head, ClientRequestInfo* del)
{
    ClientRequestInfo *tmp = NULL;

    if (*head == del)
    {
        *head = (*head)->next;
    }
    else
    {
        tmp = *head;
        while (tmp->next && (tmp->next != del))
        {
            tmp = tmp->next;
        }
        if (tmp->next)
        {
            tmp->next = del->next;
        }
    }
}



void AddCapability(OCCapability** head, OCCapability* node)
{
    OCCapability *pointer = *head;
    if (NULL == pointer)
    {
        *head = node;
    }
    else
    {
        while (pointer->next != NULL)
        {
            pointer = pointer->next;
        }

        pointer->next = node;
    }
}

void AddAction(OCAction** head, OCAction* node)
{
    OCAction *pointer = *head;
    if (NULL == pointer)
    {
        *head = node;
    }
    else
    {

        while (pointer->next != NULL)
        {
            pointer = pointer->next;
        }

        pointer->next = node;
    }
}

void AddActionSet(OCActionSet **head, OCActionSet* node)
{
    OCActionSet *pointer = *head;
    if (NULL == pointer)
    {
        *head = node;
    }
    else
    {

        while (pointer->next != NULL)
        {
            pointer = pointer->next;
        }

        pointer->next = node;
    }
}

void DeleteCapability(OCCapability *del)
{
    OCFree(del->capability);
    del->capability = NULL;
    OCFree(del->status);
    del->status = NULL;
}

void DeleteAction(OCAction** action)
{
    OCCapability* pointer = (*action)->head;
    OCCapability* pDel = NULL;

    while (pointer)
    {
        pDel = pointer;
        pointer = pointer->next;

        DeleteCapability(pDel);
        pDel->next = NULL;
    }
    OCFree((*action)->resourceUri);
    (*action)->resourceUri = NULL;
    (*action)->next = NULL;
}

void DeleteActionSet(OCActionSet** actionset)
{
    OCAction* pointer = (*actionset)->head;
    OCAction* pDel = NULL;

    while (pointer)
    {
        pDel = pointer;
        pointer = pointer->next;

        DeleteAction(&pDel);
        pDel->next = NULL;
    }

    OCFree((*actionset)->actionsetName);
    (*actionset)->head = NULL;
}

OCStackResult FindAndDeleteActionSet(OCResource **resource, const char * actionsetName)
{
    if (*resource != NULL)
    {
        OCActionSet *pointer = NULL;
        OCActionSet *pDel = NULL;

        pointer = (*resource)->actionsetHead;

        if (pointer == NULL)
        {
            return OC_STACK_ERROR;
        }
        else
        {
            if (strcmp(pointer->actionsetName, actionsetName) == 0)
            {
                if (pointer->next != NULL)
                    (*resource)->actionsetHead = pointer->next;
                else
                    (*resource)->actionsetHead = NULL;
                DeleteActionSet(&pointer);
            }
            else if (pointer->next != NULL)
            {
                while (pointer)
                {
                    if (pointer->next != NULL)
                    {
                        if (strcmp(pointer->next->actionsetName, actionsetName) == 0)
                        {
                            pDel = pointer->next;
                            pointer->next = pointer->next->next;

                            DeleteActionSet(&pDel);
                        }
                    }
                    pointer = pointer->next;
                }
            }

            return OC_STACK_OK;
        }

    }
    return OC_STACK_ERROR;
}

OCStackResult DeleteActionSets(OCResource** resource)
{
    OCActionSet *pointer = (*resource)->actionsetHead;
    OCActionSet *pDel = pointer;

    while (pointer)
    {
        pDel = pointer;
        pointer = pointer->next;

        DeleteActionSet(&pDel);
        pDel->next = NULL;
    }

    (*resource)->actionsetHead = NULL;
    return OC_STACK_OK;
}

OCStackResult GetActionSet(const char *actionName, OCActionSet *head, OCActionSet** actionset)
{
    OCActionSet *pointer = head;

    while (pointer)
    {
        if (strcmp(pointer->actionsetName, actionName) == 0)
        {
            *actionset = pointer;
            return OC_STACK_OK;
        }

        pointer = pointer->next;
    }

    return OC_STACK_ERROR;

}


#define OIC_ACTION_PREFIX               "{\"oc\":[{\"rep\":{"
#define VARIFY_POINTER_NULL(pointer, result, toExit) \
    if(pointer == NULL) \
    {\
        result = OC_STACK_NO_MEMORY;\
        goto toExit;\
    }
#define VARIFY_PARAM_NULL(pointer, result, toExit) \
    if(pointer == NULL)\
    {\
        result = OC_STACK_INVALID_PARAM;\
        goto exit;\
    }

OCStackResult ExtractKeyValueFromRequest(char *request, char **key, char **value)
{
    OCStackResult result = OC_STACK_OK;
    size_t length = 0;

    if(strlen(request) <= strlen(OIC_ACTION_PREFIX))
    {
        return OC_STACK_INVALID_PARAM;
    }

    char* pRequest = (char *)request + strlen(OIC_ACTION_PREFIX);
    char* iterToken = NULL;
    char* iterTokenPtr = NULL;

    iterToken = (char *) strtok_r(pRequest, ":", &iterTokenPtr);
    length = strlen(iterToken) + 1;

    *key = (char *)OCMalloc(length);
    VARIFY_POINTER_NULL(*key, result, exit);

    strncpy(*key, iterToken + 1, length);
    ((*key)[ (( length - 1 ) - 2) ]) = '\0';

    iterToken = (char *) strtok_r(NULL, "}", &iterTokenPtr);
    length = strlen(iterToken) + 1;

    *value = (char *)OCMalloc(length);
    VARIFY_POINTER_NULL(*key, result, exit);

    strncpy(*value, iterToken + 1, length);
    ((*value)[ (( length - 1 ) - 2) ]) = '\0';
exit:
    if(result != OC_STACK_OK)
    {
        OCFree(*key);
        OCFree(*value);
        *key = NULL;
        *value = NULL;
    }

    return result;
}

OCStackResult BuildActionSetFromString(OCActionSet **set, char* actiondesc)
{
    OCStackResult result = OC_STACK_OK;

    char *iterToken = NULL, *iterTokenPtr = NULL;
    char *descIterToken = NULL;
    char *descIterTokenPtr = NULL;
    char *attrIterToken = NULL;
    char *attrIterTokenPtr = NULL;
    char *desc = NULL;
    char *attr = NULL;
    char *key = NULL;
    char *value = NULL;

    OCAction *action = NULL;
    OCCapability *capa = NULL;

    OC_LOG(INFO, TAG, PCF("Build ActionSet Instance."));

    *set = (OCActionSet*) OCMalloc(sizeof(OCActionSet));
    VARIFY_POINTER_NULL(*set, result, exit)

    iterToken = (char *) strtok_r(actiondesc, ACTION_DELIMITER, &iterTokenPtr);

    memset(*set, 0, sizeof(OCActionSet));
    (*set)->actionsetName = (char *)OCMalloc(sizeof(OCActionSet));
    VARIFY_POINTER_NULL((*set)->actionsetName, result, exit)
    VARIFY_PARAM_NULL(iterToken, result, exit)
    strncpy((*set)->actionsetName, iterToken, strlen(iterToken) + 1);

    OC_LOG_V(INFO, TAG, "ActionSet Name : %s", (*set)->actionsetName);

    iterToken = (char *) strtok_r(NULL, ACTION_DELIMITER, &iterTokenPtr);
    while(iterToken)
    {
        desc = (char *)OCMalloc(strlen(iterToken) + 1);
        strncpy(desc, iterToken, strlen(iterToken) + 1);
        descIterToken = (char *) strtok_r(desc, ATTR_DELIMITER, &descIterTokenPtr);
        while(descIterToken)
        {
            attr = (char *)OCMalloc(strlen(descIterToken) + 1);
            strncpy(attr, descIterToken, strlen(descIterToken) + 1);

            attrIterToken = (char *) strtok_r(attr, ATTR_ASSIGN, &attrIterTokenPtr);
            key = (char *)OCMalloc(strlen(attrIterToken) + 1);
            VARIFY_POINTER_NULL(key, result, exit)

            VARIFY_PARAM_NULL(attrIterToken, result, exit)
            strncpy(key, attrIterToken, strlen(attrIterToken) + 1);
            attrIterToken = (char *) strtok_r(NULL, ATTR_ASSIGN, &attrIterTokenPtr);

            value = (char *)OCMalloc(strlen(attrIterToken) + 1);
            VARIFY_POINTER_NULL(value, result, exit)
            VARIFY_PARAM_NULL(attrIterToken, result, exit)
            strncpy(value, attrIterToken, strlen(attrIterToken) + 1);

            if(strncmp(key, "uri", sizeof("uri") - 1) == 0)
            {
                OC_LOG(INFO, TAG, PCF("Build OCAction Instance."));

                action = (OCAction*)OCMalloc(sizeof(OCAction));
                VARIFY_POINTER_NULL(action, result, exit)
                memset(action, 0, sizeof(OCAction));
                action->resourceUri = (char *)OCMalloc(strlen(value) + 1);
                VARIFY_POINTER_NULL(action->resourceUri, result, exit)
                VARIFY_PARAM_NULL(value, result, exit)
                strncpy(action->resourceUri, value, strlen(value) + 1);
            }
            else
            {
                if( (key != NULL) && (value != NULL))
                {
                    OC_LOG(INFO, TAG, PCF("Build OCCapability Instance."));
                    capa = (OCCapability*)OCMalloc(sizeof(OCCapability));
                    VARIFY_POINTER_NULL(capa, result, exit)
                    memset(capa, 0, sizeof(OCCapability));
                    capa->capability = (char *)OCMalloc(strlen(key) + 1);
                    capa->status = (char *)OCMalloc(strlen(value) + 1);

                    strncpy(capa->capability, key, strlen(key) + 1);
                    strncpy(capa->status, value, strlen(value) + 1);

                    VARIFY_POINTER_NULL(action, result, exit)

                    AddCapability(&action->head, capa);
                }
            }

            OCFree(key);
            OCFree(value);
            OCFree(attr);

            descIterToken = (char *) strtok_r(NULL, ATTR_DELIMITER, &descIterTokenPtr);
        }

        AddAction(&(*set)->head, action);
        iterToken = (char *) strtok_r(NULL, ACTION_DELIMITER, &iterTokenPtr);
        OCFree(desc);
    }

    return OC_STACK_OK;
exit:
    OCFree(attr);
    OCFree(desc);
    OCFree(capa);
    OCFree(action);
    OCFree(*set);
    *set = NULL;
    return result;
}

OCStackResult BuildStringFromActionSet(OCActionSet* actionset, char** desc)
{
    char temp[1024] = { 0 };
    uint16_t remaining = sizeof(temp) - 1;

    OCAction *action = actionset->head;

    if (remaining >= strlen(actionset->actionsetName) + 1)
    {
        strncat(temp, actionset->actionsetName, sizeof(temp));
        remaining -= strlen(actionset->actionsetName);
        strcat(temp, "*");
        remaining--;
    }
    else
    {
        return OC_STACK_ERROR;
    }

    while (action != NULL)
    {
        OC_LOG_V(INFO, TAG, "\tURI :: %s\n", action->resourceUri);
        strcat(temp, "uri=");
        remaining -= strlen("uri=");
        strcat(temp, action->resourceUri);
        remaining -= strlen(action->resourceUri);
        strcat(temp, "|");
        remaining--;

        OCCapability *capas = action->head;
        while (capas != NULL)
        {
            OC_LOG_V(INFO, TAG, "\t\t%s = %s\n", capas->capability, capas->status);
            strcat(temp, capas->capability);
            remaining -= strlen(capas->capability);
            strcat(temp, "=");
            remaining--;
            strcat(temp, capas->status);
            remaining -= strlen(capas->capability);

            capas = capas->next;
            if (capas != NULL)
            {
                strcat(temp, "|");
            }
        }

        action = action->next;
        if (action != NULL)
        {
            strcat(temp, "*");
            remaining--;
        }
    }

    *desc = (char *) OCMalloc(1024 - remaining);
    if(!*desc)
    {
        return OC_STACK_NO_MEMORY;
    }

    strcpy(*desc, temp);

    return OC_STACK_OK;
}

OCStackApplicationResult ActionSetCB(void* context, OCDoHandle handle,
        OCClientResponse* clientResponse)
{
    OC_LOG(INFO, TAG, PCF("\n\n\tcallback is called\n\n"));

    ClientRequestInfo *info = GetClientRequestInfo(clientRequestList, handle);

    if (info)
    {
        uint32_t idx = 0;

        char *responseJson = (char *) OCMalloc(
                (unsigned int) (strlen( clientResponse->resJSONPayload) + 1));

        // We need the body of response.
        // Copy the body from the response
        strncpy((char *) responseJson, (clientResponse->resJSONPayload
                + OC_JSON_PREFIX_LEN), strlen(clientResponse->resJSONPayload) + 1);
        idx = strlen((char *) responseJson) - OC_JSON_SUFFIX_LEN;
        // And insert NULL at the end of body.
        (responseJson[idx]) = 0;

        OCEntityHandlerResponse response = { };
        response.ehResult = OC_EH_OK;
        response.payload = responseJson;
        response.payloadSize = (unsigned int) strlen((char *) responseJson) + 1;
        response.persistentBufferFlag = 0;
        response.requestHandle = (OCRequestHandle) info->ehRequest;
        response.resourceHandle = (OCResourceHandle) info->collResource;

        OCDoResponse(&response);

        RemoveClientRequestInfo(&clientRequestList, info);
        OCFree(responseJson);
    }

    return OC_STACK_KEEP_TRANSACTION;
}

void ActionSetCD(void *context)
{
}

OCStackResult BuildActionJSON(OCAction* action, char* bufferPtr, uint16_t *remaining)
{
    OCStackResult ret = OC_STACK_ERROR;
    cJSON *json = NULL;
    cJSON *body = NULL;

    char *jsonStr = NULL;
    uint16_t jsonLen = 0;

    OC_LOG(INFO, TAG, PCF("Entering BuildActionJSON"));
    json = cJSON_CreateObject();

    cJSON_AddItemToObject(json, "rep", body = cJSON_CreateObject());

    OCCapability* pointerCapa = action->head;
    while (pointerCapa)
    {
        cJSON_AddStringToObject(body, pointerCapa->capability, pointerCapa->status);
        pointerCapa = pointerCapa->next;
    }

    jsonStr = cJSON_PrintUnformatted(json);

    jsonLen = strlen(jsonStr);
    if (jsonLen < *remaining)
    {
        strcat((char*) bufferPtr, jsonStr);
        *remaining -= jsonLen;
        bufferPtr += jsonLen;
        ret = OC_STACK_OK;
    }

    cJSON_Delete(json);
    OCFree(jsonStr);

    return ret;
}

uint32_t GetNumOfTargetResource(OCAction *actionset)
{
    uint32_t numOfResource = 0;

    OCAction *pointerAction = actionset;

    while (pointerAction != NULL)
    {
        numOfResource++;
        pointerAction = pointerAction->next;
    }

    return numOfResource;
}

OCStackResult SendAction(OCDoHandle *handle, const char *targetUri, const char *action)
{
    OCCallbackData cbdata = {  };
    cbdata.cb = &ActionSetCB;
    cbdata.cd = &ActionSetCD;
    cbdata.context = (void *) 0x99;

    return OCDoResource(handle, OC_REST_PUT, targetUri,
    //temp->rsrcType->resourcetypename,
            NULL, (char *) action, OC_WIFI, OC_NA_QOS, &cbdata, NULL, 0);
}

OCStackResult BuildCollectionGroupActionJSONResponse(OCMethod method/*OCEntityHandlerFlag flag*/,
        OCResource *resource, OCEntityHandlerRequest *ehRequest)
{
    OCStackResult stackRet = OC_STACK_ERROR;

    OC_LOG(INFO, TAG, PCF("Group Action is requested."));
    char *doWhat = NULL;
    char *details = NULL;

    size_t bufferLength = 0;
    char buffer[MAX_RESPONSE_LENGTH] = { 0 };
    char *bufferPtr = NULL;

    bufferPtr = buffer;

    OCResource * collResource = (OCResource *) ehRequest->resource;

    char *jsonResponse;

    ExtractKeyValueFromRequest((char *)ehRequest->reqJSONPayload, &doWhat, &details);

    cJSON *json;
    cJSON *format;


    if (method == OC_REST_PUT)
    {
        json = cJSON_CreateObject();
        cJSON_AddStringToObject(json, "href", resource->uri);
        cJSON_AddItemToObject(json, "rep", format = cJSON_CreateObject());

        OC_LOG(INFO, TAG, PCF("Group Action[PUT]."));

        if(strcmp(doWhat, "ActionSet") == 0)
        {
            OCActionSet *actionSet;
            BuildActionSetFromString(&actionSet, details);

            if(actionSet != NULL)
            {
                AddActionSet(&resource->actionsetHead, actionSet);
                stackRet = OC_STACK_OK;
            }
            else
            {
                stackRet = OC_STACK_ERROR;
            }

        }
        else if (strncmp(doWhat, "DelActionSet", sizeof("DelActionSet")) == 0)
        {
            if (FindAndDeleteActionSet(&resource, details) == OC_STACK_OK)
            {
                stackRet = OC_STACK_OK;
            }
            else
            {
                stackRet = OC_STACK_ERROR;
            }
        }

        jsonResponse = cJSON_Print(json);
        cJSON_Delete(json);

        strcat((char *) bufferPtr, jsonResponse);

        bufferLength = strlen((const char *) buffer);
        if (bufferLength > 0)
        {
            OCEntityHandlerResponse response = { 0 };
            response.ehResult = OC_EH_OK;
            response.payload = buffer;
            response.payloadSize = bufferLength + 1;
            response.persistentBufferFlag = 0;
            response.requestHandle = (OCRequestHandle) ehRequest->requestHandle;
            response.resourceHandle = (OCResourceHandle) collResource;
            stackRet = OCDoResponse(&response);
        }

        stackRet = OC_STACK_OK;
    }

    if (method == OC_REST_POST)
    {
        OC_LOG(INFO, TAG, PCF("Group Action[POST]."));

        OCActionSet *actionset = NULL;

        json = cJSON_CreateObject();
        cJSON_AddStringToObject(json, "href", resource->uri);

        if (strcmp(doWhat, "DoAction") == 0)
        {
            if (GetActionSet(details, resource->actionsetHead, &actionset) != OC_STACK_OK)
            {
                OC_LOG(INFO, TAG, PCF("ERROR"));
                stackRet = OC_STACK_ERROR;
            }

            if (actionset == NULL)
            {
                OC_LOG(INFO, TAG, PCF("ERROR"));
                stackRet = OC_STACK_ERROR;
            }
            else
            {

                OCAction *pointerAction = actionset->head;

                uint32_t num = GetNumOfTargetResource(pointerAction);

                ((OCServerRequest *) ehRequest->requestHandle)->ehResponseHandler =
                        HandleAggregateResponse;
                ((OCServerRequest *) ehRequest->requestHandle)->numResponses = num + 1;

                while (pointerAction != NULL)
                {
                    char actionDesc[MAX_RESPONSE_LENGTH] = { 0 };
                    char* actionDescPtr = actionDesc;
                    uint16_t remaining = MAX_RESPONSE_LENGTH;

                    strncpy((char *) actionDescPtr, (const char *) OC_JSON_PREFIX,
                        strlen((const char *) OC_JSON_PREFIX) + 1);
                    BuildActionJSON(pointerAction, actionDescPtr, &remaining);
                    strncat((char *) actionDescPtr, (const char *) OC_JSON_SUFFIX,
                        strlen((const char *) OC_JSON_SUFFIX));

                    ClientRequestInfo *info = (ClientRequestInfo *) OCMalloc(
                            sizeof(ClientRequestInfo));
                    memset(info, 0, sizeof(ClientRequestInfo));

                    info->collResource = resource;
                    info->ehRequest = (OCServerRequest *) ehRequest->requestHandle;

                    SendAction(&info->required, pointerAction->resourceUri, actionDescPtr);

                    AddClientRequestInfo(&clientRequestList, info);

                    pointerAction = pointerAction->next;
                }


                stackRet = OC_STACK_OK;
            }
        }
        else if (strcmp(doWhat, "GetActionSet") == 0)
        {
            char *plainText = NULL;
            OCActionSet *actionset = NULL;

            cJSON_AddItemToObject(json, "rep", format = cJSON_CreateObject());
            GetActionSet(details, resource->actionsetHead, &actionset);
            if (actionset != NULL)
            {
                BuildStringFromActionSet(actionset, &plainText);

                if (plainText != NULL)
                {
                    cJSON_AddStringToObject(format, "ActionSet", plainText);
                }

                stackRet = OC_STACK_OK;
            }
        }

        jsonResponse = cJSON_Print(json);
        cJSON_Delete(json);

        strcat((char *) bufferPtr, jsonResponse);

        bufferLength = strlen((const char *) buffer);
        if (bufferLength > 0)
        {
            OCEntityHandlerResponse response = {};
            response.ehResult = OC_EH_OK;
            response.payload = buffer;
            response.payloadSize = bufferLength + 1;
            response.persistentBufferFlag = 0;
            response.requestHandle = (OCRequestHandle) ehRequest->requestHandle;
            response.resourceHandle = (OCResourceHandle) collResource;
            stackRet = OCDoResponse(&response);
        }
    }

    OCFree(doWhat);
    OCFree(details);

    return stackRet;
}

