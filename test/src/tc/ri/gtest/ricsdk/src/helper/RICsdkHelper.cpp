/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/

#include "RICsdkHelper.h"

RICsdkHelper* RICsdkHelper::s_riCsdkHelperInstance = NULL;
std::mutex RICsdkHelper::s_mutex;
bool RICsdkHelper::s_quitFlag;
bool RICsdkHelper::s_isCallback;
bool RICsdkHelper::s_isPayloadCorrect;
bool RICsdkHelper::s_isServerResponse;
bool RICsdkHelper::s_isResourceFound;
string RICsdkHelper::s_failureMsg;
OCEntityHandlerResult RICsdkHelper::s_responseErrorCode;
int RICsdkHelper::s_unicastDiscovery = 5;
char RICsdkHelper::s_szQueryUri[100] =
{ 0 };
char RICsdkHelper::s_discoveryAddr[100];
string RICsdkHelper::s_coapQuery;
OCDevAddr* RICsdkHelper::s_remoteAddr;
OCResourceHandle RICsdkHelper::s_handle;
int64_t RICsdkHelper::s_temp = 10;
char* RICsdkHelper::s_units = "C";
int64_t RICsdkHelper::s_hour = 5;
bool RICsdkHelper::s_isBtc = false;
OCDeviceInfo RICsdkHelper::s_deviceInfo;

RICsdkHelper::RICsdkHelper()
{
    m_failureMsg = "";
    m_result = OC_STACK_ERROR;
    s_quitFlag = false;
    s_isCallback = false;
    s_isPayloadCorrect = false;
    s_isServerResponse = true;
    s_coapQuery = "";
    s_remoteAddr = nullptr;
    m_targetPayload = nullptr;
    s_isResourceFound = false;

    duplicateString(&s_deviceInfo.deviceName, DEVICE_NAME);
    duplicateString(&s_deviceInfo.specVersion, SPEC_VERSION);
    OCResourcePayloadAddStringLL(&s_deviceInfo.dataModelVersions, DATA_MODEL_VERSION);
    OCResourcePayloadAddStringLL(&s_deviceInfo.types, DEVICE_TYPE);
}

RICsdkHelper::~RICsdkHelper()
{
}

RICsdkHelper* RICsdkHelper::getInstance(void)
{
    if (s_riCsdkHelperInstance == NULL)
    {
        s_mutex.lock();

        if (s_riCsdkHelperInstance == NULL)
        {
            s_riCsdkHelperInstance = new RICsdkHelper();
        }

        s_mutex.unlock();
    }

    return s_riCsdkHelperInstance;
}

OCStackResult RICsdkHelper::initServer()
{
    return OCInit(m_ServerIp, m_ServerPort, OC_SERVER);
}

void RICsdkHelper::duplicateString(char ** targetString, std::string sourceString)
{
    *targetString = new char[sourceString.length() + 1];
    strncpy(*targetString, sourceString.c_str(), (sourceString.length() + 1));
}

OCPlatformInfo RICsdkHelper::getPlatformInfo()
{
    OCPlatformInfo platformInfo;

    duplicateString(&platformInfo.platformID, PLATFORM_ID);
    duplicateString(&platformInfo.manufacturerName, MANUFACTURER_NAME);
    duplicateString(&platformInfo.manufacturerUrl, MANUFACTURER_URL);
    duplicateString(&platformInfo.modelNumber, MODEL_NO);
    duplicateString(&platformInfo.dateOfManufacture, MANUFACTURE_DATE);
    duplicateString(&platformInfo.platformVersion, PLATFORM_VERSION);
    duplicateString(&platformInfo.operatingSystemVersion, OS_VERSION);
    duplicateString(&platformInfo.hardwareVersion, HW_VERSION);
    duplicateString(&platformInfo.firmwareVersion, FIRMWARE_VERSION);
    duplicateString(&platformInfo.supportUrl, SUPPORT_URL);
    duplicateString(&platformInfo.systemTime, SYSTEM_TIME);

    return platformInfo;
}

bool RICsdkHelper::OCResourcePayloadAddStringLL(OCStringLL **stringLL, std::string value)
{
    char *dup = NULL;
    duplicateString(&dup, value);
    if (!*stringLL)
    {
        *stringLL = (OCStringLL *) OICCalloc(1, sizeof(OCStringLL));
        (*stringLL)->value = dup;
        return true;
    }
    else
    {
        OCStringLL *temp = *stringLL;
        while (temp->next)
        {
            temp = temp->next;
        }
        temp->next = (OCStringLL *) OICCalloc(1, sizeof(OCStringLL));
        temp->next->value = dup;
        return true;
    }
    return false;
}

OCDeviceInfo RICsdkHelper::getDeviceInfo()
{
    OCDeviceInfo deviceInfo;

    duplicateString(&deviceInfo.deviceName, DEVICE_NAME);
    duplicateString(&deviceInfo.specVersion, SPEC_VERSION);
    OCResourcePayloadAddStringLL(&deviceInfo.dataModelVersions, DATA_MODEL_VERSION);
    OCResourcePayloadAddStringLL(&deviceInfo.types, DEVICE_TYPE);

    return deviceInfo;
}

OCEntityHandlerResult RICsdkHelper::ProcessGetRequest(OCEntityHandlerRequest *ehRequest,
        OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult;

    OCRepPayload *getResp = constructResponse(ehRequest);
    if (!getResp)
    {
        IOTIVITYTEST_LOG(INFO, "constructResponse failed");
        return OC_EH_ERROR;
    }

    *payload = getResp;
    ehResult = OC_EH_OK;

    return ehResult;
}

OCEntityHandlerResult RICsdkHelper::ProcessPutRequest(OCEntityHandlerRequest *ehRequest,
        OCRepPayload** payload)
{
    OCEntityHandlerResult ehResult;
    OCRepPayload *putResp = constructResponse(ehRequest);

    if (!putResp)
    {
        IOTIVITYTEST_LOG(INFO, "Failed to construct Json response");
        return OC_EH_ERROR;
    }

    *payload = putResp;
    ehResult = OC_EH_OK;

    return ehResult;
}

OCEntityHandlerResult RICsdkHelper::ProcessPostRequest(OCEntityHandlerRequest *ehRequest,
        OCRepPayload** payload)
{
    OCEntityHandlerResult ehResult;
    OCRepPayload *postResp = constructResponse(ehRequest);

    if (!postResp)
    {
        IOTIVITYTEST_LOG(INFO, "Failed to construct Json response");
        return OC_EH_ERROR;
    }

    *payload = postResp;
    ehResult = OC_EH_OK;

    return ehResult;
}

OCEntityHandlerResult RICsdkHelper::ProcessDeleteRequest(OCEntityHandlerRequest *ehRequest)
{
    if (ehRequest == NULL)
    {
        IOTIVITYTEST_LOG(INFO, "The ehRequest is NULL");
        return OC_EH_ERROR;
    }
    OCEntityHandlerResult ehResult = OC_EH_OK;

    IOTIVITYTEST_LOG(INFO, "\n\nExecuting %s for resource %d ", __func__, ehRequest->resource);

    if ((ehRequest != NULL) && (ehRequest->resource == s_handle))
    {
        OCStackResult result = OCDeleteResource(ehRequest->resource);

        if (result == OC_STACK_OK)
        {
            IOTIVITYTEST_LOG(INFO, "\n\nDelete Resource operation succeeded.");
            ehResult = OC_EH_OK;
        }
        else if (result == OC_STACK_NO_RESOURCE)
        {
            IOTIVITYTEST_LOG(INFO, "\n\nThe resource doesn't exist or it might have been deleted.");
            ehResult = OC_EH_RESOURCE_DELETED;
        }
        else
        {
            IOTIVITYTEST_LOG(INFO, "\n\nEncountered error from OCDeleteResource().");
            ehResult = OC_EH_ERROR;
        }
    }
    else if (ehRequest->resource != s_handle)
    {
        IOTIVITYTEST_LOG(INFO, "\n\nThe request is received for a non-support resource.");
        ehResult = OC_EH_FORBIDDEN;
    }

    return ehResult;
}

OCEntityHandlerResult RICsdkHelper::OCEntityHandlerCb(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest, void* /*callback*/)
{

    if (s_isBtc == true)
    {
        return OC_EH_OK;
    }

    IOTIVITYTEST_LOG(INFO, "Inside server's callback");
    IOTIVITYTEST_LOG(INFO, "Inside entity handler - flags: 0x%x", flag);

    OCEntityHandlerResult ehResult = OC_EH_OK;
    OCEntityHandlerResponse response =
    { 0, 0, OC_EH_ERROR, 0, 0,
    { },
    { 0 }, false };

    if (!entityHandlerRequest)
    {
        IOTIVITYTEST_LOG(INFO, "Invalid request pointer");
        return OC_EH_ERROR;
    }

    // Initialize certain response fields
    response.numSendVendorSpecificHeaderOptions = 0;
    memset(response.sendVendorSpecificHeaderOptions, 0,
            sizeof response.sendVendorSpecificHeaderOptions);
    memset(response.resourceUri, 0, sizeof response.resourceUri);
    OCRepPayload* payload = nullptr;

    if (flag & OC_REQUEST_FLAG)
    {
        IOTIVITYTEST_LOG(INFO, "Flag includes OC_REQUEST_FLAG");

        if (OC_REST_GET == entityHandlerRequest->method)
        {
            IOTIVITYTEST_LOG(INFO, "Received OC_REST_GET from client");
            ehResult = ProcessGetRequest(entityHandlerRequest, &payload);
        }
        else if (OC_REST_PUT == entityHandlerRequest->method)
        {
            IOTIVITYTEST_LOG(INFO, "Received OC_REST_PUT from client");
            ehResult = ProcessPutRequest(entityHandlerRequest, &payload);
        }
        else if (OC_REST_POST == entityHandlerRequest->method)
        {
            IOTIVITYTEST_LOG(INFO, "Received OC_REST_POST from client");
            ehResult = ProcessPostRequest(entityHandlerRequest, &payload);
        }
        else if (OC_REST_DELETE == entityHandlerRequest->method)
        {
            IOTIVITYTEST_LOG(INFO, "Received OC_REST_DELETE from client");
            ehResult = ProcessDeleteRequest(entityHandlerRequest);
        }
        else
        {
            IOTIVITYTEST_LOG(INFO, "Received unsupported method %d from client",
                    entityHandlerRequest->method);
            ehResult = OC_EH_ERROR;
        }
        // If the result isn't an error or forbidden, send response
        if (!((ehResult == OC_EH_ERROR) || (ehResult == OC_EH_FORBIDDEN)))
        {
            // Format the response.  Note this requires some info about the request
            response.requestHandle = entityHandlerRequest->requestHandle;
            response.resourceHandle = entityHandlerRequest->resource;
            response.ehResult = ehResult;
            s_responseErrorCode = response.ehResult;
            if (payload)
            {
                response.payload = reinterpret_cast< OCPayload* >(payload);
                IOTIVITYTEST_LOG(INFO, "Sending response with payload. Error Response = %d",
                        ehResult);
            }
            // Indicate that response is NOT in a persistent buffer
            response.persistentBufferFlag = 0;

            if (OCDoResponse(&response) != OC_STACK_OK)
            {
                IOTIVITYTEST_LOG(INFO, "Error sending response");
                RICsdkHelper::s_isServerResponse = false;
                s_failureMsg = s_failureMsg + "Server: Response is not success. ";
                ehResult = OC_EH_ERROR;
            }
        }
    }
    if (flag & OC_OBSERVE_FLAG)
    {
        IOTIVITYTEST_LOG(INFO, "Flag includes OC_OBSERVE_FLAG");

        if (OC_OBSERVE_REGISTER == entityHandlerRequest->obsInfo.action)
        {
            IOTIVITYTEST_LOG(INFO, "Received OC_OBSERVE_REGISTER from client");
        }
        else if (OC_OBSERVE_DEREGISTER == entityHandlerRequest->obsInfo.action)
        {
            IOTIVITYTEST_LOG(INFO, "Received OC_OBSERVE_DEREGISTER from client");
        }
    }

    return ehResult;
}

OCResourceHandle RICsdkHelper::createResource(const char *resourceTypeName,
        const char *resourceInterfaceName, const char *uri)
{
    m_result = OCCreateResource(&m_handle, resourceTypeName, resourceInterfaceName, uri,
            OCEntityHandlerCb, NULL, OC_DISCOVERABLE | OC_OBSERVABLE | OC_ACTIVE);
    if (m_result == OC_STACK_OK)
    {
        s_handle = m_handle;
        return m_handle;
    }
    else
    {
        m_failureMsg = "Server: OCCreateResource failed. Expected: OC_STACK_OK. Actual m_result : "
                + CommonUtil::s_OCStackResultString.at(m_result);
        return NULL;
    }
}

OCStackResult RICsdkHelper::initClient()
{
    return OCInit(m_ClientIp, m_ClientPort, OC_CLIENT);
}

bool RICsdkHelper::payloadLogPlatform(OCRepPayload* payload)
{
    s_failureMsg = s_failureMsg + "Client: Platform INFO: ";

    OCRepPayload* rep = payload;
    int i = 1;
    while (rep)
    {
        IOTIVITYTEST_LOG(INFO, "\tResource #%d", i);
        IOTIVITYTEST_LOG(INFO, "\tURI:%s", rep->uri);
        IOTIVITYTEST_LOG(INFO, "\tResource Types:");
        OCStringLL* strll = rep->types;
        while (strll)
        {
            IOTIVITYTEST_LOG(INFO, "\t\t%s", strll->value);
            strll = strll->next;
        }
        IOTIVITYTEST_LOG(INFO, "\tInterfaces:");
        strll = rep->interfaces;
        while (strll)
        {
            IOTIVITYTEST_LOG(INFO, "\t\t%s", strll->value);
            strll = strll->next;
        }

        OCRepPayloadValue* val = rep->values;

        IOTIVITYTEST_LOG(INFO, "\tValues:");

        while (val)
        {
            switch (val->type)
            {
                case OCREP_PROP_STRING:
                    IOTIVITYTEST_LOG(INFO, "\t\t%s(string):%s", val->name, val->str);
                    if (strcmp(val->name, KEY_PLATFORM_ID) == 0)
                    {
                        if (strcmp(val->str, PLATFORM_ID) != 0)
                        {
                            IOTIVITYTEST_LOG(INFO, "PLATFORM_ID is not correct");
                            s_failureMsg = s_failureMsg + "PLATFORM_ID is not correct. ";
                            return false;
                        }
                    }
                    else if (strcmp(val->name, KEY_MANUFACTURER_NAME) == 0)
                    {
                        if (strcmp(val->str, MANUFACTURER_NAME) != 0)
                        {
                            IOTIVITYTEST_LOG(INFO, "MANUFACTURER_NAME is not correct");
                            s_failureMsg = s_failureMsg + "MANUFACTURER_NAME is not correct. ";
                            return false;
                        }
                    }
                    else if (strcmp(val->name, KEY_MANUFACTURER_URL) == 0)
                    {
                        if (strcmp(val->str, MANUFACTURER_URL) != 0)
                        {
                            IOTIVITYTEST_LOG(INFO, "MANUFACTURER_URL is not correct");
                            s_failureMsg = s_failureMsg + "MANUFACTURER_URL is not correct. ";
                            return false;
                        }
                    }
                    else if (strcmp(val->name, KEY_MODEL_NUMBER) == 0)
                    {
                        if (strcmp(val->str, MODEL_NO) != 0)
                        {
                            IOTIVITYTEST_LOG(INFO, "MODEL_NO is not correct");
                            s_failureMsg = s_failureMsg + "MODEL_NO is not correct. ";
                            return false;
                        }
                    }
                    else if (strcmp(val->name, KEY_DATE_OF_MANUFACTURE) == 0)
                    {
                        if (strcmp(val->str, MANUFACTURE_DATE) != 0)
                        {
                            IOTIVITYTEST_LOG(INFO, "MANUFACTURE_DATE is not correct");
                            s_failureMsg = s_failureMsg + "MANUFACTURE_DATE is not correct. ";
                            return false;
                        }
                    }
                    else if (strcmp(val->name, KEY_PLATFORM_VERSION) == 0)
                    {
                        if (strcmp(val->str, PLATFORM_VERSION) != 0)
                        {
                            IOTIVITYTEST_LOG(INFO, "PLATFORM_VERSION is not correct");
                            s_failureMsg = s_failureMsg + "PLATFORM_VERSION is not correct. ";
                            return false;
                        }
                    }
                    else if (strcmp(val->name, KEY_OPERATING_SYSTEM) == 0)
                    {
                        if (strcmp(val->str, OS_VERSION) != 0)
                        {
                            IOTIVITYTEST_LOG(INFO, "OS_VERSION is not correct");
                            s_failureMsg = s_failureMsg + "OS_VERSION is not correct. ";
                            return false;
                        }
                    }
                    else if (strcmp(val->name, KEY_HARDWARE_VERSION) == 0)
                    {
                        if (strcmp(val->str, HW_VERSION) != 0)
                        {
                            IOTIVITYTEST_LOG(INFO, "HW_VERSION is not correct");
                            s_failureMsg = s_failureMsg + "HW_VERSION is not correct. ";
                            return false;
                        }
                    }
                    else if (strcmp(val->name, KEY_FIRMWARE_VERSION) == 0)
                    {
                        if (strcmp(val->str, FIRMWARE_VERSION) != 0)
                        {
                            IOTIVITYTEST_LOG(INFO, "FIRMWARE_VERSION is not correct");
                            s_failureMsg = s_failureMsg + "FIRMWARE_VERSION is not correct. ";
                            return false;
                        }
                    }
                    else if (strcmp(val->name, KEY_SUPPORT_URL) == 0)
                    {
                        if (strcmp(val->str, SUPPORT_URL) != 0)
                        {
                            IOTIVITYTEST_LOG(INFO, "SUPPORT_URL is not correct");
                            s_failureMsg = s_failureMsg + "SUPPORT_URL is not correct. ";
                            return false;
                        }
                    }
                    else if (strcmp(val->name, KEY_SYSTEM_TIME) == 0)
                    {
                        if (strcmp(val->str, SYSTEM_TIME) != 0)
                        {
                            IOTIVITYTEST_LOG(INFO, "SYSTEM_TIME is not correct");
                            s_failureMsg = s_failureMsg + "SYSTEM_TIME is not correct. ";
                            return false;
                        }
                    }
                    break;
                default:
                    IOTIVITYTEST_LOG(INFO, "\t\t%s <-- Unknown type!", val->name);
                    break;
            }
            val = val->next;
        }
        ++i;
        rep = rep->next;
    }
    return true;
}

bool RICsdkHelper::payloadLogDevice(OCRepPayload* payload)
{
    s_failureMsg = s_failureMsg + "Client: DEVICE INFO: ";

    OCRepPayload* rep = payload;
    int i = 1;
    while (rep)
    {
        IOTIVITYTEST_LOG(INFO, "\tResource #%d", i);
        IOTIVITYTEST_LOG(INFO, "\tURI:%s", rep->uri);
        IOTIVITYTEST_LOG(INFO, "\tResource Types:");
        OCStringLL* strll = rep->types;
        while (strll)
        {
            IOTIVITYTEST_LOG(INFO, "\t\t%s", strll->value);
            strll = strll->next;
        }
        IOTIVITYTEST_LOG(INFO, "\tInterfaces:");
        strll = rep->interfaces;
        while (strll)
        {
            IOTIVITYTEST_LOG(INFO, "\t\t%s", strll->value);
            strll = strll->next;
        }

        OCRepPayloadValue* val = rep->values;

        IOTIVITYTEST_LOG(INFO, "\tValues:");

        while (val)
        {
            switch (val->type)
            {
                case OCREP_PROP_STRING:
                    IOTIVITYTEST_LOG(INFO, "\t\t%s(string):%s", val->name, val->str);
                    if (strcmp(val->name, KEY_DEVICE_NAME) == 0)
                    {
                        if (strcmp(val->str, DEVICE_NAME) != 0)
                        {
                            IOTIVITYTEST_LOG(INFO, "DEVICE_NAME is not correct");
                            s_failureMsg = s_failureMsg + "DEVICE_NAME is not correct. ";
                            return false;
                        }
                    }
                    else if (strcmp(val->name, KEY_SPEC_VERSION) == 0)
                    {
                        if (strcmp(val->str, SPEC_VERSION) != 0)
                        {
                            IOTIVITYTEST_LOG(INFO, "SPEC_VERSION is not correct");
                            s_failureMsg = s_failureMsg + "SPEC_VERSION is not correct. ";
                            return false;
                        }
                    }
                    break;
                default:
                    IOTIVITYTEST_LOG(INFO, "\t\t%s <-- Unknown type!", val->name);
                    break;
            }
            val = val->next;
        }
        ++i;
        rep = rep->next;
    }
    return true;
}

bool RICsdkHelper::payloadLogDiscovery(OCDiscoveryPayload* payload)
{
    s_failureMsg = s_failureMsg + "Client: Discovery Payload: ";
    IOTIVITYTEST_LOG(INFO, "Payload Type: Discovery");
    int checker = 0;
    while (payload && payload->resources)
    {
        IOTIVITYTEST_LOG(INFO, "\tSID: %s", payload->sid);
        if (payload->type)
        {
            for (OCStringLL *strll = payload->type; strll; strll = strll->next)
            {
                IOTIVITYTEST_LOG(INFO, "\tResource Type: %s", strll->value);
            }
        }
        IOTIVITYTEST_LOG(INFO, "\tInterface:");
        for (OCStringLL *itf = payload->iface; itf; itf = itf->next)
        {
            IOTIVITYTEST_LOG(INFO, "\t\t%s", itf->value);
        }

        OCResourcePayload* res = payload->resources;

        int i = 1;
        while (res)
        {
            IOTIVITYTEST_LOG(INFO, "\tResource #%d", i);
            IOTIVITYTEST_LOG(INFO, "\tURI:%s", res->uri);
            IOTIVITYTEST_LOG(INFO, "\tResource Types:");
            OCStringLL* strll = res->types;
            while (strll)
            {
                IOTIVITYTEST_LOG(INFO, "\t\t%s", strll->value);
                if (strcmp(strll->value, RESOURCE_TYPE_TEMPERATURE) == 0)
                {
                    IOTIVITYTEST_LOG(INFO, "Resource Type matched");
                    checker++;
                }
                strll = strll->next;
            }
            IOTIVITYTEST_LOG(INFO, "\tInterfaces:");
            strll = res->interfaces;
            while (strll)
            {
                IOTIVITYTEST_LOG(INFO, "\t\t%s", strll->value);
                strll = strll->next;
            }

            IOTIVITYTEST_LOG(INFO, "\tBitmap: %u", res->bitmap);
            IOTIVITYTEST_LOG(INFO, "\tSecure?: %s", res->secure ? "true" : "false");
            IOTIVITYTEST_LOG(INFO, "\tPort: %u", res->port);
            IOTIVITYTEST_LOG(INFO, "");
            res = res->next;
            ++i;
        }
        payload = payload->next;
    }

    if (checker == 1)
    {
        return true;
    }

    s_failureMsg = s_failureMsg + "Resource type did not match. ";
    return false;
}

bool RICsdkHelper::payloadLogRepResource(OCRepPayload* payload)
{
    s_failureMsg = s_failureMsg + "Client: Representation Payload: ";
    IOTIVITYTEST_LOG(INFO, "Payload Type: Representation");
    OCRepPayload* rep = payload;
    int i = 1;
    int checker = 0;
    while (rep)
    {
        IOTIVITYTEST_LOG(INFO, "\tResource #%d", i);
        IOTIVITYTEST_LOG(INFO, "\tURI:%s", rep->uri);
        IOTIVITYTEST_LOG(INFO, "\tResource Types:");
        OCStringLL* strll = rep->types;
        while (strll)
        {
            if (strcmp(strll->value, RESOURCE_TYPE_PLATFORM) == 0)
            {
                return payloadLogPlatform(rep);
            }
            else if (strcmp(strll->value, RESOURCE_TYPE_DEVICE) == 0)
            {
                return payloadLogDevice(rep);
            }
            IOTIVITYTEST_LOG(INFO, "\t\t%s", strll->value);
            strll = strll->next;
        }
        IOTIVITYTEST_LOG(INFO, "\tInterfaces:");
        strll = rep->interfaces;
        while (strll)
        {
            IOTIVITYTEST_LOG(INFO, "\t\t%s", strll->value);
            strll = strll->next;
        }

        OCRepPayloadValue* val = rep->values;

        IOTIVITYTEST_LOG(INFO, "\tValues:");

        while (val)
        {
            switch (val->type)
            {
                case OCREP_PROP_NULL:
                    IOTIVITYTEST_LOG(INFO, "\t\t%s: NULL", val->name);
                    break;
                case OCREP_PROP_INT:
                    IOTIVITYTEST_LOG(INFO, "\t\t%s(int):%zd", val->name, val->i);
                    if (strcmp(val->name, KEY_TEMPERATURE) == 0)
                    {
                        if (val->i == s_temp)
                        {
                            IOTIVITYTEST_LOG(INFO, "Temperature Value is ok");
                            checker++;
                        }
                    }
                    if (strcmp(val->name, KEY_HOUR) == 0)
                    {
                        if (val->i == s_hour)
                        {
                            IOTIVITYTEST_LOG(INFO, "Hour Value is ok");
                            checker++;
                        }
                    }
                    break;
                case OCREP_PROP_DOUBLE:
                    IOTIVITYTEST_LOG(INFO, "\t\t%s(double):%f", val->name, val->d);
                    break;
                case OCREP_PROP_BOOL:
                    IOTIVITYTEST_LOG(INFO, "\t\t%s(bool):%s", val->name, val->b ? "true" : "false");
                    break;
                case OCREP_PROP_STRING:
                    IOTIVITYTEST_LOG(INFO, "\t\t%s(string):%s", val->name, val->str);
                    if (strcmp(val->name, KEY_UNITS) == 0)
                    {
                        if (strcmp(val->str, s_units) == 0)
                        {
                            IOTIVITYTEST_LOG(INFO, "Units Value is ok");
                            checker++;
                        }
                    }
                    break;
                case OCREP_PROP_BYTE_STRING:
                    IOTIVITYTEST_LOG(INFO, "\t\t%s(binary):", val->name);
                    break;
                case OCREP_PROP_OBJECT:
                    IOTIVITYTEST_LOG(INFO, "\t\t%s(OCRep):%s", val->name, val->obj->uri);
                    break;
                case OCREP_PROP_ARRAY:
                    switch (val->arr.type)
                    {
                        case OCREP_PROP_INT:
                            IOTIVITYTEST_LOG(INFO, "\t\t%s(int array):%zu x %zu x %zu", val->name,
                                    val->arr.dimensions[0], val->arr.dimensions[1],
                                    val->arr.dimensions[2]);
                            break;
                        case OCREP_PROP_DOUBLE:
                            IOTIVITYTEST_LOG(INFO, "\t\t%s(double array):%zu x %zu x %zu",
                                    val->name, val->arr.dimensions[0], val->arr.dimensions[1],
                                    val->arr.dimensions[2]);
                            break;
                        case OCREP_PROP_BOOL:
                            IOTIVITYTEST_LOG(INFO, "\t\t%s(bool array):%zu x %zu x %zu", val->name,
                                    val->arr.dimensions[0], val->arr.dimensions[1],
                                    val->arr.dimensions[2]);
                            break;
                        case OCREP_PROP_STRING:
                            IOTIVITYTEST_LOG(INFO, "\t\t%s(string array):%zu x %zu x %zu",
                                    val->name, val->arr.dimensions[0], val->arr.dimensions[1],
                                    val->arr.dimensions[2]);
                            break;
                        case OCREP_PROP_BYTE_STRING:
                            IOTIVITYTEST_LOG(INFO, "\t\t%s(byte array):%zu x %zu x %zu", val->name,
                                    val->arr.dimensions[0], val->arr.dimensions[1],
                                    val->arr.dimensions[2]);
                            break;
                        case OCREP_PROP_OBJECT:
                            IOTIVITYTEST_LOG(INFO, "\t\t%s(OCRep array):%zu x %zu x %zu", val->name,
                                    val->arr.dimensions[0], val->arr.dimensions[1],
                                    val->arr.dimensions[2]);
                            break;
                        default:
                            IOTIVITYTEST_LOG(INFO, "\t\t%s <-- Unknown/unsupported array type!",
                                    val->name);
                            break;
                    }
                    break;
                default:
                    IOTIVITYTEST_LOG(INFO, "\t\t%s <-- Unknown type!", val->name);
                    break;
            }
            val = val->next;
        }
        ++i;
        rep = rep->next;
    }
    if (checker != 3)
    {
        IOTIVITYTEST_LOG(INFO, "3 values for temperature resource have not matched.");
        s_failureMsg = s_failureMsg + "3 values for temperature resource have not matched. ";
        return false;
    }
    return true;
}

void RICsdkHelper::getPayloadData(OCClientResponse * clientResponse)
{
    bool checkPayload = false;
    switch (clientResponse->payload->type)
    {
        case PAYLOAD_TYPE_REPRESENTATION:
            IOTIVITYTEST_LOG(INFO, "PAYLOAD_TYPE: PAYLOAD_TYPE_REPRESENTATION");
            checkPayload = payloadLogRepResource((OCRepPayload*) (clientResponse->payload));
            if (checkPayload)
            {
                IOTIVITYTEST_LOG(INFO, "received payload is correct");
                s_isPayloadCorrect = true;
            }
            else
            {
                s_isPayloadCorrect = false;
                IOTIVITYTEST_LOG(INFO, "received payload is not correct");
            }
            break;
        case PAYLOAD_TYPE_DISCOVERY:
            IOTIVITYTEST_LOG(INFO, "PAYLOAD_TYPE: PAYLOAD_TYPE_DISCOVERY");
            checkPayload = payloadLogDiscovery((OCDiscoveryPayload*) (clientResponse->payload));
            if (checkPayload)
            {
                IOTIVITYTEST_LOG(INFO, "received payload is correct");
                s_isPayloadCorrect = true;
            }
            else
            {
                s_isPayloadCorrect = false;
                IOTIVITYTEST_LOG(INFO, "received payload is not correct");
            }
            break;
        default:
            IOTIVITYTEST_LOG(INFO, "Unknown Payload Type: %d", clientResponse->payload->type);
            break;
    }
}

OCRepPayload* RICsdkHelper::getPayload(int64_t valueTemp, char* valueUnits, int64_t valueHour)
{
    OCRepPayload* payload = OCRepPayloadCreate();
    if (!payload)
    {
        IOTIVITYTEST_LOG(INFO, "Failed to allocate Payload");
        return nullptr;
    }

    OCRepPayloadSetPropInt(payload, KEY_TEMPERATURE, valueTemp);
    OCRepPayloadSetPropString(payload, KEY_UNITS, valueUnits);
    OCRepPayloadSetPropInt(payload, KEY_HOUR, valueHour);

    return payload;
}

OCRepPayload* RICsdkHelper::constructResponse(OCEntityHandlerRequest *ehRequest)
{
    if (ehRequest->payload && ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        IOTIVITYTEST_LOG(INFO, "Incoming payload not a representation");
        return nullptr;
    }

    OCRepPayload* input = reinterpret_cast< OCRepPayload* >(ehRequest->payload);

    if (ehRequest->resource != s_handle)
    {
        IOTIVITYTEST_LOG(INFO, "Resource doesn't match");
    }

    if (OC_REST_PUT == ehRequest->method || OC_REST_POST == ehRequest->method)
    {
        OCRepPayloadGetPropInt(input, "temperature", &s_temp);
        if (s_temp != 20)
        {
            RICsdkHelper::s_isServerResponse = false;
            s_failureMsg =
                    s_failureMsg
                            + "Server: Received value of temperature from put or post request does not match with the set value. ";
        }
    }

    return getPayload(s_temp, s_units, s_hour);
}

OCStackApplicationResult RICsdkHelper::PlatformDiscoveryReqCB(void* ctx, OCDoHandle /*handle*/,
        OCClientResponse * clientResponse)
{
    s_isPayloadCorrect = false;

    if (ctx == (void*) DEFAULT_CONTEXT_VALUE)
    {
        IOTIVITYTEST_LOG(INFO,
                "Callback Context for Platform DISCOVER query received successfully");
    }

    if (clientResponse)
    {
        s_isCallback = true;
        IOTIVITYTEST_LOG(INFO, "Discovery Response:");
        IOTIVITYTEST_LOG(INFO, "Device Address: %s, Device Port: %d", clientResponse->devAddr.addr,
                clientResponse->devAddr.port);
        IOTIVITYTEST_LOG(INFO, "PAYLOAD RECEIVED: %s", clientResponse->payload);
        IOTIVITYTEST_LOG(INFO, "Result is: %s \n",
                CommonUtil::s_OCStackResultString.at(clientResponse->result).c_str());
        if (clientResponse->payload)
        {
            getPayloadData(clientResponse);
        }
        else
        {
            s_failureMsg = s_failureMsg + "Received Payload is Null. ";
        }
    }
    else
    {
        IOTIVITYTEST_LOG(INFO, "DeviceDiscoveryReqCB received Null clientResponse");
    }

    return (s_unicastDiscovery) ? OC_STACK_DELETE_TRANSACTION : OC_STACK_KEEP_TRANSACTION;
}

OCStackApplicationResult RICsdkHelper::DeviceDiscoveryReqCB(void* ctx, OCDoHandle /*handle*/,
        OCClientResponse* clientResponse)
{
    s_isPayloadCorrect = false;

    if (ctx == (void*) DEFAULT_CONTEXT_VALUE)
    {
        IOTIVITYTEST_LOG(INFO, "Callback Context for Device DISCOVER query received successfully");
    }

    if (clientResponse)
    {
        s_isCallback = true;
        IOTIVITYTEST_LOG(INFO, "Discovery Response:");
        IOTIVITYTEST_LOG(INFO, "Device Address: %s, Device Port: %d", clientResponse->devAddr.addr,
                clientResponse->devAddr.port);
        IOTIVITYTEST_LOG(INFO, "PAYLOAD RECEIVED: %s", clientResponse->payload);
        IOTIVITYTEST_LOG(INFO, "Result is: %s \n",
                CommonUtil::s_OCStackResultString.at(clientResponse->result).c_str());
        if (clientResponse->payload)
        {
            getPayloadData(clientResponse);
        }
        else
        {
            s_failureMsg = s_failureMsg + "Received Payload is Null. ";
        }
    }
    else
    {
        IOTIVITYTEST_LOG(INFO, "DeviceDiscoveryReqCB received Null clientResponse");
    }

    return (s_unicastDiscovery) ? OC_STACK_DELETE_TRANSACTION : OC_STACK_KEEP_TRANSACTION;
}

OCStackApplicationResult RICsdkHelper::ResourceDiscoveryReqCB(void* ctx, OCDoHandle /*handle*/,
        OCClientResponse * clientResponse)
{
    s_isPayloadCorrect = false;

    if (ctx == (void*) DEFAULT_CONTEXT_VALUE)
    {
        IOTIVITYTEST_LOG(INFO,
                "Callback Context for Resource DISCOVER query received successfully");
    }

    if (clientResponse)
    {
        s_isCallback = true;
        IOTIVITYTEST_LOG(INFO, "Discovery Response:");
        IOTIVITYTEST_LOG(INFO, "Device Address: %s, Device Port: %d", clientResponse->devAddr.addr,
                clientResponse->devAddr.port);
        s_remoteAddr = &clientResponse->devAddr;
        IOTIVITYTEST_LOG(INFO, "PAYLOAD RECEIVED: %s", clientResponse->payload);
        IOTIVITYTEST_LOG(INFO, "Result is: %s \n",
                CommonUtil::s_OCStackResultString.at(clientResponse->result).c_str());
        if (clientResponse->result == OC_STACK_OK)
        {
            s_isResourceFound = true;
        }
        else
        {
            s_isResourceFound = false;
        }
        if (clientResponse->payload)
        {
            getPayloadData(clientResponse);
        }
        else
        {
            s_failureMsg = s_failureMsg + "Received Payload is Null. ";
        }
    }
    else
    {
        IOTIVITYTEST_LOG(INFO, "ResourceDiscoveryReqCB received Null clientResponse");
    }

    return (s_unicastDiscovery) ? OC_STACK_DELETE_TRANSACTION : OC_STACK_KEEP_TRANSACTION;
}

OCStackApplicationResult RICsdkHelper::getReqCB(void* ctx, OCDoHandle /*handle*/,
        OCClientResponse * clientResponse)
{
    s_isPayloadCorrect = false;

    if (ctx == (void*) DEFAULT_CONTEXT_VALUE)
    {
        IOTIVITYTEST_LOG(INFO, "Callback Context for GetRequest received successfully");
    }

    if (clientResponse)
    {
        s_isCallback = true;
        IOTIVITYTEST_LOG(INFO, "Discovery Response:");
        IOTIVITYTEST_LOG(INFO, "Device Address: %s, Device Port: %d", clientResponse->devAddr.addr,
                clientResponse->devAddr.port);
        IOTIVITYTEST_LOG(INFO, "PAYLOAD RECEIVED: %s", clientResponse->payload);
        IOTIVITYTEST_LOG(INFO, "Result is: %s \n",
                CommonUtil::s_OCStackResultString.at(clientResponse->result).c_str());
        IOTIVITYTEST_LOG(INFO, "Result number is: %d \n", clientResponse->result);
        if (clientResponse->payload)
        {
            getPayloadData(clientResponse);
        }
        else
        {
            s_failureMsg = s_failureMsg + "Received Payload is Null. ";
        }
    }
    else
    {
        IOTIVITYTEST_LOG(INFO, "getReqCB received Null clientResponse");
    }

    return (s_unicastDiscovery) ? OC_STACK_DELETE_TRANSACTION : OC_STACK_KEEP_TRANSACTION;
}

OCStackApplicationResult RICsdkHelper::putReqCB(void* ctx, OCDoHandle /*handle*/,
        OCClientResponse * clientResponse)
{
    s_isPayloadCorrect = false;

    if (ctx == (void*) DEFAULT_CONTEXT_VALUE)
    {
        IOTIVITYTEST_LOG(INFO, "Callback Context for PutRequest received successfully");
    }

    if (clientResponse)
    {
        s_isCallback = true;
        IOTIVITYTEST_LOG(INFO, "Discovery Response:");
        IOTIVITYTEST_LOG(INFO, "Device Address: %s, Device Port: %d", clientResponse->devAddr.addr,
                clientResponse->devAddr.port);
        IOTIVITYTEST_LOG(INFO, "PAYLOAD RECEIVED: %s", clientResponse->payload);
        if (clientResponse->payload)
        {
            getPayloadData(clientResponse);
        }
        else
        {
            s_failureMsg = s_failureMsg + "Received Payload is Null. ";
        }
    }
    else
    {
        IOTIVITYTEST_LOG(INFO, "putReqCB received Null clientResponse");
    }

    return (s_unicastDiscovery) ? OC_STACK_DELETE_TRANSACTION : OC_STACK_KEEP_TRANSACTION;
}

OCStackApplicationResult RICsdkHelper::postReqCB(void* ctx, OCDoHandle /*handle*/,
        OCClientResponse * clientResponse)
{
    s_isPayloadCorrect = false;

    if (ctx == (void*) DEFAULT_CONTEXT_VALUE)
    {
        IOTIVITYTEST_LOG(INFO, "Callback Context for PostRequest received successfully");
    }

    if (clientResponse)
    {
        s_isCallback = true;
        IOTIVITYTEST_LOG(INFO, "Discovery Response:");
        IOTIVITYTEST_LOG(INFO, "Device Address: %s, Device Port: %d", clientResponse->devAddr.addr,
                clientResponse->devAddr.port);
        IOTIVITYTEST_LOG(INFO, "PAYLOAD RECEIVED: %s", clientResponse->payload);
        if (clientResponse->payload)
        {
            getPayloadData(clientResponse);
        }
        else
        {
            s_failureMsg = s_failureMsg + "Received Payload is Null. ";
        }
    }
    else
    {
        IOTIVITYTEST_LOG(INFO, "PostRequest received Null clientResponse");
    }

    return (s_unicastDiscovery) ? OC_STACK_DELETE_TRANSACTION : OC_STACK_KEEP_TRANSACTION;
}

OCStackApplicationResult RICsdkHelper::deleteReqCB(void* ctx, OCDoHandle /*handle*/,
        OCClientResponse * clientResponse)
{
    s_isPayloadCorrect = false;

    if (ctx == (void*) DEFAULT_CONTEXT_VALUE)
    {
        IOTIVITYTEST_LOG(INFO, "Callback Context for DeleteRequest received successfully");
    }

    if (clientResponse)
    {
        s_isCallback = true;
        IOTIVITYTEST_LOG(INFO, "Discovery Response:");
        IOTIVITYTEST_LOG(INFO, "Device Address: %s, Device Port: %d", clientResponse->devAddr.addr,
                clientResponse->devAddr.port);
        IOTIVITYTEST_LOG(INFO, "PAYLOAD RECEIVED: %s", clientResponse->payload);
        IOTIVITYTEST_LOG(INFO, "Result is: %s \n",
                CommonUtil::s_OCStackResultString.at(clientResponse->result).c_str());
        if (clientResponse->payload)
        {
            getPayloadData(clientResponse);
        }
        else
        {
            s_failureMsg = s_failureMsg + "Received Payload is Null. ";
        }
    }
    else
    {
        IOTIVITYTEST_LOG(INFO, "DeleteRequest received Null clientResponse");
    }

    return (s_unicastDiscovery) ? OC_STACK_DELETE_TRANSACTION : OC_STACK_KEEP_TRANSACTION;
}

OCStackApplicationResult RICsdkHelper::obsReqCB(void* ctx, OCDoHandle /*handle*/,
        OCClientResponse * clientResponse)
{
    s_isPayloadCorrect = false;

    if (ctx == (void*) DEFAULT_CONTEXT_VALUE)
    {
        IOTIVITYTEST_LOG(INFO, "Callback Context for ObserveRequest received successfully");
    }

    if (clientResponse)
    {
        s_isCallback = true;
        IOTIVITYTEST_LOG(INFO, "Discovery Response:");
        IOTIVITYTEST_LOG(INFO, "Device Address: %s, Device Port: %d", clientResponse->devAddr.addr,
                clientResponse->devAddr.port);
        IOTIVITYTEST_LOG(INFO, "PAYLOAD RECEIVED: %s", clientResponse->payload);
        IOTIVITYTEST_LOG(INFO, "SEQUENCE NUMBER: %d", clientResponse->sequenceNumber);

        IOTIVITYTEST_LOG(INFO, "Result is: %s \n",
                CommonUtil::s_OCStackResultString.at(clientResponse->result).c_str());
        if (clientResponse->sequenceNumber == OC_OBSERVE_REGISTER)
        {
            IOTIVITYTEST_LOG(INFO, "Observe Registration Confirmed");
        }
        else if (clientResponse->sequenceNumber == OC_OBSERVE_DEREGISTER)
        {
            IOTIVITYTEST_LOG(INFO, "Observe Cancellation Confirmed");
            return OC_STACK_DELETE_TRANSACTION;
        }
        else if (clientResponse->sequenceNumber == OC_OBSERVE_NO_OPTION)
        {
            IOTIVITYTEST_LOG(INFO, "Observe Registration/Canellation Error");
            return OC_STACK_DELETE_TRANSACTION;
        }
    }
    else
    {
        IOTIVITYTEST_LOG(INFO, "DeleteRequest received Null clientResponse");
    }

    return (s_unicastDiscovery) ? OC_STACK_DELETE_TRANSACTION : OC_STACK_KEEP_TRANSACTION;
}

OCDoHandle RICsdkHelper::doResource(OCMethod method, const char *requestUri, OCQualityOfService qos)
{
    if (method == OC_REST_DISCOVER)
    {
        if (!strcmp(requestUri, PLATFORM_DISCOVERY_QUERY))
        {
            m_cbData.cb = PlatformDiscoveryReqCB;
            IOTIVITYTEST_LOG(INFO, "platform discovery selected");
        }
        else if (!strcmp(requestUri, DEVICE_DISCOVERY_QUERY))
        {
            m_cbData.cb = DeviceDiscoveryReqCB;
            IOTIVITYTEST_LOG(INFO, "device discovery selected");
        }
        else
        {
            m_cbData.cb = ResourceDiscoveryReqCB;
            IOTIVITYTEST_LOG(INFO, "resource discovery selected");
        }
    }
    else if (method == OC_REST_GET)
    {
        m_cbData.cb = getReqCB;
        requestUri = "/TemperatureResURI?if=oic.if.baseline";
        s_coapQuery = "";
        IOTIVITYTEST_LOG(INFO, "get request selected");
    }
    else if (method == OC_REST_PUT)
    {
        m_cbData.cb = putReqCB;
        requestUri = "/TemperatureResURI";
        s_coapQuery = "";

        OCRepPayload* payload = OCRepPayloadCreate();

        if (!payload)
        {
            IOTIVITYTEST_LOG(INFO, "Failed to create put payload object");
        }

        OCRepPayloadSetPropInt(payload, "temperature", 20);
        m_targetPayload = (OCPayload*) payload;

        IOTIVITYTEST_LOG(INFO, "put request selected");
    }
    else if (method == OC_REST_POST)
    {
        m_cbData.cb = postReqCB;
        requestUri = "/TemperatureResURI?if=oic.if.baseline";
        s_coapQuery = "";
        OCRepPayload* payload = OCRepPayloadCreate();

        if (!payload)
        {
            IOTIVITYTEST_LOG(INFO, "Failed to create post payload object");
        }

        OCRepPayloadSetPropInt(payload, "temperature", 20);
        m_targetPayload = (OCPayload*) payload;

        IOTIVITYTEST_LOG(INFO, "post request selected");
    }
    else if (method == OC_REST_DELETE)
    {
        m_cbData.cb = deleteReqCB;
        requestUri = "/TemperatureResURI";
        s_coapQuery = "";
        IOTIVITYTEST_LOG(INFO, "delete request selected");
    }
    else if (method == OC_REST_OBSERVE)
    {
        m_cbData.cb = obsReqCB;
        requestUri = "/TemperatureResURI";
        s_coapQuery = "";
        IOTIVITYTEST_LOG(INFO, "observer request selected");
    }

    m_cbData.context = (void*) DEFAULT_CONTEXT_VALUE;
    m_cbData.cd = NULL;
    OCDoHandle doHandle;
    snprintf(s_szQueryUri, sizeof(s_szQueryUri) - 1, requestUri, s_discoveryAddr);
    std::string userQuery = std::string(s_szQueryUri) + s_coapQuery;

    if (s_remoteAddr)
    {
        IOTIVITYTEST_LOG(INFO, "Sending request to:\t coap://%s:%d", s_remoteAddr->addr,
                s_remoteAddr->port);
    }
    IOTIVITYTEST_LOG(INFO, "sending using query:%s", userQuery.c_str());

    m_result = OCDoResource(&doHandle, method, userQuery.c_str(), s_remoteAddr, m_targetPayload,
            CT_DEFAULT, qos, &m_cbData, INT_ZERO, INT_ZERO);

    if (m_result == OC_STACK_OK)
    {
        return doHandle;
    }
    else
    {
        m_failureMsg = "Client: OCDoResource failed. Expected: OC_STACK_OK. Actual m_result : "
                + CommonUtil::s_OCStackResultString.at(m_result);
        return NULL;
    }
}

void RICsdkHelper::runThreadForOCProcess()
{
    OCStackResult expectedResult = OC_STACK_OK;
    OCStackResult actualResult;

    while (!s_quitFlag)
    {
        actualResult = OCProcess();
        if (actualResult != expectedResult)
        {
            IOTIVITYTEST_LOG(INFO, "OCProcess returns error");
        }
    }
}

void RICsdkHelper::startOCProcess()
{
    pthread_create(&m_p_thread, NULL, &RICsdkHelper::runThreadForOCProcess, NULL);
}

string RICsdkHelper::getFailureMessage()
{
    return m_failureMsg;
}

OCStackResult RICsdkHelper::initClientServer()
{
    return OCInit(m_ServerIp, m_ServerPort, OC_CLIENT_SERVER);
}
