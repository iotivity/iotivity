/******************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
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

#include <sstream>
#include <ResourceHelper.h>
#ifdef HAVE_WINDOWS_H
#include "CommonUtil.h"
#include <windows.h>
#endif

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

bool ResourceHelper::waitForResourceFound(vector< shared_ptr< OCResource > > &foundResourceList,
        int timeOut)
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
    string uri = rep.getUri();
    if (uri.compare("") == 0)
    {
        cout << "The representation has no uri." << endl;
    }
    else
    {
        cout << "The representation has uri: " << uri << endl;
    }

    if (rep.hasAttribute(INTERFACE_KEY))
    {
        cout << "The representation has interface: " << rep.getValueToString(INTERFACE_KEY) << endl;
    }

    if (rep.hasAttribute(RESOURCE_TYPE_KEY))
    {
        cout << "The representation has interface: " << rep.getValueToString(RESOURCE_TYPE_KEY)
                << endl;
    }

    if(rep.getPayload())
    {
        printPayload(rep.getPayload(), rep);
    }
}

void ResourceHelper::printPayload(OCRepPayload* incomingPayload, OCRepresentation rep, int level)
{
    bool hasInterface = false;
    bool hasType = false;
    level++;

    OCStringLL* interfaces = incomingPayload->interfaces;
    string interfaceInfo = "The representation Interfaces are: [";
    while (interfaces)
    {
        hasInterface = true;

        interfaceInfo += string(interfaces->value) + " ";
        interfaces = interfaces->next;
    }

    if (hasInterface)
    {
        cout << interfaceInfo << "]" << endl;
    }

    OCStringLL* types = incomingPayload->types;
    string typeInfo = "The representation Types are: [";
    while (types)
    {
        hasType = true;

        typeInfo += string(types->value) + " ";
        types = types->next;
    }

    if (hasType)
    {
        cout << typeInfo << "]" << endl;
    }

    if (level == 1)
    {
        cout << "The representation has following Attributes: " << endl;
    }

    while (incomingPayload)
    {
        OCRepPayloadValue *repValue = incomingPayload->values;
        while (repValue)
        {
            string value = "";
            string tabValue = "";
            for (int i = 0; i < level; i++)
            {
                tabValue += "\t\t\t";
            }
            cout << tabValue << repValue->name << " : ";

            if (repValue->type == OCRepPayloadPropType::OCREP_PROP_INT)
            {
#ifdef __LINUX__
                value = to_string(repValue->i);
#endif

#if defined(__ANDROID__) || defined(__TIZEN__)
                std::stringstream strstream;
                strstream << repValue->i;
                strstream >> value;
#endif
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
            if (repValue->type == OCRepPayloadPropType::OCREP_PROP_ARRAY)
            {
                if (repValue->arr.type == OCRepPayloadPropType::OCREP_PROP_OBJECT)
                {
                    cout << "[" << endl;
                    size_t arraySize = repValue->arr.dimensions[0];
                    OCRepPayload** objectArray = repValue->arr.objArray;
                    for (size_t sz = 0; sz < arraySize; sz++)
                    {
                        OCRepPayload* payload = *objectArray;
                        cout << "\t\t\t\t\t {" << endl;
                        printPayload(payload, rep, level);
                        cout << "\t\t\t\t\t }" << endl;
                        objectArray++;
                    }
                    cout << "\t\t\t\t ]" << endl;
                }
                else
                {
                    value = rep.getValueToString(repValue->name);
                }
            }
            if (repValue->type == OCRepPayloadPropType::OCREP_PROP_OBJECT)
            {
                cout << "\t{" << endl;
                printPayload(repValue->obj, rep, level);
                cout << "\t}" << endl;
            }
            if (repValue->type == OCRepPayloadPropType::OCREP_PROP_NULL)
            {
                value = "[]";
            }

            cout << value << endl;
            repValue = repValue->next;
        }
        incomingPayload = incomingPayload->next;
    }
}

void ResourceHelper::waitInSecond(unsigned int seconds)
{
    sleep(seconds);
}

void ResourceHelper::duplicateString(char ** targetString, string sourceString)
{
    *targetString = new char[sourceString.length() + 1];
    strncpy(*targetString, sourceString.c_str(), (sourceString.length() + 1));
}

bool ResourceHelper::containsElement(vector< string > list, string target)
{
    bool isFound = false;
    for (string element : list)
    {
        if (element.find(target) != string::npos)
        {
            isFound = true;
            break;
        }
    }
    return isFound;
}

string ResourceHelper::getOnlyTCPHost(vector< string > allHosts)
{
    string tcpHost = "";
    for (string host : allHosts)
    {
        cout << "Found a host: " << host << endl;
        if (host.find("tcp") != string::npos)
        {
            tcpHost = host;
            break;
        }
    }

    return tcpHost;
}

string ResourceHelper::getOnlySecuredHost(vector< string > allHosts)
{
    string securedHost = "";
    for (string host : allHosts)
    {
        cout << "Found a host: " << host << endl;
        if (host.find(SECURED_ENDPOINT_KEY) != string::npos)
        {
            securedHost = host;
            break;
        }
    }

    return securedHost;
}

string ResourceHelper::executeCommand(string cmd) {
    array<char, 128> buffer;
    string result;
    shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}