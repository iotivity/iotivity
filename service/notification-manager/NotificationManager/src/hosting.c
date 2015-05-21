//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

// Hosting Header
#include "hosting.h"
#include "virtualResource.h"

// External Lib
#include "cJSON.h"

/*
 * internal function & static variable
 */
///////////////////////////////////////////////////////////////////////////////////////////////////
static MirrorResourceList *s_mirrorResourceList = NULL;
static RequestHandleList *s_requestHandleList = NULL;

#define OIC_COORDINATING_FLAG "/hosting"
#define OIC_STRING_MAX_VALUE 100

#define OC_DEFAULT_ADDRESS               "224.0.1.187"
#define OC_WELL_KNOWN_COORDINATING_QUERY "coap://224.0.1.187:5683/oc/core?rt=Resource.Hosting"
#define OC_COORDINATING_QUERY            "/oc/core?rt=Resource.Hosting"
#define DEFAULT_CONTEXT_VALUE 0x99

/*
 * Presence Func for hosting
 */

/**
 *
 * request presence for coordinating
 *
 * @param[in] originResourceAddr - pointer of address string of original resource
 *
 * @return
 *     OC_STACK_OK
 *     OC_STACK_ERROR
 */
OCStackResult requestPresence(char *originResourceAddr);

/**
 *
 * callback function that call when response of presence request received
 *
 * @param[in] originResourceAddr - pointer of address string of original resource
 *
 * @return
 *     OC_STACK_OK
 *     OC_STACK_ERROR
 */
OCStackApplicationResult requestPresenceCB(void *context, OCDoHandle handle,
        OCClientResponse *clientResponse);

/**
 *
 * build mirror resource list by clientResponse
 *
 * @param[in] handle - not using...
 * @param[in] clientResponse - client response that mirror resources are stored
 *
 * @return
 *     pointer of result MirrorResourceList
 */
MirrorResourceList *buildMirrorResourceList(OCDoHandle handle, OCClientResponse *clientResponse);

/**
 *
 * build mirror resource by JSON payload
 *
 * @param[in] ocArray_sub - pointer of json payload string
 *
 * @return
 *     pointer of result MirrorResource
 */
MirrorResource *buildMirrorResource(cJSON *ocArray_sub);

/**
 *
 * This method is used when setting queryUri, registering callback function and starting OCDoResource() Function in order to find Coordinatee Candidate
 *
 * @brief discover coordinatee candidate
 *
 * @return
 *     OC_STACK_OK               - no errors
 *     OC_STACK_INVALID_CALLBACK - invalid callback function pointer
 *     OC_STACK_INVALID_METHOD   - invalid resource method
 *     OC_STACK_INVALID_URI      - invalid required or reference URI
 *     OC_STACK_INVALID_QUERY    - number of resource types specified for filtering presence
 *                                 notifications exceeds @ref MAX_PRESENCE_FILTERS.
 *     OC_STACK_ERROR            - otherwise error(initialized value)
 */
int requestCoordinateeCandidateDiscovery(char *address);

/**
 *
 * This method is used to add a coordinator resource callback method in mirrorResourceList when host resource discovered.
 *
 * @param[in] context
 *              Context for callback method
 * @param[in] handle
 *              Handle to an @ref OCDoResource invocation.
 * @param[in] clientResponse
 *              Response from queries to remote servers. Queries are made by calling the @ref OCDoResource API.
 *
 * @brief callback for receiving response of discoverCoordinateeCandidate()
 *
 * @return
 *     PRINT("Callback Context for DISCOVER query recvd successfully")      - context is DEFAULT_CONTEXT_VALUE
 *     call the buildMirrorResource() method                                    - clientResponse is not NULL && clientResponse->result is OC_STACK_OK
 *     OC_STACK_KEEP_TRANSACTION                                                - otherwise case
 */
OCStackApplicationResult requestCoordinateeCandidateDiscoveryCB(void *context, OCDoHandle handle,
        OCClientResponse *clientResponse);

/**
 *
 * This method is used when setting queryUri, registering callback function and starting OCDoResource() Function in order to request resource coordination
 *
 * @brief
 *
 * @param[in] mirrorResource
 *          mirrorResource for using in order to request resource coordination
 *
 * @return
 *     OC_STACK_OK               - no errors
 *     OC_STACK_INVALID_CALLBACK - invalid callback function pointer
 *     OC_STACK_INVALID_METHOD   - invalid resource method
 *     OC_STACK_INVALID_URI      - invalid required or reference URI
 *     OC_STACK_INVALID_QUERY    - number of resource types specified for filtering presence
 *                                 notifications exceeds @ref MAX_PRESENCE_FILTERS.
 *     OC_STACK_ERROR            - otherwise error(initialized value)
 */
OCStackResult requestResourceObservation(MirrorResource *mirrorResource);

/**
 *
 * This method is used to handle callback of requestCoordination method.
 *
 * @param[in] context
 *              Context for callback method
 * @param[in] handle
 *              Handle to update mirror resource and check errorResponse
 * @param[in] clientResponse
 *              Response from queries to remote servers. Queries are made by calling the @ref OCDoResource API.
 *
 * @brief callback when receiving response of coordinating requestion.
 *
 * @todo diverge return value
 *
 * @return
 *
 *     OC_STACK_KEEP_TRANSACTION                                            - otherwise case
 */
OCStackApplicationResult requestResourceObservationCB(void *context, OCDoHandle handle,
        OCClientResponse *clientResponse);

/**
 *
 * This method is used to check resource validation and delete resource if it is not exist(not alive).
 *
 * @brief check mirror resource is alive
 *
 * @param[in] requestHandle
 *              Handle to check mirror resource
 *
 * @return
 *
 *     OC_STACK_DELETE_TRANSACTION                                              - otherwise case
 */
OCStackApplicationResult checkResourceValidation(OCDoHandle requestHandle);

/**
 *
 * register Mirror resource in the base resource list
 *
 * @param[in] requestHandle
 *              Handle to check mirror resource
 *
 * @return
 *     OC_STACK_OK
 *     OC_STACK_ERROR
 */
OCStackResult registerMirrorResource(MirrorResource *node);

/**
 *
 * update resource
 *
 * @param[in] sourceHandle - handle of source resource
 * @param[in] payload - pointer of json payload string that update items stored
 *
 * @return
 *     pointer of mirror resource. return NULL if there is any error.
 */
MirrorResource *updateMirrorResource(OCDoHandle sourceHandle, const char *payload);

/**
 *
 * build response payload
 *
 * @param[in] ehRequest - pointer of handler of entity handler request that to be responded
 *
 * @return
 *     OC_STACK_OK
 *     OC_STACK_ERROR
 */
char *buildResponsePayload (OCEntityHandlerRequest *ehRequest);

/**
 *
 * handle "Get" request
 *
 * @param[in] ehRequest - pointer of handler of entity handler request
 * @param[out] payload - pointer of payload to be responded
 * @param[in] maxPayloadSize - size of payload
 *
 * @return
 *     OC_EH_OK - success to copy response payload
 *     OC_EH_ERROR - error to copy response payload
 */
OCEntityHandlerResult handleGetRequest (OCEntityHandlerRequest *ehRequest,
                                        char *payload, uint16_t maxPayloadSize);

/**
 *
 * handle request for non-existing resource
 *
 * @param[in] ehRequest - pointer of handler of entity handler request
 * @param[out] payload - pointer of payload to be responded
 * @param[in] maxPayloadSize - size of payload
 *
 * @return
 *     OC_EH_RESOURCE_DELETED - resource deleted
 */
OCEntityHandlerResult handleNonExistingResourceRequest(OCEntityHandlerRequest *ehRequest,
        char *payload, uint16_t maxPayloadSize);

/**
 *
 * callback function that called when source resource changed
 *
 * @param[in] flag - entity handler flag
 * @param[in] entityHandlerRequest - pointer of entity handler request
 *
 * @return
 *     OC_EH_OK
 *     OC_EH_ERROR
 */
OCEntityHandlerResult resourceEntityHandlerCB (OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest);

/**
 *
 * request that address is alive
 *
 * @param[in] address - pointer of address string
 *
 * @return
 *     OC_STACK_OK
 *     OC_STACK_ERROR
 */
OCStackResult requestIsAlive(const char *address);

/**
 *
 * get string value of OCStackResult code
 *
 * @param[in] result - OCStringResult code
 *
 * @return
 *     pointer of result string value
 */
const char *getResultString(OCStackResult result);

OCStackResult requestQuery(RequestHandle *request, OCMethod method,
                           const char *queryAddress, const char *queryUri);
OCStackApplicationResult requestQueryCB(void *context, OCDoHandle handle,
                                        OCClientResponse *clientResponse);
OCEntityHandlerResponse buildEntityHandlerResponse(OCEntityHandlerRequest *entityHandlerRequest,
        const char *clientPayload);
OCEntityHandlerResult handleRequestPayload (OCEntityHandlerRequest *entityHandlerRequest,
        char *payload, uint16_t maxPayloadSize);

/*
 * for Lite Device Side
 */

/**
 *
 * register resource as coordinatable
 *
 * @param[in] handle - resource handle
 * @param[in] resourceTypeName - resource type name
 * @param[in] resourceInterfaceName - resource interface name
 * @param[in] resourceUri - resource URI
 * @param[in] entityHandler - entity handler
 * @param[in] resourceProperties - resource properties
 *
 * @return
 *     pointer of result string value
 */
OCStackResult registerResourceAsCoordinatable(OCResourceHandle *handle,
        const char *resourceTypeName, const char *resourceInterfaceName,
        const char *resourceUri, OCEntityHandler entityHandler, uint8_t resourceProperties);

OCStackResult registerResourceAsCoordinatable(OCResourceHandle *handle,
        const char *resourceTypeName,
        const char *resourceInterfaceName,
        const char *resourceUri,
        OCEntityHandler entityHandler,
        uint8_t resourceProperties)
{
    OCStackResult ret = OC_STACK_OK;
    size_t coordinateUriLen = sizeof(char) * (strlen(resourceUri) +
            strlen(OIC_COORDINATING_FLAG) + 1);
    char *coordinatingURI = (char *)malloc(coordinateUriLen);
    if(coordinatingURI == NULL)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "memory alloc fail : coordinatingURI");
        return OC_STACK_NO_MEMORY;
    }

    snprintf(coordinatingURI, coordinateUriLen,"%s%s", resourceUri, OIC_COORDINATING_FLAG);

    OC_LOG_V(DEBUG, HOSTING_TAG, "requiredUri+coordinatingFlag = %s", coordinatingURI);

    ret = OCCreateResource(handle, resourceTypeName, resourceInterfaceName,
            coordinatingURI, entityHandler, resourceProperties);
    free(coordinatingURI);
    return ret;
}

/*
 *  for Hosting Device Side
 */
OCStackResult OICStartCoordinate()
{
    OCStackResult ret = OC_STACK_ERROR;
    if (OCInit((char *) NULL, 0, OC_CLIENT_SERVER) != OC_STACK_OK)
    {
        OC_LOG(ERROR, HOSTING_TAG, PCF("OCStack init ERROR"));
    }
    else
    {
        s_mirrorResourceList = createMirrorResourceList();
        s_requestHandleList = createRequestHandleList();
        ret = requestPresence(OC_MULTICAST_PREFIX);
    }

    return ret;
}

OCStackResult OICStopCoordinate()
{
    OCStackResult result = OC_STACK_ERROR;

    if (OCStop() == OC_STACK_OK)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "OCStack Stop OK");
    }

    result = destroyMirrorResourceList(s_mirrorResourceList);
    s_mirrorResourceList = NULL;
    if(result != OC_STACK_OK)
    {
        return OC_STACK_ERROR;
    }

    return result;
}

int requestCoordinateeCandidateDiscovery(char *sourceResourceAddress)
{
    OCStackResult result;
    OCCallbackData cbData;
    OCDoHandle handle;

    /* Start a discovery query*/
    char queryUri[OIC_STRING_MAX_VALUE] = { '\0' };
    if (sourceResourceAddress == NULL)
    {
        strncpy(queryUri, OC_WELL_KNOWN_COORDINATING_QUERY, sizeof(queryUri));
    }
    else
    {
        snprintf(queryUri, sizeof(queryUri), "coap://%s%s",
                sourceResourceAddress , OC_COORDINATING_QUERY);
    }

    cbData.cb = requestCoordinateeCandidateDiscoveryCB;
    cbData.context = (void *)DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;

    result = OCDoResource(&handle, OC_REST_GET, queryUri, OIC_COORDINATING_FLAG, 0,
            OC_TRANSPORT, OC_LOW_QOS, &cbData, NULL, 0);
    if (result != OC_STACK_OK)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "OCStack resource error");
    }
    else
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "Host Resource Finding...");
    }
    return result;
}

OCStackResult requestPresence(char *sourceResourceAddress)
{
    OCStackResult result = OC_STACK_ERROR;
    OCCallbackData cbData;
    OCDoHandle handle;

    if (sourceResourceAddress == NULL)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "SourceResourceAddress is not available.");
        return result;
    }

    cbData.cb = requestPresenceCB;
    cbData.context = (void *)DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;

    char queryUri[OIC_STRING_MAX_VALUE] = { '\0' };
    snprintf(queryUri, sizeof(queryUri), "coap://%s%s", sourceResourceAddress , OC_PRESENCE_URI);
    OC_LOG_V(DEBUG, HOSTING_TAG, "initializePresenceForCoordinating Query : %s", queryUri);

    result = OCDoResource(&handle, OC_REST_PRESENCE, queryUri, 0, 0,
            OC_TRANSPORT, OC_LOW_QOS, &cbData, NULL, 0);

    if (result != OC_STACK_OK)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "initializePresenceForCoordinating error");
        result = OC_STACK_ERROR;
    }
    else
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "Success initializePresenceForCoordinating");
    }

    return result;
}

OCStackApplicationResult requestPresenceCB(void *context, OCDoHandle handle,
        OCClientResponse *clientResponse)
{
    uint8_t remoteIpAddress[4];
    uint16_t remotePortNumber;
    char address[OIC_STRING_MAX_VALUE] = { '\0' };

    if (context == (void *) DEFAULT_CONTEXT_VALUE)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "\tCallback Context for presence CB recv successfully");
    }
    if (clientResponse)
    {
        OCDevAddrToIPv4Addr((OCDevAddr *) clientResponse->addr, remoteIpAddress,
                            remoteIpAddress + 1, remoteIpAddress + 2, remoteIpAddress + 3);
        OCDevAddrToPort((OCDevAddr *) clientResponse->addr, &remotePortNumber);
        OC_LOG_V(DEBUG, HOSTING_TAG, "\tStackResult: %s",  getResultString(clientResponse->result));
        OC_LOG_V(DEBUG, HOSTING_TAG, "\tStackResult: %d",  clientResponse->result);
        OC_LOG_V(DEBUG, HOSTING_TAG,
                 "\tPresence Device =============> Presence %s @ %d.%d.%d.%d:%d",
                 clientResponse->resJSONPayload, remoteIpAddress[0], remoteIpAddress[1],
                 remoteIpAddress[2], remoteIpAddress[3], remotePortNumber);

        snprintf(address, sizeof(address), "%d.%d.%d.%d:%d", remoteIpAddress[0], remoteIpAddress[1],
                remoteIpAddress[2], remoteIpAddress[3], remotePortNumber);
        if (clientResponse->result == OC_STACK_OK)
        {
            requestCoordinateeCandidateDiscovery(address);
        }
        if (clientResponse->result == OC_STACK_PRESENCE_STOPPED
            || clientResponse->result == OC_STACK_PRESENCE_TIMEOUT
            || clientResponse->result == OC_STACK_PRESENCE_DO_NOT_HANDLE)
        {
            requestIsAlive(address);
        }

    }
    return OC_STACK_KEEP_TRANSACTION;
}

OCStackApplicationResult requestCoordinateeCandidateDiscoveryCB(void *ctx, OCDoHandle handle,
        OCClientResponse *clientResponse)
{
    OC_LOG(DEBUG, HOSTING_TAG, "Found Host Resource");
    OCStackResult ret = OC_STACK_DELETE_TRANSACTION;

    if (ctx == (void *) DEFAULT_CONTEXT_VALUE)
    {
        OC_LOG(DEBUG, HOSTING_TAG, "Callback Context for DISCOVER query recvd successfully");
    }
    if (clientResponse && clientResponse->result == OC_STACK_OK)
    {
        MirrorResourceList *vList = buildMirrorResourceList(handle, clientResponse);
        if (vList != NULL)
        {

            if (vList->headerNode == NULL)
            {
                OC_LOG(DEBUG, HOSTING_TAG, "This Discover Response is empty");
                destroyMirrorResourceList(vList);
                return ret;
            }

            // register All of VirtualResource
            while (vList->headerNode)
            {
                MirrorResource *mirrorResource = vList->headerNode;
                ret = ejectMirrorResource(vList, mirrorResource);
                mirrorResource->next = NULL;
                OC_LOG_V(DEBUG, HOSTING_TAG,
                        "register virtual resource uri : %s", mirrorResource->uri);
                if (ret != OC_STACK_OK)
                {
                    continue;
                }

                ret = registerMirrorResource(mirrorResource);
                if (ret != OC_STACK_OK)
                {
                    continue;
                }

                ret = insertMirrorResource(s_mirrorResourceList, mirrorResource);
                if (ret != OC_STACK_OK)
                {
                    OCDeleteResource(mirrorResource->resourceHandle[OIC_MIRROR_HANDLE]);
                    continue;
                }
                printMirrorResourceList(s_mirrorResourceList);

                ret = requestResourceObservation(mirrorResource);
                if (ret != OC_STACK_OK)
                {
                    OCDeleteResource(mirrorResource->resourceHandle[OIC_MIRROR_HANDLE]);
                    deleteMirrorResourceFromList(s_mirrorResourceList, mirrorResource);
                    continue;
                }
            }
            destroyMirrorResourceList(vList);
            if (ret != OC_STACK_OK)
            {
                return ret;
            }
        }
        ret = OC_STACK_KEEP_TRANSACTION;
    }
    return ret;
}

MirrorResourceList *buildMirrorResourceList(OCDoHandle handle, OCClientResponse *clientResponse)
{

    cJSON *discoveryJson = cJSON_CreateObject();
    discoveryJson = cJSON_Parse((char *)clientResponse->resJSONPayload);

    cJSON *ocArray = cJSON_GetObjectItem(discoveryJson, "oc");
    char *ocArray_str = cJSON_PrintUnformatted(ocArray);

    if ( strstr(ocArray_str, "[{}") == ocArray_str )
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "invalid payload : %s", ocArray_str);
        cJSON_Delete(discoveryJson);
        return NULL;
    }

    MirrorResourceList *retList = createMirrorResourceList();

    uint8_t remoteIpAddr[4];
    uint16_t remotePortNum;

    OCDevAddrToIPv4Addr((OCDevAddr *) clientResponse->addr, remoteIpAddr,
                        remoteIpAddr + 1, remoteIpAddr + 2, remoteIpAddr + 3);
    OCDevAddrToPort((OCDevAddr *) clientResponse->addr, &remotePortNum);

    char sourceaddr[OIC_STRING_MAX_VALUE] = {'\0'};
    snprintf(sourceaddr, sizeof(sourceaddr), "%d.%d.%d.%d:%d", remoteIpAddr[0], remoteIpAddr[1],
            remoteIpAddr[2], remoteIpAddr[3], remotePortNum);

    OC_LOG_V(DEBUG, HOSTING_TAG, "Host Device =============> Discovered %s @ %s",
             clientResponse->resJSONPayload, sourceaddr);

    int arraySize = cJSON_GetArraySize(ocArray);
    for (int i = 0; i < arraySize; ++i)
    {
        cJSON *ocArray_sub = cJSON_GetArrayItem(ocArray, i);
        MirrorResource *mirrorResource = buildMirrorResource(ocArray_sub);

        if (mirrorResource == NULL)
        {
            continue;
        }
        mirrorResource->address[OIC_SOURCE_ADDRESS] =
                (char *)malloc(sizeof(char) * OIC_STRING_MAX_VALUE);
        if(mirrorResource->address[OIC_SOURCE_ADDRESS] == NULL)
        {
            OC_LOG_V(DEBUG, HOSTING_TAG, "memory alloc fail : mirrorResource address_source");
            destroyMirrorResource(mirrorResource);
            continue;
        }
        snprintf(mirrorResource->address[OIC_SOURCE_ADDRESS],
                sizeof(char) * OIC_STRING_MAX_VALUE, "%s", sourceaddr);

        mirrorResource->address[OIC_MIRROR_ADDRESS] =
                (char *)malloc(sizeof(char) * OIC_STRING_MAX_VALUE);
        if(mirrorResource->address[OIC_MIRROR_ADDRESS] == NULL)
        {
            OC_LOG_V(DEBUG, HOSTING_TAG, "memory alloc fail : mirrorResource address_mirror");
            destroyMirrorResource(mirrorResource);
            continue;
        }
        snprintf(mirrorResource->address[OIC_MIRROR_ADDRESS],
                sizeof(char) * OIC_STRING_MAX_VALUE, "0.0.0.0:00");

        if (OC_STACK_OK != insertMirrorResource(retList, mirrorResource))
        {
            OC_LOG_V(DEBUG, HOSTING_TAG, "buildVirtualResourceList : insert resource fail");
            destroyMirrorResource(mirrorResource);
        }
    }

    cJSON_Delete(discoveryJson);
    return retList;
}

MirrorResource *buildMirrorResource(cJSON *ocArray_sub)
{
    MirrorResource *mirrorResource = NULL;
    const char *curValuestring = cJSON_GetObjectItem(ocArray_sub, "href")->valuestring;

    if ( strstr(curValuestring, OIC_COORDINATING_FLAG) )
    {
        mirrorResource = createMirrorResource();
        if(mirrorResource == NULL)
        {
            OC_LOG_V(DEBUG, HOSTING_TAG, "memory alloc fail for mirrorResource");
            goto RET_ERROR;
        }

        mirrorResource->uri = (char *)malloc(sizeof(char) * OIC_STRING_MAX_VALUE);
        if(mirrorResource->uri == NULL)
        {
            OC_LOG_V(DEBUG, HOSTING_TAG, "memory alloc fail for mirrorResource uri");
            goto RET_ERROR;
        }
        strncpy(mirrorResource->uri, curValuestring, strlen(curValuestring) - strlen(OIC_COORDINATING_FLAG));
        mirrorResource->uri[strlen(curValuestring) - strlen(OIC_COORDINATING_FLAG)] = '\0';
        OC_LOG_V(DEBUG, HOSTING_TAG, "VirtualResource URI : %s", mirrorResource->uri);

        cJSON *inArray_sub = cJSON_GetObjectItem(ocArray_sub, "prop");

        cJSON *tmpJSON = NULL;
        int sizetemp = 0;

        tmpJSON = cJSON_GetObjectItem(inArray_sub, "rt");
        sizetemp = cJSON_GetArraySize(tmpJSON);
        mirrorResource->prop.countResourceType = sizetemp;
        mirrorResource->prop.resourceType = (char **)malloc(sizeof(char *)*sizetemp);
        if (mirrorResource->prop.resourceType == NULL)
        {
            OC_LOG_V(DEBUG, HOSTING_TAG,
                    "memory alloc fail for mirrorResource number of resourceType");
            goto RET_ERROR;
        }
        else
        {
            for (int k = 0; k < sizetemp; ++k)
            {
                mirrorResource->prop.resourceType[k] =
                        (char *)malloc(sizeof(char) * OIC_STRING_MAX_VALUE);
                if (mirrorResource->prop.resourceType[k] == NULL)
                {
                    OC_LOG_V(DEBUG, HOSTING_TAG,
                            "memory alloc fail for mirrorResource resourceType[n]");
                    goto RET_ERROR;
                }
                memset(mirrorResource->prop.resourceType[k], '\0', OIC_STRING_MAX_VALUE);
                strncpy(mirrorResource->prop.resourceType[k],
                        cJSON_GetArrayItem(tmpJSON, k)->valuestring,
                        sizeof(char) * OIC_STRING_MAX_VALUE);
            }
        }

        tmpJSON = cJSON_GetObjectItem(inArray_sub, "if");
        sizetemp = cJSON_GetArraySize(tmpJSON);
        mirrorResource->prop.countInterface = sizetemp;
        mirrorResource->prop.resourceInterfaceName = (char **)malloc(sizeof(char *)*sizetemp);
        if (mirrorResource->prop.resourceInterfaceName == NULL)
        {
            OC_LOG_V(DEBUG, HOSTING_TAG,
                    "memory alloc fail for mirrorResource number of resourceInterfaceName");
            goto RET_ERROR;
        }

        for (int k = 0; k < sizetemp; ++k)
        {
            mirrorResource->prop.resourceInterfaceName[k] =
                    (char *)malloc(sizeof(char) * OIC_STRING_MAX_VALUE);
            if (mirrorResource->prop.resourceInterfaceName[k] == NULL)
            {
                OC_LOG_V(DEBUG, HOSTING_TAG,
                        "memory alloc fail for mirrorResource resourceInterfaceName[n]");
                goto RET_ERROR;
            }

            memset(mirrorResource->prop.resourceInterfaceName[k], '\0', OIC_STRING_MAX_VALUE);
            strncpy(mirrorResource->prop.resourceInterfaceName[k],
                    cJSON_GetArrayItem(tmpJSON, k)->valuestring,
                    sizeof(char) * OIC_STRING_MAX_VALUE);
        }
    }

    return mirrorResource;

RET_ERROR:
    destroyMirrorResource(mirrorResource);
    return NULL;
}

OCStackResult registerMirrorResource(MirrorResource *mirrorResource)
{
    OCStackResult result = OC_STACK_ERROR;

    MirrorResource *foundMirrorResource = findMirrorResourceUsingAddressAndURI(s_mirrorResourceList,
                                          mirrorResource->address[OIC_MIRROR_ADDRESS], OIC_MIRROR_ADDRESS, mirrorResource->uri);
    if (foundMirrorResource != NULL)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "Already registered resource");
        goto RETURN_ERR;
    }

    result = OCCreateResource(&(mirrorResource->resourceHandle[OIC_MIRROR_HANDLE]),
                              mirrorResource->prop.resourceType[0],
                              mirrorResource->prop.resourceInterfaceName[0],
                              mirrorResource->uri,
                              resourceEntityHandlerCB,
                              OC_DISCOVERABLE | OC_OBSERVABLE);

    OC_LOG_V(DEBUG, HOSTING_TAG, "created mirror resource Handle : %u",mirrorResource->resourceHandle[OIC_MIRROR_HANDLE]);

    if (result != OC_STACK_OK)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "error return = %s", getResultString(result));
        mirrorResource->next = NULL;
        destroyMirrorResource(mirrorResource);
        return result;
    }

    if (mirrorResource->prop.countResourceType > 1)
    {
        int i = 0;
        for (i = 1; i < mirrorResource->prop.countResourceType; ++i)
        {
            result = OCBindResourceTypeToResource(
                             mirrorResource->resourceHandle[OIC_MIRROR_HANDLE],
                             mirrorResource->prop.resourceType[i]);
            if (result != OC_STACK_OK)
            {
                OC_LOG_V(DEBUG, HOSTING_TAG, "Virtual Resource Registration Fail : BindResourceType");
                goto RETURN_ERR;
            }
        }
    }

    if (mirrorResource->prop.countInterface > 1)
    {
        int i = 0;
        for (i = 1; i < mirrorResource->prop.countInterface; ++i)
        {
            result = OCBindResourceInterfaceToResource(
                         mirrorResource->resourceHandle[OIC_MIRROR_HANDLE],
                         mirrorResource->prop.resourceInterfaceName[i]);
            if (result != OC_STACK_OK)
            {
                OC_LOG_V(DEBUG, HOSTING_TAG,
                        "Virtual Resource Registration Fail : BindResourceInterfaceName");
                goto RETURN_ERR;
            }
        }
    }

    OC_LOG_V(DEBUG, HOSTING_TAG, "Mirror Resource Registration Success");
    OC_LOG_V(DEBUG, HOSTING_TAG, "Mirror Resource uri : %s", mirrorResource->uri);
    OC_LOG_V(DEBUG, HOSTING_TAG, "Mirror Resource source address : %s",
             mirrorResource->address[OIC_SOURCE_ADDRESS]);
    OC_LOG_V(DEBUG, HOSTING_TAG, "Mirror Resource virtual address : %s",
             mirrorResource->address[OIC_MIRROR_ADDRESS]);
    return result;

RETURN_ERR:
    OCDeleteResource(mirrorResource->resourceHandle[OIC_MIRROR_HANDLE]);
    mirrorResource->next = NULL;
    destroyMirrorResource(mirrorResource);

    return result;
}

OCStackResult requestResourceObservation(MirrorResource *mirrorResource)
{
    OCStackResult result;
    OCCallbackData cbData;

    cbData.cb = requestResourceObservationCB;
    cbData.context = (void *)DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;

    char query[OIC_STRING_MAX_VALUE] = {'\0'};
    snprintf(query, sizeof(query), "coap://%s%s%s", mirrorResource->address[OIC_SOURCE_ADDRESS], mirrorResource->uri,
            OIC_COORDINATING_FLAG);

    result = OCDoResource(&mirrorResource->resourceHandle[OIC_REQUEST_HANDLE], OC_REST_OBSERVE, query,
                          0, NULL, OC_TRANSPORT,
                          OC_HIGH_QOS, &cbData, NULL, 0);

    if (result != OC_STACK_OK)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "OCDoResource returns error %s with method %d",
                 getResultString(result), OC_REST_OBSERVE);
    }

    return result;
}

OCStackApplicationResult requestResourceObservationCB(void *context, OCDoHandle handle,
        OCClientResponse *clientResponse)
{
    OCStackApplicationResult ret = OC_STACK_DELETE_TRANSACTION;

    if (context == (void *)DEFAULT_CONTEXT_VALUE)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "Callback Context for OBS query recvd successfully");
    }

    if (clientResponse && clientResponse->result != OC_STACK_OK)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "observeCB result error = %s",
                 getResultString(clientResponse->result));
        return checkResourceValidation(handle);
    }

    if (clientResponse && clientResponse->result == OC_STACK_OK)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG,
                 "<=============Callback Context for OBSERVE notification recvd successfully");
        OC_LOG_V(DEBUG, HOSTING_TAG, "SEQUENCE NUMBER: %d", clientResponse->sequenceNumber);
        OC_LOG_V(DEBUG, HOSTING_TAG, "JSON = %s =============> Obs Response",
                 clientResponse->resJSONPayload);

        MirrorResource *foundMirrorResource = updateMirrorResource(handle, clientResponse->resJSONPayload);
        if (foundMirrorResource == NULL)
        {
            OC_LOG_V(DEBUG, HOSTING_TAG, "Cannot found Mirror Resource : Fail");
            return ret;
        }

        if ( OC_STACK_OK != OCNotifyAllObservers(foundMirrorResource->resourceHandle[OIC_MIRROR_HANDLE],
                OC_HIGH_QOS) )
        {
            OC_LOG_V(DEBUG, HOSTING_TAG, "Notify Mirror Resource's Subscriber : Fail");
        }
        else
        {
            OC_LOG_V(DEBUG, HOSTING_TAG, "Notify Mirror Resource's Subscriber : Success");
        }

        if (clientResponse->sequenceNumber == OC_OBSERVE_REGISTER)
        {
            OC_LOG_V(DEBUG, HOSTING_TAG, "This also serves as a registration confirmation");
        }
        else if (clientResponse->sequenceNumber == OC_OBSERVE_DEREGISTER)
        {
            OC_LOG_V(DEBUG, HOSTING_TAG, "This also serves as a deregistration confirmation");
            return ret;
        }
        else if (clientResponse->sequenceNumber == OC_OBSERVE_NO_OPTION)
        {
            OC_LOG_V(DEBUG, HOSTING_TAG, "This also tells you that registration/deregistration failed");
            return ret;
        }
        ret = OC_STACK_KEEP_TRANSACTION;
    }
    return ret;
}

OCStackApplicationResult checkResourceValidation(OCDoHandle handle)
{
    OCStackApplicationResult ret = OC_STACK_DELETE_TRANSACTION;

    RequestHandle *foundRequestHandle = findRequestHandle(s_requestHandleList, handle,
                                        OIC_REQUEST_BY_COORDINATOR);

    if (foundRequestHandle == NULL)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "Not found any request.");
        return ret;
    }

    if (foundRequestHandle->isAliveCheck)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "This response is Alive Check : Expired resource");
        OCDeleteResource(foundRequestHandle->requestHandle[OIC_REQUEST_BY_CLIENT]);
    }
    deleteRequestHandleFromList(s_requestHandleList, foundRequestHandle);
    return ret;
}

MirrorResource *updateMirrorResource(OCDoHandle handle, const char *payload)
{
    MirrorResource *foundMirrorResource = findMirrorResourceUsingHandle(
            s_mirrorResourceList, handle, OIC_REQUEST_HANDLE);

    if (!foundMirrorResource)
    {
        // TODO
        OC_LOG_V(DEBUG, HOSTING_TAG, "Cannot found Mirror Resource. In updateMirrorResource");
        return NULL;
    }

    cJSON *repData = NULL;
    cJSON *observeJson = cJSON_Parse(payload);

    if (observeJson)
    {
        cJSON *ocArray = cJSON_GetObjectItem(observeJson, "oc");
        cJSON *ocArray_sub = cJSON_GetArrayItem(ocArray, 0);
        cJSON *tempData = cJSON_GetObjectItem(ocArray_sub, "rep");
        char *temp = cJSON_PrintUnformatted(tempData);

        repData = cJSON_Parse(temp);
        if (temp != NULL)
        {
            free(temp);
        }
        cJSON_Delete(observeJson);
    }
    else
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "Mirror resource payload is not correct");
        return NULL;
    }

    if (foundMirrorResource->rep)
    {
        cJSON_Delete(foundMirrorResource->rep);
        foundMirrorResource->rep = NULL;
    }
    foundMirrorResource->rep = repData;

    cJSON *json = cJSON_CreateObject();

    char nodeData[OIC_STRING_MAX_VALUE] = {'\0'};
    snprintf(nodeData, sizeof(nodeData), "%s", foundMirrorResource->uri);
    cJSON_AddStringToObject(json, "href", nodeData);

    cJSON *nodeRep = cJSON_Parse(cJSON_PrintUnformatted(foundMirrorResource->rep));
    cJSON_AddItemToObject(json, "rep", nodeRep);
    OC_LOG_V(DEBUG, HOSTING_TAG, "It will notify resource : %s", cJSON_PrintUnformatted(json));

    cJSON_Delete(json);

    return foundMirrorResource;
}

char *buildResponsePayload (OCEntityHandlerRequest *entityHandlerRequest)
{
    MirrorResource *mirrorResource = findMirrorResourceUsingHandle(s_mirrorResourceList,
                                     entityHandlerRequest->resource, OIC_MIRROR_HANDLE);
    if (!mirrorResource)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "Cannot found Mirror Resource. In buildResponsePayload()");
        OC_LOG_V(DEBUG, HOSTING_TAG, "Mirror Resource's Handle : %x.", entityHandlerRequest->resource);
        return NULL;
    }

    if (entityHandlerRequest->method == OC_REST_PUT)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "oc_rest_put");
        if (mirrorResource->rep)
        {
            cJSON_Delete(mirrorResource->rep);
            mirrorResource->rep = NULL;
        }
        mirrorResource->rep = cJSON_CreateObject();
        mirrorResource->rep = cJSON_Parse(entityHandlerRequest->reqJSONPayload);
    }

    OC_LOG_V(DEBUG, HOSTING_TAG, "node's uri : %s", mirrorResource->uri);
    OC_LOG_V(DEBUG, HOSTING_TAG, "node's source address : %s", mirrorResource->address[0]);
    OC_LOG_V(DEBUG, HOSTING_TAG, "node's mirror address : %s", mirrorResource->address[1]);
    OC_LOG_V(DEBUG, HOSTING_TAG, "node's rep : %s", cJSON_PrintUnformatted(mirrorResource->rep));

    cJSON *jsonObject = cJSON_CreateObject();

    char uriString[OIC_STRING_MAX_VALUE] = {'\0'};
    snprintf(uriString, sizeof(uriString), "%s", mirrorResource->uri);
    cJSON_AddStringToObject(jsonObject, "href", uriString);

    cJSON *itemRep = cJSON_Parse(cJSON_PrintUnformatted(mirrorResource->rep));
    cJSON_AddItemToObject(jsonObject, "rep", itemRep);
    OC_LOG_V(DEBUG, HOSTING_TAG, "Will response resource : %s", cJSON_PrintUnformatted(jsonObject));

    char *jsonResponse = cJSON_Print(jsonObject);
    cJSON_Delete(jsonObject);

    return jsonResponse;
}

OCEntityHandlerResult
resourceEntityHandlerCB (OCEntityHandlerFlag entifyHandlerFlag,
                         OCEntityHandlerRequest *entityHandlerRequest)
{
    OC_LOG_V(DEBUG, HOSTING_TAG, "Inside device default entity handler - flags: 0x%x",
             entifyHandlerFlag);

    OCEntityHandlerResult entityHandlerResult = OC_EH_OK;
    OCEntityHandlerResponse entityHandlerResponse;
    char payload[MAX_RESPONSE_LENGTH] = {0};

    // Validate pointer
    if (!entityHandlerRequest)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "Invalid request pointer");
        return OC_EH_ERROR;
    }

    // Initialize certain response fields
    entityHandlerResponse.numSendVendorSpecificHeaderOptions = 0;
    memset(entityHandlerResponse.sendVendorSpecificHeaderOptions, 0,
           sizeof entityHandlerResponse.sendVendorSpecificHeaderOptions);
    memset(entityHandlerResponse.resourceUri, 0, sizeof entityHandlerResponse.resourceUri);

    if (entifyHandlerFlag & OC_REQUEST_FLAG)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "Flag includes OC_REQUEST_FLAG");
        if (entityHandlerRequest->resource == NULL)
        {
            OC_LOG_V(DEBUG, HOSTING_TAG, "Received request from client to a non-existing resource");
            entityHandlerResult = handleNonExistingResourceRequest(entityHandlerRequest, payload,
                                  sizeof(payload) - 1);
        }
        else if (OC_REST_GET == entityHandlerRequest->method)
        {
            OC_LOG_V(DEBUG, HOSTING_TAG, "Received OC_REST_GET from client");
            entityHandlerResult = handleGetRequest (entityHandlerRequest, payload, sizeof(payload) - 1);
        }
        else if (OC_REST_PUT == entityHandlerRequest->method ||
                 OC_REST_DELETE == entityHandlerRequest->method )
        {
            OC_LOG_V(DEBUG, HOSTING_TAG, "Received OC_REST_PUT/DELETE from client");

            RequestHandle *request = createRequestHandle();

            request->requestHandle[OIC_REQUEST_BY_CLIENT] = entityHandlerRequest;
            request->resourceHandle = entityHandlerRequest->resource;
            request->method = entityHandlerRequest->method;
            request->entityRequestHandle = entityHandlerRequest->requestHandle;

            OCStackResult result = insertRequestHandle(s_requestHandleList, request);
            if (result != OC_STACK_OK)
            {
                OC_LOG_V(DEBUG, HOSTING_TAG, "Insert request list : fail2(%d)", result);
                return result;
            }

            MirrorResource *mirrorResource = findMirrorResourceUsingHandle(s_mirrorResourceList,
                                             entityHandlerRequest->resource, OIC_MIRROR_HANDLE);
            if (mirrorResource == NULL)
            {
                OC_LOG_V(DEBUG, HOSTING_TAG, "Not found requested resource");
                return OC_EH_ERROR;
            }

            result = requestQuery(request,
                    entityHandlerRequest->method, mirrorResource->address[OIC_SOURCE_ADDRESS],
                    mirrorResource->uri);
            if (result != OC_STACK_OK)
            {
                OC_LOG_V(DEBUG, HOSTING_TAG, "Request query failed");
                deleteRequestHandleFromList(s_requestHandleList, request);
            }
            return OC_EH_OK;
        }
        else
        {
            OC_LOG_V(DEBUG, HOSTING_TAG, "Received unsupported method %d from client",
                     entityHandlerRequest->method);
            entityHandlerResult = OC_EH_ERROR;
        }

        // If the result isn't an error or forbidden, send response
        if (!((entityHandlerResult == OC_EH_ERROR) || (entityHandlerResult == OC_EH_FORBIDDEN)))
        {
            // Format the response.  Note this requires some info about the request
            entityHandlerResponse.requestHandle = entityHandlerRequest->requestHandle;
            entityHandlerResponse.resourceHandle = entityHandlerRequest->resource;
            entityHandlerResponse.ehResult = entityHandlerResult;
            entityHandlerResponse.payload = (char *)payload;
            entityHandlerResponse.payloadSize = strlen(payload);
            // Indicate that response is NOT in a persistent buffer
            entityHandlerResponse.persistentBufferFlag = 0;

            // Handle vendor specific options
            if (entityHandlerRequest->rcvdVendorSpecificHeaderOptions &&
                entityHandlerRequest->numRcvdVendorSpecificHeaderOptions)
            {
                OC_LOG_V(DEBUG, HOSTING_TAG, "Received vendor specific options");
                OCHeaderOption *receivedVenderSpecificHeaderOptions =
                    entityHandlerRequest->rcvdVendorSpecificHeaderOptions;
                for ( int i = 0; i < entityHandlerRequest->numRcvdVendorSpecificHeaderOptions; i++)
                {
                    if (((OCHeaderOption)receivedVenderSpecificHeaderOptions[i]).protocolID == OC_COAP_ID)
                    {
                        OC_LOG_V(DEBUG, HOSTING_TAG, "Received option with OC_COAP_ID and ID %u with",
                                 ((OCHeaderOption)receivedVenderSpecificHeaderOptions[i]).optionID );
                    }
                }
                OCHeaderOption *sendVenderSpecificHeaderOptions =
                    entityHandlerResponse.sendVendorSpecificHeaderOptions;
                uint8_t option2[] = {21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
                uint8_t option3[] = {31, 32, 33, 34, 35, 36, 37, 38, 39, 40};
                sendVenderSpecificHeaderOptions[0].protocolID = OC_COAP_ID;
                sendVenderSpecificHeaderOptions[0].optionID = 2248;
                memcpy(sendVenderSpecificHeaderOptions[0].optionData, option2, sizeof(option2));
                sendVenderSpecificHeaderOptions[0].optionLength = 10;
                sendVenderSpecificHeaderOptions[1].protocolID = OC_COAP_ID;
                sendVenderSpecificHeaderOptions[1].optionID = 2600;
                memcpy(sendVenderSpecificHeaderOptions[1].optionData, option3, sizeof(option3));
                sendVenderSpecificHeaderOptions[1].optionLength = 10;
                entityHandlerResponse.numSendVendorSpecificHeaderOptions = 2;
            }

            // Send the response
            if (OCDoResponse(&entityHandlerResponse) != OC_STACK_OK)
            {
                OC_LOG(ERROR, HOSTING_TAG, "Error sending response");
                entityHandlerResult = OC_EH_ERROR;
            }
        }
    }
    if (entifyHandlerFlag & OC_OBSERVE_FLAG)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "Flag includes OC_OBSERVE_FLAG");
        if (OC_OBSERVE_REGISTER == entityHandlerRequest->obsInfo.action)
        {
            OC_LOG_V(DEBUG, HOSTING_TAG, "Received OC_OBSERVE_REGISTER from client");
        }
        else if (OC_OBSERVE_DEREGISTER == entityHandlerRequest->obsInfo.action)
        {
            OC_LOG_V(DEBUG, HOSTING_TAG, "Received OC_OBSERVE_DEREGISTER from client");
        }
    }

    return entityHandlerResult;
}
OCEntityHandlerResult
handleGetRequest (OCEntityHandlerRequest *entityHandlerRequest,
        char *payload, uint16_t maxPayloadSize)
{
    OC_LOG_V(DEBUG, HOSTING_TAG, "ProcessGetRequest in....");

    OCEntityHandlerResult entityHandlerResult = OC_EH_ERROR;
    char *responsePayload = buildResponsePayload(entityHandlerRequest);
    if(!responsePayload)
    {
        return entityHandlerResult;
    }

    if (maxPayloadSize > strlen ((char *)responsePayload))
    {
        strncpy(payload, responsePayload, strlen((char *)responsePayload));
        entityHandlerResult = OC_EH_OK;
    }
    else
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "Response buffer: %d bytes is too small", maxPayloadSize);
    }

    free(responsePayload);

    return entityHandlerResult;
}
OCEntityHandlerResult
handleNonExistingResourceRequest(OCEntityHandlerRequest *entityHandlerRequest,
        char *payload, uint16_t maxPayloadSize)
{
    OC_LOG_V(INFO, HOSTING_TAG, "Executing %s ", __func__);

    char responsePayload[OIC_STRING_MAX_VALUE] = {'\0'};
    strncpy(responsePayload, "{App determines payload: The resource does not exist.}",
            sizeof(responsePayload));

    if ( (entityHandlerRequest != NULL) &&
         (maxPayloadSize > strlen ((char *)responsePayload)) )
    {
        strncpy((char *)payload, responsePayload, strlen((char *)responsePayload));
    }
    else
    {
        OC_LOG_V (INFO, HOSTING_TAG, "Response buffer: %d bytes is too small",
                  maxPayloadSize);
    }

    return OC_EH_RESOURCE_DELETED;
}

OCStackResult requestIsAlive(const char *address)
{
    MirrorResourceList *requestMirrorResourceList = findMirrorResourceListUsingAddress(
                s_mirrorResourceList, address, OIC_SOURCE_ADDRESS);

    if (requestMirrorResourceList == NULL)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "Cannot found any mirror resource1");
        return OC_STACK_ERROR;
    }

    if (requestMirrorResourceList->headerNode == NULL)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "Cannot found any mirror resource2");
        return OC_STACK_ERROR;
    }

    MirrorResource *mirrorResource = requestMirrorResourceList->headerNode;
    while (mirrorResource)
    {
        RequestHandle *requestAlive = createRequestHandle();
        requestAlive->isAliveCheck = 1;
        requestAlive->requestHandle[OIC_REQUEST_BY_CLIENT] =
            mirrorResource->resourceHandle[OIC_MIRROR_HANDLE];

        OCStackResult result = insertRequestHandle(s_requestHandleList, requestAlive);
        if (result != OC_STACK_OK)
        {
            OC_LOG_V(DEBUG, HOSTING_TAG, "Insert request list : fail3");
            destroyRequestHandle(requestAlive);
            mirrorResource = mirrorResource->next;
            continue;
        }

        result = requestQuery(requestAlive, OC_REST_GET, address, mirrorResource->uri);
        if (result != OC_STACK_OK)
        {
            deleteRequestHandleFromList(s_requestHandleList, requestAlive);
        }
        mirrorResource = mirrorResource->next;
    }
    destroyMirrorResourceList(requestMirrorResourceList);

    return OC_STACK_OK;
}

const char *getResultString(OCStackResult result)
{
    switch (result)
    {
        case OC_STACK_OK:
            return "OC_STACK_OK";
        case OC_STACK_RESOURCE_CREATED:
            return "OC_STACK_RESOURCE_CREATED";
        case OC_STACK_RESOURCE_DELETED:
            return "OC_STACK_RESOURCE_DELETED";
        case OC_STACK_INVALID_URI:
            return "OC_STACK_INVALID_URI";
        case OC_STACK_INVALID_QUERY:
            return "OC_STACK_INVALID_QUERY";
        case OC_STACK_INVALID_IP:
            return "OC_STACK_INVALID_IP";
        case OC_STACK_INVALID_PORT:
            return "OC_STACK_INVALID_PORT";
        case OC_STACK_INVALID_CALLBACK:
            return "OC_STACK_INVALID_CALLBACK";
        case OC_STACK_INVALID_METHOD:
            return "OC_STACK_INVALID_METHOD";
        case OC_STACK_NO_MEMORY:
            return "OC_STACK_NO_MEMORY";
        case OC_STACK_COMM_ERROR:
            return "OC_STACK_COMM_ERROR";
        case OC_STACK_INVALID_PARAM:
            return "OC_STACK_INVALID_PARAM";
        case OC_STACK_NOTIMPL:
            return "OC_STACK_NOTIMPL";
        case OC_STACK_NO_RESOURCE:
            return "OC_STACK_NO_RESOURCE";
        case OC_STACK_RESOURCE_ERROR:
            return "OC_STACK_RESOURCE_ERROR";
        case OC_STACK_SLOW_RESOURCE:
            return "OC_STACK_SLOW_RESOURCE";
        case OC_STACK_NO_OBSERVERS:
            return "OC_STACK_NO_OBSERVERS";
        case OC_STACK_VIRTUAL_DO_NOT_HANDLE:
            return "OC_STACK_VIRTUAL_DO_NOT_HANDLE";
        case OC_STACK_PRESENCE_STOPPED:
            return "OC_STACK_PRESENCE_STOPPED";
        case OC_STACK_PRESENCE_TIMEOUT:
            return "OC_STACK_PRESENCE_TIMEOUT";
        case OC_STACK_PRESENCE_DO_NOT_HANDLE:
            return "OC_STACK_PRESENCE_DO_NOT_HANDLE";
        case OC_STACK_ERROR:
            return "OC_STACK_ERROR";
        default:
            return "UNKNOWN";
    }
}

void getJsonArrayPair(cJSON *tempData)
{
    int countofrep = cJSON_GetArraySize(tempData);
    OC_LOG_V(DEBUG, HOSTING_TAG,
             "//////////////////////////////////////////////////////////////////////////");
    OC_LOG_V(DEBUG, HOSTING_TAG, "//Test");
    OC_LOG_V(DEBUG, HOSTING_TAG, "rep Size : %d", countofrep);

    for (int i = 0; i < countofrep; ++i)
    {
        cJSON *arrayJSON = cJSON_GetArrayItem(tempData, i);
        OC_LOG_V(DEBUG, HOSTING_TAG, "rep#%d's name : %s", i, arrayJSON->string);

        switch (arrayJSON->type)
        {
            case cJSON_False:
            case cJSON_True:
                OC_LOG_V(DEBUG, HOSTING_TAG, "rep#%d's value : %d", i, arrayJSON->valueint);
                break;
            case cJSON_Number:
                OC_LOG_V(DEBUG, HOSTING_TAG, "rep#%d's value : %f", i, arrayJSON->valuedouble);
                break;
            case cJSON_String:
                OC_LOG_V(DEBUG, HOSTING_TAG, "rep#%d's value : %s", i, arrayJSON->valuestring);
                break;
            case cJSON_NULL:
            default:
                OC_LOG_V(DEBUG, HOSTING_TAG, "rep#%d's value : NULL", i);
                break;
        }
    }
    OC_LOG_V(DEBUG, HOSTING_TAG,
             "//////////////////////////////////////////////////////////////////////////");
}

OCStackResult requestQuery(RequestHandle *request, OCMethod method,
                           const char *queryAddress, const char *queryUri)
{

    OCStackResult result = OC_STACK_ERROR;
    OCCallbackData cbData;

    /* Start a discovery query*/
    char queryFullUri[OIC_STRING_MAX_VALUE] = {'\0'};
    if (queryAddress == NULL)
    {
        return result;
    }
    else
    {
        snprintf(queryFullUri, sizeof(queryFullUri) ,"coap://%s%s%s", queryAddress , queryUri, OIC_COORDINATING_FLAG);
    }

    cbData.cb = requestQueryCB;
    cbData.context = (void *)DEFAULT_CONTEXT_VALUE;
    cbData.cd = NULL;

    if(method == OC_REST_PUT)
    {
        char payload[OIC_STRING_MAX_VALUE] = {'\0'};
        snprintf(payload , sizeof(payload), "%s" ,
         ((OCEntityHandlerRequest*)request->requestHandle[OIC_REQUEST_BY_CLIENT])->reqJSONPayload);

        result = OCDoResource(&request->requestHandle[OIC_REQUEST_BY_COORDINATOR],
                method, queryFullUri, NULL, payload, OC_TRANSPORT, OC_LOW_QOS, &cbData, NULL, 0);
    }
    else
    {
        result = OCDoResource(&request->requestHandle[OIC_REQUEST_BY_COORDINATOR],
                method, queryFullUri, NULL, 0, OC_TRANSPORT, OC_LOW_QOS, &cbData, NULL, 0);
    }

    if (result != OC_STACK_OK)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "OCStack resource error");
    }

    return result;
}

OCStackApplicationResult requestQueryCB(void *context, OCDoHandle handle,
                                        OCClientResponse *clientResponse)
{
    OCStackApplicationResult ret = OC_STACK_DELETE_TRANSACTION;

    if (context == (void *) DEFAULT_CONTEXT_VALUE)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "Callback Context for Request query recvd successfully");
    }

    if (clientResponse && clientResponse->result != OC_STACK_OK && clientResponse->result != OC_STACK_RESOURCE_DELETED)
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "requestQueryCB result error = %s",
                 getResultString(clientResponse->result));
        return checkResourceValidation(handle);
    }

    if (clientResponse && (clientResponse->result == OC_STACK_OK || clientResponse->result == OC_STACK_RESOURCE_DELETED))
    {
        RequestHandle *request = findRequestHandle(s_requestHandleList, handle, OIC_REQUEST_BY_COORDINATOR);
        if (request == NULL)
        {
            OC_LOG_V(DEBUG, HOSTING_TAG, "Not found Any request");
            return ret;
        }
        if (request->isAliveCheck == 1)
        {
            OC_LOG_V(DEBUG, HOSTING_TAG, "This response is Alive Check : Keep resource");
        }
        else
        {
            OC_LOG_V(DEBUG, HOSTING_TAG, "requestCB's payload: %s", clientResponse->resJSONPayload);
            OCEntityHandlerRequest *entityHandler = (OCEntityHandlerRequest *)(
                    request->requestHandle[OIC_REQUEST_BY_CLIENT]);
            OC_LOG_V(DEBUG, HOSTING_TAG, "requested resource handle : %u", entityHandler->resource
                    );

            entityHandler->resource = request->resourceHandle;
            entityHandler->method = request->method;
            entityHandler->requestHandle = request->entityRequestHandle;

            OCEntityHandlerResponse response = buildEntityHandlerResponse(
                                                   entityHandler, clientResponse->resJSONPayload);
            if (OCDoResponse(&response) != OC_STACK_OK)
            {
                OC_LOG_V(DEBUG, HOSTING_TAG, "Error sending response");
                deleteRequestHandleFromList(s_requestHandleList, request);
                return ret;
            }
            if (entityHandler->method == OC_REST_DELETE)
            {
                OCDeleteResource(entityHandler->resource);
            }
        }
        deleteRequestHandleFromList(s_requestHandleList, request);
        ret = OC_STACK_KEEP_TRANSACTION;
    }

    return ret;
}

OCEntityHandlerResponse buildEntityHandlerResponse(OCEntityHandlerRequest *entityHandlerRequest,
        const char *clientPayload)
{
    OC_LOG_V(DEBUG, HOSTING_TAG, "enter buildEntityHandlerResponse");
    OCEntityHandlerResponse response;
    memset(&response, 0, sizeof(response));
    OCEntityHandlerResult entityHandlerResult = OC_EH_OK;
    char payload[MAX_RESPONSE_LENGTH] = {'\0'};

    // Initialize certain response fields
    response.numSendVendorSpecificHeaderOptions = 0;
    memset(response.sendVendorSpecificHeaderOptions, 0,
           sizeof response.sendVendorSpecificHeaderOptions);
    memset(response.resourceUri, 0, sizeof response.resourceUri);

    char *temp = NULL;
    if(entityHandlerRequest->method == OC_REST_PUT)
    {
        cJSON *observeJson = cJSON_CreateObject();
        observeJson = cJSON_Parse(clientPayload);

        cJSON *ocArray = cJSON_GetObjectItem(observeJson, "oc");
        cJSON *ocArray_sub = cJSON_GetArrayItem(ocArray, 0);

        cJSON *tempData = cJSON_GetObjectItem(ocArray_sub, "rep");
        temp = cJSON_PrintUnformatted(tempData);

        cJSON_Delete(observeJson);

        entityHandlerRequest->reqJSONPayload = temp;
    }
    entityHandlerResult = handleRequestPayload(entityHandlerRequest, payload, sizeof(payload) - 1);

    // Format the response.  Note this requires some info about the request
    response.requestHandle = entityHandlerRequest->requestHandle;
    response.resourceHandle = entityHandlerRequest->resource;
    response.ehResult = entityHandlerResult;

    response.payload = (char *)payload;
    response.payloadSize = strlen(payload);
    // Indicate that response is NOT in a persistent buffer
    response.persistentBufferFlag = 0;

    if(entityHandlerRequest->method == OC_REST_PUT){
        if(temp){
            free(temp);
        }
    }

    return response;
}

OCEntityHandlerResult handleRequestPayload (OCEntityHandlerRequest *entityHandlerRequest,
        char *payload, uint16_t maxPayloadSize)
{
    OC_LOG_V(DEBUG, HOSTING_TAG, "enter handleRequestPayload");
    OCEntityHandlerResult entityHandlerResult = OC_EH_ERROR;

    if (entityHandlerRequest->method == OC_REST_DELETE)
    {
        memset(payload, '\0', sizeof(char) * (maxPayloadSize + 1));
        OC_LOG_V(DEBUG, HOSTING_TAG, "DELETE");
        return OC_EH_RESOURCE_DELETED;
    }

    char *responsePayload = buildResponsePayload(entityHandlerRequest);
    if(!responsePayload)
    {
        return entityHandlerResult;
    }

    if (maxPayloadSize > strlen ((char *)responsePayload))
    {
        strncpy(payload, responsePayload, strlen ((char *)responsePayload));
        entityHandlerResult = OC_EH_OK;
    }
    else
    {
        OC_LOG_V(DEBUG, HOSTING_TAG, "Response buffer: %d bytes is too small", maxPayloadSize);
        entityHandlerResult = OC_EH_ERROR;
    }

    free(responsePayload);

    return entityHandlerResult;
}
