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

#include "ConformanceHelper.h"

ConformanceHelper *ConformanceHelper::s_conformanceHelperInstance = NULL;
std::mutex ConformanceHelper::s_mutex;

ConformanceHelper::ConformanceHelper(void)
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

ConformanceHelper::~ConformanceHelper()
{
    if (s_conformanceHelperInstance != NULL)
    {
        delete s_conformanceHelperInstance;
        s_conformanceHelperInstance = NULL;
    }
}

ConformanceHelper *ConformanceHelper::getInstance(void)
{
    if (s_conformanceHelperInstance == NULL)
    {
        s_mutex.lock();

        if (s_conformanceHelperInstance == NULL)
        {
            s_conformanceHelperInstance = new ConformanceHelper();

        }

        s_mutex.unlock();
    }

    return s_conformanceHelperInstance;
}

void ConformanceHelper::initializeResultMap(void)
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

void ConformanceHelper::onFoundCandidate(vector< std::shared_ptr< OCResource > > resourceList)
{
    s_mutex.lock();
    cout << "....Inside onFoundCandidateResources...." << endl;

    m_resourceFoundCallbackCount++;

    unsigned int resourceCount = resourceList.size();
    cout << "Total resource found: " << resourceCount << endl;
    if (resourceCount > (unsigned int) MAX_RESOURCE_COUNT)
    {
        cout << "....Leaving onFoundCandidateResources...." << endl;
        s_mutex.unlock();
        return;
    }

    m_resourceList.clear();
    m_resourceList = resourceList;
    m_isResourceEnlisted = true;
    for (auto resource : m_resourceList)
    {
        cout << "Found Resource URI = " << resource->uri() << endl;
    }

    cout << "....Leaving onFoundCandidateResources...." << endl;
    s_mutex.unlock();
}

// this callback type is not defined yet. This will be modified later
void ConformanceHelper::onFoundGroup(std::shared_ptr< OCResource > groupResource)
{
    s_mutex.lock();
    cout << "....Inside onFoundGroup...." << endl;

    if (groupResource != NULL)
    {
        bool isAlreadyFound = false;
        cout << "URI of the found group: " << groupResource->uri() << endl;
        for (shared_ptr< OCResource > alreadyFoundgroup : m_groupList)
        {
            if (alreadyFoundgroup->uri().compare(groupResource->uri()) == 0)
            {
                isAlreadyFound = true;
                cout << "Group exists in found group list!!!" << endl;
                break;
            }
        }

        if (isAlreadyFound)
        {
            cout << "Ignoring already found group with uri: " << groupResource->uri() << endl;
        }
        else
        {
            cout << "Adding group to the found group list with uri: " << groupResource->uri()
                    << endl;
            m_groupList.push_back(groupResource);
            m_groupFoundCallbackCount++;

        }

    }
    else
    {
        cerr << "Found Group is null, Omitting null group" << endl;
    }
    cout << "....Leaving onFoundGroup...." << endl;
    s_mutex.unlock();
}

void ConformanceHelper::onConfigurationReceived(const HeaderOptions &headerOptions,
        const OCRepresentation &rep, const int eCode)
{
    cout << "....Inside onConfigurationReceived.... , with error code: " << eCode << endl;

    s_mutex.lock();
    m_callbackErrorCode = eCode;
    m_configReceivedCallbackCount++;
    m_headerOption = headerOptions;
    m_representation = rep;

    string value = "";
    m_representation.getValue("power", value);
    cout << "power is: = " << value << endl;
    m_representation.getValue("intensity", value);
    cout << "intensity is: = " << value << endl;
    m_representation.getValue("manufacturer", value);
    cout << "manufacturer is: = " << value << endl;

    std::string jsonRep = rep.getPayload()->values->str;
    cout << "State is: = " << jsonRep << endl;

    s_mutex.unlock();
}

void ConformanceHelper::onPresenceReceived(std::string presence, OCStackResult result)
{
    cout << "....Inside onPresenceReceived...." << endl;

    m_presenceReceivedCallbackCount++;
    m_presenceCallbackResult = result;

    if (m_presenceCallbackResult == OC_STACK_OK)
    {
        cout << "Successfully subscribed collection(group) for presence, presence message is: "
                << presence << endl;

    }
    else
    {
        cerr << "Unable to subscribe collection(group) for presence, result is = "
                << m_resultMap[m_presenceCallbackResult] << endl;
    }
}

void ConformanceHelper::onGet(const HeaderOptions &headerOptions, const OCRepresentation &rep,
        const int eCode)
{
    cout << "....Inside onGet.... , with error code: " << eCode << endl;
    m_headerOption = headerOptions;
    m_callbackErrorCode = eCode;
    m_actionSetCallbackCount++;
    m_representation = rep;

    std::string jsonRep = rep.getPayload()->values->str;
    cout << "State is: = " << jsonRep << endl;
}

void ConformanceHelper::onPost(const HeaderOptions &headerOptions, const OCRepresentation &rep,
        const int eCode)
{
    cout << "....Inside onPost.... , with error code: " << eCode << endl;
    m_headerOption = headerOptions;
    m_callbackErrorCode = eCode;
    m_actionSetCallbackCount++;
    m_representation = rep;

    std::string jsonRep = rep.getPayload()->values->str;
    cout << "State is: = " << jsonRep << endl;
}

void ConformanceHelper::onPut(const HeaderOptions &headerOptions, const OCRepresentation &rep,
        const int eCode)
{
    cout << "....Inside onPut.... , with error code: " << eCode << endl;
    m_headerOption = headerOptions;
    m_callbackErrorCode = eCode;
    m_actionSetCallbackCount++;
    m_representation = rep;

    std::string jsonRep = rep.getPayload()->values->str;
    cout << "State is: = " << jsonRep << endl;
}

void ConformanceHelper::onBootstrap(const HeaderOptions &headerOptions, const OCRepresentation &rep,
        const int eCode)
{
    cout << "Bottstrap callback called" << endl;
    m_callbackErrorCode = eCode;

    if (eCode == SUCCESS_RESPONSE)
    {
        cout << "\n\nGET request was successful" << endl;
        cout << "\tResource URI: " << rep.getUri() << endl;

        m_defaultRegionValue = rep.getValue < std::string > ("regionValue");
        m_defaultTimeValue = rep.getValue < std::string > ("timeValue");
        m_defaultCurrentTimeValue = rep.getValue < std::string > ("currentTimeValue");
        m_defaultNetworkValue = rep.getValue < std::string > ("networkValue");
        m_defaultIPAddressValue = rep.getValue < std::string > ("IPAddressValue");
        m_defaultSecurityValue = rep.getValue < std::string > ("securityValue");
        m_defaultModeValue = rep.getValue < std::string > ("modeValue");
        m_defaultConfigurationValue = rep.getValue < std::string > ("configurationValue");
        m_defaultFactorySetValue = rep.getValue < std::string > ("factorySetValue");

        cout << "\tregionValue : " << m_defaultRegionValue << endl;
        cout << "\ttimeValue : " << m_defaultTimeValue << endl;
        cout << "\tcurrentTimeValue : " << m_defaultCurrentTimeValue << endl;
        cout << "\tnetworkValue : " << m_defaultNetworkValue << endl;
        cout << "\tIPAddressValue : " << m_defaultIPAddressValue << endl;
        cout << "\tsecurityValue : " << m_defaultSecurityValue << endl;
        cout << "\tmodeValue : " << m_defaultModeValue << endl;
        cout << "\tconfigurationValue : " << m_defaultConfigurationValue << endl;
        cout << "\tfactorySetValue : " << m_defaultFactorySetValue << endl;
    }
    else
    {
        cerr << "onGET Response error: " << eCode << endl;
        std::exit(-1);
    }
}

vector< shared_ptr< OCResource > > ConformanceHelper::getResourceList(void)
{
    return m_resourceList;
}

vector< shared_ptr< OCResource > > ConformanceHelper::getGroupList(void)
{
    return m_groupList;
}

bool ConformanceHelper::waitForResourceFound(
        std::vector< std::shared_ptr< OCResource > > &foundResourceList, int timeOut)
{
    bool isTimeOut = false;
    int passedTime = 0;
    while (foundResourceList.empty())
    {
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
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

bool ConformanceHelper::IsCallbackTimeoutOccured(int &callbackMonitor, int timeOut)
{
    bool isTimeOut = false;
    clock_t startTime = clock();
    while (callbackMonitor == 0)
    {
        CommonUtil::waitInSecond(1);

        if ((clock() - startTime) / 1000 > timeOut)
        {
            isTimeOut = true;
            break;
        }
    }

    return isTimeOut;
}

void ConformanceHelper::launchApp(std::string app)
{
#ifdef __LINUX__
    string cmd = "nohup gnome-terminal -x sh -c ";
    cmd += app;
    cmd += " &";
    system(cmd.c_str());
#endif

}

void ConformanceHelper::killApp(std::string app)
{
    string appName = app.substr(app.find_last_of("/\\") + 1);
#ifdef __LINUX__
    std::string prefix = "kill $(pgrep ";
    std::string postfix = ")";
    std::string command = prefix + appName + postfix;
    system(command.c_str());
#endif
}

bool ConformanceHelper::containsResource(vector< shared_ptr< OCResource > > resourceList,
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

void ConformanceHelper::printRepresentation(OCRepresentation rep)
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
                value = std::to_string(repValue->i);
            }
            if (repValue->type == OCRepPayloadPropType::OCREP_PROP_DOUBLE)
            {
                value = std::to_string(repValue->d);
            }
            if (repValue->type == OCRepPayloadPropType::OCREP_PROP_BOOL)
            {
                value = repValue->b ? "true" : "false";
            }
            if (repValue->type == OCRepPayloadPropType::OCREP_PROP_STRING)
            {
                value = std::string(repValue->str);
            }
            cout << "\t\t\t" << repValue->name << " : " << value << endl;
            repValue = repValue->next;
        }
        incomingPayload = incomingPayload->next;
    }
}
