//******************************************************************
//
// Copyright 2018 Beechwoods Software All Rights Reserved.
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

// Defining _POSIX_C_SOURCE macro with 200112L (or greater) as value
// causes header files to expose definitions
// corresponding to the POSIX.1-2001 base
// specification (excluding the XSI extension).
// For POSIX.1-2001 base specification,
// Refer http://pubs.opengroup.org/onlinepubs/009695399/
#define _POSIX_C_SOURCE 200112L

#include "ocatomicmeasurement.h"
#include "ocpayload.h"
#include "ocendpoint.h"
#include "ocstack.h"
#include "ocstackinternal.h"
#include "oic_string.h"
#include "experimental/payload_logging.h"
#include "cainterface.h"
#define TAG "OIC_RI_ATOMICMEASUREMENT"

//************************************************************************
//*                                                                      *
//* AddRTSBaselinePayload                                                *
//*                                                                      *
//*     Add RTS values to the response to the baseline interface request *
//*                                                                      *
//************************************************************************
static bool AddRTSBaselinePayload(OCRepPayload **linkArray, int size, OCRepPayload **amPayload)
{
    size_t arraySize = 0;
    int i;
    unsigned int j;

    for (i = 0; i < size; i++)
    {
        for (OCStringLL *pResType = linkArray[i]->types; pResType; pResType = pResType->next, arraySize++);
    }

    OIC_LOG_V(DEBUG, TAG, "Number of RTS elements : %zd", arraySize);
    size_t dim[MAX_REP_ARRAY_DEPTH] = {arraySize, 0, 0};
    char **rts = (char **)OICMalloc(sizeof(char *) * arraySize);
    if (!rts)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed!");
        return OC_STACK_NO_MEMORY;
    }

    j = 0;
    for (i = 0; i < size; i++)
    {
        for (OCStringLL *pResType = linkArray[i]->types; pResType; pResType = pResType->next)
        {
            rts[j++] = OICStrdup(pResType->value);
        }
    }

    bool b = OCRepPayloadSetStringArrayAsOwner(*amPayload, OC_RSRVD_RTS, rts, dim);

    if (!b)
    {
        for (j = 0; j < arraySize; j++)
        {
            OICFree(rts[j]);
        }
        OICFree(rts);
    }

    return b;
}

//**************************************************************************
//*                                                                        *
//* AddRTSMBaselinePayload                                                 *
//*                                                                        *
//*     Add RTS-M values to the response to the baseline interface request *
//*                                                                        *
//**************************************************************************
static OCStackResult AddRTSMBaselinePayload(OCResource *amResource, OCRepPayload **amPayload)
{
    size_t arraySize = 0;
    int i = 0;

    for (OCResourceType *rsrcTypeM = amResource->rsrcTypeM; rsrcTypeM; rsrcTypeM = rsrcTypeM->next, arraySize++);

    OIC_LOG_V(DEBUG, TAG, "Number of RTS-M elements : %zd", arraySize);

    if (arraySize > 0)
    {
        size_t dim[MAX_REP_ARRAY_DEPTH] = {arraySize, 0, 0};
        char **rtsm = (char **)OICMalloc(sizeof(char *) * arraySize);
        if (!rtsm)
        {
            OIC_LOG(ERROR, TAG, "Memory allocation failed!");
            return OC_STACK_NO_MEMORY;
        }

        for (OCResourceType *rsrcTypeM = amResource->rsrcTypeM; rsrcTypeM; rsrcTypeM = rsrcTypeM->next, i++)
        {
            rtsm[i] = OICStrdup(rsrcTypeM->resourcetypename);
        }

        bool b = OCRepPayloadSetStringArrayAsOwner(*amPayload, OC_RSRVD_RTSM, rtsm, dim);

        if (!b)
        {
            for (size_t j = 0; j < arraySize; j++)
            {
                OICFree(rtsm[j]);
            }
            OICFree(rtsm);
        }

        return b;
    }
    else
        return OC_STACK_OK;
}

//***********************************************
//*                                             *
//* SendResponse                                *
//*                                             *
//*     Send a response to the request received *
//*                                             *
//***********************************************
static OCStackResult SendResponse(const OCRepPayload *payload,
                        const OCEntityHandlerRequest *ehRequest, OCEntityHandlerResult ehResult)
{
    OCEntityHandlerResponse response = {0};
    response.ehResult = ehResult;
    response.payload = (OCPayload*)payload;
    response.persistentBufferFlag = 0;
    response.requestHandle = (OCRequestHandle) ehRequest->requestHandle;
    return OCDoResponse(&response);
}

//**************************************************************************
//*                                                                        *
//* GetNumOfResourcesInAtomicMeasurement                                   *
//*                                                                        *
//*     Get the total number of resources composing the atomic measurement *
//*                                                                        *
//**************************************************************************
static uint8_t GetNumOfResourcesInAtomicMeasurement(const OCResource *amResource)
{
    uint8_t size = 0;
    for (OCChildResource *tempChildResource = amResource->rsrcChildResourcesHead;
        tempChildResource; tempChildResource = tempChildResource->next)
    {
        size++;
    }
    OIC_LOG_V(DEBUG, TAG, "GetNumOfResourcesInAtomicMeasurement: there are %u resources in the atomic measurement", size);
    return size;
}

//*****************************************************************
//*                                                               *
//* HandleLinkedListInterface                                     *
//*                                                               *
//*     Handler for the linked list (oic.if.ll) interface request *
//*                                                               *
//*****************************************************************
static OCStackResult HandleLinkedListInterface(OCEntityHandlerRequest *ehRequest, char *ifQueryParam)
{
    if (!ehRequest)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCResource *amResource = (OCResource *)ehRequest->resource;
    if (!amResource)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OIC_LOG(DEBUG, TAG, "Entering HandleLinkedListInterface...");

    uint8_t size = GetNumOfResourcesInAtomicMeasurement(amResource);
    OCRepPayload *amPayload = NULL;
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    OCStackResult ret = OC_STACK_ERROR;
    size_t dim[MAX_REP_ARRAY_DEPTH] = {size, 0, 0};
    OCRepPayload **linkArr = NULL;

    if (!(linkArr = OCLinksPayloadArrayCreateAM(amResource->uri, ehRequest, false, NULL)))
    {
        OIC_LOG(ERROR, TAG, "Failed getting LinksPayloadArray");
        ret = OC_STACK_ERROR;
        goto exit;
    }

    if (size < 1)
    {
        ret = OC_STACK_NO_RESOURCE;
        goto exit;
    }

    OCPayloadFormat contentFormat = OC_FORMAT_UNDEFINED;
    OCGetRequestPayloadVersion(ehRequest, &contentFormat, NULL);
    // from the OCF1.0 linklist specification, ll has array of links
    if ((0 == strcmp(ifQueryParam, OC_RSRVD_INTERFACE_LL)) && (contentFormat == OC_FORMAT_VND_OCF_CBOR))
    {
        for (int n = 0; n < (int)size - 1; n++)
        {
            linkArr[n]->next = linkArr[n + 1];
        }
        amPayload = linkArr[0];
        OICFree(linkArr);
        ret = OC_STACK_OK;
        goto exit;
    }
    else if ((contentFormat == OC_FORMAT_VND_OCF_CBOR || contentFormat == OC_FORMAT_CBOR))
    {
        amPayload = OCRepPayloadCreate();
        VERIFY_PARAM_NON_NULL(TAG, linkArr, "Failed creating LinksPayloadArray");
        if (0 == strcmp(OC_RSRVD_INTERFACE_DEFAULT, ifQueryParam))
        {
            //TODO : Add resource type filtering once atomic measurements
            // start supporting queries.
            OCRepPayloadAddResourceType(amPayload, OC_RSRVD_RESOURCE_TYPE_AM);
            for (OCResourceType *types = amResource->rsrcType; types; types = types->next)
            {
                if (0 != strcmp(OC_RSRVD_RESOURCE_TYPE_AM, types->resourcetypename))
                {
                    OCRepPayloadAddResourceType(amPayload, types->resourcetypename);
                }
            }
            for (OCResourceInterface *itf = amResource->rsrcInterface; itf; itf = itf->next)
            {
                OCRepPayloadAddInterface(amPayload, itf->name);
            }
            AddRTSBaselinePayload(linkArr, size, &amPayload);
            AddRTSMBaselinePayload(amResource, &amPayload);
        }
        OCRepPayloadSetPropObjectArrayAsOwner(amPayload, OC_RSRVD_LINKS, linkArr, dim);
        ret = OC_STACK_OK;
    }
exit:
    if (0 == strcmp(ifQueryParam, OC_RSRVD_INTERFACE_LL))
    {
        OCRepPayloadSetPayloadRepType(amPayload, PAYLOAD_REP_ARRAY);
    }
    else
    {
        OCRepPayloadSetPayloadRepType(amPayload, PAYLOAD_REP_OBJECT_ARRAY);
    }

    if (ret == OC_STACK_OK)
    {
        ehResult = OC_EH_OK;
    }
    else
    {
        ehResult = (ret == OC_STACK_NO_RESOURCE) ? OC_EH_RESOURCE_NOT_FOUND : OC_EH_ERROR;
    }
    ret = SendResponse(amPayload, ehRequest, ehResult);
    OIC_LOG_V(INFO, TAG, "Send Response result from HandleLinkedListInterface = %d", (int)ret);
    OIC_LOG_PAYLOAD(DEBUG, (OCPayload *)amPayload);
    OCRepPayloadDestroy(amPayload);

    return ret;
}

//**********************************************************
//*                                                        *
//* HandleBatchInterface                                   *
//*                                                        *
//*     Handler for the batch (oic.if.b) interface request *
//*                                                        *
//**********************************************************
static OCStackResult HandleBatchInterface(OCEntityHandlerRequest *ehRequest)
{
    if (!ehRequest)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult stackRet = OC_STACK_OK;
    OCEntityHandlerResult ehResult = OC_EH_OK;
    char *storeQuery = NULL;
    OCResource *amResource = (OCResource *)ehRequest->resource;

    if (amResource->rsrcChildResourcesHead)
    {
        storeQuery = ehRequest->query;
        ehRequest->query = NULL;
        OIC_LOG_V(DEBUG, TAG, "Query : %s", ehRequest->query);
    }

    uint8_t numRes = 0;
    for (OCChildResource *tempChildResource = amResource->rsrcChildResourcesHead;
        tempChildResource; tempChildResource = tempChildResource->next, numRes++)
    {
        OCResource* tempRsrcResource = tempChildResource->rsrcResource;
        if (tempRsrcResource)
        {
            // Note that all entity handlers called through an atomic measurement
            // will get the same pointer to ehRequest, the only difference
            // is ehRequest->resource
            ehRequest->resource = (OCResourceHandle) tempRsrcResource;
            ehResult = tempRsrcResource->amEntityHandler(OC_REQUEST_FLAG,
                                       ehRequest, tempRsrcResource->entityHandlerCallbackParam);

            // atomic measurement doesn't handle slow resources!
            if (ehResult == OC_EH_SLOW)
            {
                OIC_LOG(ERROR, TAG, "This is a slow resource, can not be handled in atomic measurement!");
                ehResult = OC_EH_INTERNAL_SERVER_ERROR;
            }
            if (ehResult != OC_EH_OK)
            {
                OIC_LOG_V(ERROR, TAG, "Error while retrieving measurement from a resource in an atomic measurement (error = %d)!", ehResult);
                stackRet = EntityHandlerCodeToOCStackCode(ehResult);
                break;
            }
        }
        else
        {
            // One of the resources in the atomic measurement does not have a resource handler!
            OIC_LOG(ERROR, TAG, "One of the resources in the atomic measurement does not have a resource handler!!");
            ehResult = OC_EH_INTERNAL_SERVER_ERROR;
            stackRet = EntityHandlerCodeToOCStackCode(ehResult);
            break;
        }
    }
    ehRequest->resource = (OCResourceHandle) amResource;
    ehRequest->query = storeQuery;
    return stackRet;
}

//****************************************************************************************
//*                                                                                      *
//* DefaultAtomicMeasurementEntityHandler                                                *
//*                                                                                      *
//*     This function is the default entity handler for the atomic measurement resource. *
//*     It deals with all the queries coming to the atomic measurement, and controls the *
//*     response from the atomic measurement. It interfaces with and controls all the    *
//*     individual resources that belong to it.                                          *
//*                                                                                      *
//****************************************************************************************
OCStackResult DefaultAtomicMeasurementEntityHandler(OCEntityHandlerFlag flag, OCEntityHandlerRequest *ehRequest)
{
    (void) flag;

    if (!ehRequest || !ehRequest->query)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OIC_LOG_V(INFO, TAG, "DefaultAtomicMeasurementEntityHandler with query %s", ehRequest->query);

    char *ifQueryParam = NULL;
    char *rtQueryParam = NULL;
    OCStackResult result = ExtractFiltersFromQuery(ehRequest->query, &ifQueryParam, &rtQueryParam);
    if (result != OC_STACK_OK)
    {
        return OC_STACK_NO_RESOURCE;
    }

    // Delete is not supported for any interface query method.
    if (ehRequest->method == OC_REST_DELETE)
    {
        result = OC_STACK_ERROR;
        goto exit;
    }

    // If a rt filter is received, send back FORBIDDEN, as per the spec 
    if (rtQueryParam)
    {
        result = OC_STACK_FORBIDDEN_REQ;
        goto exit;
    }

    // Default interface for atomic measurement is batch
    if (!ifQueryParam)
    {
        ifQueryParam = OICStrdup(OC_RSRVD_INTERFACE_BATCH);
    }

    VERIFY_PARAM_NON_NULL(TAG, ifQueryParam, "Invalid Parameter ifQueryParam");

    if (0 == strcmp(ifQueryParam, OC_RSRVD_INTERFACE_LL) || 0 == strcmp (ifQueryParam, OC_RSRVD_INTERFACE_DEFAULT))
    {
        if (ehRequest->method == OC_REST_PUT || ehRequest->method == OC_REST_POST)
        {
            result =  OC_STACK_ERROR;
        }
        else
        {
            result = HandleLinkedListInterface(ehRequest, ifQueryParam);
        }
    }
    else if (0 == strcmp(ifQueryParam, OC_RSRVD_INTERFACE_BATCH))
    {
        if (ehRequest->method != OC_REST_GET)
        {
            result =  OC_STACK_INVALID_METHOD;
        }
        else
        {
            OCServerRequest *request = (OCServerRequest *)ehRequest->requestHandle;
            if (request)
            {
                request->numResponses = GetNumOfResourcesInAtomicMeasurement((OCResource *)ehRequest->resource);
                request->ehResponseHandler = HandleAggregateResponse;
                result = HandleBatchInterface(ehRequest);
            }
        }
    }

exit:
    if (result != OC_STACK_OK)
    {
        if (result == OC_STACK_ERROR)
            result = OC_STACK_INVALID_QUERY;
        result = SendResponse(NULL, ehRequest, OCStackCodeToEntityHandlerCode(result));
    }
    OICFree(ifQueryParam);
    OICFree(rtQueryParam);
    return result;
}

//***************************************************************************************
//*                                                                                     *
//* DefaultAtomicMeasurementResourceEntityHandler                                       *
//*                                                                                     *
//*     This function is the default entity handler for the individual resources member *
//*     of an atomic measurement. The atomic measurement substitutes this handler to    *
//*     each of its individual resources' entity handler, in order to correctly control *
//*     and process atomic measurement, and implement its restrictions.                 *
//*                                                                                     *
//***************************************************************************************
OCEntityHandlerResult DefaultAtomicMeasurementResourceEntityHandler(OCEntityHandlerFlag flag,
    OCEntityHandlerRequest *ehRequest, void *ehCallbackParam)
{
    OCEntityHandlerResult ret = OC_EH_OK;

    // Those parameters are unused
    (void) flag;
    (void) ehCallbackParam;

    if (!ehRequest || !ehRequest->query)
    {
        ret = OC_EH_ERROR;
    }
    else
    {
        // Individual requests to resources member of an AM are forbidden
        if (SendResponse(NULL, ehRequest, OC_EH_FORBIDDEN) != OC_STACK_OK)
            ret = OC_EH_ERROR;
    }

    return ret;
}

//**************************************************************************************
//*                                                                                    *
//* NewAtomicMeasurementReadyNotificationHandler                                       *
//*                                                                                    *
//*     This function notifies the atomic measurement that a new measurement is ready. *
//*     This notification will trigger the atomic measurement sending GET requests to  *
//*     all the individual resources composing it.                                     *
//*                                                                                    *
//**************************************************************************************
OCStackResult NewAtomicMeasurementReadyNotificationHandler(const OCResourceHandle amResourceHandle)
{
    OCStackResult result;

    if (amResourceHandle == NULL)
    {
        OIC_LOG(ERROR, TAG, "NewAtomicMeasurementReadyNotificationHandler: invalid resource handle!");
        result = OC_STACK_ERROR;
    }
    else
    {
        if (((OCResource *)amResourceHandle)->observersHead != NULL)
        {
            if ((result = OCNotifyAllObservers(amResourceHandle, OC_NA_QOS)) == OC_STACK_OK)
            {
                OIC_LOG(DEBUG, TAG, "NewAtomicMeasurementReadyNotificationHandler: OCNotifyAllObservers succeeded...");
            }
            else
            {
                OIC_LOG(ERROR, TAG, "NewAtomicMeasurementReadyNotificationHandler: OCNotifyAllObservers failed!");
            }
        }
        else
        {
            OIC_LOG(DEBUG, TAG, "NewAtomicMeasurementReadyNotificationHandler: no observer registered, don't call OCNotifyAllObservers...");
            result = OC_STACK_NO_OBSERVERS;
        }
    }

    return result;
}

//**************************************************************************************************
//*                                                                                                *
//* addPolicyPayload                                                                               *
//*                                                                                                *
//*     Add the security values to the response to the baseline and linked list interface requests *
//*                                                                                                *
//**************************************************************************************************
static bool addPolicyPayload(OCResourceHandle* resourceHandle, OCDevAddr* devAddr,
                             bool isOCFContentFormat, OCRepPayload** outPolicy)
{
    if (resourceHandle == NULL || devAddr == NULL || outPolicy == NULL)
    {
        return false;
    }

    OCResourceProperty p = OCGetResourceProperties(resourceHandle);
    OCRepPayload* policy = OCRepPayloadCreate();
    if (policy)
    {
        OCRepPayloadSetPropInt(policy, OC_RSRVD_BITMAP, ((p & OC_DISCOVERABLE) | (p & OC_OBSERVABLE)));
        if (!isOCFContentFormat)
        {
            OCRepPayloadSetPropBool(policy, OC_RSRVD_SECURE, p & OC_SECURE);

            if (p & OC_SECURE)
            {
                uint16_t securePort = 0;
                if (devAddr)
                {
                    if (devAddr->adapter == OC_ADAPTER_IP)
                    {
                        if (devAddr->flags & OC_IP_USE_V6)
                        {
                            securePort = caglobals.ip.u6s.port;
                        }
                        else if (devAddr->flags & OC_IP_USE_V4)
                        {
                            securePort = caglobals.ip.u4s.port;
                        }
                    }
                }
                OCRepPayloadSetPropInt(policy, OC_RSRVD_HOSTING_PORT, securePort);

#if defined(TCP_ADAPTER) && defined(__WITH_TLS__)
                // tls
                if (devAddr)
                {
                    uint16_t tlsPort = 0;
                    GetTCPPortInfo(devAddr, &tlsPort, true);
                    OCRepPayloadSetPropInt(policy, OC_RSRVD_TLS_PORT, tlsPort);
                }
#endif
            }
#ifdef TCP_ADAPTER
#ifdef  __WITH_TLS__
            if (!(p & OC_SECURE))
            {
#endif
                // tcp
                if (devAddr)
                {
                    uint16_t tcpPort = 0;
                    GetTCPPortInfo(devAddr, &tcpPort, false);
                    OCRepPayloadSetPropInt(policy, OC_RSRVD_TCP_PORT, tcpPort);
                }
#ifdef  __WITH_TLS__
            }
#endif
#endif
        }
    }
    else
    {
        return false;
    }

    *outPolicy = policy;
    return true;
}

//**************************************************************************************************
//*                                                                                                *
//* translateEndpointsPayload                                                                      *
//*                                                                                                *
//*     Add the endpoint values to the response to the baseline and linked list interface requests *
//*                                                                                                *
//**************************************************************************************************
static bool translateEndpointsPayload(OCEndpointPayload* epPayloadOrg,
                                      size_t size, OCRepPayload*** outArrayPayload)
{
    bool result = false;
    OCRepPayload** arrayPayload = (OCRepPayload**)OICMalloc(sizeof(OCRepPayload*) * (size));
    VERIFY_PARAM_NON_NULL(TAG, arrayPayload, "Failed creating arrayPayload");
    VERIFY_PARAM_NON_NULL(TAG, epPayloadOrg, "Invalid Parameter epPayload");
    VERIFY_PARAM_NON_NULL(TAG, outArrayPayload, "Invalid Parameter outArrayPayload");
    OCEndpointPayload* epPayload = epPayloadOrg;

    for (size_t i = 0; (i < size) && (epPayload != NULL) ; i++)
    {
        arrayPayload[i] = OCRepPayloadCreate();
        if (!arrayPayload[i])
        {
            for (size_t j = 0; j < i; j++)
            {
                OCRepPayloadDestroy(arrayPayload[j]);
            }
            result = false;
            goto exit;
        }
        char* createdEPStr = OCCreateEndpointString(epPayload);
        OIC_LOG_V(DEBUG, TAG, " OCCreateEndpointString() = %s", createdEPStr);
        OCRepPayloadSetPropString(arrayPayload[i], OC_RSRVD_ENDPOINT, createdEPStr);
        OICFree(createdEPStr);

        // in case of pri as 1, skip set property
        if (epPayload->pri != 1 )
            OCRepPayloadSetPropInt(arrayPayload[i], OC_RSRVD_PRIORITY, epPayload->pri);

        epPayload = epPayload->next;
        result = true;
    }
    *outArrayPayload = arrayPayload;
exit:
    OCEndpointPayloadDestroy(epPayloadOrg);
    if (result == false)
    {
        OICFree(arrayPayload);
    }
    return result;
}

//****************************************************************************************
//*                                                                                      *
//* BuildAtomicMeasurementLinksPayloadArray                                              *
//*                                                                                      *
//*     Create the RepPayloadArray for links parameter of an atomic measurement resource *
//*                                                                                      *
//****************************************************************************************
OCRepPayload** BuildAtomicMeasurementLinksPayloadArray(const char* resourceUri,
    bool isOCFContentFormat, OCDevAddr* devAddr, bool insertSelfLink, size_t* createdArraySize)
{
    bool result = false;
    OCRepPayload** arrayPayload = NULL;
    size_t childCount = 0;

    const OCResourceHandle amResourceHandle = OCGetResourceHandleAtUri(resourceUri);
    VERIFY_PARAM_NON_NULL(TAG, amResourceHandle, "Failed geting amResourceHandle");

    const OCChildResource* childResource = ((OCResource*)amResourceHandle)->rsrcChildResourcesHead;
    VERIFY_PARAM_NON_NULL(TAG, childResource, "Failed geting childResource");

    //children resources count calculation
    const OCChildResource* childCountResource = childResource;
    do {
        childCount++;
        childCountResource = childCountResource->next;
    } while (childCountResource);

    if (insertSelfLink)
    {
        childCount++;
    }

    arrayPayload = (OCRepPayload**)OICMalloc(sizeof(OCRepPayload*) * (childCount));
    VERIFY_PARAM_NON_NULL(TAG, arrayPayload, "Failed creating arrayPayload");

    OCResource* iterResource = childResource->rsrcResource;
    for (size_t i = 0; i < childCount; i++)
    {
        arrayPayload[i] = OCRepPayloadCreate();
        if (!arrayPayload[i])
        {
            for (size_t j = 0; j < i; j++)
            {
                OCRepPayloadDestroy(arrayPayload[j]);
            }
            result = false;
            goto exit;
        }

        OCRepPayloadSetUri(arrayPayload[i], iterResource->uri);

        for (OCResourceType* resType = iterResource->rsrcType; resType;
            resType = resType->next)
        {
            OCRepPayloadAddResourceType(arrayPayload[i], resType->resourcetypename);
        }

        for (OCResourceInterface* resInterface = iterResource->rsrcInterface; resInterface;
                                  resInterface = resInterface->next)
        {
            OCRepPayloadAddInterface(arrayPayload[i], resInterface->name);
        }

        OCRepPayload* outPolicy = NULL;
        //Policy Map will have tls and tcp properties for legacy support,
        // in case contents format is cbor instead of vnd.ocf/cbor
        if (!addPolicyPayload((OCResourceHandle*)iterResource, devAddr, isOCFContentFormat,
                               &outPolicy) ||
            !OCRepPayloadSetPropObjectAsOwner(arrayPayload[i], OC_RSRVD_POLICY, outPolicy))
        {
            OCRepPayloadDestroy(outPolicy);
            for (size_t j = 0; j <= i; j++)
            {
                OCRepPayloadDestroy(arrayPayload[j]);
            }
            result = false;
            goto exit;
        }

        //EP is added in case contents format is vnd.ocf/cbor
        if (isOCFContentFormat)
        {
            CAEndpoint_t *info = NULL;
            size_t networkSize = 0;
            size_t epSize = 0;
            CAGetNetworkInformation(&info, &networkSize);
            OIC_LOG_V(DEBUG, TAG, "Network Information size = %d", (int) networkSize);

            OCEndpointPayload *listHead = NULL;
            CreateEndpointPayloadList(iterResource,
                devAddr, info, networkSize, &listHead, &epSize, NULL);
            OICFree(info);
            OIC_LOG_V(DEBUG, TAG, "Result of CreateEndpointPayloadList() = %s",
                                  listHead ? "true":"false");

            OCRepPayload** epArrayPayload = NULL;
            size_t epsDim[MAX_REP_ARRAY_DEPTH] = { epSize, 0, 0 };

            if (!translateEndpointsPayload(listHead, epSize, &epArrayPayload) ||
                !OCRepPayloadSetPropObjectArrayAsOwner(arrayPayload[i],
                            OC_RSRVD_ENDPOINTS, epArrayPayload, epsDim))
            {
                if (epArrayPayload)
                {
                    for (size_t j = 0; j < epSize; j++)
                    {
                        OCRepPayloadDestroy(epArrayPayload[j]);
                    }
                    OICFree(epArrayPayload);
                }

                for (size_t j = 0; j <= i; j++)
                {
                    OCRepPayloadDestroy(arrayPayload[j]);
                }
                result = false;
                goto exit;
            }
        }

        if (iterResource != amResourceHandle)
        {
            childResource = childResource->next;
            if (childResource)
            {
                iterResource = childResource->rsrcResource;
            }
            else if (insertSelfLink)
            {
                iterResource = amResourceHandle;
            }
        }
        else // handling selfLink case
        {
            OIC_LOG(INFO, TAG, "adding rel for self link");
            const char* relArray[2] = { "self", "item" };
            size_t dimensions[MAX_REP_ARRAY_DEPTH] = { 2, 0, 0 };
            if (!OCRepPayloadSetStringArray(arrayPayload[i], OC_RSRVD_REL, relArray, dimensions))
            {
                OIC_LOG(ERROR, TAG, "Failed setting rel property");
                result = false;
                goto exit;
            }
        }
        result = true;
    }

exit:
    if (!result && (arrayPayload != NULL))
    {
        OICFree(arrayPayload);
        arrayPayload = NULL;
    }

    if (arrayPayload != NULL && createdArraySize != NULL)
        *createdArraySize = childCount;
    else if (createdArraySize != NULL)
        *createdArraySize = 0;

    return arrayPayload;
}
