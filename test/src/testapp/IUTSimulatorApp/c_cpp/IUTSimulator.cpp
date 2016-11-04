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

#include <vector>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#ifdef __LINUX__
#include <execinfo.h>
#endif

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "SampleResource.h"
#include "ResourceHelper.h"
#include "OCPlatform.h"
#include "OCApi.h"
#include "pinoxmcommon.h"

using namespace OC;
using namespace std;

vector< SampleResource * > g_createdResourceList;
vector< shared_ptr< OCResource > > g_foundResourceList;
vector< shared_ptr< OCResource > > g_foundCollectionList;
vector< OCResourceHandle > g_handleList;
SampleResource *g_createdLightResource;
SampleResource *g_createdFanResource;
SampleResource *g_securedLightResource;
SampleResource *g_securedFanResource;
SampleResource *g_invisibleLightResource;
SampleResource *g_invisibleFanResource;
OCResourceHandle g_childHandle = NULL;
OCResourceHandle g_collectionHandle = NULL;
SampleResource* g_manyResources[MAX_LIGHT_RESOURCE_COUNT];
OCRepresentation g_resourceRepresentation;

SampleResource *g_tvDevice;
SampleResource *g_acDevice;
SampleResource *g_tvSwitchResource;
SampleResource *g_tvAudioResource;
SampleResource *g_tvMediaSourceListResource;
SampleResource *g_acSwitchResource;
SampleResource *g_acAirFlowResource;
SampleResource *g_acTemperatureResource;
SampleResource *g_acTimerResource;

bool g_hasCallbackArrived = false;
bool g_isObservingResource = false;
bool g_isGroupCreated = false;
bool g_isLightFanResourceCreated = false;
bool g_isSecuredResourceCreated = false;
bool g_isManyLightCreated = false;
bool g_isInvisibleResourceCreated = false;
bool g_isTvDeviceCreated = false;
bool g_isAirConDeviceCreated = false;

QualityOfService g_qos = QualityOfService::LowQos;
ResourceHelper *g_resourceHelper;

static mutex s_mutex;
static const char CRED_FILE_SERVER[] = "oic_svr_db_server.dat";
static const char CRED_FILE_CLIENT[] = "oic_svr_db_client.dat";
static const int CLIENT_MODE = 1;
static const int SERVER_MODE = 2;
static const char sourceFileNames[][100] = { "../../oic_svr_db_server_justworks.dat", "../../oic_svr_db_server_randompin.dat" };

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

void handleMenu(int argc, char* argv[]);
void showMenu(int argc, char* argv[]);
void createTvDevice(bool isSecured = false);
void createAirConDevice(bool isSecured = false);
void createResource(void);
void createSecuredResource(void);
void createInvisibleResource(void);
void createResourceWithUrl(void);
void createManyLightResources(void);
void deleteResource(void);
void findCollection(string);
void findResource(string resourceType, string host = "");
void findAllResources(string host = "", string query = "");
void discoverDevice(bool);
void discoverPlatform(bool isMulticast = true);
void sendGetRequest();
void sendGetRequestWithQuery(string, string);
void sendPutRequestUpdate(void);
void sendPostRequestUpdate(void);
void sendPostRequestUpdateUserInput(void);
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
bool joinGroup(OCResourceHandle, OCResourceHandle);
void deleteGroup(void);
int selectResource(void);
int selectLocalResource(void);
AttributeValue getAttributeValueFromUser(void);
void updateGroup(void);
void updateLocalResource(void);
void sendSpecialPost(void);
vector< OCRepresentation > createLinkRepresentation();
string getHost();
FILE* server_fopen(const char*, const char*);
FILE* client_fopen(const char*, const char*);

void handler(int sig)
{
    void *array[10];
    size_t size;

#ifdef __LINUX__
    // get void*'s for all entries on the stack
    size = backtrace(array, 10);
#endif

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);

#ifdef __LINUX__    
    backtrace_symbols_fd(array, size, STDERR_FILENO);
#endif

    exit(1);
}

void replaceDatFile(int modeType, int securityType)
{
    int num;
    char buffer[1000];

    FILE *source;

    source = fopen(sourceFileNames[securityType], "rb");
    if(source == NULL)
    {
      cout << "Unable to read source file " << endl;
      exit(-1);
    }

    if (modeType == CLIENT_MODE)
    {
        FILE *clientFile = fopen(CRED_FILE_CLIENT, "wb");
        if(clientFile == NULL)
        {
            fclose(source);
            cout << "Unable to write client dat file " << endl;
            exit(-1);
        }

        while ((num = fread(buffer, sizeof(char), sizeof(buffer), source)) != 0)
        {
            fwrite(buffer, sizeof(char), num, clientFile);
        }

        cout << "Dat File copied successfully" << endl;
        fclose(clientFile);
    }
    else if(modeType == SERVER_MODE)
    {
        FILE *serverFile = fopen(CRED_FILE_SERVER, "wb");
        if(serverFile == NULL)
        {
            fclose(source);
            cout << "Unable to write server dat file " << endl;
            exit(-1);
        }

        while ((num = fread(buffer, sizeof(char), sizeof(buffer), source)) != 0)
        {
            fwrite(buffer, sizeof(char), num, serverFile);
        }

        cout << "Dat File copied successfully" << endl;

        fclose(serverFile);
    }

    fclose(source);
}

void generatePinCB(char* pin, size_t pinSize)
{
    if(NULL == pin || pinSize <= 0)
    {
        cout << "Invalid PIN" << endl;
    }
    else
    {
        cout << "============================" << endl;
        cout << "    PIN CODE : " << string(pin) << endl;
        cout << "============================" << endl;
    }
}

int main(int argc, char* argv[])
{
    signal(SIGSEGV, handler);
    g_createdResourceList.clear();
    g_foundResourceList.clear();
    g_handleList.clear();
    g_resourceHelper = ResourceHelper::getInstance();

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
                g_qos = QualityOfService::HighQos;
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

            if (optionSelected == 0 || optionSelected == 1)
            {
                cout << "Using secured client...." << endl;
                isSecuredClient = true;
                SetGeneratePinCB(generatePinCB);
                replaceDatFile(CLIENT_MODE, optionSelected%2);
            }
            else if (optionSelected == 2 || optionSelected == 3)
            {
                cout << "Using secured server...." << endl;
                isSecuredServer = true;
                SetGeneratePinCB(generatePinCB);
                replaceDatFile(SERVER_MODE, optionSelected%2);
            }
            else
            {
                cout << "Using unsecured server & client...." << endl;
            }

            if (optionSelected >= 0 && optionSelected <= 3)
            {
                if(optionSelected%2)
                {
                    cout << "Supported Security Mode: randompin" << endl;
                }
                else
                {
                    cout << "Supported Security Mode: justworks" << endl;
                }
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
        { OC::ServiceType::InProc, OC::ModeType::Both, ipVer, ipVer, g_qos, &ps };
        result = SampleResource::constructServer(cfg);
    }
    else if (isSecuredServer)
    {
        OCPersistentStorage ps =
        { server_fopen, fread, fwrite, fclose, unlink };
        PlatformConfig cfg
        { OC::ServiceType::InProc, OC::ModeType::Server, ipVer, ipVer, g_qos, &ps };
        result = SampleResource::constructServer(cfg);
    }
    else
    {
        PlatformConfig cfg
        { OC::ServiceType::InProc, OC::ModeType::Both, ipVer, ipVer, g_qos };
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
    showMenu(argc, argv);

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
        g_foundResourceList.push_back(resource);
        cout << "uri of the found resource is " << resource->uri() << endl;
        g_hasCallbackArrived = true;

        cout << "Host of found resource: ";
        cout << resource->host() << endl;
        cout << "di( OCResource.sid() ) of found resource is = ";
        cout << resource->sid() << endl;
        cout << "unique identifier of found resource is = ";
        cout << resource->uniqueIdentifier() << endl;
    }
    else
    {
        cout << "Found resource is invalid!!" << endl;
    }

    s_mutex.unlock();
}

void onCollectionFound(shared_ptr< OCResource > collection)
{
    cout << "Found a Collection!!" << endl;

    if (collection)
    {
        g_foundCollectionList.push_back(collection);
        cout << "uri of the found collection is " << collection->uri() << endl;
        g_hasCallbackArrived = true;

        cout << "Host of found resource: " << collection->host() << endl;
        cout << "sid of found resource is = " << collection->sid() << endl;
        cout << "unique identifier of found resource is = " << collection->uniqueIdentifier()
                << endl;
        collection->get("", DEFAULT_INTERFACE, QueryParamsMap(), onGet);
        collection->get("", BATCH_INTERFACE, QueryParamsMap(), onGet);
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
    g_resourceHelper->printRepresentation(rep);
    g_hasCallbackArrived = true;
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

    g_resourceHelper->printRepresentation(rep);
    g_hasCallbackArrived = true;
}

// callback handler on GET request
void onGet(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    if (eCode == SUCCESS_RESPONSE)
    {
        cout << "Response: GET request was successful" << endl;

        vector< string > interfacelist = rep.getResourceInterfaces();

        bool isCollection = false;
        for (auto interface = interfacelist.begin(); interface != interfacelist.end(); interface++)
        {
            if (((*interface).compare(GROUP_INTERFACE) == 0)
                    || ((*interface).compare(BATCH_INTERFACE) == 0))
            {
                isCollection = true;
                break;
            }
        }

        if (isCollection)
        {
            vector< OCRepresentation > children = rep.getChildren();

            cout << "\nCHILD RESOURCE OF GROUP" << endl;
            for (auto iter = children.begin(); iter != children.end(); ++iter)
            {
                cout << "\tURI :: " << (*iter).getUri() << endl;
            }
            cout << endl;
        }
        else
        {
            g_resourceRepresentation = rep;
            cout << "THe GET Response has the following representation:" << endl;
            g_resourceHelper->printRepresentation(rep);
        }
    }
    else
    {
        cout << "onGET Response error: " << eCode << endl;
    }
    g_hasCallbackArrived = true;
}

// callback handler on PUT request
void onPut(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    if (eCode == SUCCESS_RESPONSE || eCode == OC_STACK_OK || eCode == OC_STACK_RESOURCE_CREATED)
    {
        cout << "Response: PUT request was successful" << endl;
        cout << "THe PUT response has the following representation:" << endl;
        g_resourceHelper->printRepresentation(rep);
    }
    else
    {
        cout << "onPUT Response error: " << eCode << endl;
    }
    g_hasCallbackArrived = true;
}

// callback handler on POST request
void onPost(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    if (eCode == SUCCESS_RESPONSE || eCode == OC_STACK_OK || eCode == OC_STACK_RESOURCE_CREATED)
    {
        cout << "Response: POST request was successful" << endl;
        cout << "THe POST Response has the following representation:" << endl;
        g_resourceHelper->printRepresentation(rep);
    }
    else
    {
        cout << "onPOST Response error: " << eCode << endl;
    }
    g_hasCallbackArrived = true;

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
        cout << "onDELETE Response error: " << eCode << endl;
    }
    g_hasCallbackArrived = true;
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
            ResourceHelper::getInstance()->printRepresentation(rep);
        }
    }
    catch (std::exception &e)
    {
        std::cout << "Exception: " << e.what() << " in onObserve" << std::endl;
    }
    g_hasCallbackArrived = true;

}

void waitForCallback()
{
    int elapsedSecond = 0;
    while (g_hasCallbackArrived == false)
    {
        g_resourceHelper->waitInSecond(CALLBACK_WAIT_MIN);
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
    if (g_childHandle == NULL)
    {
        cout << "Creating Resource with complete URL" << endl;
        shared_ptr< OCResource > resource;
        if (g_foundResourceList.size() == 0)
        {
            findResource(RESOURCE_TYPE_LIGHT);
        }
        resource = g_foundResourceList.at(0);
        OCPlatform::registerResource(g_childHandle, resource);
    }
    else
    {
        cout << "Resource with complete URL already created!!" << endl;
    }
}

void createResource()
{

    cout << "createResource called!!" << endl;
    if (g_isLightFanResourceCreated == false)
    {
        g_createdLightResource = new SampleResource();
        g_createdLightResource->setResourceProperties(LIGHT_1_URI, RESOURCE_TYPE_LIGHT,
        ACTUATOR_INTERFACE);
        g_createdLightResource->setAsDiscoverableResource();
        g_createdLightResource->setAsObservableResource();
        OCStackResult result = g_createdLightResource->startResource();

        if (result == OC_STACK_OK)
        {
            cout << "Resource created successfully" << endl;
            g_createdResourceList.push_back(g_createdLightResource);
        }
        else
        {
            cout << "Unable to create light resource" << endl;
        }

        g_createdFanResource = new SampleResource();
        g_createdFanResource->setResourceProperties(FAN_1_URI, RESOURCE_TYPE_FAN,
                ACTUATOR_INTERFACE);
        g_createdFanResource->setAsDiscoverableResource();
        result = g_createdFanResource->startResource();

        if (result == OC_STACK_OK)
        {
            cout << "Resource created successfully" << endl;
            g_createdResourceList.push_back(g_createdFanResource);
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

void createTvDevice(bool isSecured)
{

    cout << "Creating TV Device Resources!!" << endl;
    OCStackResult result = OC_STACK_ERROR;
    uint8_t resourceProperty = OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE;
    if (isSecured)
    {
        resourceProperty = resourceProperty | OC_SECURE;
    }
    if (g_isTvDeviceCreated == false)
    {
        SampleResource::setDeviceInfo("Vendor Smart Home TV Device", Device_TYPE_TV);

        g_tvSwitchResource = new SampleResource();
        g_tvSwitchResource->setResourceProperties(TV_SWITCH_URI, SWITCH_RESOURCE_TYPE,
        SWITCH_RESOURCE_INTERFACE);
        OCRepresentation switchRep;
        string value = "";
        switchRep.setValue("value", true);
        g_tvSwitchResource->setResourceRepresentation(switchRep);

        result = g_tvSwitchResource->startResource(resourceProperty);

        if (result == OC_STACK_OK)
        {
            cout << "TV Binary Switch Resource created successfully" << endl;
            g_createdResourceList.push_back(g_tvSwitchResource);
            g_isTvDeviceCreated = true;
        }
        else
        {
            cout << "Unable to create TV Binary Switch resource" << endl;
        }

        g_tvMediaSourceListResource = new SampleResource();
        g_tvMediaSourceListResource->setResourceProperties(TV_MEDIA_SOURCE_LIST_URI,
        MEDIA_SOURCE_LIST_RESOURCE_TYPE,
        MEDIA_SOURCE_LIST_RESOURCE_INTERFACE);

        OCRepresentation mainRep;
        OCRepresentation rep1;
        OCRepresentation rep2;
        vector< OCRepresentation > list;
        value = "HDMI-CEC";
        rep1.setValue("sourceName", value);
        rep1.setValue("sourceNumber", 1);
        value = "videoPlusAudio";
        rep1.setValue("sourceType", value);
        rep1.setValue("status", true);
        value = "dualRCA";
        rep2.setValue("sourceName", value);
        rep2.setValue("sourceNumber", 1);
        value = "audioOnly";
        rep2.setValue("sourceType", value);
        rep2.setValue("status", false);
        list.push_back(rep1);
        list.push_back(rep2);
        mainRep.setValue("sources", list);
        g_tvMediaSourceListResource->setResourceRepresentation(mainRep);
        result = g_tvMediaSourceListResource->startResource(resourceProperty);
        g_tvMediaSourceListResource->setAsSlowResource();

        if (result == OC_STACK_OK)
        {
            cout << "TV Media Source Input Resource created successfully" << endl;
            g_createdResourceList.push_back(g_tvMediaSourceListResource);
            g_isTvDeviceCreated = true;
        }
        else
        {
            cout << "Unable to create TV Media Source Input resource" << endl;
        }

        g_tvAudioResource = new SampleResource();
        g_tvAudioResource->setResourceProperties(TV_AUDIO_URI, AUDIO_RESOURCE_TYPE,
        AUDIO_RESOURCE_INTERFACE);
        OCRepresentation audioRep;
        int volume = 10;
        audioRep.setValue("mute", true);
        audioRep.setValue("volume", volume);
        g_tvAudioResource->setResourceRepresentation(audioRep);

        result = g_tvAudioResource->startResource(resourceProperty);

        if (result == OC_STACK_OK)
        {
            cout << "TV Audio Resource created successfully" << endl;
            g_createdResourceList.push_back(g_tvAudioResource);
            g_isTvDeviceCreated = true;
        }
        else
        {
            cout << "Unable to create TV Audio resource" << endl;
        }
    }
    else
    {
        cout << "Already Smart Home TV Device Resources are  created!!" << endl;
    }
}

void createAirConDevice(bool isSecured)
{

    OCStackResult result = OC_STACK_ERROR;
    cout << "Creating AirCon Device Resources!!" << endl;
    uint8_t resourceProperty = OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE;
    if (isSecured)
    {
        resourceProperty = resourceProperty | OC_SECURE;
    }
    if (g_isAirConDeviceCreated == false)
    {
        SampleResource::setDeviceInfo("Vendor Smart Home AirCon Device", Device_TYPE_AC);

        g_acSwitchResource = new SampleResource();
        g_acSwitchResource->setResourceProperties(AC_SWITCH_URI, SWITCH_RESOURCE_TYPE,
        SWITCH_RESOURCE_INTERFACE);
        OCRepresentation switchRep;
        string value = "";
        vector< int > range;
        vector< double > rangeTemperature;
        switchRep.setValue("value", true);
        g_acSwitchResource->setResourceRepresentation(switchRep);

        result = g_acSwitchResource->startResource(resourceProperty);

        if (result == OC_STACK_OK)
        {
            cout << "AirConditioner Binary Switch Resource created successfully" << endl;
            g_createdResourceList.push_back(g_acSwitchResource);
            g_isAirConDeviceCreated = true;
        }
        else
        {
            cout << "Unable to create AirConditioner Binary Switch resource" << endl;
        }

        g_acTemperatureResource = new SampleResource();
        g_acTemperatureResource->setResourceProperties(AC_TEMPERATURE_URI,
        TEMPERATURE_RESOURCE_TYPE,
        TEMPERATURE_RESOURCE_INTERFACE);
        OCRepresentation temperatureRep;
        value = "C";
        temperatureRep.setValue("units", value);
        g_acTemperatureResource->setAsReadOnly("units");
        rangeTemperature.push_back(10.01);
        rangeTemperature.push_back(40.99);
        temperatureRep.setValue("range", rangeTemperature);
        g_acTemperatureResource->setAsReadOnly("range");
        double temperature = 24.50;
        temperatureRep.setValue("temperature", temperature);
        g_acTemperatureResource->setResourceRepresentation(temperatureRep);
        result = g_acTemperatureResource->startResource(resourceProperty);

        if (result == OC_STACK_OK)
        {
            cout << "Air Conditioner Temperature Resource created successfully" << endl;
            g_createdResourceList.push_back(g_acTemperatureResource);
            g_isAirConDeviceCreated = true;
        }
        else
        {
            cout << "Unable to create Air Conditioner Temperature resource" << endl;
        }

        g_acAirFlowResource = new SampleResource();
        setlocale(LC_ALL, "");
        g_acAirFlowResource->setResourceProperties(AC_AIR_FLOW_URI, AIR_FLOW_RESOURCE_TYPE,
        AIR_FLOW_RESOURCE_INTERFACE);

        OCRepresentation airFlowRep;
        int speed = 10;
        value = "up";
        airFlowRep.setValue("direction", value);
        airFlowRep.setValue("speed", speed);
        range.clear();
        range.push_back(0);
        range.push_back(30);
        airFlowRep.setValue("range", range);
        g_acAirFlowResource->setAsReadOnly("range");
        g_acAirFlowResource->setResourceRepresentation(airFlowRep);

        result = g_acAirFlowResource->startResource(resourceProperty);

        if (result == OC_STACK_OK)
        {
            cout << "Air Conditioner AirFlow Resource created successfully" << endl;
            g_createdResourceList.push_back(g_acAirFlowResource);
            g_isAirConDeviceCreated = true;
        }
        else
        {
            cout << "Unable to create Air Conditioner AirFlow resource" << endl;
        }

        resourceProperty = OC_ACTIVE | OC_DISCOVERABLE;
        if (isSecured)
        {
            resourceProperty = resourceProperty | OC_SECURE;
        }
        g_acTimerResource = new SampleResource();
        setlocale(LC_ALL, "");
        g_acTimerResource->setResourceProperties(AC_TIMER_URI, TIMER_RESOURCE_TYPE,
        TIMER_RESOURCE_INTERFACE);

        OCRepresentation clockRep;
        int time = 10;
        clockRep.setValue("x.com.vendor.timer.hour", time);
        time = 30;
        clockRep.setValue("x.com.vendor.timer.minute", time);
        clockRep.setValue("x.com.vendor.timer.second", time);
        clockRep.setValue("x.com.vendor.timer.reset", false);

        g_acTimerResource->setResourceRepresentation(clockRep);

        result = g_acTimerResource->startResource(resourceProperty);
        g_acTimerResource->setAsSlowResource();

        if (result == OC_STACK_OK)
        {
            cout << "Air Conditioner Timer Resource created successfully" << endl;
            g_createdResourceList.push_back(g_acTimerResource);
            g_isAirConDeviceCreated = true;
        }
        else
        {
            cout << "Unable to create Air Conditioner Timer resource" << endl;
        }
    }
    else
    {
        cout << "Already Smart Home AirCon Device Resources are  created!!" << endl;
    }
}

void createManyLightResources()
{
    cout << "createResource called!!" << endl;
    string baseUri = "/device/light-";
    int lightCount = 2;
    if (g_isManyLightCreated == false)
    {
        string uri = "";
        for (int i = 0; i < MAX_LIGHT_RESOURCE_COUNT; i++)
        {
            uri = baseUri + to_string(lightCount++);
            g_manyResources[i] = new SampleResource();
            g_manyResources[i]->setResourceProperties(uri, RESOURCE_TYPE_LIGHT,
            ACTUATOR_INTERFACE);
            g_manyResources[i]->setAsDiscoverableResource();
            g_manyResources[i]->setAsObservableResource();
            OCStackResult result = g_manyResources[i]->startResource();

            if (result == OC_STACK_OK)
            {
                cout << "Light Resource created successfully with uri: " << uri << endl;
                g_createdResourceList.push_back(g_manyResources[i]);
                g_isManyLightCreated = true;
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
    if (g_isSecuredResourceCreated == false)
    {

        g_securedLightResource = new SampleResource();
        g_securedLightResource->setResourceProperties(LIGHT_SECURED_URI, RESOURCE_TYPE_LIGHT,
        ACTUATOR_INTERFACE);
        uint8_t resourceProperty = OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE;
        OCStackResult result = g_securedLightResource->startResource(resourceProperty);

        if (result == OC_STACK_OK)
        {
            cout << "Secure Light Resource created successfully" << endl;
            g_isSecuredResourceCreated = true;
            g_createdResourceList.push_back(g_securedLightResource);
        }
        else
        {
            cout << "Unable to create secured light resource" << endl;
        }

        g_securedFanResource = new SampleResource();
        g_securedFanResource->setResourceProperties(FAN_SECURED_URI, RESOURCE_TYPE_FAN,
        ACTUATOR_INTERFACE);
        resourceProperty = OC_ACTIVE | OC_DISCOVERABLE | OC_SECURE;
        result = g_securedFanResource->startResource(resourceProperty);

        if (result == OC_STACK_OK)
        {
            cout << "Secured Fan Resource created successfully" << endl;
            g_isSecuredResourceCreated = true;
            g_createdResourceList.push_back(g_securedFanResource);
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
    if (g_isInvisibleResourceCreated == false)
    {
        g_invisibleFanResource = new SampleResource();
        g_invisibleFanResource->setResourceProperties(FAN_INVISIBLE_URI, RESOURCE_TYPE_FAN,
        ACTUATOR_INTERFACE);
        uint8_t resourceProperty = OC_ACTIVE;
        OCStackResult result = g_invisibleFanResource->startResource(resourceProperty);

        if (result == OC_STACK_OK)
        {
            cout << "Invisible fan resource created successfully" << endl;
            g_createdResourceList.push_back(g_invisibleFanResource);
            g_isInvisibleResourceCreated = true;
        }
        else
        {
            cout << "Unable to create resource" << endl;
        }

        string interface = string(ACTUATOR_INTERFACE) + " " + string(DEFAULT_INTERFACE);
        g_invisibleLightResource = new SampleResource();
        g_invisibleLightResource->setResourceProperties(LIGHT_INVISIBLE_URI, RESOURCE_TYPE_LIGHT,
                interface);
        resourceProperty = OC_ACTIVE | OC_OBSERVABLE | OC_EXPLICIT_DISCOVERABLE;
        result = g_invisibleLightResource->startResource(resourceProperty);

        if (result == OC_STACK_OK)
        {
            cout << "Invisible light resource created successfully" << endl;
            g_createdResourceList.push_back(g_invisibleLightResource);
            g_isInvisibleResourceCreated = true;
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

void handleResponse(std::shared_ptr< OCResourceRequest > request)
{
    auto pResponse = std::make_shared< OC::OCResourceResponse >();
    pResponse->setRequestHandle(request->getRequestHandle());
    pResponse->setResourceHandle(request->getResourceHandle());

    // Get the request type and request flag
    std::string requestType = request->getRequestType();
    RequestHandlerFlag requestFlag = (RequestHandlerFlag) request->getRequestHandlerFlag();

    if (requestFlag == RequestHandlerFlag::RequestFlag)
    {
        cout << "\t\trequestFlag : Request\n";

        // If the request type is GET
        if (requestType == "GET")
        {
            cout << "\t\t\trequestType : GET\n";

            // Check for query params (if any)
            QueryParamsMap queryParamsMap = request->getQueryParameters();

        }
        else if (requestType == "PUT")
        {
            cout << "\t\t\trequestType : PUT\n";

            OCRepresentation incomingRepresentation = request->getResourceRepresentation();

            // Check for query params (if any)
            QueryParamsMap queryParamsMap = request->getQueryParameters();

        }
        else if (requestType == "POST")
        {
            // POST request operations
            cout << "\t\t\trequestType : POST\n";

            OCRepresentation incomingRepresentation = request->getResourceRepresentation();

            // Check for query params (if any)
            QueryParamsMap queryParamsMap = request->getQueryParameters();

        }
        else if (requestType == "DELETE")
        {
            // DELETE request operations
            cout << "\t\t\trequestType : Delete\n";

            OCRepresentation incomingRepresentation = request->getResourceRepresentation();
            // Check for query params (if any)
            QueryParamsMap queryParamsMap = request->getQueryParameters();

        }
    }
    else if (requestFlag & RequestHandlerFlag::ObserverFlag)
    {
        // OBSERVE flag operations
        cout << "\t\t\trequestType : Observe\n";

        // Check for query params (if any)
        QueryParamsMap queryParamsMap = request->getQueryParameters();

    }
}

void createGroup(string groupType)
{
    if (g_isGroupCreated)
    {
        cout << "Group[ already created!!" << endl;
    }
    else
    {
        try
        {
            string resourceURI = "/core/a/collection";
            string resourceInterface = BATCH_INTERFACE;

            OCPlatform::registerResource(g_collectionHandle, resourceURI, groupType,
                    resourceInterface, NULL,
                    OC_DISCOVERABLE | OC_OBSERVABLE);

            cout << "Create Group is called." << endl;

            OCPlatform::bindInterfaceToResource(g_collectionHandle, GROUP_INTERFACE);
            OCPlatform::bindInterfaceToResource(g_collectionHandle, DEFAULT_INTERFACE);

            g_isGroupCreated = true;
            cout << "Successfully Created Group!!" << endl;
            if (g_createdResourceList.size() > 0)
            {
                for (SampleResource* resource : g_createdResourceList)
                {
                    joinGroup(g_collectionHandle, resource->getResourceHandle());
                }

            }
        }
        catch (OCException& e)
        {
            oclog() << "Exception in main: " << e.what();
            g_isGroupCreated = false;
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
        OCPlatform::registerResource(g_childHandle, child);
    }
    catch (exception& e)
    {
        cout << "Exception while registering remote resource, exception is : " << e.what() << endl;
    }

    if (g_isGroupCreated == false || g_collectionHandle == NULL)
    {
        cout << "Local group does not exist!! Please create Local group first!!" << endl;
        return;
    }

    if (g_childHandle)
    {
        g_handleList.push_back(g_childHandle);
        g_resourceHelper->waitInSecond(2);
        cout << "Joining resource with uri: " << child->uri() << " to local collection" << endl;
        try
        {
            expectedResult = OCPlatform::bindResource(g_collectionHandle, g_childHandle);
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

bool joinGroup(OCResourceHandle collectionHandle, OCResourceHandle childHandle)
{
    bool isJoined = false;

    try
    {
        OCStackResult expectedResult = OCPlatform::bindResource(collectionHandle, childHandle);
        if (expectedResult == OC_STACK_OK)
        {
            cout << "Joining to the group completed!!" << endl;
            isJoined = true;
        }
        else
        {
            cout << "Unable to join to group" << endl;
        }
    }
    catch (exception& e)
    {
        cout << "Exception occurred while trying to join to grop, exception is: " << e.what()
                << endl;
    }

    return isJoined;
}

void deleteGroup()
{
    if (g_isGroupCreated)
    {
        try
        {
            OCStackResult result = OCPlatform::unregisterResource(g_collectionHandle);
            if (result == OC_STACK_OK)
            {
                cout << "Successfully Deleted Group!!" << endl;
                g_isGroupCreated = false;
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

    while (g_handleList.size() > 0)
    {
        int index = g_handleList.size() - 1;
        OCPlatform::unregisterResource(g_handleList[index]);
        g_handleList.pop_back();
    }
}

void deleteResource()
{
    while (g_handleList.size() > 0)
    {
        int index = g_handleList.size() - 1;
        OCPlatform::unregisterResource(g_handleList[index]);
        g_handleList.pop_back();
    }

    OCStackResult result = OC_STACK_ERROR;
    if (g_createdResourceList.size() != 0)
    {

        for (auto resource : g_createdResourceList)
        {
            result = resource->stopResource();
            if (result == OC_STACK_OK)
            {
                cout << "Successfully stopped Resource with URI: " << resource->getUri() << endl;
                delete resource;
            }
        }
    }
    else
    {
        cout << "There is no resource available to delete!!" << endl;
    }

    g_createdResourceList.clear();
    g_isLightFanResourceCreated = false;
    g_isSecuredResourceCreated = false;
    g_isManyLightCreated = false;
    g_isInvisibleResourceCreated = false;
    g_isTvDeviceCreated = false;
    g_isAirConDeviceCreated = false;

}

void findResource(string resourceType, string host)
{
    g_foundResourceList.clear();
    std::ostringstream requestURI;

    try
    {
        // makes it so that all boolean values are printed as 'true/false' in this stream
        std::cout.setf(std::ios::boolalpha);
        // Find all resources
        requestURI << OC_RSRVD_WELL_KNOWN_URI << "?rt=" << resourceType;

        OCPlatform::findResource(host, requestURI.str(), OCConnectivityType::CT_DEFAULT,
                &onResourceFound, g_qos);
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
    g_foundCollectionList.clear();
    ostringstream requestURI;

    try
    {
        // makes it so that all boolean values are printed as 'true/false' in this stream
        std::cout.setf(std::ios::boolalpha);
        // Find all colletion
        requestURI << OC_RSRVD_WELL_KNOWN_URI << "?rt=" << collectionType;

        OCPlatform::findResource("", requestURI.str(), OCConnectivityType::CT_DEFAULT,
                &onCollectionFound, g_qos);
        cout << "Finding Collection...." << endl;

    }
    catch (OCException &e)
    {
        oclog() << "Exception when finding collection: " << e.what();
    }
    waitForCallback();
}

void findAllResources(string host, string query)
{
    g_foundResourceList.clear();
    std::ostringstream requestURI;

    try
    {
        // makes it so that all boolean values are printed as 'true/false' in this stream
        std::cout.setf(std::ios::boolalpha);
        // Find all resources
        if (query.compare("") == 0)
        {
            requestURI << OC_RSRVD_WELL_KNOWN_URI;
        }
        else
        {
            requestURI << OC_RSRVD_WELL_KNOWN_URI << "?" << query;
        }

        OCPlatform::findResource(host, requestURI.str(), OCConnectivityType::CT_DEFAULT,
                &onResourceFound, g_qos);
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
        if (g_foundResourceList.size() == 0)
        {
            cout << "Finding OIC Servers to send Unicast Discovery Request" << endl;
            findAllResources();
        }

        g_hasCallbackArrived = false;
        if (g_foundResourceList.size() > 0)
        {
            host = g_foundResourceList.at(0)->host();
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
void discoverPlatform(bool isMulticast)
{
    string host = "";
    ostringstream platformDiscoveryRequest;
    string platformDiscoveryURI = "/oic/p";

    if (isMulticast)
    {
        platformDiscoveryRequest << OC_MULTICAST_PREFIX << platformDiscoveryURI;
        cout << "Discovering Platform using Multicast... " << endl;

    }
    else
    {
        if (g_foundResourceList.size() == 0)
        {
            findAllResources();
            g_hasCallbackArrived = false;
        }

        host = g_foundResourceList.at(0)->host();
        platformDiscoveryRequest << platformDiscoveryURI;
        cout << "Discovering Platform using Unicast... " << endl;
    }

    try
    {
        OCStackResult ret = OC_STACK_ERROR;
        ret = OCPlatform::getPlatformInfo(host, platformDiscoveryRequest.str(),
                OCConnectivityType::CT_DEFAULT, &onPlatformInfoReceived);

        cout << "Platform discovery ";
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
        QueryParamsMap qpMap;
        shared_ptr< OCResource > targetResource = g_foundResourceList.at(selection);
        cout << "Sending Get Request to the resource with: " << targetResource->host()
                << targetResource->uri() << endl;
        targetResource->get(qpMap, onGet, g_qos);
        cout << "GET request sent!!" << endl;
        waitForCallback();

    }
    else
    {
        cout << "No resource to send GET!!" << endl;
    }
}

void sendGetRequestWithQuery(string key, string value)
{
    int selection = selectResource();
    if (selection != -1)
    {
        QueryParamsMap qpMap;
        qpMap[key] = value;
        g_foundResourceList.at(selection)->get(qpMap, onGet, g_qos);
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

        cout << "Sending Complete Update Message(PUT)..." << endl;

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
        QueryParamsMap query;
        g_foundResourceList.at(selection)->put(rep, query, &onPut, g_qos);
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

        cout << "Sending Create Resource Message(PUT)..." << endl;

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
        QueryParamsMap query;
        g_foundResourceList.at(selection)->put(rep, query, &onPut, g_qos);
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

        cout << "Sending Partial Update Message(POST)..." << endl;

        string key = REGION_KEY;
        string value = "Rajshahi, Bangladesh";
        rep.setValue(key, value);

        key = INTENSITY_KEY;
        int intensityValue = 100;
        rep.setValue(key, intensityValue);

        // Invoke resource's put API with rep, query map and the callback parameter
        g_foundResourceList.at(selection)->post(rep, QueryParamsMap(), &onPost, g_qos);
        cout << "POST request sent!!" << endl;
        waitForCallback();

    }
    else
    {
        cout << "No resource to send POST!!" << endl;
    }
}

void updateGroup()
{
    cout << "Please select only Collection resource" << endl;
    int selection = selectResource();
    if (selection != -1)
    {
        OCRepresentation rep;
        OCRepresentation childRep;
        vector< OCRepresentation > childrenRep;

        cout << "Sending Partial Update Message(POST) to Collection..." << endl;

        string key = REGION_KEY;
        string value = "allbulba";

        key = INTENSITY_KEY;
        int intensityValue = 100;
        childRep.setValue(key, intensityValue);
        value = "core.light";
        childRep.setValue("rt", value);
        value = ACTUATOR_INTERFACE;
        childRep.setValue("if", value);
        value = "/device/light-1";
        childRep.setValue("href", value);

        childrenRep.push_back(childRep);
        value = RESOURCE_TYPE_ROOM;
        rep.setValue("rt", value);
        value = "oic.if.baseline oic.if.b oic.mi.grp";
        rep.setValue("if", value);
        value = "/core/a/collection";
        rep.setValue("href", value);
        rep.setChildren(childrenRep);

        // Invoke resource's put API with rep, query map and the callback parameter
        g_foundResourceList.at(selection)->post(RESOURCE_TYPE_ROOM, GROUP_INTERFACE, rep,
                QueryParamsMap(), &onPost, g_qos);
        cout << "POST request sent!!" << endl;
        waitForCallback();

    }
}

AttributeValue getAttributeValueFromUser()
{
    AttributeValue value;
    string valueString = "";
    bool valueBool = false;
    int valueInt = 0;
    float valueFloat = 0.0;
    double valueDouble = 0.0;
    string valueArray = "";
    string userInput = "";
    char valueLine[MAX_ATTRIBUTE_VALUE_LENGTH];
    long int choice = 0;
    bool validChoice = false;

    do
    {
        cout << "Please select attribute data type and press Enter: " << endl;
        cout << "\t\t 1. Integer" << endl;
        cout << "\t\t 2. Floating Point - Single Precision" << endl;
        cout << "\t\t 3. Floating Point - Double Precision" << endl;
        cout << "\t\t 4. Boolean" << endl;
        cout << "\t\t 5. String" << endl;
        cout << "\t\t 6. Array" << endl;

        cin >> userInput;

        choice = strtol(userInput.c_str(), NULL, 10);
        if (choice > 0 && choice < 6)
        {
            validChoice = true;
        }
        else
        {
            validChoice = false;
            cout << "Invalid input for attribute data type. Please select between 1 and 5" << endl;
        }

    } while (!validChoice);

    cout << "Please input Attribute Value: ";
    switch (choice)
    {
        case 1:
            cin >> valueInt;
            value = valueInt;
            break;
        case 2:
            cin >> valueFloat;
            value = valueFloat;
            break;
        case 3:
            cin >> valueDouble;
            value = valueDouble;
            break;
        case 4:
            cout << "Please provide boolean value(O for False, 1 for True) : ";
            cin >> valueBool;
            value = valueBool;
            break;
        case 5:
            cin.getline(valueLine, sizeof(value));
            getline(cin, valueString);
            value = valueString;
            break;
        case 6:
            cin.getline(valueLine, sizeof(value));
            getline(cin, valueArray);
            value = valueArray;
            break;
    }

    return value;
}

void updateLocalResource()
{
    int selection = selectLocalResource();
    if (selection != -1)
    {
        string key = "";
        AttributeValue value;

        cout << "Please input Attribute Key: ";
        cin >> key;
        value = getAttributeValueFromUser();

        OCRepresentation rep = g_createdResourceList.at(selection)->getRepresentation();
        if (rep.hasAttribute(key))
        {
            rep.setValue(key, value);
            g_createdResourceList.at(selection)->setResourceRepresentation(rep);
            cout << "Successfully updated resource attribute!!" << endl;
            ResourceHelper::getInstance()->printRepresentation(rep);
            g_createdResourceList.at(selection)->notifyObservers(g_createdResourceList.at(selection));
        }
        else
        {
            cout << "The resource does not have the mentioned attribute" << endl;
        }

    }
    else
    {
        cout << "No resource to Update!!" << endl;
    }
}

void sendPostRequestUpdateUserInput()
{
    int selection = selectResource();
    if (selection != -1)
    {
        OCRepresentation rep;
        string key = "";
        AttributeValue value;

        cout << "Please input Attribute Key: ";
        cin >> key;
        value = getAttributeValueFromUser();
        rep.setValue(key, value);

        // Invoke resource's put API with rep, query map and the callback parameter
        cout << "Sending Partial Update Message(POST)..." << endl;
        QueryParamsMap query;
        g_foundResourceList.at(selection)->post(rep, query, &onPost, g_qos);
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

        cout << "Sending Subordinate Resource Create Message(POST)..." << endl;

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
        g_foundResourceList.at(selection)->post(rep, QueryParamsMap(), &onPost, g_qos);
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

        cout << "Sending Delete Request..." << endl;

        // Invoke resource's delete API with the callback parameter
        shared_ptr< OCResource > resource = g_foundResourceList.at(selection);
        resource->deleteResource(&onDelete);
        cout << "DELETE request sent!!" << endl;
        waitForCallback();
        if (resource->uri().compare("/device/fan-1") == 0)
        {
            g_foundResourceList.clear();
            g_createdResourceList.clear();
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

        cout << "Observing resource..." << endl;

        shared_ptr< OCResource > resource = g_foundResourceList.at(selection);
        resource->observe(ObserveType::Observe, QueryParamsMap(), &onObserve, g_qos);
        cout << "Observe request sent!!" << endl;
        g_isObservingResource = true;
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
        if (g_isObservingResource)
        {
            OCRepresentation rep;

            cout << "Canceling Observe resource..." << endl;

            shared_ptr< OCResource > resource = g_foundResourceList.at(selection);
            resource->cancelObserve(g_qos);
            cout << "Cancel Observe request sent!!" << endl;
            g_isObservingResource = false;
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
        OCPlatform::registerResource(g_childHandle, g_foundResourceList.at(0));
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
    g_resourceHelper->printRepresentation(linkRep);
    links.push_back(linkRep);

    return links;

}

void cancelObservePassively()
{
    int selection = selectResource();
    if (selection != -1)
    {
        if (g_isObservingResource)
        {
            OCRepresentation rep;

            cout << "Canceling Observe passively..." << endl;

            // Currently, there is no api to cancel observe passively
            shared_ptr< OCResource > resource = g_foundResourceList.at(selection);
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
        host = g_foundResourceList.at(selection)->host();
    }
    else
    {
        cout << "Please enter the IP of the Resource host, then press Enter: ";
        cin >> ip;
        cout << "Please enter the port of the Resource host, then press Enter: ";
        cin >> port;

        host = ip + ":" + port;
    }

    return host;
}

int selectResource()
{
    int selection = -1;
    int totalResource = g_foundResourceList.size();
    if (totalResource > 0)
    {
        cout << "\t" << "Please select your desired resource no. to send request and press Enter:"
                << endl;

        for (int i = 1; i <= totalResource; i++)
        {
            cout << "\t\t" << i << ". " << g_foundResourceList.at(i - 1)->uniqueIdentifier() << endl;
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

int selectLocalResource()
{
    int selection = -1;
    int totalResource = g_createdResourceList.size();
    if (totalResource > 0)
    {
        cout << "\t" << "Please select your desired resource no. to update attribute:" << endl;

        int resourceCount = 1;
        setlocale(LC_ALL, "");
        for (SampleResource* localResource : g_createdResourceList)
        {
            cout << "\t\t" << resourceCount++ << ". " << localResource->getUri() << endl;
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

void showMenu(int argc, char* argv[])
{
    cout << "Please Select an option from the menu and press Enter" << endl;
    cout << "\t\t 0. Quit IUT Simulator" << endl;
    cout << "\t Server Operations:" << endl;
    cout << "\t\t 1. Create Normal Resource" << endl;
    cout << "\t\t 2. Create Invisible Resource" << endl;
    cout << "\t\t 3. Create Resource With Complete URL" << endl;
    cout << "\t\t 4. Create Secured Resource" << endl;
    cout << "\t\t 5. Create " << MAX_LIGHT_RESOURCE_COUNT << " Light Resources" << endl;
    cout << "\t\t 6. Create Group Resource" << endl;
    cout << "\t\t 7. Delete All Resources" << endl;
    cout << "\t\t 8. Delete Created Group" << endl;
    cout << "\t Client Operations:" << endl;
    cout << "\t\t 10. Find Resource using Interface Query" << endl;
    cout << "\t\t 11. Find Specific Type Of Resource" << endl;
    cout << "\t\t 12. Find All Resources" << endl;
    cout << "\t\t 13. Find core.light Type Resource - Unicast" << endl;
    cout << "\t\t 14. Find Specific Type Of Resource - Unicast" << endl;
    cout << "\t\t 15. Find All Resources - Unicast" << endl;
    cout << "\t\t 16. Join Found Resource To The Group" << endl;
    cout << "\t\t 17. Send GET Request" << endl;
    cout << "\t\t 18. Send GET Request with query" << endl;
    cout << "\t\t 19. Send PUT Request - Create Resource" << endl;
    cout << "\t\t 20. Send PUT Request - Complete Update" << endl;
    cout << "\t\t 21. Send POST Request - Partial Update - Default" << endl;
    cout << "\t\t 22. Send POST Request - Partial Update - User Input" << endl;
    cout << "\t\t 23. Send POST Request - Create Sub-Ordinate Resource" << endl;
    cout << "\t\t 24. Send Delete Request" << endl;
    cout << "\t\t 25. Observe Resource - Retrieve Request with Observe" << endl;
    cout << "\t\t 26. Cancel Observing Resource" << endl;
    cout << "\t\t 27. Cancel Observing Resource Passively" << endl;
    cout << "\t\t 28. Discover Device - Unicast" << endl;
    cout << "\t\t 29. Discover Device - Multicast" << endl;
    cout << "\t\t 30. Discover Platform - Multicast" << endl;
    cout << "\t\t 32. Find Group" << endl;
    cout << "\t\t 32. Join Found Resource To Found Group" << endl;
    cout << "\t\t 33. Update Collection" << endl;
    cout << "\t\t 34. Update Local Resource" << endl;
    cout << "\t\t 35. Set Quality of Service - CON(Confirmable)" << endl;
    cout << "\t\t 36. Set Quality of Service - NON(Non-Confirmable)" << endl;
    cout << "\t Smart Home Vertical Resource Creation:" << endl;
    cout << "\t\t 101. Create Smart TV Device" << endl;
    cout << "\t\t 102. Create Air Conditioner Device" << endl;
    cout << "\t\t 103. Create Secured Smart TV Device" << endl;
    cout << "\t\t 104. Create Secured Air Conditioner Device" << endl;

    g_hasCallbackArrived = false;
    handleMenu(argc, argv);
}

void handleMenu(int argc, char* argv[])
{
    string userInput;
    string userResourceType;
    string userInterfaceType;
    string resourceHost = "";
    string collectionType = "";
    string queryKey = "";
    string queryValue = "";
    AttributeValue attrVal;
    OCRepresentation linkRep;
    bool isMulticast = false;

    if (argc > 4)
    {
        userInput = string(argv[4]);
    }
    else
    {
        cin >> userInput;
    }

    long int choice = strtol(userInput.c_str(), NULL, 10);
    if ((choice > 36 && choice < 101) || choice < 0 || (choice > 8 && choice < 10) || choice > 104)
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
                cout << "Please type query(key=value), then press Enter: ";
                cin >> userInterfaceType;
                resourceHost = "";
                findAllResources(resourceHost, userInterfaceType);
                if (g_foundResourceList.size() == 0)
                {
                    cout << "No resource found!!" << endl;
                }
                break;

            case 11:
                cout << "Please type the Resource Type to find, then press Enter: ";
                cin >> userResourceType;
                findResource(userResourceType);
                if (g_foundResourceList.size() == 0)
                {
                    cout << "No resource found!!" << endl;
                }
                break;

            case 12:
                findAllResources();
                if (g_foundResourceList.size() == 0)
                {
                    cout << "No resource found!!" << endl;
                }
                break;

            case 13:
                resourceHost = getHost();
                findResource(RESOURCE_TYPE_LIGHT, resourceHost);
                if (g_foundResourceList.size() == 0)
                {
                    cout << "No resource found!!" << endl;
                }
                break;

            case 14:
                resourceHost = getHost();
                cout << "Please type the Resource Type to find, then press Enter: ";
                cin >> userResourceType;
                findResource(userResourceType, resourceHost);
                if (g_foundResourceList.size() == 0)
                {
                    cout << "No resource found!!" << endl;
                }
                break;

            case 15:
                resourceHost = getHost();
                findAllResources(resourceHost);
                if (g_foundResourceList.size() == 0)
                {
                    cout << "No resource found!!" << endl;
                }
                break;

            case 16:
//                joinGroup();
                break;

            case 17:
                sendGetRequest();
                break;

            case 18:
                cout << "Please type query key, then press Enter: ";
                cin >> queryKey;
                cout << "Please type query value, then press Enter: ";
                cin >> queryValue;
                sendGetRequestWithQuery(queryKey, queryValue);
                break;

            case 19:
                sendPutRequestCreate();
                break;

            case 20:
                sendPutRequestUpdate();
                break;

            case 21:
                sendPostRequestUpdate();
                break;

            case 22:
                sendPostRequestUpdateUserInput();
                break;

            case 23:
                sendPostRequestCreate();
                break;

            case 24:
                sendDeleteRequest();
                break;

            case 25:
                observeResource();
                break;

            case 26:
                try
                {
                    cancelObserveResource();
                }
                catch (exception& e)
                {
                    cout << "Unable to cancel observing resource." << endl;
                    cout << "Error is: " << e.what() << endl;
                }

                break;

            case 27:
                cancelObservePassively();
                break;

            case 28:
                isMulticast = false;
                discoverDevice(isMulticast);
                break;

            case 29:
                isMulticast = true;
                discoverDevice(isMulticast);
                break;

            case 30:
                discoverPlatform();
                break;

            case 31:
                collectionType = GROUP_TYPE_ROOM;
                findCollection(collectionType);
                if (g_foundCollectionList.size() == 0)
                {
                    cout << "No collection found!!" << endl;
                }
                break;

            case 32:
                collectionType = GROUP_TYPE_ROOM;
                if (g_foundCollectionList.size() == 0)
                {
                    cout << "No Group available, starting find Group." << endl;
                    findCollection(collectionType);
                    if (g_foundCollectionList.size() == 0)
                    {
                        cout << "Still no Resource found, quitting join resource to group." << endl;
                        break;
                    }
                    else
                    {
                        g_hasCallbackArrived = false;
                    }
                }

                g_foundResourceList.at(0)->get(QueryParamsMap(), onGet);
                waitForCallback();

                if (g_resourceRepresentation.hasAttribute("links"))
                {
                    cout << "link is available." << endl;
                    g_resourceRepresentation.getAttributeValue("links", attrVal);
                    linkRep["links"] = attrVal;
                }
                else
                {
                    cout << "link is not available. creating link..." << endl;
                    linkRep.setValue("links",
                            (string) "[{\"href\":\"/device/light-1\",\"rt\":\"core.light\",\"if\":\"oic.if.baseline\",\"ins\":\"123\",\"p\":{\"bm\":3}}]");
                }

                g_foundResourceList.at(0)->post(collectionType, LINK_INTERFACE, linkRep,
                        QueryParamsMap(), onPost, g_qos);

                break;

            case 33:
                updateGroup();
                break;

            case 34:
                updateLocalResource();
                break;

            case 35:
                g_qos = QualityOfService::HighQos;
                cout << "CON Type Message Selected for Client" << endl;
                break;

            case 36:
                g_qos = QualityOfService::LowQos;
                cout << "NON Type Message Selected for Client" << endl;
                break;

            case 101:
                createTvDevice();
                break;

            case 102:
                createAirConDevice();
                break;

            case 103:
                createTvDevice(true);
                break;

            case 104:
                createAirConDevice(true);
                break;

            case 0:
                if (g_createdResourceList.size() > 0)
                {
                    deleteResource();
                }
                if (g_isGroupCreated)
                {
                    deleteGroup();
                }
                if (g_childHandle != NULL)
                {
                    OCPlatform::unregisterResource(g_childHandle);
                }
                g_createdResourceList.clear();
                g_foundResourceList.clear();
                return;
                break;
        }
    }
    showMenu(0, NULL);
}
