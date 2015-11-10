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

#include <vector>
#include <iostream>
#include <stdlib.h>

#include "SampleResource.h"
#include "CommonUtil.h"
#include "ConformanceHelper.h"
#include "OCPlatform.h"
#include "OCApi.h"

#define LIGHT_1_URI "/device/light-1"
#define LIGHT_2_URI "/device/light-2"
#define LIGHT_3_URI "/device/light-3"
#define LIGHT_SECURED_URI "/device/light-1"
#define FAN_1_URI "/device/fan-1"
#define FAN_2_URI "/device/fan-2"
#define FAN_SECURED_URI "/device/fan-1"
#define ROOM_1_URI "/store/room-1"
#define ROOM_2_URI "/store/room-2"
#define LIGHT_INVISIBLE_URI "/device/light-invisible"
#define FAN_INVISIBLE_URI "/device/fan-invisible"
#define RESOURCE_TYPE_LIGHT "core.light"
#define RESOURCE_TYPE_FAN "core.fan"
#define GROUP_TYPE_ROOM "oic.wk.col"
#define SERVER_IP_V4 "0.0.0.0"
#define SERVER_IP_V6 ":::::"
#define SERVER_PORT 0
#define MAX_LIGHT_RESOURCE_COUNT 100

using namespace OC;
using namespace std;

vector< SampleResource * > createdResourceList;
vector< SampleResource * > createdInvisibleResourceList;
vector< shared_ptr< OCResource > > foundResourceList;
vector< shared_ptr< OCResource > > foundCollectionList;
vector< OCResourceHandle > handleList;
SampleResource *createdLightResource;
SampleResource *createdFanResource;
SampleResource *securedLightResource;
SampleResource *securedFanResource;
SampleResource *invisibleLightResource;
SampleResource *invisibleFanResource;
OCResourceHandle childHandle = NULL;
OCResourceHandle collectionHandle = NULL;
SampleResource* manyResources[MAX_LIGHT_RESOURCE_COUNT];
OCRepresentation resourceRepresentation;
bool hasCallbackArrived = false;
bool isObservingResource = false;
bool isGroupCreated = false;
bool isSecuredResourceCreated = false;
bool isManyLightCreated = false;
string host = "";
QualityOfService qos = QualityOfService::LowQos;
ConformanceHelper *conformanceHelper;

static std::mutex s_mutex;
//static char CRED_FILE_SERVER[] = "oic_svr_db_server_justworks.json";
static char CRED_FILE_SERVER[] = "oic_svr_db_server.json";
static char CRED_FILE_CLIENT[] = "oic_svr_db_client.json";
//static char CRED_FILE_CLIENT[] = "oic_svr_db_client_prov.json";

void onObserve(const HeaderOptions headerOptions, const OCRepresentation &rep, const int &eCode,
        const int &sequenceNumber);
void onDelete(const HeaderOptions &headerOptions, const int eCode);
void onPost(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);
void onPut(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);
void onGet(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);
void onDeviceInfoReceived(const OCRepresentation& rep);
void onPlatformInfoReceived(const OCRepresentation& rep);
void onCollectionFound(shared_ptr< OCResource > collection);
void onResourceFound(shared_ptr< OCResource > resource);

void handleMenu(void);
void showMenu(void);
void createResource(void);
void createSecuredResource(void);
void createInvisibleResource(void);
void createResourceWithUrl(void);
void createManyLightResources(void);
void deleteResource(void);
void findCollection(string);
void findResource(string resourceType, string host = "");
void findAllResources(string host = "");
void discoverDevice(bool);
void discoverPlatform(void);
void sendGetRequest();
void sendPutRequestUpdate(void);
void sendPostRequestUpdate(void);
void sendPutRequestCreate(void);
void sendPostRequestCreate(void);
void sendDeleteRequest(void);
void waitForCallback(void);
void observeResource(void);
void cancelObserveResource(void);
void cancelObservePassively(void);
void createGroup(string);
void joinToGroup(shared_ptr< OCResource >, shared_ptr< OCResource >);
void joinResourceToLocalGroup(shared_ptr< OCResource >);
void joinGroup(void);
void deleteGroup(void);
int selectResource(void);
vector< OCRepresentation > createLinkRepresentation();
string getHost();
FILE* server_fopen(const char*, const char*);
FILE* client_fopen(const char*, const char*);

int main(int argc, char* argv[])
{

    createdResourceList.clear();
    foundResourceList.clear();
    handleList.clear();
    conformanceHelper = ConformanceHelper::getInstance();

    OCStackResult result = OC_STACK_OK;
    string serverIp = SERVER_IP_V6;
    OCConnectivityType ipVer = CT_DEFAULT;
    bool isSecuredClient = false;
    bool isSecuredServer = false;

    if (argc > 1)
    {
        try
        {
            int optionSelected = stoi(argv[1]);

            if (optionSelected == 1)
            {
                cout << "Using CON Server" << endl;
                qos = QualityOfService::HighQos;
            }
            else if (optionSelected == 0)
            {
                cout << "Using NON Server" << endl;
            }
            else
            {
                cout << "Supplied quality of service is invalid. Using default server type: NON"
                        << endl;
            }
        }
        catch (exception&)
        {
            cout << "Invalid input argument. Using default QoS: NON" << endl;
        }
    }
    else
    {
        std::cout << "No QoS supplied. Using default: NON" << std::endl;
    }
    if (argc > 2)
    {
        try
        {
            int optionSelected = stoi(argv[2]);

            if (optionSelected == 6)
            {
                cout << "Using IP version: IPv6" << endl;
                ipVer = CT_IP_USE_V6;
            }
            else if (optionSelected == 4)
            {
                cout << "Using IP version: IPv4" << endl;
                ipVer = CT_IP_USE_V4;
                serverIp = SERVER_IP_V6;
            }
            else
            {
                cout << "Supplied IP version is not a valid IP version. Using default: IPv6"
                        << endl;
            }
        }
        catch (exception&)
        {
            cout << "Invalid input argument. Using default: IPv6" << endl;
        }
    }
    else
    {
        cout << "No IP version supplied. Using default: IPv6" << endl;
    }

    if (argc > 3)
    {
        try
        {
            int optionSelected = stoi(argv[3]);

            if (optionSelected == 1)
            {
                cout << "Using secured client...." << endl;
                isSecuredClient = true;
            }
            else if (optionSelected == 2)
            {
                cout << "Using secured server...." << endl;
                isSecuredServer = true;
            }
            else
            {
                cout << "Using unsecured server & client...." << endl;
            }
        }
        catch (std::exception&)
        {
            cout << "Invalid input argument. Using default: unsecured server & client" << endl;
        }
    }
    else
    {
        cout << "Security mode not specified. Using default: unsecured server & client" << endl;
    }

    if (isSecuredClient)
    {
        OCPersistentStorage ps =
        { client_fopen, fread, fwrite, fclose, unlink };
        PlatformConfig cfg
        { OC::ServiceType::InProc, OC::ModeType::Both, ipVer, ipVer, qos, &ps };
        result = SampleResource::constructServer(cfg);
    }
    else if (isSecuredServer)
    {
        OCPersistentStorage ps =
        { server_fopen, fread, fwrite, fclose, unlink };
        PlatformConfig cfg
        { OC::ServiceType::InProc, OC::ModeType::Server, ipVer, ipVer, qos, &ps };
        result = SampleResource::constructServer(cfg);
    }
    else
    {
        PlatformConfig cfg
        { OC::ServiceType::InProc, OC::ModeType::Both, ipVer, ipVer, qos };
        //    { OC::ServiceType::InProc, OC::ModeType::Both, ipVer, CT_DEFAULT, serverIp, (uint16_t) SERVER_PORT, qos };
        result = SampleResource::constructServer(cfg);
    }

    if (result != OC_STACK_OK)
    {
        cout << "Unable to start Iotivity servers" << endl;
        return 1;
    }
    else
    {
        cout << "Iotivity Server started successfully" << endl;
    }

    cout << "Conformance Simulator started successfully" << endl << endl;
    showMenu();

    cout << "Iotivity Server stopped successfully" << endl;
    cout << "Quitting Conformance Simulator App.... Done!!" << endl;

    return 0;
}

void onResourceFound(shared_ptr< OCResource > resource)
{
    s_mutex.lock();
    cout << "Found a resource!!" << endl;

    if (resource)
    {
        foundResourceList.push_back(resource);
        cout << "uri of the found resource is " << resource->uri() << endl;
        hasCallbackArrived = true;

        cout << "Host of found resource: " << resource->host() << endl;
        cout << "sid of found resource is = " << resource->sid() << endl;
        cout << "unique identifier of found resource is = " << resource->uniqueIdentifier() << endl;
    }
    else
    {
        cout << "Found resource is invalid!!" << endl;
    }

    s_mutex.unlock();
}

void onCollectionFound(shared_ptr< OCResource > collection)
{
    cout << "Found a resource!!" << endl;

    if (collection)
    {
        foundCollectionList.push_back(collection);
        cout << "uri of the found resource is " << collection->uri() << endl;
        hasCallbackArrived = true;

        cout << "Host of found resource: " << collection->host() << endl;
        cout << "sid of found resource is = " << collection->sid() << endl;
        cout << "unique identifier of found resource is = " << collection->uniqueIdentifier()
                << endl;
        collection->get("", DEFAULT_INTERFACE, QueryParamsMap(), onGet);
    }

}

void onPlatformInfoReceived(const OCRepresentation& rep)
{
    cout << "\nPlatform Information received ---->\n";
    string value;
    string values[] =
    { "pi", "Platform ID                    ", "mnmn", "Manufacturer name              ", "mnml",
            "Manufacturer url               ", "mnmo", "Manufacturer Model No          ", "mndt",
            "Manufactured Date              ", "mnpv", "Manufacturer Platform Version  ", "mnos",
            "Manufacturer OS version        ", "mnhw", "Manufacturer hardware version  ", "mnfv",
            "Manufacturer firmware version  ", "mnsl", "Manufacturer support url       ", "st",
            "Manufacturer system time       " };

    for (unsigned int i = 0; i < sizeof(values) / sizeof(values[0]); i += 2)
    {
        if (rep.getValue(values[i], value))
        {
            std::cout << values[i + 1] << " : " << value << std::endl;
        }
    }
    conformanceHelper->printRepresentation(rep);
    hasCallbackArrived = true;
}

void onDeviceInfoReceived(const OCRepresentation& rep)
{
    cout << "\nDevice Information received ---->\n";
    string value;
    string values[] =
    { "di", "Device ID        ", "n", "Device name      ", "lcv", "Spec version url ", "dmv",
            "Data Model Model ", };

    for (unsigned int i = 0; i < sizeof(values) / sizeof(values[0]); i += 2)
    {
        if (rep.getValue(values[i], value))
        {
            std::cout << values[i + 1] << " : " << value << std::endl;
        }
    }

    conformanceHelper->printRepresentation(rep);
    hasCallbackArrived = true;
}

// callback handler on GET request
void onGet(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    if (eCode == SUCCESS_RESPONSE)
    {
        cout << "Response: GET request was successful" << endl;

//        conformanceHelper->printIncomingRepresentation(rep);

        vector< string > interfacelist = rep.getResourceInterfaces();

        bool isCollection = false;
        for (auto interface = interfacelist.begin(); interface != interfacelist.end(); interface++)
        {
            if ((*interface).compare(GROUP_INTERFACE) == 0)
            {
                isCollection = true;
                break;
            }
        }

        if (isCollection)
        {
            std::vector< OCRepresentation > children = rep.getChildren();

            cout << "\nCHILD RESOURCE OF GROUP" << endl;
            for (auto iter = children.begin(); iter != children.end(); ++iter)
            {
                cout << "\tURI :: " << (*iter).getUri() << endl;
            }
            cout << endl;
        }
        else
        {
            resourceRepresentation = rep;
            cout << "THe GET Response has the following representation:" << endl;
            conformanceHelper->printRepresentation(rep);
        }
    }
    else
    {
        cout << "onGET Response error: " << eCode << endl;
    }
    hasCallbackArrived = true;
}

// callback handler on PUT request
void onPut(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    if (eCode == SUCCESS_RESPONSE || eCode == OC_STACK_OK || eCode == OC_STACK_RESOURCE_CREATED)
    {
        cout << "Response: PUT request was successful" << endl;
        cout << "THe PUT response has the following representation:" << endl;
        conformanceHelper->printRepresentation(rep);
    }
    else
    {
        cout << "onPUT Response error: " << eCode << endl;
    }
    hasCallbackArrived = true;
}

// callback handler on POST request
void onPost(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    if (eCode == SUCCESS_RESPONSE || eCode == OC_STACK_OK || eCode == OC_STACK_RESOURCE_CREATED)
    {
        cout << "Response: POST request was successful" << endl;
        cout << "THe POST Response has the following representation:" << endl;
        conformanceHelper->printRepresentation(rep);
    }
    else
    {
        cout << "onPOST Response error: " << eCode << endl;
    }
    hasCallbackArrived = true;
}

// callback handler on DELETE request
void onDelete(const HeaderOptions &headerOptions, const int eCode)
{
    if (eCode == SUCCESS_RESPONSE)
    {
        cout << "Response: DELETE request was successful" << endl;
    }
    else
    {
        cout << "onPOST Response error: " << eCode << endl;
    }
    hasCallbackArrived = true;
}

void onObserve(const HeaderOptions headerOptions, const OCRepresentation &rep, const int &eCode,
        const int &sequenceNumber)
{
    try
    {
        if (eCode == SUCCESS_RESPONSE || eCode == OC_STACK_OK)
        {
            std::cout << "OBSERVE RESULT:" << std::endl;
            std::cout << "\tSequenceNumber: " << sequenceNumber << std::endl;
            string region = "";
            string power = "";
            string manufacturer = "";
            int intensity = 0;
            double version = 0.0;
            rep.getValue("region", region);
            rep.getValue("power", power);
            rep.getValue("manufacturer", manufacturer);
            rep.getValue("intensity", intensity);
            rep.getValue("ver", version);

            std::cout << "\tpower: " << power << std::endl;
            std::cout << "\tregion: " << region << std::endl;
            std::cout << "\tmanufacturer: " << manufacturer << std::endl;
            std::cout << "\tintensity: " << intensity << std::endl;
            std::cout << "\tversion: " << version << std::endl;
        }
    }
    catch (std::exception &e)
    {
        std::cout << "Exception: " << e.what() << " in onObserve" << std::endl;
    }
    hasCallbackArrived = true;

}

void waitForCallback()
{
    int elapsedSecond = 0;
    while (hasCallbackArrived == false)
    {
        CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
        elapsedSecond++;
        if (elapsedSecond > CALLBACK_WAIT_MAX)
        {
            break;
        }
    }
}

FILE* server_fopen(const char *path, const char *mode)
{
    (void) path;
    return fopen(CRED_FILE_SERVER, mode);
}

FILE* client_fopen(const char *path, const char *mode)
{
    (void) path;
    return fopen(CRED_FILE_CLIENT, mode);
}

void createResourceWithUrl()
{
    if (childHandle == NULL)
    {
        cout << "Creating Resource with complete URL" << endl;
        shared_ptr< OCResource > resource;
        if (foundResourceList.size() == 0)
        {
            findResource(RESOURCE_TYPE_LIGHT);
        }
        resource = foundResourceList.at(0);
        OCPlatform::registerResource(childHandle, resource);
    }
    else
    {
        cout << "Resource with complete URL already created!!" << endl;
    }
}

void createResource()
{

    cout << "createResource called!!" << endl;
    if (createdResourceList.size() == 0)
    {
        createdLightResource = new SampleResource();
        createdLightResource->setResourceProperties(LIGHT_1_URI, RESOURCE_TYPE_LIGHT,
        OC_RSRVD_INTERFACE_DEFAULT);
        OCStackResult result = createdLightResource->startServer();

        if (result == OC_STACK_OK)
        {
            cout << "Resource created successfully" << endl;
            createdResourceList.push_back(createdLightResource);
        }
        else
        {
            cout << "Unable to create light resource" << endl;
        }

        createdFanResource = new SampleResource();
        createdFanResource->setResourceProperties(FAN_1_URI, RESOURCE_TYPE_FAN, DEFAULT_INTERFACE);
        uint8_t resourceProperty = OC_ACTIVE | OC_DISCOVERABLE;
        result = createdFanResource->startServer(resourceProperty);
        createdFanResource->setAsSlowResource();

        if (result == OC_STACK_OK)
        {
            cout << "Resource created successfully" << endl;
//            createdResourceList.push_back(createdFanResource);
        }
        else
        {
            cout << "Unable to create fan resource" << endl;
        }
    }
    else
    {
        cout << "Normal Resource already  created!!" << endl;
    }
}

void createManyLightResources()
{
    cout << "createResource called!!" << endl;
    string baseUri = "/device/light-";
    int lightCount = 2;
    if (isManyLightCreated == false)
    {
        string uri = "";
        for (int i = 0; i < MAX_LIGHT_RESOURCE_COUNT; i++)
        {
            uri = baseUri + to_string(lightCount++);
            manyResources[i] = new SampleResource();
            manyResources[i]->setResourceProperties(uri, RESOURCE_TYPE_LIGHT,
            OC_RSRVD_INTERFACE_DEFAULT);
            //                "oic.if.a");
            OCStackResult result = manyResources[i]->startServer();

            if (result == OC_STACK_OK)
            {
                cout << "Light Resource created successfully with uri: " << uri << endl;
                isManyLightCreated = true;
            }
            else
            {
                cout << "Unable to create Light resource with uri" << uri << endl;
            }
        }

    }
    else
    {
        cout << "Many Light Resources already created!!" << endl;
    }

}

void createSecuredResource()
{
    cout << "createSecuredResource called!!" << endl;
    if (isSecuredResourceCreated == false)
    {

        securedLightResource = new SampleResource();
        securedLightResource->setResourceProperties(LIGHT_SECURED_URI, RESOURCE_TYPE_LIGHT,
        OC_RSRVD_INTERFACE_DEFAULT);
        uint8_t resourceProperty = OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE;
        OCStackResult result = securedLightResource->startServer(resourceProperty);

        if (result == OC_STACK_OK)
        {
            cout << "Secure Light Resource created successfully" << endl;
            isSecuredResourceCreated = true;
        }
        else
        {
            cout << "Unable to create secured light resource" << endl;
        }

        securedFanResource = new SampleResource();
        securedFanResource->setResourceProperties(FAN_SECURED_URI, RESOURCE_TYPE_FAN,
                DEFAULT_INTERFACE);
        resourceProperty = OC_ACTIVE | OC_DISCOVERABLE | OC_SECURE;
        result = securedFanResource->startServer(resourceProperty);

        if (result == OC_STACK_OK)
        {
            cout << "Secured Fan Resource created successfully" << endl;
            isSecuredResourceCreated = true;
        }
        else
        {
            cout << "Unable to create secured fan resource" << endl;
        }
    }
    else
    {
        cout << "Secured Resource already  created!!" << endl;
    }
}

void createInvisibleResource()
{

    cout << "createInvisibleResource called!!" << endl;
    if (createdInvisibleResourceList.size() == 0)
    {
        invisibleFanResource = new SampleResource();
        invisibleFanResource->setResourceProperties(FAN_INVISIBLE_URI, RESOURCE_TYPE_FAN,
                DEFAULT_INTERFACE);
        uint8_t resourceProperty = OC_ACTIVE;
        OCStackResult result = invisibleFanResource->startServer(resourceProperty);

        if (result == OC_STACK_OK)
        {
            cout << "Invisible fan resource created successfully" << endl;
            createdInvisibleResourceList.push_back(invisibleFanResource);
        }
        else
        {
            cout << "Unable to create resource" << endl;
        }

        invisibleLightResource = new SampleResource();
        invisibleLightResource->setResourceProperties(LIGHT_INVISIBLE_URI, RESOURCE_TYPE_LIGHT,
                DEFAULT_INTERFACE);
        resourceProperty = OC_ACTIVE;
        result = invisibleLightResource->startServer(resourceProperty);

        if (result == OC_STACK_OK)
        {
            cout << "Invisible light resource created successfully" << endl;
            createdInvisibleResourceList.push_back(invisibleLightResource);
        }
        else
        {
            cout << "Unable to create invisible light resource" << endl;
        }
    }
    else
    {
        cout << "Resource already  created!!" << endl;
    }
}

void createGroup(string groupType)
{
    if (isGroupCreated)
    {
        cout << "Group[ already created!!" << endl;
    }
    else
    {
        try
        {
            string resourceURI = "/core/a/collection";
            string resourceInterface = BATCH_INTERFACE;

            OCPlatform::registerResource(collectionHandle, resourceURI, groupType,
                    resourceInterface,
                    NULL,
                    //&entityHandler, // entityHandler
                    OC_DISCOVERABLE | OC_OBSERVABLE);

            cout << "Create Group is called." << endl;

//	        requestURI << OC_RSRVD_WELL_KNOWN_URI << "?rt=core.light";
//
//	        OCPlatform::findResource("", requestURI.str(),
//	                                 connectivityType, &foundResource);

            OCPlatform::bindInterfaceToResource(collectionHandle, GROUP_INTERFACE);
            OCPlatform::bindInterfaceToResource(collectionHandle, DEFAULT_INTERFACE);

            isGroupCreated = true;
            cout << "Successfully Created Group!!" << endl;
        }
        catch (OCException& e)
        {
            oclog() << "Exception in main: " << e.what();
            isGroupCreated = false;
            cout << "Unable to Create Group!!" << endl;
        }

    }

}

void joinToGroup(shared_ptr< OCResource > group, shared_ptr< OCResource > child)
{

}

void joinResourceToLocalGroup(shared_ptr< OCResource > child)
{
    OCStackResult expectedResult = OC_STACK_OK;
    string errorMessage = "";

    try
    {
        OCPlatform::registerResource(childHandle, child);
    }
    catch (exception& e)
    {
        cout << "Exception while registering remote resource, exception is : " << e.what() << endl;
    }

    if (isGroupCreated == false || collectionHandle == NULL)
    {
        cout << "Local group does not exist!! Please create Local group first!!" << endl;
        return;
    }

    if (childHandle)
    {
        handleList.push_back(childHandle);
        CommonUtil::waitInSecond(2);
        cout << "Joining resource with uri: " << child->uri() << " to local collection" << endl;
        try
        {
            expectedResult = OCPlatform::bindResource(collectionHandle, childHandle);
            if (expectedResult == OC_STACK_OK)
            {
                cout << "Joining to the group completed!!" << endl;
            }
            else
            {
                cout << "Unable to join to group" << endl;
            }
        }
        catch (exception& e)
        {
            cout << "Exception occured while trying to join to grop, exception is: " << e.what()
                    << endl;
        }

    }
    else
    {
        cout << "No child available to join to group!!" << endl;
    }
}

void joinGroup()
{
    if (foundResourceList.size() > 0)
    {
        joinResourceToLocalGroup(foundResourceList.at(0));
    }
    else
    {
        cout << "No resource found to join to group!!" << endl;
    }

}

void deleteGroup()
{
    if (isGroupCreated)
    {
        try
        {
            OCStackResult result = OCPlatform::unregisterResource(collectionHandle);
            if (result == OC_STACK_OK)
            {
                cout << "Successfully Deleted Group!!" << endl;
            }
            else
            {
                cout << "Unable to delete group!!" << endl;
            }
        }
        catch (exception& e)
        {
            std::cout << "Exception occurred while deleting group, exception is :  " << e.what()
                    << std::endl;
        }

    }
    else
    {
        cout << "No group exists to delete!!" << endl;
    }

    while (handleList.size() > 0)
    {
        int index = handleList.size() - 1;
        OCPlatform::unregisterResource(handleList[index]);
        handleList.pop_back();
    }
}

void deleteResource()
{
    while (handleList.size() > 0)
    {
        int index = handleList.size() - 1;
        OCPlatform::unregisterResource(handleList[index]);
        handleList.pop_back();
    }

    OCStackResult result = OC_STACK_ERROR;
    if (createdResourceList.size() != 0)
    {

        result = createdLightResource->stopServer();
        if (result == OC_STACK_OK)
        {
            cout << "Normal Light Resource deleted successfully" << endl;
        }

        delete createdLightResource;
        createdLightResource = NULL;

        result = createdFanResource->stopServer();
        if (result == OC_STACK_OK)
        {
            cout << "Normal Fan Resource deleted successfully" << endl;
        }

        delete createdFanResource;
        createdFanResource = NULL;

        createdResourceList.clear();
    }
    else
    {
        cout << "There is no normal  resource available to delete!!" << endl;
    }

    if (createdInvisibleResourceList.size() != 0)
    {
        result = invisibleFanResource->stopServer();
        if (result == OC_STACK_OK)
        {
            cout << "Invisible Fan Resource deleted successfully" << endl;
        }

        delete invisibleFanResource;
        invisibleFanResource = NULL;

        result = invisibleLightResource->stopServer();
        if (result == OC_STACK_OK)
        {
            cout << "Invisible Light Resource deleted successfully" << endl;
        }

        delete invisibleLightResource;
        invisibleLightResource = NULL;

        createdInvisibleResourceList.clear();
    }
    else
    {
        cout << "There is no invisible  resource available to delete!!" << endl;
    }

    if (isSecuredResourceCreated)
    {

        result = securedLightResource->stopServer();
        if (result == OC_STACK_OK)
        {
            cout << "Secured Light Resource deleted successfully" << endl;
        }

        delete securedLightResource;
        securedLightResource = NULL;

        result = securedFanResource->stopServer();
        if (result == OC_STACK_OK)
        {
            cout << "Secured Fan Resource deleted successfully" << endl;
        }

        delete securedFanResource;
        securedFanResource = NULL;
    }
    else
    {
        cout << "There is no secured  resource available to delete!!" << endl;
    }
    if (isManyLightCreated)
    {
        for (int i = 0; i < MAX_LIGHT_RESOURCE_COUNT; i++)
        {
            result = manyResources[i]->stopServer();
            if (result == OC_STACK_OK)
            {
                cout << "Light Resource no: " << 2 + i << " deleted successfully" << endl;
                isManyLightCreated = false;
            }

        }
//        delete manyResources;
//        manyResources = NULL;
    }
}

void findResource(string resourceType, string host)
{
    foundResourceList.clear();
    std::ostringstream requestURI;

    try
    {
        // makes it so that all boolean values are printed as 'true/false' in this stream
        std::cout.setf(std::ios::boolalpha);
        // Find all resources
        requestURI << OC_RSRVD_WELL_KNOWN_URI << "?rt=" << resourceType;

        OCPlatform::findResource(host, requestURI.str(), OCConnectivityType::CT_DEFAULT,
                &onResourceFound, qos);
        cout << "Finding Resource...." << endl;

    }
    catch (OCException &e)
    {
        oclog() << "Exception in main: " << e.what();
    }
    waitForCallback();
}

void findCollection(string collectionType)
{
    foundCollectionList.clear();
    ostringstream requestURI;

    try
    {
        // makes it so that all boolean values are printed as 'true/false' in this stream
        std::cout.setf(std::ios::boolalpha);
        // Find all colletion
        requestURI << OC_RSRVD_WELL_KNOWN_URI << "?rt=" << collectionType;

        OCPlatform::findResource("", requestURI.str(), OCConnectivityType::CT_DEFAULT,
                &onCollectionFound, qos);
        cout << "Finding Collection...." << endl;

    }
    catch (OCException &e)
    {
        oclog() << "Exception when finding collection: " << e.what();
    }
    waitForCallback();
}

void findAllResources(string host)
{
    foundResourceList.clear();
    std::ostringstream requestURI;

    try
    {
        // makes it so that all boolean values are printed as 'true/false' in this stream
        std::cout.setf(std::ios::boolalpha);
        // Find all resources
        requestURI << OC_RSRVD_WELL_KNOWN_URI;

        OCPlatform::findResource(host, requestURI.str(), OCConnectivityType::CT_DEFAULT,
                &onResourceFound, qos);
        cout << "Finding Resource...." << endl;

    }
    catch (OCException &e)
    {
        oclog() << "Exception in main: " << e.what();
    }
    waitForCallback();
}

void discoverDevice(bool isMulticast)
{

    string host = "";
    ostringstream deviceDiscoveryRequest;
    string deviceDiscoveryURI = "/oic/d";
    OCStackResult result = OC_STACK_ERROR;

    if (isMulticast)
    {
        deviceDiscoveryRequest << OC_MULTICAST_PREFIX << deviceDiscoveryURI;
        cout << "Discovering Device using Multicast... " << endl;
    }
    else
    {
        if (foundResourceList.size() == 0)
        {
            cout << "Finding OIC Servers to send Unicast Discovery Request" << endl;
            findAllResources();
        }

        hasCallbackArrived = false;
        if (foundResourceList.size() > 0)
        {
            host = foundResourceList.at(0)->host();
            deviceDiscoveryRequest << deviceDiscoveryURI;
            cout << "Discovering Device using Unicast... " << endl;
            cout << "Sending Unicast device discovery to the host: " << host << endl;
        }
        else
        {
            cout << "No server found to send Unicast device discovery" << endl;
            cout << "Sending Multicast Device discovery request..." << endl;
            deviceDiscoveryRequest << OC_MULTICAST_PREFIX << deviceDiscoveryURI;
        }

    }

    try
    {
        result = OCPlatform::getDeviceInfo(host, deviceDiscoveryRequest.str(),
                OCConnectivityType::CT_DEFAULT, &onDeviceInfoReceived);

        if (result == OC_STACK_OK)
        {
            cout << "Device discovery done successfully" << endl;
            waitForCallback();
        }
        else
        {
            cout << "Device discovery failed." << endl;
        }

    }
    catch (OCException& e)
    {
        cerr << "Exception occurred while discovering device, reason is: " << e.reason() << endl;
    }

}
void discoverPlatform()
{
    if (foundResourceList.size() == 0)
    {
        findAllResources();
        hasCallbackArrived = false;
    }

    string host = foundResourceList.at(0)->host();
    ostringstream platformDiscoveryRequest;

    std::string platformDiscoveryURI = "/oic/p";
    try
    {
//            platformDiscoveryRequest << OC_MULTICAST_PREFIX << platformDiscoveryURI;
        platformDiscoveryRequest << platformDiscoveryURI;

        OCStackResult ret;

        cout << "Querying for platform information... ";

        ret = OCPlatform::getPlatformInfo(host, platformDiscoveryRequest.str(),
                OCConnectivityType::CT_DEFAULT, &onPlatformInfoReceived);

        if (ret == OC_STACK_OK)
        {
            cout << "done." << endl;
            waitForCallback();
        }
        else
        {
            cout << "failed." << endl;
        }

    }
    catch (OCException& e)
    {
        cerr << "Failure in main thread: " << e.reason() << endl;
    }
}

void sendGetRequest()
{
    int selection = selectResource();
    if (selection != -1)
    {
        foundResourceList.at(selection)->get(QueryParamsMap(), onGet, qos);
        cout << "GET request sent!!" << endl;
        waitForCallback();

    }
    else
    {
        cout << "No resource to send GET!!" << endl;
    }
}

void sendPutRequestUpdate()
{
    int selection = selectResource();
    if (selection != -1)
    {
        OCRepresentation rep;

        std::cout << "Sending Create Resource Message(PUT)..." << std::endl;

        string key = "region";
        string value = "Rajshahi, Bangladesh";
        rep.setValue(key, value);
        key = "power";
        value = "on";
        rep.setValue(key, value);
        key = "intensity";
        value = "50";
        rep.setValue(key, value);
        key = "manufacturer";
        value = "SRBD";
        rep.setValue(key, value);

// Invoke resource's put API with rep, query map and the callback parameter
        foundResourceList.at(selection)->put(rep, QueryParamsMap(), &onPut, qos);
        cout << "PUT request sent!!" << endl;
        waitForCallback();

    }
    else
    {
        cout << "No resource to send PUT!!" << endl;
    }
}

void sendPutRequestCreate()
{
    int selection = selectResource();
    if (selection != -1)
    {
        OCRepresentation rep;

        std::cout << "Sending Complete Update Message(PUT)..." << std::endl;

        vector< string > resourceTypes;
        string key = "href";
        string value = "/new-device/created-light-1";
        rep.setUri(value);
        value = "core.light";
        resourceTypes.push_back(value);
        rep.setResourceTypes(resourceTypes);
        key = "power";
        value = "off";
        rep.setValue(key, value);
        key = "intensity";
        value = "0";
        rep.setValue(key, value);

        // Invoke resource's post API with rep, query map and the callback parameter
        foundResourceList.at(selection)->put(rep, QueryParamsMap(), &onPut, qos);
        cout << "PUT request sent!!" << endl;
        waitForCallback();

    }
    else
    {
        cout << "No resource to send PUT!!" << endl;
    }
}

void sendPostRequestUpdate()
{
    int selection = selectResource();
    if (selection != -1)
    {
        OCRepresentation rep;

        std::cout << "Sending Partial Update Message(POST)..." << std::endl;

        string key = REGION_KEY;
        string value = "Rajshahi, Bangladesh";
        rep.setValue(key, value);

        key = INTENSITY_KEY;
        int intensityValue = 100;
        rep.setValue(key, intensityValue);

        // Invoke resource's put API with rep, query map and the callback parameter
        foundResourceList.at(selection)->post(rep, QueryParamsMap(), &onPost, qos);
        cout << "POST request sent!!" << endl;
        waitForCallback();

    }
    else
    {
        cout << "No resource to send POST!!" << endl;
    }
}

void sendPostRequestCreate()
{
    int selection = selectResource();
    if (selection != -1)
    {
        OCRepresentation rep;

        std::cout << "Sending Subordinate Create Message(POST)..." << std::endl;

        vector< string > resourceTypes;
        string key = "href";
        string value = "/subordinate-device/subordinate-light-1";
        rep.setUri(value);
        value = "core.light";
        resourceTypes.push_back(value);
        rep.setResourceTypes(resourceTypes);
        key = "power";
        value = "off";
        rep.setValue(key, value);
        key = "intensity";
        value = "0";
        rep.setValue(key, value);

        // Invoke resource's post API with rep, query map and the callback parameter
        foundResourceList.at(selection)->post(rep, QueryParamsMap(), &onPost, qos);
        cout << "POST request sent!!" << endl;
        waitForCallback();

    }
    else
    {
        cout << "No resource to send POST!!" << endl;
    }
}

void sendDeleteRequest()
{
    int selection = selectResource();
    if (selection != -1)
    {
        OCRepresentation rep;

        std::cout << "Putting region representation..." << std::endl;

        // Invoke resource's delete API with the callback parameter
        shared_ptr< OCResource > resource = foundResourceList.at(selection);
        resource->deleteResource(&onDelete);
        cout << "DELETE request sent!!" << endl;
        waitForCallback();
        if (resource->uri().compare("/device/fan-1") == 0)
        {
            foundResourceList.clear();
            createdResourceList.clear();
        }
    }
    else
    {
        cout << "No resource to send DELETE!!" << endl;
    }
}

void observeResource()
{
    int selection = selectResource();
    if (selection != -1)
    {
        OCRepresentation rep;

        std::cout << "Observing resource..." << std::endl;

        shared_ptr< OCResource > resource = foundResourceList.at(selection);
        resource->observe(ObserveType::Observe, QueryParamsMap(), &onObserve, qos);
        cout << "Observe request sent!!" << endl;
        isObservingResource = true;
        waitForCallback();

    }
    else
    {
        cout << "No resource to Observe!!" << endl;
    }
}

void cancelObserveResource()
{
    int selection = selectResource();
    if (selection != -1)
    {
        if (isObservingResource)
        {
            OCRepresentation rep;

            std::cout << "Canceling Observe resource..." << std::endl;

            shared_ptr< OCResource > resource = foundResourceList.at(selection);
            resource->cancelObserve(qos);
            cout << "Cancel Observe request sent!!" << endl;
            isObservingResource = false;
//            waitForCallback();
        }
        else
        {
            cout << "No resource is being Observed currently!!" << endl;
        }
    }
    else
    {
        cout << "No resource to cancel Observe!!" << endl;
    }
}

vector< OCRepresentation > createLinkRepresentation()
{
    vector< OCRepresentation > links;
    OCRepresentation linkRep;
    OCRepresentation pRep;

    try
    {
        OCPlatform::registerResource(childHandle, foundResourceList.at(0));
    }
    catch (exception& e)
    {
        cout << "Exception while registering remote resource, exception is : " << e.what() << endl;
    }

    pRep["bm"] = 3;
    linkRep["href"] = (string) LIGHT_1_URI;
    linkRep["rt"] = (string) RESOURCE_TYPE_LIGHT;
    linkRep["if"] = (string) DEFAULT_INTERFACE;
    linkRep["p"] = pRep;
    linkRep["ins"] = (string) "123";
    conformanceHelper->printRepresentation(linkRep);
    links.push_back(linkRep);

    return links;

}

void cancelObservePassively()
{
    int selection = selectResource();
    if (selection != -1)
    {
        if (isObservingResource)
        {
            OCRepresentation rep;

            std::cout << "Canceling Observe passively..." << std::endl;

            // Currently, there is no api to cancel observe passively
            shared_ptr< OCResource > resource = foundResourceList.at(selection);
//            resource->cancelObserve(qos);
//            cout << "Cancel Observe request sent!!" << endl;
//            isObservingResource = false;
            cout << "Cancel Observe request not sent!! Currently there is no API!!" << endl;
        }
        else
        {
            cout << "No resource is being Observed currently!!" << endl;
        }
    }
    else
    {
        cout << "No resource to cancel Observe!!" << endl;
    }
}

string getHost()
{
    string host = "";
    string ip = "";
    string port = "";

    int selection = selectResource();
    if (selection != -1)
    {
        host = foundResourceList.at(selection)->host();
    }
    else
    {
        cout << "Please enter the IP of the Resource host, then press Enter: ";
        cin >> ip;
        cout << "Please enter the IP of the Resource host, then press Enter: ";
        cin >> port;

        host = ip + ":" + port;
    }

    return host;
}

int selectResource()
{
    int selection = -1;
    int totalResource = foundResourceList.size();
    if (totalResource > 0)
    {
        cout << "\t" << "Please select your desired resource no. to send request and press Enter:"
                << endl;

        for (int i = 1; i <= totalResource; i++)
        {
            cout << "\t\t" << i << ". " << foundResourceList.at(i - 1)->uniqueIdentifier() << endl;
        }

        cin >> selection;

        while (selection < 1 || selection > totalResource)
        {
            cout << "Invalid selection of resource. Please select a resource no. between 1 & "
                    << totalResource << endl;
            cin >> selection;
        }
        selection--;
    }

    return selection;
}

void showMenu()
{
    cout << "Please Select an option from the menu and press Enter" << endl;
    cout << "\t Server Operations:" << endl;
    cout << "\t\t 1. Create Normal Resource" << endl;
    cout << "\t\t 2. Create Invisible Resource" << endl;
    cout << "\t\t 3. Create Resource With Complete URL" << endl;
    cout << "\t\t 4. Create Secured Resource" << endl;
    cout << "\t\t 5. Create 100 Light Resources" << endl;
    cout << "\t\t 6. Create Group Resource" << endl;
    cout << "\t\t 7. Delete All Resources" << endl;
    cout << "\t\t 8. Delete Created Group" << endl;
    cout << "\t Client Operations:" << endl;
    cout << "\t\t 10. Find core.light Type Resource" << endl;
    cout << "\t\t 11. Find Specific Type Of Resource" << endl;
    cout << "\t\t 12. Find All Resources" << endl;
    cout << "\t\t 13. Find core.light Type Resource - Unicast" << endl;
    cout << "\t\t 14. Find Specific Type Of Resource - Unicast" << endl;
    cout << "\t\t 15. Find All Resources - Unicast" << endl;
    cout << "\t\t 16. Join Found Resource To The Group" << endl;
    cout << "\t\t 17. Send GET Request" << endl;
    cout << "\t\t 18. Send PUT Request - Create Resource" << endl;
    cout << "\t\t 19. Send PUT Request - Complete Update" << endl;
    cout << "\t\t 20. Send POST Request - Partial Update" << endl;
    cout << "\t\t 21. Send POST Request - Create Sub-Ordinate Resource" << endl;
    cout << "\t\t 22. Send Delete Request" << endl;
    cout << "\t\t 23. Observe Resource - Retrieve Request with Observe" << endl;
    cout << "\t\t 24. Cancel Observing Resource" << endl;
    cout << "\t\t 25. Cancel Observing Resource Passively" << endl;
    cout << "\t\t 26. Discover Device - Unicast" << endl;
    cout << "\t\t 27. Discover Device - Multicast" << endl;
    cout << "\t\t 28. Discover Platform - Unicast" << endl;
    cout << "\t\t 29. Find Group" << endl;
    cout << "\t\t 30. Join Found Resource To Found Group" << endl;
    cout << "\t\t 31. Quit Conformance Simulator App" << endl;
    hasCallbackArrived = false;
    handleMenu();
}

void handleMenu()
{
    string userInput;
    string userResourceType;
    string resourceHost = "";
    string collectionType = "";
    AttributeValue attrVal;
    OCRepresentation linkRep;
    bool isMulticast = false;
    cin >> userInput;

    long int choice = strtol(userInput.c_str(), NULL, 10);
    if (choice > 31 || choice < 1 || (choice > 8 && choice < 10))
    {
        cout << "Invalid Input. Please input your choice again" << endl;
    }
    else
    {
        switch (choice)
        {
            case 1:
                createResource();
                break;

            case 2:
                createInvisibleResource();
                break;

            case 3:
                createResourceWithUrl();
                break;

            case 4:
                createSecuredResource();
                break;

            case 5:
                createManyLightResources();
                break;

            case 6:
                createGroup(GROUP_TYPE_ROOM);
                break;

            case 7:
                deleteResource();
                break;

            case 8:
                deleteGroup();
                break;

            case 10:
                findResource(RESOURCE_TYPE_LIGHT);
                if (foundResourceList.size() == 0)
                {
                    cout << "No resource found!!" << endl;
                }
                break;

            case 11:
                cout << "Please type the Resource Type to find, then press Enter: ";
                cin >> userResourceType;
                findResource(userResourceType);
                if (foundResourceList.size() == 0)
                {
                    cout << "No resource found!!" << endl;
                }
                break;

            case 12:
                findAllResources();
                if (foundResourceList.size() == 0)
                {
                    cout << "No resource found!!" << endl;
                }
                break;

            case 13:
                resourceHost = getHost();
                findResource(RESOURCE_TYPE_LIGHT, resourceHost);
                if (foundResourceList.size() == 0)
                {
                    cout << "No resource found!!" << endl;
                }
                break;

            case 14:
                resourceHost = getHost();
                cout << "Please type the Resource Type to find, then press Enter: ";
                cin >> userResourceType;
                findResource(userResourceType, resourceHost);
                if (foundResourceList.size() == 0)
                {
                    cout << "No resource found!!" << endl;
                }
                break;

            case 15:
                resourceHost = getHost();
                findAllResources(resourceHost);
                if (foundResourceList.size() == 0)
                {
                    cout << "No resource found!!" << endl;
                }
                break;

            case 16:
                joinGroup();
                break;

            case 17:
                sendGetRequest();
                break;

            case 18:
                sendPutRequestCreate();
                break;

            case 19:
                sendPutRequestUpdate();
                break;

            case 20:
                sendPostRequestUpdate();
                break;

            case 21:
                sendPostRequestCreate();
                break;

            case 22:
                sendDeleteRequest();
                break;

            case 23:
                observeResource();
                break;

            case 24:
                cancelObserveResource();
                break;

            case 25:
                cancelObservePassively();
                break;

            case 26:
                isMulticast = false;
                discoverDevice(isMulticast);
                break;

            case 27:
                isMulticast = true;
                discoverDevice(isMulticast);
                break;

            case 28:
                discoverPlatform();
                break;

            case 29:
                collectionType = GROUP_TYPE_ROOM;
                findCollection(collectionType);
                if (foundCollectionList.size() == 0)
                {
                    cout << "No collection found!!" << endl;
                }
                break;

            case 30:
                collectionType = GROUP_TYPE_ROOM;
                if (foundCollectionList.size() == 0)
                {
                    cout << "No Group available, starting find Group." << endl;
                    findCollection(collectionType);
                    if (foundCollectionList.size() == 0)
                    {
                        cout << "Still no Resource found, quitting join resource to group." << endl;
                        break;
                    }
                    else
                    {
                        hasCallbackArrived = false;
                    }
                }

                foundResourceList.at(0)->get(QueryParamsMap(), onGet);
                waitForCallback();

                if (resourceRepresentation.hasAttribute("links"))
                {
                    cout << "link is available." << endl;
                    resourceRepresentation.getAttributeValue("links", attrVal);
                    linkRep["links"] = attrVal;
                }
                else
                {
                    cout << "link is not available. creating link..." << endl;
//                	linkRep["links"] = createLinkRepresentation();
//                	conformanceHelper->printRepresentation(linkRep);
//                	cout << linkRep << endl;
                    linkRep.setValue("links",
                            (string) "[{\"href\":\"/device/light-1\",\"rt\":\"core.light\",\"if\":\"oic.if.baseline\",\"ins\":\"123\",\"p\":{\"bm\":3}}]");
                }

                foundResourceList.at(0)->post(collectionType, LINK_INTERFACE, linkRep,
                        QueryParamsMap(), onPost, qos);

                break;

            case 31:
                if (createdResourceList.size() > 0 || createdInvisibleResourceList.size() > 0
                        || isSecuredResourceCreated)
                {
                    deleteResource();
                }
                if (isGroupCreated)
                {
                    deleteGroup();
                }
                if (childHandle != NULL)
                {
                    OCPlatform::unregisterResource(childHandle);
                }
                createdResourceList.clear();
                foundResourceList.clear();
                return;
                break;
        }
    }
    showMenu();
}
