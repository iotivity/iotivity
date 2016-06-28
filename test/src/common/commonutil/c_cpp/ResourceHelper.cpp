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
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#include <ResourceHelper.h>

ResourceHelper *ResourceHelper::s_resourceHelperInstance = NULL;
mutex ResourceHelper::s_mutex;

ResourceHelper::ResourceHelper(void)
{
    initializeResultMap();

    m_resourceFoundCallbackCount = 0;
    m_groupFoundCallbackCount = 0;
    m_configReceivedCallbackCount = 0;
    m_presenceReceivedCallbackCount = 0;
    m_presenceCallbackResult = OC_STACK_OK;
    m_isResourceEnlisted = 0;
    m_callbackErrorCode = 0;
    m_actionSetCallbackCount = 0;

    m_defaultRegionValue = "";
    m_defaultTimeValue = "";
    m_defaultCurrentTimeValue = "";
    m_defaultNetworkValue = "";
    m_defaultIPAddressValue = "";
    m_defaultSecurityValue = "";
    m_defaultModeValue = "";
    m_defaultConfigurationValue = "";
    m_defaultFactorySetValue = "";
}

ResourceHelper::~ResourceHelper()
{
    if (s_resourceHelperInstance != NULL)
    {
        delete s_resourceHelperInstance;
        s_resourceHelperInstance = NULL;
    }
}

ResourceHelper *ResourceHelper::getInstance(void)
{
    if (s_resourceHelperInstance == NULL)
    {
        s_mutex.lock();

        if (s_resourceHelperInstance == NULL)
        {
            s_resourceHelperInstance = new ResourceHelper();

        }

        s_mutex.unlock();
    }

    return s_resourceHelperInstance;
}

void ResourceHelper::initializeResultMap(void)
{
    m_resultMap[OC_STACK_OK] = "OC_STACK_OK";
    m_resultMap[OC_STACK_RESOURCE_CREATED] = "OC_STACK_RESOURCE_CREATED";
    m_resultMap[OC_STACK_RESOURCE_DELETED] = "OC_STACK_RESOURCE_DELETED";
    m_resultMap[OC_STACK_CONTINUE] = "OC_STACK_CONTINUE";
    m_resultMap[OC_STACK_INVALID_URI] = "OC_STACK_INVALID_URI";
    m_resultMap[OC_STACK_INVALID_QUERY] = "OC_STACK_INVALID_QUERY";
    m_resultMap[OC_STACK_INVALID_IP] = "OC_STACK_INVALID_IP";
    m_resultMap[OC_STACK_INVALID_PORT] = "OC_STACK_INVALID_PORT";
    m_resultMap[OC_STACK_INVALID_CALLBACK] = "OC_STACK_INVALID_CALLBACK";
    m_resultMap[OC_STACK_INVALID_METHOD] = "OC_STACK_INVALID_METHOD";
    m_resultMap[OC_STACK_INVALID_PARAM] = "OC_STACK_INVALID_PARAM";
    m_resultMap[OC_STACK_INVALID_OBSERVE_PARAM] = "OC_STACK_INVALID_OBSERVE_PARAM";
    m_resultMap[OC_STACK_NO_MEMORY] = "OC_STACK_NO_MEMORY";
    m_resultMap[OC_STACK_COMM_ERROR] = "OC_STACK_COMM_ERROR";
    m_resultMap[OC_STACK_NOTIMPL] = "OC_STACK_NOTIMPL";
    m_resultMap[OC_STACK_NO_RESOURCE] = "OC_STACK_NO_RESOURCE";
    m_resultMap[OC_STACK_RESOURCE_ERROR] = "OC_STACK_RESOURCE_ERROR";
    m_resultMap[OC_STACK_SLOW_RESOURCE] = "OC_STACK_SLOW_RESOURCE";
    m_resultMap[OC_STACK_NO_OBSERVERS] = "OC_STACK_NO_OBSERVERS";
    m_resultMap[OC_STACK_OBSERVER_NOT_FOUND] = "OC_STACK_OBSERVER_NOT_FOUND";
    m_resultMap[OC_STACK_PRESENCE_STOPPED] = "OC_STACK_PRESENCE_STOPPED";
    m_resultMap[OC_STACK_PRESENCE_TIMEOUT] = "OC_STACK_PRESENCE_TIMEOUT";
    m_resultMap[OC_STACK_PRESENCE_DO_NOT_HANDLE] = "OC_STACK_PRESENCE_DO_NOT_HANDLE";
    m_resultMap[OC_STACK_VIRTUAL_DO_NOT_HANDLE] = "OC_STACK_VIRTUAL_DO_NOT_HANDLE";
    m_resultMap[OC_STACK_INVALID_OPTION] = "OC_STACK_INVALID_OPTION";
    m_resultMap[OC_STACK_MALFORMED_RESPONSE] = "OC_STACK_MALFORMED_RESPONSE";
    m_resultMap[OC_STACK_PERSISTENT_BUFFER_REQUIRED] = "OC_STACK_PERSISTENT_BUFFER_REQUIRED";
    m_resultMap[OC_STACK_INVALID_REQUEST_HANDLE] = "OC_STACK_INVALID_REQUEST_HANDLE";
    m_resultMap[OC_STACK_ERROR] = "OC_STACK_ERROR";
}

bool ResourceHelper::waitForResourceFound(
        vector< shared_ptr< OCResource > > &foundResourceList, int timeOut)
{
    bool isTimeOut = false;
    int passedTime = 0;
    while (foundResourceList.empty())
    {
        waitInSecond(CALLBACK_WAIT_MIN);
        passedTime++;
        cout << "Waited " << passedTime << " seconds for finding resources" << endl;

        if (passedTime > timeOut)
        {
            isTimeOut = true;
            break;
        }
    }

    return isTimeOut;
}

bool ResourceHelper::IsCallbackTimeoutOccured(int &callbackMonitor, int timeOut)
{
    bool isTimeOut = false;
    clock_t startTime = clock();
    while (callbackMonitor == 0)
    {
        waitInSecond(1);

        if ((clock() - startTime) / 1000 > timeOut)
        {
            isTimeOut = true;
            break;
        }
    }

    return isTimeOut;
}

bool ResourceHelper::containsResource(vector< shared_ptr< OCResource > > resourceList,
        vector< string > resourceTypeList, string &errorMessage)
{
    bool isAllEnlisted = true;
    errorMessage += "TestCase Failed.";

    for (auto resourceType : resourceTypeList)
    {
        bool isEnlisted = false;
        for (auto resource : resourceList)
        {
            string fromType = "";
            string uri = "";
            string fromUri = "";
            QueryParamsMap paramsMap;
            fromType = resourceType.substr(resourceType.find_last_of(".") + 1, string::npos);
            uri = resource->uri();
            fromUri = uri.substr(uri.find_last_of("/") + 1,
                    uri.find_last_of("-") - uri.find_last_of("/") - 1);

            if (fromUri.compare(fromType) == 0)
            {
                isEnlisted = true;
                cout << "Resource Type Found: " << resourceType << endl;
                break;
            }
        }

        if (!isEnlisted)
        {
            isAllEnlisted = false;
            errorMessage += " The following resource type not found: " + resourceType + "\n";

        }

    }

    return isAllEnlisted;
}

void ResourceHelper::printRepresentation(OCRepresentation rep)
{
    OCRepPayload *incomingPayload = rep.getPayload();
    string uri = incomingPayload->uri;
    if (uri.compare("") == 0)
    {
        cout << "The representation has no uri." << endl;
    }
    else
    {
        cout << "The representation has uri: " << incomingPayload->uri << endl;
    }

    while (incomingPayload)
    {
        OCRepPayloadValue *repValue = incomingPayload->values;
        cout << "The representation has following value: " << endl;
        while (repValue)
        {
            string value = "";
            if (repValue->type == OCRepPayloadPropType::OCREP_PROP_INT)
            {
                value = to_string(repValue->i);
            }
            if (repValue->type == OCRepPayloadPropType::OCREP_PROP_DOUBLE)
            {
                value = to_string(repValue->d);
            }
            if (repValue->type == OCRepPayloadPropType::OCREP_PROP_BOOL)
            {
                value = repValue->b ? "true" : "false";
            }
            if (repValue->type == OCRepPayloadPropType::OCREP_PROP_STRING)
            {
                value = string(repValue->str);
            }

            cout << "\t\t\t" << repValue->name << " : " << value << endl;
            repValue = repValue->next;
        }
        incomingPayload = incomingPayload->next;
    }
}

void ResourceHelper::waitInSecond(unsigned int seconds)
{
#ifdef __LINUX__
    sleep(seconds);
#endif

#ifdef __WINDOWS__
    sleep(seconds * 1000);
#endif
#ifdef __TIZEN__
    sleep(seconds);
#endif
}

void ResourceHelper::duplicateString(char ** targetString, string sourceString)
{
    *targetString = new char[sourceString.length() + 1];
    strncpy(*targetString, sourceString.c_str(), (sourceString.length() + 1));
}
