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
    m_pThingsConfiguration = ThingsConfiguration::getInstance();
    m_pThingsMaintenance = ThingsMaintenance::getInstance();

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

        m_defaultRegionValue = rep.getValue< std::string >("regionValue");
        m_defaultTimeValue = rep.getValue< std::string >("timeValue");
        m_defaultCurrentTimeValue = rep.getValue< std::string >("currentTimeValue");
        m_defaultNetworkValue = rep.getValue< std::string >("networkValue");
        m_defaultIPAddressValue = rep.getValue< std::string >("IPAddressValue");
        m_defaultSecurityValue = rep.getValue< std::string >("securityValue");
        m_defaultModeValue = rep.getValue< std::string >("modeValue");
        m_defaultConfigurationValue = rep.getValue< std::string >("configurationValue");
        m_defaultFactorySetValue = rep.getValue< std::string >("factorySetValue");

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

bool ConformanceHelper::findCandidateResourcesAndVerify(vector< string > resourceTypeList,
        OCStackResult expectedResult, int expectedResourceCount, string &errorMsg)
{
    int resourceCount = 0;
    errorMsg = "From \"findCandidateResources\" Expected : " + m_resultMap[expectedResult]
            + " Actual : ";
    cout << "inside findCandidateResourcesAndVerify" << endl;
    m_resourceList.clear();
    m_resourceFoundCallbackCount = 0;
    m_isResourceEnlisted = false;
    cout << "inside findCandidateResourcesAndVerify 2" << endl;
    OCStackResult result = m_groupManager.findCandidateResources(resourceTypeList,
            std::bind(&ConformanceHelper::onFoundCandidate, this, std::placeholders::_1), -1);
    cout << "inside findCandidateResourcesAndVerify 3" << endl;
    waitForResourceFound(m_resourceList, CALLBACK_WAIT_MAX * 2);
    cout << "inside findCandidateResourcesAndVerify 4" << endl;
    cout << "Wait Complete" << endl;

    if (result != expectedResult)
    {
        errorMsg += m_resultMap[result];
        return false;
    }
    cout << "inside findCandidateResourcesAndVerify 5" << endl;
    if (((m_resourceList.size() > 0) && (m_resourceFoundCallbackCount != 1))
            || ((m_resourceList.size() == 0) && (m_resourceFoundCallbackCount != 0)))
    {
        errorMsg = "From \"findCandidateResources\" Callback count Expected : 1, Actual : "
                + to_string(m_resourceFoundCallbackCount);
        return false;
    }

    if (expectedResult == OC_STACK_OK)
    {
        resourceCount = m_resourceList.size();

        if ((expectedResourceCount > 0 && resourceCount == 0)
                || (expectedResourceCount == 0 && resourceCount > 0))
        {
            errorMsg += m_resultMap[result] + "; Expected ResourceCount : "
                    + to_string(expectedResourceCount) + ", Actual Resource Count: "
                    + to_string(resourceCount);
            return false;
        }
    }

    errorMsg = "";
    return true;
}

//bool ConformanceHelper::findGroupAndVerify(vector< string > groupTypeList,
//        OCStackResult expectedResult, string &errorMsg)
//{
//    m_groupList.clear();
//    m_groupFoundCallbackCount = 0;
//
//    int groupCount = 0;
//    int expectedGroupCount = groupTypeList.size();
//    errorMsg = "From \"findGroup\" Expected : " + m_resultMap[expectedResult] + " Actual : ";
//
//    FindCallback findCallback = std::bind(&ConformanceHelper::onFoundGroup, this,
//                                          std::placeholders::_1);
//    OCStackResult result = m_pGroupSynchronization->findGroup(groupTypeList, findCallback);
//
//    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
//
//    if ((result != expectedResult))
//    {
//        errorMsg += m_resultMap[result];
//        return false;
//    }
//
//    if (m_groupFoundCallbackCount != expectedGroupCount)
//    {
//        errorMsg = "From \"findGroup\" Callback count Expected : " + to_string(expectedGroupCount)
//                   + ", Actual : " + to_string(m_groupFoundCallbackCount);
//        return false;
//    }
//
//    if (expectedResult == OC_STACK_OK)
//    {
//        groupCount = m_groupList.size();
//        if ((expectedGroupCount > 0 && groupCount == 0)
//            || (expectedGroupCount == 0 && groupCount > 0))
//        {
//            errorMsg = "From \"findGroup\" : GroupCount : " + to_string(groupCount);
//            return false;
//        }
//    }
//
//    errorMsg = "";
//    return true;
//}

//bool ConformanceHelper::createGroupAndVerify(string groupType, OCStackResult expectedResult,
//        string &errorMsg)
//{
//    m_groupList.clear();
//
//    int groupCount = 0;
//    errorMsg = "From \"createGroup\" Expected : " + m_resultMap[expectedResult] + " Actual : ";
//
//    OCStackResult createGroupResult = m_pGroupSynchronization->createGroup(groupType);
//
//    if ((createGroupResult != expectedResult))
//    {
//        errorMsg += m_resultMap[createGroupResult];
//        return false;
//    }
//
//    if (expectedResult == OC_STACK_OK)
//    {
//        vector < string > groupTypeList;
//        groupTypeList.push_back(groupType);
//        FindCallback findCallback = std::bind(&ConformanceHelper::onFoundGroup, this,
//                                              std::placeholders::_1);
//
//        m_groupList.clear();
//        OCStackResult result = m_pGroupSynchronization->findGroup(groupTypeList, findCallback);
//        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
//
//        if ((result != OC_STACK_OK))
//        {
//            errorMsg =
//                "createGroup returns OC_STACK_OK but Unable to Verify. From Find Group, Expected : OC_STACK_OK, Actual : "
//                + m_resultMap[result];
//            return false;
//        }
//        else
//        {
//            groupCount = m_groupList.size();
//            if (groupCount == 0)
//            {
//                errorMsg += m_resultMap[createGroupResult] + ", GroupCount Mismatch,  GroupCount = "
//                            + to_string(groupCount);
//                return false;
//            }
//        }
//    }
//
//    errorMsg = "";
//    return true;
//
//}

//bool ConformanceHelper::joinGroupAndVerify(const std::shared_ptr< OCResource > group,
//        OCResourceHandle &resourceHandle, OCStackResult expectedResult, string &errorMsg)
//{
//    // Currently there are no other way to verify whether it really joined or not
//    // Currently it is just acting as a wrapper but it will be updated to verify
//    // whether it is really joined or not
//
//    errorMsg = "Join Group returns Error Code. Expected : " + m_resultMap[expectedResult]
//               + ", Actual : ";
//
//    cout << "Joining to group with uri: " << group->uri() << endl;
//    OCStackResult result = m_pGroupSynchronization->joinGroup(group, resourceHandle);
//
//    if (result != expectedResult)
//    {
//        errorMsg += m_resultMap[result];
//        return false;
//    }
//    CommonUtil::waitInSecond(2);
//
//    errorMsg = "";
//    return true;
//}

//bool ConformanceHelper::joinGroupAndVerify(string groupType, OCResourceHandle &resourceHandle,
//        OCStackResult expectedResult, string &errorMsg)
//{
//    // Currently there are no other way to verify whether it really joined or not
//    // Currently it is just acting as a wrapper but it will be updated to verify
//    // whether it is really joined or not
//
//    errorMsg = "Join Group returns Error Code. Expected : " + m_resultMap[expectedResult]
//               + ", Actual : ";
//
//    OCStackResult result = m_pGroupSynchronization->joinGroup(groupType, resourceHandle);
//    if (result != expectedResult)
//    {
//        errorMsg += m_resultMap[result];
//        return false;
//    }
//
//    errorMsg = "";
//    return true;
//}

bool ConformanceHelper::bindResourceToGroupAndVerify(string groupType, string resourceType,
        OCResourceHandle &childHandle, OCStackResult expectedResult, string &errorMsg)
{
    errorMsg = "bindResourceToGroup returns Error. Expected : " + m_resultMap[expectedResult]
            + ", Actual : ";

    vector< string > groupTypeList;
    vector< string > resourceTypeList;

    groupTypeList.push_back(groupType);
    resourceTypeList.push_back(resourceType);

    cout << "-------------------Finding Candidate Resource-------------------\n";

    if (!findCandidateResourcesAndVerify(resourceTypeList, OCStackResult::OC_STACK_OK, 1, errorMsg))
    {
        errorMsg = "bindResourceToGroup Failed and " + errorMsg;
        return false;
    }

    cout << "-------------------Finding Group-------------------\n";

//    if (!findGroupAndVerify(groupTypeList, OC_STACK_OK, errorMsg))
//    {
//        errorMsg = "bindResourceToGroup Failed and " + errorMsg;
//        return false;
//    }

    cout << "-------------------Registering Group-------------------\n";

    OCResourceHandle collectionHandle;
//    OCStackResult result = OCPlatform::registerResource(collectionHandle, m_groupList.at(0));
    OCStackResult result = OCPlatform::registerResource(collectionHandle, m_resourceList.at(1));

    if (result != OC_STACK_OK)
    {
        errorMsg =
                "bindResourceToGroup Failed. Failed to register Group. Expected : OC_STACK_OK,  Actual : "
                        + m_resultMap[result];
        return false;
    }

    cout << "-------------------Binding Resource to Group-------------------\n";

    result = m_groupManager.bindResourceToGroup(childHandle, m_resourceList.at(0),
            collectionHandle);
    if (result != expectedResult)
    {
        errorMsg =
                "bindResourceToGroup Failed. Failed to bind resource to Group. Expected : OC_STACK_OK,  Actual : "
                        + m_resultMap[result];
        return false;
    }

    return true;
}

//bool ConformanceHelper::leaveGroupAndVerify(string groupType, OCResourceHandle &resourceHandle,
//        OCStackResult expectedResult, string &errorMsg)
//{
//    // Currently there are no other way to verify whether it really left or not
//    // Currently it is just acting as a wrapper but it will be updated to verify
//    // whether it is really left or not
//
//    errorMsg = "Leave Group returns Error Code. Expected : " + m_resultMap[expectedResult]
//               + ", Actual : ";
//
//    OCStackResult result = m_pGroupSynchronization->leaveGroup(groupType, resourceHandle);
//
//    if (result != expectedResult)
//    {
//        errorMsg += m_resultMap[result];
//        return false;
//    }
//
//    errorMsg = "";
//    return true;
//}

//bool ConformanceHelper::deleteGroupAndVerify(string groupType, string &errorMsg)
//{
//    m_groupList.clear();
//
//    int groupCount = 0;
//    errorMsg = "From \"deleteGroup\" Expected GroupCount = 0, Actual GroupCount = ";
//
//    m_pGroupSynchronization->deleteGroup(groupType);
//
//    vector < string > groupTypeList;
//    groupTypeList.push_back(groupType);
//    FindCallback findCallback = std::bind(&ConformanceHelper::onFoundGroup, this,
//                                          std::placeholders::_1);
//
//    m_groupList.clear();
//    OCStackResult result = m_pGroupSynchronization->findGroup(groupTypeList, findCallback);
//    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
//
//    if ((result != OC_STACK_OK))
//    {
//        errorMsg = "Unable to verify deleteGroup. From FindGroup, Expected : OC_STACK_OK, Actual : "
//                   + m_resultMap[result];
//        return false;
//    }
//    else
//    {
//        groupCount = m_groupList.size();
//        if (groupCount != 0)
//        {
//            errorMsg += to_string(groupCount);
//            for (shared_ptr< OCResource > group : m_groupList)
//            {
//                cout << "URI of found group: " << group->uri() << endl;
//            }
//            return false;
//        }
//    }
//
//    errorMsg = "";
//    return true;
//}

bool ConformanceHelper::getConfigurationAndVerify(const std::shared_ptr< OCResource > resource,
        OCStackResult expectedResult, bool isCallbackExpected, int expectedErrorCode,
        string expectedValue, string &errorMsg)
{
    vector< string > configurationNameList;
    configurationNameList.push_back("power");
    configurationNameList.push_back("intensity");

    errorMsg = "GetResourceConfiguration, Expected : " + m_resultMap[expectedResult];

    m_callbackErrorCode = -1;
    m_configReceivedCallbackCount = 0;

    OCStackResult result = m_pThingsConfiguration->getConfigurations(resource,
            configurationNameList,
            std::bind(&ConformanceHelper::onConfigurationReceived, this, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3));

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if (result != expectedResult)
    {
        errorMsg += " Actual : " + m_resultMap[result];
        return false;
    }

    if (m_configReceivedCallbackCount != (int) isCallbackExpected)
    {
        errorMsg = "From \"getConfigurations\" Callback count Expected : "
                + to_string(isCallbackExpected) + ", Actual : "
                + to_string(m_configReceivedCallbackCount);
        return false;
    }

    if (m_callbackErrorCode != expectedErrorCode)
    {
        errorMsg = "From \"getConfigurations\", callback error code mismatches. Expected :"
                + to_string(expectedErrorCode) + ", Actual : " + to_string(m_callbackErrorCode);
        return false;
    }

    string configurationValue = "";
    m_representation.getValue("power", configurationValue);
    if (configurationValue.compare(expectedValue) != 0)
    {
        errorMsg =
                "From \"getConfigurations\", got configuration mismatches. Expected power state : "
                        + expectedValue + ", Actual power state : " + configurationValue;
        return false;
    }
    errorMsg = "";
    return true;
}

bool ConformanceHelper::updateGroupConfigurationAndVerify(
        const std::shared_ptr< OCResource > resource, OCStackResult expectedResult,
        bool isCallbackExpected, int expectedErrorCode, string updatedValue, string &errorMsg)
{
    m_callbackErrorCode = -1;
    map< string, string > configurationMap;
    string cofigurationKey = "region";
    string cofigurationValue = updatedValue;
    configurationMap[cofigurationKey] = cofigurationValue;

    errorMsg = "UpdateResourceConfiguration, Expected : " + m_resultMap[expectedResult];

    m_configReceivedCallbackCount = 0;

    OCStackResult result = m_pThingsConfiguration->updateConfigurations(resource, configurationMap,
            std::bind(&ConformanceHelper::onConfigurationReceived, this, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3));

    CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);

    if (result != expectedResult)
    {
        errorMsg += " Actual : " + m_resultMap[result];
        return false;
    }

    if (m_configReceivedCallbackCount != (int) isCallbackExpected)
    {
        errorMsg = "From \"updateConfigurations\" Callback not called. count Expected : "
                + to_string(isCallbackExpected) + ", Actual : "
                + to_string(m_configReceivedCallbackCount);
        return false;
    }

    if (m_callbackErrorCode != expectedErrorCode)
    {
        errorMsg = "From \"updateConfigurations\", callback error code mismatches. Expected :"
                + to_string(expectedErrorCode) + ", Actual : " + to_string(m_callbackErrorCode);
        return false;
    }

    if (expectedErrorCode == 0)
    {
        vector< string > configurationNameList;
        configurationNameList.push_back(cofigurationKey);
        result = m_pThingsConfiguration->getConfigurations(resource, configurationNameList,
                std::bind(&ConformanceHelper::onConfigurationReceived, this, std::placeholders::_1,
                        std::placeholders::_2, std::placeholders::_3));

        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        string configurationValue = "";
        m_representation.getValue("region", configurationValue);
        if (configurationValue.compare(updatedValue) != 0)
        {
            errorMsg =
                    "From \"updateConfigurations\", updated configuration mismatches. Expected region value : "
                            + updatedValue + ", Actual region value : " + configurationValue;
            return false;
        }
    }

    errorMsg = "";
    return true;
}

bool ConformanceHelper::updateConfigurationAndVerify(const std::shared_ptr< OCResource > resource,
        OCStackResult expectedResult, bool isCallbackExpected, int expectedErrorCode,
        string updatedValue, string &errorMsg)
{
    m_callbackErrorCode = -1;
    map< string, string > configurationMap;
    string cofigurationKey = "power";
    string cofigurationValue = updatedValue;
    configurationMap[cofigurationKey] = cofigurationValue;

    errorMsg = "UpdateResourceConfiguration, Expected : " + m_resultMap[expectedResult];

    m_configReceivedCallbackCount = 0;

    OCStackResult result = m_pThingsConfiguration->updateConfigurations(resource, configurationMap,
            std::bind(&ConformanceHelper::onConfigurationReceived, this, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3));

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if (result != expectedResult)
    {
        errorMsg += " Actual : " + m_resultMap[result];
        return false;
    }

    if (m_configReceivedCallbackCount != (int) isCallbackExpected)
    {
        errorMsg = "From \"updateConfigurations\" Callback not called. count Expected : "
                + to_string(isCallbackExpected) + ", Actual : "
                + to_string(m_configReceivedCallbackCount);
        return false;
    }

    if (m_callbackErrorCode != expectedErrorCode)
    {
        errorMsg = "From \"updateConfigurations\", callback error code mismatches. Expected :"
                + to_string(expectedErrorCode) + ", Actual : " + to_string(m_callbackErrorCode);
        return false;
    }

    string configurationValue = "";
    m_representation.getValue("power", configurationValue);
    if (configurationValue.compare(updatedValue) != 0)
    {
        errorMsg =
                "From \"updateConfigurations\", updated configuration mismatches. Expected power state : "
                        + updatedValue + ", Actual power state : " + configurationValue;
        return false;
    }

    errorMsg = "";
    return true;
}

bool ConformanceHelper::rebootAndVerify(const std::shared_ptr< OCResource > resource,
        OCStackResult expectedResult, int expectedErrorCode, string expectedlValue,
        string &errorMsg)
{
    errorMsg = "Reboot, Expected : " + m_resultMap[expectedResult];
    m_callbackErrorCode = -1;

    OCStackResult result = m_pThingsMaintenance->reboot(resource,
            std::bind(&ConformanceHelper::onConfigurationReceived, this, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3));

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if (result != expectedResult)
    {
        errorMsg += " Actual : " + m_resultMap[result];
        return false;
    }

    if (m_callbackErrorCode != expectedErrorCode)
    {
        errorMsg = "From \"Reboot\", callback error code mismatches. Expected :"
                + to_string(expectedErrorCode) + ", Actual : " + to_string(m_callbackErrorCode);
        return false;
    }

    string configurationValue = "";
    m_representation.getValue("rb", configurationValue);
    if (configurationValue.compare(expectedlValue) != 0)
    {
        errorMsg = "From \"reboot\", unable to reboot resource. Expected reboot state : "
                + expectedlValue + ", Actual reboot state : " + configurationValue;
        return false;
    }

    errorMsg = "";
    return true;
}

bool ConformanceHelper::factoryResetAndVerify(const std::shared_ptr< OCResource > resource,
        OCStackResult expectedResult, int expectedErrorCode, bool supportsReset, string &errorMsg)
{
    errorMsg = "FactoryReset, Expected : " + m_resultMap[expectedResult];
    m_callbackErrorCode = -1;

    OCStackResult result = m_pThingsMaintenance->factoryReset(resource,
            std::bind(&ConformanceHelper::onConfigurationReceived, this, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3));

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if (result != expectedResult)
    {
        errorMsg += " Actual : " + m_resultMap[result];
        return false;
    }

    if (m_callbackErrorCode != expectedErrorCode)
    {
        errorMsg = "From \"factoryReset\", callback error code mismatches. Expected :"
                + to_string(expectedErrorCode) + ", Actual : " + to_string(m_callbackErrorCode);
        return false;
    }

    string configurationValue = "";
    string originalValue = DEFAULT_POWER_STATE;
    m_representation.getValue("power", configurationValue);
    if (supportsReset && (configurationValue.compare(originalValue) != 0))
    {
        errorMsg =
                "From \"factoryReset\", unable to revert to factory settings. Expected power state : "
                        + originalValue + ", Actual power state : " + configurationValue;
        return false;
    }

    if (!supportsReset && (configurationValue.compare("") != 0))
    {
        errorMsg =
                "From \"factoryReset\", unable to revert to factory settings. Expected power state : , Actual power state : "
                        + configurationValue;
        return false;
    }

    errorMsg = "";
    return true;
}

bool ConformanceHelper::doBootstrapAndVerify(OCStackResult expectedResult, int expectedErrorCode,
        string &errorMsg)
{
    errorMsg = "doBootstrap  Returned error. Expected : " + m_resultMap[expectedResult];
    m_callbackErrorCode = -1;

    OCStackResult result = m_pThingsConfiguration->doBootstrap(
            std::bind(&ConformanceHelper::onBootstrap, this, PH::_1, PH::_2, PH::_3));
    errorMsg += " Actual : " + m_resultMap[result];

    CommonUtil::waitInSecond(5);

    if (result != expectedResult)
    {
        return false;
    }

    if (m_callbackErrorCode != expectedErrorCode)
    {
        errorMsg += ". But the callback returned error code. Expected = "
                + to_string(expectedErrorCode) + "; Actual = "
                + std::to_string(m_callbackErrorCode);
        return false;
    }

    errorMsg = "";
    return true;

}

bool ConformanceHelper::subscribeCollectionPresenceAndVerify(
        const std::shared_ptr< OCResource > resource, OCStackResult expectedResult,
        int expectedErrorCode, string &errorMsg, bool isCallbackExpected)
{
    errorMsg = "subscribeCollectionPresence, Expected : OC_STACK_OK, ";

    m_callbackErrorCode = -1;
    m_presenceReceivedCallbackCount = 0;
    m_presenceCallbackResult = OC_STACK_NOTIMPL;

    OCStackResult result = m_groupManager.subscribeCollectionPresence(resource,
            std::bind(&ConformanceHelper::onPresenceReceived, this, std::placeholders::_1,
                    std::placeholders::_2));

    CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);

    if (result != OC_STACK_OK)
    {
        errorMsg += " Actual : " + m_resultMap[result];
        return false;
    }

    if (m_presenceReceivedCallbackCount != (int) isCallbackExpected)
    {
        errorMsg = "From \"subscribeCollectionPresence\" Callback count Expected : "
                + to_string((int) isCallbackExpected) + " , Actual : "
                + to_string(m_presenceReceivedCallbackCount);
        return false;
    }

    if (isCallbackExpected)
    {
        if (m_presenceCallbackResult != expectedResult)
        {
            errorMsg =
                    "From \"subscribeClollectionPresence\", callback error code mismatches. Expected :"
                            + to_string(expectedResult) + ", Actual : "
                            + to_string(m_presenceCallbackResult);
            return false;
        }
    }

    errorMsg = "";
    return true;
}

//bool ConformanceHelper::initialzeGroupUsingGroupType(string groupType, string resourceType,
//        OCResourceHandle &resourceHandle, int expectedResourceCount, bool isCreateRequested,
//        bool isJoinRequested, string &errorMsg)
//{
//    vector < string > groupTypeList;
//    vector < string > resourceTypeList;
//
//    groupTypeList.push_back(groupType);
//    resourceTypeList.push_back(resourceType);
//
//    if (isCreateRequested)
//    {
//        cout << "-------------------Creating Group-------------------\n";
//
//        if (!createGroupAndVerify(groupType, OC_STACK_OK, errorMsg))
//        {
//            errorMsg = "InitializeGroup Failed and " + errorMsg;
//            return false;
//        }
//    }
//
//    cout << "-------------------Finding Candidate Resource-------------------\n";
//
//    if (!findCandidateResourcesAndVerify(resourceTypeList, OC_STACK_OK, expectedResourceCount,
//                                         errorMsg))
//    {
//        errorMsg = "InitializeGroup Failed and " + errorMsg;
//        return false;
//    }
//
//    cout << "-------------------Registering Resource-------------------\n";
//
//    cout << "Registering resource uri = " << m_resourceList.at(0)->uri() << endl;
//    OCStackResult registerResResult = OCPlatform::registerResource(resourceHandle,
//                                      m_resourceList.at(0));
//
//    if (registerResResult != OC_STACK_OK)
//    {
//        errorMsg =
//            "Initialize Group Failed. Failed to register Group. Expected : OC_STACK_OK,  Actual : "
//            + m_resultMap[registerResResult];
//        return false;
//    }
//
//    cout << "Resource Handle ready resource uri = " << m_resourceList.at(0)->uri() << endl;
//
//    if (isJoinRequested)
//    {
//        cout << "-------------------Joining Group-------------------\n";
//
//        if (!joinGroupAndVerify(groupType, resourceHandle, OC_STACK_OK, errorMsg))
//        {
//            errorMsg = "InitializeGroup Failed and " + errorMsg;
//            return false;
//        }
//    }
//
//    cout << "-------------------Initializing Group Finished-------------------\n";
//    errorMsg = "";
//    return true;
//}
//
//bool ConformanceHelper::initialzeGroupUsingGroupPointer(string groupType, string resourceType,
//        OCResourceHandle &resourceHandle, int expectedResourceCount, bool isCreateRequested,
//        bool isJoinRequested, string &errorMsg)
//{
//    vector < string > groupTypeList;
//    vector < string > resourceTypeList;
//
//    groupTypeList.push_back(groupType);
//    resourceTypeList.push_back(resourceType);
//
//    if (isCreateRequested)
//    {
//        cout << "-------------------Creating Group-------------------\n";
//
//        if (!createGroupAndVerify(groupType, OC_STACK_OK, errorMsg))
//        {
//            errorMsg = "InitializeGroup Failed and " + errorMsg;
//            return false;
//        }
//    }
//
//    cout << "-------------------Finding Group-------------------\n";
//
//    if (!findGroupAndVerify(groupTypeList, OC_STACK_OK, errorMsg))
//    {
//        errorMsg = "InitializeGroup Failed and " + errorMsg;
//        return false;
//    }
//    CommonUtil::waitInSecond(5);
//    cout << "-------------------Finding Candidate Resource-------------------\n";
//
//    if (!findCandidateResourcesAndVerify(resourceTypeList, OC_STACK_OK, expectedResourceCount,
//                                         errorMsg))
//    {
//        errorMsg = "InitializeGroup Failed and " + errorMsg;
//        return false;
//    }
//
//    cout << "-------------------Registering Resource-------------------\n";
//
//    OCStackResult registerResResult = OCPlatform::registerResource(resourceHandle,
//                                      m_resourceList.at(0));
//
//    if (registerResResult != OC_STACK_OK)
//    {
//        errorMsg =
//            "Initialize Group Failed. Failed to register Group. Expected : OC_STACK_OK,  Actual : "
//            + m_resultMap[registerResResult];
//        return false;
//    }
//
//    if (isJoinRequested)
//    {
//        cout << "-------------------Joining Group-------------------\n";
//
//        if (!joinGroupAndVerify(m_groupList.at(0), resourceHandle, OC_STACK_OK, errorMsg))
//        {
//            errorMsg = "InitializeGroup Failed and " + errorMsg;
//            return false;
//        }
//        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
//    }
//
//    cout << "-------------------Initializing Group Finished-------------------\n";
//    errorMsg = "";
//    return true;
//}

Action *ConformanceHelper::createAction(shared_ptr< OCResource > resource, string actionCapability,
        string capabilityStatus)
{
    Action *action = new Action();
    action->target = resource->uri();

    Capability *capability = new Capability();
    capability->capability = actionCapability;
    capability->status = capabilityStatus;

    action->listOfCapability.push_back(capability);

    return action;
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

//bool ConformanceHelper::initializeAndAddActionSet(GroupManager *groupManager,
//        ActionSet *actionSet, string &errorMsg)
//{
//    std::vector < std::string > groupTypeList;
//    groupTypeList.push_back(COLLECTION_TYPE_ROOM_LARGE);
//
//    cout << "-------------------Finding Group-------------------\n";
//
//    if (!findGroupAndVerify(groupTypeList, OC_STACK_OK, errorMsg))
//    {
//        errorMsg = "initializeAndAddActionSet Failed " + errorMsg;
//        return false;
//    }
//
////    waitForResourceFound(&m_groupList, CALLBACK_WAIT_DEFAULT);
//
//    PutCallback putCallback;
//    putCallback = std::bind(&ConformanceHelper::onPut, this, PH::_1, PH::_2, PH::_3);
//
//    cout << "-------------------Adding Action Set-------------------\n";
//
//    OCStackResult result = groupManager->addActionSet(m_groupList.at(0), actionSet, putCallback);
//
//    if (result != OC_STACK_OK)
//    {
//        errorMsg = "Failed to add action set. Result is " + m_resultMap[result];
//        return false;
//    }
//
//    return true;
//}

bool ConformanceHelper::initializeAndAddActionSet(ActionSet *actionSet, string &errorMessage,
        OCStackResult expectedResult, ACTIONSET_TYPE actionSetType, bool isPrevious)
{
    std::vector< std::string > resourceTypeList;
    resourceTypeList.push_back(RESOURCE_TYPE_FAN);

    bool result = findCandidateResourcesAndVerify(resourceTypeList, OCStackResult::OC_STACK_OK,
            resourceTypeList.size(), errorMessage);

    if (!result)
    {
        return false;
    }

    actionSet->actionsetName = "AllBulbOn";

    if (actionSetType == ACTIONSET_TYPE::RECURSIVE)
    {
        actionSet->type = ACTIONSET_TYPE::RECURSIVE;

        actionSet->mTime.tm_year = 0;
        actionSet->mTime.tm_mon = 0;
        actionSet->mTime.tm_mday = 0;
        actionSet->mTime.tm_hour = 0;
        actionSet->mTime.tm_min = 0;
        if (isPrevious)
        {
            actionSet->mTime.tm_sec = -15;
        }
        else
        {
            actionSet->mTime.tm_sec = 5;
        }

        actionSet->setDelay(actionSet->getSecAbsTime());
    }
    else if (actionSetType == ACTIONSET_TYPE::SCHEDULED)
    {
        actionSet->type = ACTIONSET_TYPE::SCHEDULED;

        struct tm currentTime;
        CommonUtil::getCurrentTime(currentTime);
        actionSet->mTime.tm_isdst = currentTime.tm_isdst;
        actionSet->mTime.tm_year = currentTime.tm_year;
        actionSet->mTime.tm_mon = currentTime.tm_mon;

        actionSet->mTime.tm_hour = currentTime.tm_hour;
        actionSet->mTime.tm_min = currentTime.tm_min;
        if (isPrevious)
        {
            actionSet->mTime.tm_mday = currentTime.tm_mday - 2;
            actionSet->mTime.tm_sec = currentTime.tm_sec - 50;
        }
        else
        {
            actionSet->mTime.tm_mday = currentTime.tm_mday;
            actionSet->mTime.tm_sec = currentTime.tm_sec + 10;
        }

        actionSet->setDelay(actionSet->getSecondsFromAbsoluteTime());

    }
    else
    {
        actionSet->type = OIC::ACTIONSET_TYPE::NONE;
    }

    for (shared_ptr< OCResource > resource : m_resourceList)
    {
        Action *action = createAction(resource, "power", "on");
        actionSet->listOfAction.push_back(action);

        if (isPrevious)
        {
            PutCallback putCallback;
            putCallback = std::bind(&ConformanceHelper::onPut, this, PH::_1, PH::_2, PH::_3);
            OCStackResult apiResult = m_groupManager.addActionSet(resource, actionSet, putCallback);

            if (apiResult != expectedResult)
            {
                errorMessage =
                        "Failed addActionSet when adding actionSet in previous time. Expected result = "
                                + m_resultMap[expectedResult] + ", Actual result = "
                                + m_resultMap[apiResult];
                return false;
            }
        }
        else
        {
            if (!addActionSetAndVerify(resource, actionSet, errorMessage, expectedResult))
            {
                return false;
            }
        }

    }

    return true;
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

bool ConformanceHelper::getStringFromActionSetAndVerify(ActionSet *actionSet,
        vector< shared_ptr< OCResource > > resourceList, string actionCapability,
        string capabilityStatus, string &errorMessage)
{
    const string actionsetDesc = m_groupManager.getStringFromActionSet(actionSet);
    errorMessage = "String from Action Set : \"" + actionsetDesc + "\" and ";

    if (actionsetDesc.compare("") == 0)
    {
        errorMessage = "getStringFromActionSet returned empty string";
        return false;
    }

    size_t found = actionsetDesc.find(actionSet->actionsetName);

    if (found == string::npos)
    {
        errorMessage += "ActionSet name \"" + actionSet->actionsetName + "\" was not Present there";
        return false;
    }

    for (shared_ptr< OCResource > resource : resourceList)
    {
        string actionSetStringPerResource = "uri=" + resource->uri() + "|" + actionCapability + "="
                + capabilityStatus;
        found = actionsetDesc.find(actionSetStringPerResource);

        if (found == string::npos)
        {
            errorMessage += actionSetStringPerResource + " was not present there";
            return false;
        }
    }

    return true;
}

bool ConformanceHelper::getActionSetfromStringAndVerify(string actionSetName,
        vector< string > resourceUriList, string capability, string capabilityStatus,
        bool isNullTest, string &errorMessage)
{
    string testActionsetDesc = actionSetName + "*0 0";

    for (string resourceURI : resourceUriList)
    {
        testActionsetDesc += "*uri=" + resourceURI + "|" + capability + "=" + capabilityStatus;
    }

    cout << "\n[ActionsetDesc] " << testActionsetDesc << endl;

    ActionSet *testActionset = NULL;
    testActionset = m_groupManager.getActionSetfromString(testActionsetDesc);

    if (isNullTest)
    {
        if (testActionset != NULL)
        {
            errorMessage = "ActionSet is Not Null for description string = " + testActionsetDesc;
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        if (testActionset == NULL)
        {
            errorMessage = "ActionSet is Null";
            return false;
        }
    }

    if (testActionset->actionsetName.compare(actionSetName) != 0)
    {
        errorMessage = "ActionSet Name Mismatch, Actual : " + testActionset->actionsetName
                + " Expected : " + actionSetName;
        return false;
    }

    for (Action *action : testActionset->listOfAction)
    {
        auto iterator = find(resourceUriList.begin(), resourceUriList.end(), action->target);

        if (iterator > resourceUriList.end())
        {
            errorMessage = "In the Action, Target \"" + action->target + "\" was not found";
            return false;
        }

        string actionCapability = action->listOfCapability.at(0)->capability;

        if (capability.compare(actionCapability) != 0)
        {
            errorMessage = "In the Target \"" + action->target + "\" Capability Mismatch. Actual : "
                    + actionCapability + ", Expected : " + capability;
            return false;
        }

        string status = action->listOfCapability.at(0)->status;

        if (capabilityStatus.compare(status) != 0)
        {
            errorMessage = "In the Target \"" + action->target
                    + "\" Capability Status Mismatch. Actual : " + capability + "=" + status
                    + ", Expected : " + capability + "=" + capabilityStatus;
            return false;
        }
    }

    delete testActionset;

    return true;
}

bool ConformanceHelper::addActionSetAndVerify(shared_ptr< OCResource > resource,
        ActionSet *actionSet, string &errorMessage, OCStackResult expectedResult)
{
    PutCallback putCallback;
    putCallback = std::bind(&ConformanceHelper::onPut, this, PH::_1, PH::_2, PH::_3);

    m_callbackErrorCode = -1;
    m_actionSetCallbackCount = 0;
    errorMessage = "[addActionsetAndVerify] ";

    OCStackResult result = m_groupManager.addActionSet(resource, actionSet, putCallback);
    if (result != expectedResult)
    {
        errorMessage += "Unable to add action set. Actual: " + m_resultMap[result] + " Expected: "
                + m_resultMap[expectedResult];
        return false;
    }

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if (m_actionSetCallbackCount != 1)
    {
        errorMessage +=
                "addActionSet should call the PUT callback once but failed. Actual Callback call count: "
                        + to_string(m_actionSetCallbackCount) + " Expected count: 1";
        return false;
    }

    if (m_callbackErrorCode != 0)
    {
        errorMessage += "Failed to generate proper PUT Callback Error Code. Actual: "
                + to_string(m_callbackErrorCode) + " Expected: 0";
        return false;
    }

    if (m_representation.empty())
    {
        errorMessage += "PUT callback is called but the OCRepresentation is Empty";
        return false;
    }
    else
    {
        string actualRepresentation = "";
        m_representation.getValue("ActionSet", actualRepresentation);

        string expectedRepresentation = m_groupManager.getStringFromActionSet(actionSet);

        if (expectedRepresentation.compare(actualRepresentation) != 0)
        {
            errorMessage += "OCRepresentation Mismatch. Actual : " + actualRepresentation
                    + " Expected : " + expectedRepresentation;
            return false;
        }
    }

    return true;
}

bool ConformanceHelper::getActionSetAndVerify(shared_ptr< OCResource > resource,
        ActionSet *actionSetToCompare, string &errorMessage)
{
    GetCallback getCallback;
    getCallback = std::bind(&ConformanceHelper::onGet, this, PH::_1, PH::_2, PH::_3);

    m_callbackErrorCode = -1;
    m_actionSetCallbackCount = 0;
    errorMessage = "[getActionsetAndVerify] ";

    OCStackResult result = m_groupManager.getActionSet(resource, actionSetToCompare->actionsetName,
            getCallback);
    if (result != OC_STACK_OK)
    {
        errorMessage += "Unable to get ActionSet. Actual: " + m_resultMap[result]
                + " Expected: OC_STACK_OK";
        return false;
    }

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

    if (m_actionSetCallbackCount != 1)
    {
        errorMessage +=
                "getActionSet should call the GET callback once but failed. Actual Callback call count: "
                        + to_string(m_actionSetCallbackCount) + " Expected count: 1";
        return false;
    }

    if (m_callbackErrorCode != 0)
    {
        errorMessage += "Failed to generate proper GET Callback Error Code. Actual: "
                + to_string(m_callbackErrorCode) + " Expected: 0";
        return false;
    }

    if (m_representation.empty())
    {
        errorMessage += "GET callback is called but the OCRepresentation is Empty";
        return false;
    }
    else
    {
        string actualRepresentation = "";
        m_representation.getValue("ActionSet", actualRepresentation);

        string expectedRepresentation = m_groupManager.getStringFromActionSet(actionSetToCompare);
        string actionSetName = "";
        string actionSetBody = "";
        int delimeterPosition = expectedRepresentation.find_first_of("|");
        actionSetName = expectedRepresentation.substr(0, delimeterPosition - 1);
        actionSetBody = expectedRepresentation.substr(delimeterPosition + 1);
        unsigned int lastPos = string::npos;

        if ((actualRepresentation.find_first_of(actionSetName) == lastPos)
                || (actualRepresentation.find_first_of(actionSetBody) == lastPos))
        {
            errorMessage += "OCRepresentation Mismatch. Actual : " + actualRepresentation
                    + " Expected : " + expectedRepresentation;
            return false;
        }
    }

    return true;
}

bool ConformanceHelper::deleteActionSetAndVerify(ActionSet *actionSet, string &errorMessage)
{
    PostCallback postCallback;
    postCallback = std::bind(&ConformanceHelper::onPost, this, PH::_1, PH::_2, PH::_3);
    bool isDeleteFailed = false;
    errorMessage = "Delete ActionSet failed. ";

    OCStackResult result = OC_STACK_ERROR;

    for (shared_ptr< OCResource > resource : m_resourceList)
    {
        m_callbackErrorCode = -1;
        m_actionSetCallbackCount = 0;

        result = m_groupManager.deleteActionSet(resource, actionSet->actionsetName, postCallback);

        if (result != OC_STACK_OK)
        {
            errorMessage += "[Resource Uri : " + resource->uri()
                    + "] deleteActionSet, Expected : OC_STACK_OK, Actual : " + m_resultMap[result];
            isDeleteFailed = true;
        }

        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        if (m_actionSetCallbackCount != 1)
        {
            errorMessage += "[Resource Uri : " + resource->uri()
                    + "] Actual POST Callback call count: " + to_string(m_actionSetCallbackCount)
                    + " Expected count: 1";
            isDeleteFailed = true;
        }

        if (m_callbackErrorCode != 0)
        {
            errorMessage += "[Resource Uri : " + resource->uri()
                    + "] Failed to generate proper POST Callback Error Code. Actual: "
                    + to_string(m_callbackErrorCode) + " Expected: 0";
            isDeleteFailed = true;
        }

        if (m_representation.empty())
        {
            errorMessage += "[Resource Uri : " + resource->uri()
                    + "] PUT callback is called but the OCRepresentation is Empty";
            isDeleteFailed = true;
        }
        else
        {
            string actualRepresentation = "";
            m_representation.getValue("ActionSet", actualRepresentation);

            if (actualRepresentation.compare("") != 0)
            {
                errorMessage += "OCRepresentation Mismatch. Actual : " + actualRepresentation
                        + " Expected : Empty ActionSet";
                return false;
            }
        }
    }

    if (isDeleteFailed)
    {
        return false;
    }

    return true;
}

bool ConformanceHelper::executeActionSetAndVerify(ActionSet *actionSet, string &errorMessage,
        OCStackResult expectedResult, bool isDelayed, int delaySec, bool isCancelDue)
{
    PostCallback postCallback;
    postCallback = std::bind(&ConformanceHelper::onPost, this, PH::_1, PH::_2, PH::_3);
    bool isExecuteFailed = false;
    errorMessage = "Execute ActionSet failed. ";

    OCStackResult result = OC_STACK_ERROR;

    for (shared_ptr< OCResource > resource : m_resourceList)
    {
        m_callbackErrorCode = -1;
        m_actionSetCallbackCount = 0;

        if (isDelayed)
        {
            result = m_groupManager.executeActionSet(resource, actionSet->actionsetName, delaySec,
                    postCallback);
            CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
        }
        else
        {
            result = m_groupManager.executeActionSet(resource, actionSet->actionsetName,
                    postCallback);
            CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        }

        if (result != expectedResult)
        {
            errorMessage += "[Resource Uri : " + resource->uri()
                    + "] executeActionSet, Expected : OC_STACK_OK, Actual : " + m_resultMap[result];
            isExecuteFailed = true;
            break;
        }

        if (expectedResult != OC_STACK_OK)
        {
            continue;
        }

        if (m_actionSetCallbackCount != 1)
        {
            errorMessage += "[Resource Uri : " + resource->uri()
                    + "] Actual POST Callback call count: " + to_string(m_actionSetCallbackCount)
                    + " Expected count: 1";
            isExecuteFailed = true;
        }

        if (m_callbackErrorCode != 0)
        {
            errorMessage += "[Resource Uri : " + resource->uri()
                    + "] Failed to generate proper POST Callback Error Code. Actual: "
                    + to_string(m_callbackErrorCode) + " Expected: 0";
            isExecuteFailed = true;
        }

        if (isCancelDue)
        {
            break;
        }

        CommonUtil::waitInSecond(CALLBACK_WAIT_MAX + CALLBACK_WAIT_MIN);
        if (!getConfigurationAndVerify(resource, OCStackResult::OC_STACK_OK, true, 0, "on",
                errorMessage))
        {
            isExecuteFailed = true;
        }

    }

    if (isExecuteFailed)
    {
        return false;
    }

    return true;
}

bool ConformanceHelper::cancelActionSetAndVerify(ActionSet *actionSet, string &errorMessage,
        ACTIONSET_TYPE actionSetType)
{
    PostCallback postCallback;
    postCallback = std::bind(&ConformanceHelper::onPost, this, PH::_1, PH::_2, PH::_3);
    bool isCancelFailed = false;
    errorMessage = "Cancel ActionSet failed. ";

    OCStackResult result = OC_STACK_ERROR;

    for (shared_ptr< OCResource > resource : m_resourceList)
    {
        m_callbackErrorCode = -1;
        m_actionSetCallbackCount = 0;

        result = m_groupManager.cancelActionSet(resource, actionSet->actionsetName, postCallback);
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

        if (result != OC_STACK_OK)
        {
            errorMessage += "[Resource Uri : " + resource->uri()
                    + "] caancelActionSet, Expected : OC_STACK_OK, Actual : " + m_resultMap[result];
            isCancelFailed = true;
        }

        if (m_actionSetCallbackCount != 1)
        {
            errorMessage += "[Resource Uri : " + resource->uri()
                    + "] Actual POST Callback call count: " + to_string(m_actionSetCallbackCount)
                    + " Expected count: 1";
            isCancelFailed = true;
        }

        if (m_callbackErrorCode != 0)
        {
            errorMessage += "[Resource Uri : " + resource->uri()
                    + "] Failed to generate proper POST Callback Error Code. Actual: "
                    + to_string(m_callbackErrorCode) + " Expected: 0";
            isCancelFailed = true;
        }

    }

    for (shared_ptr< OCResource > resource : m_resourceList)
    {
        map< string, string > updateMap;
        updateMap["power"] = "off";
        if (actionSetType == ACTIONSET_TYPE::RECURSIVE)
        {
            m_pThingsConfiguration->updateConfigurations(resource, updateMap,
                    std::bind(&ConformanceHelper::onConfigurationReceived, this,
                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        }
    }

    CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT + CALLBACK_WAIT_MIN);

    for (shared_ptr< OCResource > resource : m_resourceList)
    {

        if (actionSetType == ACTIONSET_TYPE::RECURSIVE)
        {
            if (!getConfigurationAndVerify(resource, OCStackResult::OC_STACK_OK, true, 0, "off",
                    errorMessage))
            {
                isCancelFailed = true;
            }
        }
        else if (actionSetType == ACTIONSET_TYPE::SCHEDULED)
        {
            CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
            if (!getConfigurationAndVerify(resource, OCStackResult::OC_STACK_OK, true, 0, "off",
                    errorMessage))
            {
                isCancelFailed = true;
            }
        }
    }

    if (isCancelFailed)
    {
        return false;
    }

    return true;
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
