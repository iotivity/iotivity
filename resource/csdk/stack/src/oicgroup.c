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

#include "oicgroup.h"
#include "cJSON.h"
#include "ocmalloc.h"
#include "occollection.h"
#include "logger.h"
#include "timer.h"

#ifndef WITH_ARDUINO
#include <pthread.h>
#endif

#define TAG PCF("OICGROUP")

#define DESC_DELIMITER          "\""
#define ACTION_DELIMITER        "*"
#define ATTR_DELIMITER          "|"
#define ATTR_ASSIGN             "="

// Definitions for operations related to actions
#define DO_ACTION               "DoAction"
#define GET_ACTIONSET           "GetActionSet"
#define ACTIONSET               "ActionSet"
#define DELETE_ACTIONSET        "DelActionSet"

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

#define OCFREE(pointer) \
    { \
        OCFree(pointer); \
        pointer = NULL; \
    }

#ifndef WITH_ARDUINO
pthread_mutex_t lock;
#endif

enum ACTION_TYPE
{
    NONE = 0, SCHEDULED, RECURSIVE
};

typedef struct scheduledresourceinfo
{
    OCResource *resource;
    OCActionSet *actionset;

    int timer_id;

    OCServerRequest *ehRequest;

    time_t time;
    struct scheduledresourceinfo* next;
} ScheduledResourceInfo;

ScheduledResourceInfo *scheduleResourceList = NULL;

void AddScheduledResource(ScheduledResourceInfo **head,
        ScheduledResourceInfo* add)
{
    OC_LOG(INFO, TAG, PCF("AddScheduledResource Entering..."));

#ifndef WITH_ARDUINO
    pthread_mutex_lock(&lock);
#endif
    ScheduledResourceInfo *tmp = NULL;

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
#ifndef WITH_ARDUINO
    pthread_mutex_unlock(&lock);
#endif
}

ScheduledResourceInfo* GetScheduledResource(ScheduledResourceInfo *head)
{
    OC_LOG(INFO, TAG, PCF("GetScheduledResource Entering..."));

#ifndef WITH_ARDUINO
    pthread_mutex_lock(&lock);
#endif

    time_t t_now;

    ScheduledResourceInfo *tmp = NULL;
    tmp = head;

#ifndef WITH_ARDUINO
    time(&t_now);
#else
    t_now = now();
#endif

    if (tmp)
    {
        while (tmp)
        {
            time_t diffTm = 0;
#ifndef WITH_ARDUINO
            diffTm = timespec_diff(tmp->time, t_now);
#else
            diffTm = timespec_diff(tmp->time, t_now);
#endif

            if (diffTm <= (time_t) 0)
            {
                OC_LOG(INFO, TAG, PCF("return Call INFO."));
                goto exit;
            }

            tmp = tmp->next;
        }
    }

    exit:
#ifndef WITH_ARDUINO
    pthread_mutex_unlock(&lock);
#endif
    if (tmp == NULL)
    {
        OC_LOG(INFO, TAG, PCF("Cannot Find Call Info."));
    }
    return tmp;
}

ScheduledResourceInfo* GetScheduledResourceByActionSetName(ScheduledResourceInfo *head, char *setName)
{
    OC_LOG(INFO, TAG, PCF("GetScheduledResourceByActionSetName Entering..."));

#ifndef WITH_ARDUINO
    pthread_mutex_lock(&lock);
#endif
    ScheduledResourceInfo *tmp = NULL;
    tmp = head;

    if (tmp)
    {
        while (tmp)
        {
            if (strcmp(tmp->actionset->actionsetName, setName) == 0)
            {
                OC_LOG(INFO, TAG, PCF("return Call INFO."));
                goto exit;
            }
            tmp = tmp->next;
        }
    }

exit:
#ifndef WITH_ARDUINO
    pthread_mutex_unlock(&lock);
#endif
    if (tmp == NULL)
    {
        OC_LOG(INFO, TAG, PCF("Cannot Find Call Info."));
    }
    return tmp;
}

void RemoveScheduledResource(ScheduledResourceInfo **head,
        ScheduledResourceInfo* del)
{
#ifndef WITH_ARDUINO
    pthread_mutex_lock(&lock);
#endif
    OC_LOG(INFO, TAG, PCF("RemoveScheduledResource Entering..."));
    ScheduledResourceInfo *tmp = NULL;

    if (del == NULL)
    {
        return;
    }

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

    OCFREE(del)
#ifndef WITH_ARDUINO
    pthread_mutex_unlock(&lock);
#endif
}

typedef struct aggregatehandleinfo
{
    OCServerRequest *ehRequest;
    OCDoHandle required;
    OCResource *collResource;

    struct aggregatehandleinfo *next;
} ClientRequestInfo;

ClientRequestInfo *clientRequstList = NULL;

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

ClientRequestInfo* GetClientRequestInfo(ClientRequestInfo *head,
        OCDoHandle handle)
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

    if (del == NULL)
        return;

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

OCStackResult AddActionSet(OCActionSet **head, OCActionSet* node)
{
    OCActionSet *pointer = *head;
    OCActionSet *prev = NULL;
    if(node == NULL)
    {
        return OC_STACK_ERROR;
    }
    if (NULL == pointer)
    {
        *head = node;
    }
    else
    {
        prev = pointer;
        while (pointer != NULL)
        {
            // check the uniqeness of actionsetname.
            if (strcmp(pointer->actionsetName, node->actionsetName) == 0)
            {
                return OC_STACK_ERROR;
            }

            prev = pointer;
            pointer = pointer->next;
        }

        prev->next = node;
    }

    return OC_STACK_OK;
}

void DeleteCapability(OCCapability *del)
{
    OCFREE(del->capability)
    del->capability = NULL;
    OCFREE(del->status)
    del->status = NULL;
    OCFREE(del)
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
    }
    OCFREE((*action)->resourceUri)
    (*action)->next = NULL;
    OCFREE(*action)
}

void DeleteActionSet(OCActionSet** actionset)
{
    if(*actionset == NULL)
        return;

    OCAction* pointer = (*actionset)->head;
    OCAction* pDel = NULL;

    while (pointer)
    {
        pDel = pointer;
        pointer = pointer->next;

        DeleteAction(&pDel);
    }
    //    (*actionset)->head = NULL;
    OCFREE((*actionset)->actionsetName)
    OCFREE(*actionset)
}

OCStackResult FindAndDeleteActionSet(OCResource **resource,
        const char * actionsetName)
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
                        if (strcmp(pointer->next->actionsetName, actionsetName)
                                == 0)
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

OCStackResult GetActionSet(const char *actionName, OCActionSet *head,
        OCActionSet** actionset)
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

OCStackResult ExtractKeyValueFromRequest(char *request, char **key,
        char **value)
{
    OCStackResult result = OC_STACK_OK;
    size_t length = 0;

    char* pRequest = (char *) request + strlen(OIC_ACTION_PREFIX);
    char* iterToken, *iterTokenPtr;

    iterToken = (char *) strtok_r(pRequest, ":", &iterTokenPtr);
    VARIFY_POINTER_NULL(iterToken, result, exit);
    length = strlen(iterToken) + 1;

    *key = (char *) OCMalloc(length);
    VARIFY_POINTER_NULL(*key, result, exit)

    strncpy(*key, iterToken + 1, length);
    ((*key)[((length - 1) - 2)]) = '\0';

    iterToken = (char *) strtok_r(NULL, "}", &iterTokenPtr);
    VARIFY_POINTER_NULL(iterToken, result, exit);
    length = strlen(iterToken) + 1;

    *value = (char *) OCMalloc(length);
    VARIFY_POINTER_NULL(*value, result, exit)

    strncpy(*value, iterToken + 1, length);
    ((*value)[((length - 1) - 2)]) = '\0';

exit:
    if (result != OC_STACK_OK)
    {
        OCFREE(*key)
        OCFREE(*value)
    }

    return result;
}

OCStackResult ExtractActionSetNameAndDelaytime(char *pChar, char **setName,
        long int *pa)
{
    char *token, *tokenPtr;
    OCStackResult result = OC_STACK_OK;

    token = (char*) strtok_r(pChar, ACTION_DELIMITER, &tokenPtr);
    *setName = (char *) OCMalloc(strlen(token) + 1);
    VARIFY_POINTER_NULL(*setName, result, exit)
    VARIFY_PARAM_NULL(token, result, exit)
    strncpy(*setName, token, strlen(token) + 1);

    token = strtok_r(NULL, ACTION_DELIMITER, &tokenPtr);
    VARIFY_POINTER_NULL(pa, result, exit)
    VARIFY_PARAM_NULL(token, result, exit)
    *pa = atoi(token);

    return OC_STACK_OK;

exit:
    OCFREE(*setName);
    return result;
}

OCStackResult BuildActionSetFromString(OCActionSet **set, char* actiondesc)
{
    OCStackResult result = OC_STACK_OK;

    char *iterToken = NULL, *iterTokenPtr = NULL;
    char *descIterToken = NULL, *descIterTokenPtr = NULL;
    char *attrIterToken = NULL, *attrIterTokenPtr = NULL;
    char *desc = NULL, *attr = NULL;
    char *key = NULL, *value = NULL;

    OCAction *action = NULL;
    OCCapability *capa = NULL;

    OC_LOG(INFO, TAG, PCF("Build ActionSet Instance."));

    *set = (OCActionSet*) OCMalloc(sizeof(OCActionSet));
    VARIFY_POINTER_NULL(*set, result, exit)

    iterToken = (char *) strtok_r(actiondesc, ACTION_DELIMITER, &iterTokenPtr);

    // ActionSet Name
    memset(*set, 0, sizeof(OCActionSet));
    (*set)->actionsetName = (char *) OCMalloc(strlen(iterToken) + 1);
    VARIFY_POINTER_NULL((*set)->actionsetName, result, exit)
    VARIFY_PARAM_NULL(iterToken, result, exit)
    strncpy((*set)->actionsetName, iterToken, strlen(iterToken) + 1);

    // Time info. for Scheduled/Recursive Group action.
    // d is meant Day of the week.
    // T is meant ActionType.
    // yyyy-mm-dd hh:mm:ss d
    iterToken = (char *) strtok_r(NULL, ACTION_DELIMITER, &iterTokenPtr);
    VARIFY_PARAM_NULL(iterToken, result, exit)
#ifndef WITH_ARDUINO
    sscanf(iterToken, "%ld %d", &(*set)->timesteps, &(*set)->type);
#endif

    OC_LOG_V(INFO, TAG, "ActionSet Name : %s", (*set)->actionsetName);

    iterToken = (char *) strtok_r(NULL, ACTION_DELIMITER, &iterTokenPtr);
    while (iterToken)
    {
        desc = (char *) OCMalloc(strlen(iterToken) + 1);
        VARIFY_POINTER_NULL(desc, result, exit)
        VARIFY_PARAM_NULL(desc, result, exit)
        strncpy(desc, iterToken, strlen(iterToken) + 1);
        descIterToken = (char *) strtok_r(desc, ATTR_DELIMITER,
                &descIterTokenPtr);
        while (descIterToken)
        {
            attr = (char *) OCMalloc(strlen(descIterToken) + 1);
            VARIFY_POINTER_NULL(attr, result, exit)
            VARIFY_PARAM_NULL(descIterToken, result, exit)
            strncpy(attr, descIterToken, strlen(descIterToken) + 1);

            attrIterToken = (char *) strtok_r(attr, ATTR_ASSIGN,
                    &attrIterTokenPtr);
            key = (char *) OCMalloc(strlen(attrIterToken) + 1);
            VARIFY_POINTER_NULL(key, result, exit)
            VARIFY_PARAM_NULL(attrIterToken, result, exit)
            strncpy(key, attrIterToken, strlen(attrIterToken) + 1);

            attrIterToken = (char *) strtok_r(NULL, ATTR_ASSIGN,
                    &attrIterTokenPtr);
            value = (char *) OCMalloc(strlen(attrIterToken) + 1);
            VARIFY_POINTER_NULL(value, result, exit)
            VARIFY_PARAM_NULL(attrIterToken, result, exit)
            strncpy(value, attrIterToken, strlen(attrIterToken) + 1);

            if (strcmp(key, "uri") == 0)
            {
                OC_LOG(INFO, TAG, PCF("Build OCAction Instance."));

                action = (OCAction*) OCMalloc(sizeof(OCAction));
                VARIFY_POINTER_NULL(action, result, exit)
                memset(action, 0, sizeof(OCAction));
                action->resourceUri = (char *) OCMalloc(strlen(value) + 1);
                VARIFY_POINTER_NULL(action->resourceUri, result, exit)
                VARIFY_PARAM_NULL(value, result, exit)
                strncpy(action->resourceUri, value, strlen(value) + 1);
            }
            else
            {
                if ((key != NULL) && (value != NULL))
                {
                    OC_LOG(INFO, TAG, PCF("Build OCCapability Instance."));

                    capa = (OCCapability*) OCMalloc(sizeof(OCCapability));
                    VARIFY_POINTER_NULL(capa, result, exit)
                    memset(capa, 0, sizeof(OCCapability));

                    capa->capability = (char *) OCMalloc(strlen(key) + 1);
                    VARIFY_POINTER_NULL(capa->capability, result, exit)
                    VARIFY_PARAM_NULL(key, result, exit)
                    strncpy(capa->capability, key, strlen(key) + 1);

                    capa->status = (char *) OCMalloc(strlen(value) + 1);
                    VARIFY_POINTER_NULL(capa->status, result, exit)
                    VARIFY_PARAM_NULL(value, result, exit)
                    strncpy(capa->status, value, strlen(value) + 1);

                    VARIFY_POINTER_NULL(action, result, exit)

                    AddCapability(&action->head, capa);
                }
            }

            OCFREE(key)
            OCFREE(value)
            OCFREE(attr)

            descIterToken = (char *) strtok_r(NULL, ATTR_DELIMITER,
                    &descIterTokenPtr);
        }

        AddAction(&(*set)->head, action);
        iterToken = (char *) strtok_r(NULL, ACTION_DELIMITER, &iterTokenPtr);
        OCFREE(desc);
    }

    return OC_STACK_OK;
exit:
    OCFREE(attr)
    OCFREE(desc)
    OCFREE(capa)
    OCFREE(action)
    OCFREE(*set)
    OCFREE(key)
    OCFREE(value)
    OCFREE(attr)

    return result;
}

OCStackResult BuildStringFromActionSet(OCActionSet* actionset, char** desc)
{
    char temp[1024] = { 0 };
    int remaining = 1023;
    OCStackResult res = OC_STACK_ERROR;

    OCAction *action = actionset->head;

    if (remaining >= strlen(actionset->actionsetName) + 1)
    {
        strcat(temp, actionset->actionsetName);
        remaining -= strlen(actionset->actionsetName);
        strcat(temp, ACTION_DELIMITER);
        remaining--;
    }
    else
    {
        res = OC_STACK_ERROR;
        goto exit;
    }

    while (action != NULL)
    {
        strcat(temp, "uri=");
        remaining -= strlen("uri=");
        strcat(temp, action->resourceUri);
        remaining -= strlen(action->resourceUri);
        strcat(temp, "|");
        remaining--;

        OCCapability *capas = action->head;
        while (capas != NULL)
        {
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
            strcat(temp, ACTION_DELIMITER);
            remaining--;
        }
    }

    *desc = (char *) OCMalloc(1024 - remaining);
    VARIFY_POINTER_NULL(*desc, res, exit);
    strcpy(*desc, temp);

    return OC_STACK_OK;

exit:
    OCFREE(*desc);
    return res;

}

OCStackApplicationResult ActionSetCB(void* context, OCDoHandle handle,
        OCClientResponse* clientResponse)
{
    OC_LOG(INFO, TAG, PCF("Entering BuildActionJSON"));

    ClientRequestInfo *info = GetClientRequestInfo(clientRequstList, handle);

    if (info)
    {
        int idx;

        unsigned char *responseJson;
        responseJson = (unsigned char *) OCMalloc(
                (unsigned int) (strlen((char *) clientResponse->resJSONPayload)
                        + 1));

        if( responseJson == NULL )
            return OC_STACK_DELETE_TRANSACTION;

        // We need the body of response.
        // Copy the body from the response
        strcpy((char *) responseJson,
                ((char *) clientResponse->resJSONPayload + OC_JSON_PREFIX_LEN));
        idx = strlen((char *) responseJson) - OC_JSON_SUFFIX_LEN;
        // And insert NULL at the end of body.
        (responseJson[idx]) = 0;

        OCEntityHandlerResponse response = { 0 };
        response.ehResult = OC_EH_OK;
        response.payload = (char*)responseJson;
        response.payloadSize = (unsigned int) strlen((char *) responseJson) + 1;
        response.persistentBufferFlag = 0;
        response.requestHandle = (OCRequestHandle) info->ehRequest;
        response.resourceHandle = (OCResourceHandle) info->collResource;

        OCDoResponse(&response);

        RemoveClientRequestInfo(&clientRequstList, info);
        OCFREE(info)
        OCFREE(responseJson)
    }

    return OC_STACK_KEEP_TRANSACTION;
}

void ActionSetCD(void *context)
{
}

OCStackResult BuildActionJSON(OCAction* action, unsigned char* bufferPtr,
        uint16_t *remaining)
{
    OCStackResult ret = OC_STACK_ERROR;
    cJSON *json;
    cJSON *body;

    char *jsonStr;
    uint16_t jsonLen;

    OC_LOG(INFO, TAG, PCF("Entering BuildActionJSON"));
    json = cJSON_CreateObject();

    cJSON_AddItemToObject(json, "rep", body = cJSON_CreateObject());

    OCCapability* pointerCapa = action->head;
    while (pointerCapa)
    {
        cJSON_AddStringToObject(body, pointerCapa->capability,
                pointerCapa->status);
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
    free(jsonStr);

    return ret;
}

unsigned int GetNumOfTargetResource(OCAction *actionset)
{
    int numOfResource = 0;

    OCAction *pointerAction = actionset;

    while (pointerAction != NULL)
    {
        numOfResource++;
        pointerAction = pointerAction->next;
    }

    return numOfResource;
}


#define DEFAULT_CONTEXT_VALUE 0x99

OCStackResult SendAction(OCDoHandle *handle, const char *targetUri,
        const unsigned char *action)
{
    OCCallbackData cbdata = { 0 };
    cbdata.cb = &ActionSetCB;
    cbdata.cd = NULL;
    cbdata.context = (void*)DEFAULT_CONTEXT_VALUE;

// TODO: Selecting OC_IPV4.
// It is temporary change as OC_ALL is not working currently. Remove this code and use OC_ALL
// once it is functioning.

    return OCDoResource(handle, OC_REST_PUT, targetUri,
    NULL, (char *) action, OC_IPV4, OC_NA_QOS, &cbdata, NULL, 0);
}

OCStackResult DoAction(OCResource* resource, OCActionSet* actionset,
        OCServerRequest* requestHandle)
{
    OCStackResult result = OC_STACK_ERROR;
    OCAction *pointerAction = actionset->head;

    while (pointerAction != NULL)
    {
        unsigned char actionDesc[MAX_RESPONSE_LENGTH] = { 0 };
        unsigned char* actionDescPtr = actionDesc;
        uint16_t remaining = MAX_RESPONSE_LENGTH;

        strncpy((char *) actionDescPtr, (const char *) OC_JSON_PREFIX,
                strlen((const char *) OC_JSON_PREFIX) + 1);
        BuildActionJSON(pointerAction, actionDescPtr, &remaining);
        strncat((char *) actionDescPtr, (const char *) OC_JSON_SUFFIX,
                strlen((const char *) OC_JSON_SUFFIX));

        ClientRequestInfo *info = (ClientRequestInfo *) OCMalloc(
                sizeof(ClientRequestInfo));

        if( info == NULL )
            return OC_STACK_NO_MEMORY;

        memset(info, 0, sizeof(ClientRequestInfo));

        info->collResource = resource;
        info->ehRequest = requestHandle;

        result = SendAction(&info->required, pointerAction->resourceUri,
                actionDescPtr);
        if (result != OC_STACK_OK)
        {
            return result;
        }

        AddClientRequestInfo(&clientRequstList, info);

        pointerAction = pointerAction->next;
    }

    return result;
}

void DoScheduledGroupAction()
{
    OC_LOG(INFO, TAG, PCF("DoScheduledGroupAction Entering..."));
    ScheduledResourceInfo* info = GetScheduledResource(scheduleResourceList);

    if (info == NULL)
    {
        OC_LOG(INFO, TAG, PCF("Target resource is NULL"));
        goto exit;
    }
    else if (info->resource == NULL)
    {
        OC_LOG(INFO, TAG, PCF("Target resource is NULL"));
        goto exit;
    }
    else if (info->actionset == NULL)
    {
        OC_LOG(INFO, TAG, PCF("Target ActionSet is NULL"));
        goto exit;
    }
    else if (info->ehRequest == NULL)
    {
        OC_LOG(INFO, TAG, PCF("Target ActionSet is NULL"));
        goto exit;
    }
#ifndef WITH_ARDUINO
    pthread_mutex_lock(&lock);
#endif
    DoAction(info->resource, info->actionset, info->ehRequest);
#ifndef WITH_ARDUINO
    pthread_mutex_unlock(&lock);
#endif

    if (info->actionset->type == RECURSIVE)
    {
        ScheduledResourceInfo *schedule;
        schedule = (ScheduledResourceInfo *) OCMalloc(
                sizeof(ScheduledResourceInfo));

        if (schedule)
        {
            OC_LOG(INFO, TAG, PCF("Building New Call Info."));
            memset(schedule, 0, sizeof(ScheduledResourceInfo));

            if (info->actionset->timesteps > 0)
            {
#ifndef WITH_ARDUINO
                pthread_mutex_lock(&lock);
#endif
                schedule->resource = info->resource;
                schedule->actionset = info->actionset;
                schedule->ehRequest = info->ehRequest;

                schedule->time = registerTimer(info->actionset->timesteps,
                        &schedule->timer_id,
                        &DoScheduledGroupAction);

                OC_LOG(INFO, TAG, PCF("Reregisteration."));
#ifndef WITH_ARDUINO
                pthread_mutex_unlock(&lock);
#endif
                AddScheduledResource(&scheduleResourceList, schedule);
            }
        }
    }

    RemoveScheduledResource(&scheduleResourceList, info);

    exit:

    return;
}

OCStackResult BuildCollectionGroupActionJSONResponse(
        OCMethod method/*OCEntityHandlerFlag flag*/, OCResource *resource,
        OCEntityHandlerRequest *ehRequest)
{
    OCStackResult stackRet = OC_STACK_ERROR;

    OC_LOG(INFO, TAG, PCF("Group Action is requested."));
    // if (stackRet == OC_STACK_OK)
    {
        char *doWhat = NULL;
        char *details = NULL;

        size_t bufferLength = 0;
        unsigned char buffer[MAX_RESPONSE_LENGTH] = { 0 };
        unsigned char *bufferPtr = NULL;

        bufferPtr = buffer;

        OCResource * collResource = (OCResource *) ehRequest->resource;

        char *jsonResponse;

        stackRet = ExtractKeyValueFromRequest((char *) ehRequest->reqJSONPayload,
                &doWhat, &details);

        if(stackRet != OC_STACK_OK)
        {
            OC_LOG_V(ERROR, TAG, "ExtractKeyValueFromRequest failed: %d", stackRet);
            return stackRet;
        }

        stackRet = OC_STACK_ERROR;

        cJSON *json;
        cJSON *format;

        if (method == OC_REST_PUT)
        {
            json = cJSON_CreateObject();
            cJSON_AddStringToObject(json, "href", resource->uri);
            cJSON_AddItemToObject(json, "rep", format = cJSON_CreateObject());

            OC_LOG(INFO, TAG, PCF("Group Action[PUT]."));

            if (strcmp(doWhat, ACTIONSET) == 0)
            {
                OCActionSet *actionSet = NULL;
                stackRet = BuildActionSetFromString(&actionSet, details);

                if(stackRet == OC_STACK_OK)
                {
                    if (actionSet != NULL)
                    {
                        stackRet = AddActionSet(&resource->actionsetHead,
                                actionSet);
                        if (stackRet == OC_STACK_ERROR)
                        {
                            if(actionSet != NULL)
                            {
                                DeleteActionSet( &actionSet );
                            }
                            OC_LOG(INFO, TAG, PCF("Duplicated ActionSet "));
                        }
                    }
                }
                else
                {
                    stackRet = OC_STACK_ERROR;
                }

            }
            else if (strcmp(doWhat, DELETE_ACTIONSET) == 0)
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
                if(stackRet == OC_STACK_OK)
                    response.ehResult = OC_EH_OK;
                else
                    response.ehResult = OC_EH_ERROR;
                response.payload = (char*)buffer;
                response.payloadSize = bufferLength + 1;
                response.persistentBufferFlag = 0;
                response.requestHandle =
                        (OCRequestHandle) ehRequest->requestHandle;
                response.resourceHandle = (OCResourceHandle) collResource;
                stackRet = OCDoResponse(&response);
            }
        }

        if (method == OC_REST_POST)
        {
            OCActionSet *actionset = NULL;

            json = cJSON_CreateObject();
            cJSON_AddStringToObject(json, "href", resource->uri);

            if ((strcmp(doWhat, DO_ACTION) == 0)
                    || (strcmp(doWhat, "DoScheduledAction") == 0))
            {
                char *pActionsetName = NULL;
                long int delay = -1;

                if (strcmp(doWhat, "DoScheduledAction") == 0)
                {
                    stackRet = ExtractActionSetNameAndDelaytime(details,
                            &pActionsetName, &delay);

                    OCFREE(details)
                    details = pActionsetName;
                }
                else
                {
                    stackRet = OC_STACK_OK;
                }

                if (stackRet == OC_STACK_OK)
                {
                    if (GetActionSet(details, resource->actionsetHead,
                            &actionset) != OC_STACK_OK)
                    {
                        OC_LOG(INFO, TAG, PCF("ERROR"));
                        stackRet = OC_STACK_ERROR;
                    }

                    if (actionset == NULL)
                    {
                        OC_LOG(INFO, TAG, PCF("Cannot Find ActionSet"));
                        stackRet = OC_STACK_ERROR;
                    }
                    else
                    {
                        OC_LOG(INFO, TAG, PCF("Group Action[POST]."));
                        if (actionset->type == NONE)
                        {
                            OC_LOG_V(INFO, TAG, "Execute ActionSet : %s",
                                    actionset->actionsetName);
                            unsigned int num = GetNumOfTargetResource(
                                    actionset->head);

                            ((OCServerRequest *) ehRequest->requestHandle)->ehResponseHandler =
                                    HandleAggregateResponse;
                            ((OCServerRequest *) ehRequest->requestHandle)->numResponses =
                                    num + 1;

                            DoAction(resource, actionset,
                                    (OCServerRequest*) ehRequest->requestHandle);
                            stackRet = OC_STACK_OK;
                        }
                        else
                        {
                            OC_LOG_V(INFO, TAG, "Execute Scheduled ActionSet : %s",
                                    actionset->actionsetName);

                            delay =
                                    (delay == -1 ? actionset->timesteps : delay);

                            ScheduledResourceInfo *schedule;
                            schedule = (ScheduledResourceInfo *) OCMalloc(
                                    sizeof(ScheduledResourceInfo));

                            if (schedule)
                            {
                                OC_LOG(INFO, TAG, PCF("Building New Call Info."));
                                memset(schedule, 0,
                                        sizeof(ScheduledResourceInfo));

                                schedule->resource = resource;
                                schedule->actionset = actionset;
                                schedule->ehRequest =
                                        (OCServerRequest*) ehRequest->requestHandle;

                                if (delay > 0)
                                {
                                    OC_LOG_V(INFO, TAG, "delay_time is %lf seconds.",
                                            actionset->timesteps);

                                    schedule->time = registerTimer(delay,
                                            &schedule->timer_id,
                                            &DoScheduledGroupAction);

                                    AddScheduledResource(&scheduleResourceList,
                                            schedule);
                                    stackRet = OC_STACK_OK;
                                }
                                else
                                {
                                    stackRet = OC_STACK_ERROR;
                                }
                            }
                        }
                    }
                }
            }
            else if (strcmp(doWhat, "CancelAction") == 0)
            {
                ScheduledResourceInfo *info =
                        GetScheduledResourceByActionSetName(scheduleResourceList, details);

                if(info != NULL)
                {
                    unregisterTimer(info->timer_id);

                    RemoveScheduledResource(&scheduleResourceList, info);
                    stackRet = OC_STACK_OK;
                }
                else
                {
                    stackRet = OC_STACK_ERROR;
                }
            }

            else if (strcmp(doWhat, GET_ACTIONSET) == 0)
            {
                char *plainText = NULL;
                OCActionSet *actionset = NULL;

                cJSON_AddItemToObject(json, "rep", format =
                        cJSON_CreateObject());
                GetActionSet(details, resource->actionsetHead, &actionset);
                if (actionset != NULL)
                {
                    BuildStringFromActionSet(actionset, &plainText);

                    if (plainText != NULL)
                    {
                        cJSON_AddStringToObject(format, ACTIONSET, plainText);
                    }
                    OCFree(plainText);
                    stackRet = OC_STACK_OK;
                }
            }


            jsonResponse = cJSON_Print(json);
            cJSON_Delete(json);

            strcat((char *) bufferPtr, jsonResponse);

            bufferLength = strlen((const char *) buffer);
            if (bufferLength > 0)
            {
                OCEntityHandlerResponse response = { 0 };
                if(stackRet == OC_STACK_OK)
                    response.ehResult = OC_EH_OK;
                else
                    response.ehResult = OC_EH_ERROR;
                response.payload = (char *)buffer;
                response.payloadSize = bufferLength + 1;
                response.persistentBufferFlag = 0;
                response.requestHandle =
                        (OCRequestHandle) ehRequest->requestHandle;
                response.resourceHandle = (OCResourceHandle) collResource;
                stackRet = OCDoResponse(&response);
            }
        }

        OCFREE(doWhat)
        OCFREE(details)
    }

    return stackRet;
}
