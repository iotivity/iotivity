/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <signal.h>
#include <stdlib.h>
#include "SampleResource.h"
#include "ResourceHelper.h"
#include "OCPlatform.h"
#include "OCApi.h"

#define BINARY_SWITCH_KEY "value"
#define BINARY_SWITCH_VALUE true

#define TEMPERATURE_RANGE_KEY "range"
#define TEMPERATURE_UNITS_KEY "units"
#define TEMPERATURE_UNITS_SUPPORTED_VALUE "C"
#define TEMPERATURE_RANGE_START_VALUE 10.01
#define TEMPERATURE_RANGE_END_VALUE 40.01
#define TEMPERATURE_KEY "temperature"
#define TEMPERATURE_VALUE 15.05

#define AIRFLOW_RANGE_KEY "range"
#define AIRFLOW_SPEED_KEY "speed"
#define AIRFLOW_DIRECTION_KEY "direction"
#define AIRFLOW_SPEED_VALUE 20
#define AIRFLOW_DIRECTION_VALUE "up"
#define AIRFLOW_SPEED_RANGE_START_VALUE 0
#define AIRFLOW_SPEED_RANGE_END_VALUE 50

#define TIMER_HOUR_KEY "x.com.vendor.timer.hour"
#define TIMER_MINUTE_KEY "x.com.vendor.timer.minute"
#define TIMER_SECOND_KEY "x.com.vendor.timer.second"
#define TIMER_RESET_KEY "x.com.vendor.timer.reset"
#define TIMER_HOUR_VALUE 10
#define TIMER_MINUTE_VALUE 30
#define TIMER_SECOND_VALUE 30
#define TIMER_RESET_VALUE true

#define CHILD_LOCK_KEY "x.com.vendor.child.lock.value"
#define CHILD_LOCK_VALUE false

#define NEW_RESOURCE_URI "/new-resource/created-light-1"

using namespace OC;
using namespace std;

enum class ResourceSelectionType
{
    VERTICAL_RESOURCE = 0,
    SYSTEM_RESOURCE,
    VERTICAL_OBSERVABLE_RESOURCE,
    VERTICAL_NON_OBSERVABLE_RESOURCE
};

vector< SampleResource * > g_createdResourceList;
vector< shared_ptr< OCResource > > g_foundResourceList;
vector< OCResourceHandle > g_handleList;
OCRepresentation g_resourceRepresentation;

SampleResource *g_acSwitchResource;
SampleResource *g_acAirFlowResource;
SampleResource *g_acTemperatureResource;
SampleResource *g_acTimerResource;
SampleResource *g_acSwingResource;

bool g_hasCallbackArrived = false;
bool g_isObservingResource = false;
bool g_isAirConDeviceCreated = false;

QualityOfService g_qos = QualityOfService::LowQos;
OCConnectivityType g_connectivityType = CT_DEFAULT;
bool g_isSecuredServer = false;
bool g_isSecuredClient = false;
string g_commonSmartHomeUri = "URI";
string g_commonVendorUri = "Vendor";
ResourceHelper *g_resourceHelper;

static mutex s_mutex;

static bool s_isFirstTime = true;
static const char CRED_FILE_SERVER[] = "oic_svr_db_server.dat";
static const char CRED_FILE_CLIENT[] = "oic_svr_db_client.dat";

void onObserve(const HeaderOptions headerOptions, const OCRepresentation &rep, const int &eCode,
        const int &sequenceNumber);
void onDelete(const HeaderOptions &headerOptions, const int eCode);
void onPost(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);
void onPut(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);
void onGet(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);
void onDeviceInfoReceived(const OCRepresentation& rep);
void onPlatformInfoReceived(const OCRepresentation& rep);
void onResourceFound(shared_ptr< OCResource > resource);

void handleMenu(void);
void showMenu(void);
void onlyShowMenu(void);
OCStackResult initiateServer(void);
void createAirConDevice(bool isSecured = false);
void deleteResource(void);
void findResource(string resourceType, string host = "");
void findAllResources(string host = "", string query = "");
void discoverDevice(bool);
void discoverPlatform(bool isMulticast = true);
void sendGetRequest(ResourceSelectionType type);
void sendPutRequest(ResourceSelectionType type);
void sendPostRequest(ResourceSelectionType type);
void sendBlockwisePost(void);
void sendDeleteRequest(ResourceSelectionType type);
void waitForCallback(void);
void observeResource(ResourceSelectionType type);
void cancelObserveResource(ResourceSelectionType type);
vector< shared_ptr< OCResource > > getRemoteResourceList(ResourceSelectionType type);
void updateLocalResources(void);
AttributeValue getResourceAttributeValue(string uri, string& key);
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

int main(int argc, char* argv[])
{
    signal(SIGSEGV, handler);
    g_createdResourceList.clear();
    g_foundResourceList.clear();
    g_handleList.clear();
    g_resourceHelper = ResourceHelper::getInstance();
    g_connectivityType = CT_DEFAULT;

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
                g_connectivityType = CT_IP_USE_V6;
            }
            else if (optionSelected == 4)
            {
                cout << "Using IP version: IPv4" << endl;
                g_connectivityType = CT_IP_USE_V4;
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
                g_isSecuredClient = true;
            }
            else if (optionSelected == 2)
            {
                cout << "Using secured server...." << endl;
                g_isSecuredServer = true;
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

    cout << "Inter-Operability App for RI started successfully" << endl << endl;
    showMenu();

    cout << "Iotivity Server stopped successfully" << endl;
    cout << "Quitting InterOperability  App for RI.... Done!!" << endl;

    return 0;
}

void onResourceFound(shared_ptr< OCResource > resource)
{
    s_mutex.lock();

    cout << "Found a resource!!" << endl;

    if (resource)
    {
        if(g_connectivityType == CT_ADAPTER_TCP)
        {
            cout << "total host count: " << resource->getAllHosts().size() << endl;
            vector<string> allHosts = resource->getAllHosts();
            string tcpHost = g_resourceHelper->getOnlyTCPHost(allHosts);
            cout << "TCP host is:  " << tcpHost << endl;
            try
            {
                resource->setHost(tcpHost);
            }
            catch(exception& e)
            {
                cout << "Unable to set host, reason: " << e.what() << endl;
            }

        }

        if (resource->host().compare("") != 0)
        {
            g_foundResourceList.push_back(resource);
        }

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
    s_mutex.lock();
    if (eCode == SUCCESS_RESPONSE || eCode == OC_STACK_OK)
    {
        cout << "Response: GET request was successful" << endl;

        vector< string > interfaceList = rep.getResourceInterfaces();

        bool isCollection = false;
        for (auto resourceInterface = interfaceList.begin(); resourceInterface != interfaceList.end(); resourceInterface++)
        {
            if (((*resourceInterface).compare(GROUP_INTERFACE) == 0)
                    || ((*resourceInterface).compare(BATCH_INTERFACE) == 0))
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
    s_mutex.unlock();
}

// callback handler on PUT request
void onPut(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    if (eCode == SUCCESS_RESPONSE || eCode == OC_STACK_RESOURCE_CREATED
            || eCode == OC_STACK_RESOURCE_CHANGED)
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
    if (eCode == SUCCESS_RESPONSE || eCode == OC_STACK_RESOURCE_CHANGED
            || eCode == OC_STACK_RESOURCE_CREATED)
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
    if (eCode == SUCCESS_RESPONSE || eCode == OC_STACK_RESOURCE_DELETED)
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
            cout << "OBSERVE RESULT:" << endl;
            cout << "\tSequenceNumber: " << sequenceNumber << endl;
            ResourceHelper::getInstance()->printRepresentation(rep);
        }
        else
        {
            cout << "Observe Response/Notification Error: " << eCode << endl;
        }
    }
    catch (exception &e)
    {
        cout << "Exception: " << e.what() << " in onObserve" << endl;
    }
    g_hasCallbackArrived = true;

    if (sequenceNumber > 0)
    {
        onlyShowMenu();
    }
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

OCStackResult initiateServer()
{
    OCStackResult result = OC_STACK_ERROR;
    if (g_isSecuredClient)
    {
        OCPersistentStorage ps =
        { client_fopen, fread, fwrite, fclose, unlink };
        if (g_connectivityType == CT_ADAPTER_TCP)
        {
            PlatformConfig cfg
            { OC::ServiceType::InProc, OC::ModeType::Both, (OCConnectivityType) (g_connectivityType | CT_ADAPTER_IP),
                g_connectivityType | CT_ADAPTER_IP, g_qos, &ps };
            result = SampleResource::constructServer(cfg);
        }
        else
        {
            PlatformConfig cfg
            { OC::ServiceType::InProc, OC::ModeType::Both, g_connectivityType, g_connectivityType,
                    g_qos, &ps };
            result = SampleResource::constructServer(cfg);
        }

    }
    else if (g_isSecuredServer)
    {
        OCPersistentStorage ps =
        { server_fopen, fread, fwrite, fclose, unlink };
        PlatformConfig cfg
        { OC::ServiceType::InProc, OC::ModeType::Server, g_connectivityType, g_connectivityType,
                g_qos, &ps };
        result = SampleResource::constructServer(cfg);
    }
    else
    {
        PlatformConfig cfg
        { OC::ServiceType::InProc, OC::ModeType::Both, g_connectivityType, g_connectivityType, g_qos };
        result = SampleResource::constructServer(cfg);
    }

    if (result != OC_STACK_OK)
    {
        cout << "Unable to start Iotivity servers" << endl;
    }
    else
    {
        cout << "Iotivity Server started successfully" << endl;
    }

    return result;
}

void createAirConDevice(bool isSecured)
{

    OCStackResult result = OC_STACK_ERROR;
    if (g_isAirConDeviceCreated == false)
    {
        vector<string>temp;
        temp.push_back(Device_TYPE_AC);
        cout << "Creating AirCon Device Resources!!" << endl;
        SampleResource::setDeviceInfo("Vendor Smart Home AirCon Device", temp);

        g_acSwitchResource = new SampleResource();
        g_acSwitchResource->setResourceProperties(AC_SWITCH_URI, SWITCH_RESOURCE_TYPE,
        SWITCH_RESOURCE_INTERFACE);
        g_acSwitchResource->setAsDiscoverableResource();
        g_acSwitchResource->setAsObservableResource();
        if (isSecured)
        {
            g_acSwitchResource->setAsSecuredResource();
        }
        OCRepresentation switchRep;
        string key = BINARY_SWITCH_KEY;
        string value = "";
        vector< int > range;
        vector< double > rangeTemperature;
        switchRep.setValue(key, BINARY_SWITCH_VALUE);
        g_acSwitchResource->setResourceRepresentation(switchRep);

        result = g_acSwitchResource->startResource();

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
        g_acTemperatureResource->setAsDiscoverableResource();
        g_acTemperatureResource->setAsObservableResource();
        if (isSecured)
        {
            g_acTemperatureResource->setAsSecuredResource();
        }
        OCRepresentation temperatureRep;
        value = TEMPERATURE_UNITS_SUPPORTED_VALUE;
        key = TEMPERATURE_UNITS_KEY;
        temperatureRep.setValue(key, value);
        g_acTemperatureResource->setAsReadOnly(key);
        rangeTemperature.push_back(TEMPERATURE_RANGE_START_VALUE);
        rangeTemperature.push_back(TEMPERATURE_RANGE_END_VALUE);
        key = TEMPERATURE_RANGE_KEY;
        temperatureRep.setValue(key, rangeTemperature);
        g_acTemperatureResource->setAsReadOnly(key);
        double temperature = TEMPERATURE_VALUE;
        key = TEMPERATURE_KEY;
        temperatureRep.setValue(key, temperature);
        g_acTemperatureResource->setResourceRepresentation(temperatureRep);
        result = g_acTemperatureResource->startResource();

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
        g_acAirFlowResource->setAsDiscoverableResource();
        g_acAirFlowResource->setAsObservableResource();
        if (isSecured)
        {
            g_acAirFlowResource->setAsSecuredResource();
        }

        OCRepresentation airFlowRep;
        int speed = AIRFLOW_SPEED_VALUE;
        value = AIRFLOW_DIRECTION_VALUE;
        key = AIRFLOW_DIRECTION_KEY;
        airFlowRep.setValue(key, value);
        key = AIRFLOW_SPEED_KEY;
        airFlowRep.setValue(key, speed);
        range.clear();
        range.push_back(AIRFLOW_SPEED_RANGE_START_VALUE);
        range.push_back(AIRFLOW_SPEED_RANGE_END_VALUE);
        key = AIRFLOW_RANGE_KEY;
        airFlowRep.setValue(key, range);
        g_acAirFlowResource->setAsReadOnly(key);
        g_acAirFlowResource->setResourceRepresentation(airFlowRep);

        result = g_acAirFlowResource->startResource();

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

        g_acTimerResource = new SampleResource();
        g_acTimerResource->setResourceProperties(AC_TIMER_URI, TIMER_RESOURCE_TYPE,
        TIMER_RESOURCE_INTERFACE);
        g_acTimerResource->setAsDiscoverableResource();
        if (isSecured)
        {
            g_acTimerResource->setAsSecuredResource();
        }

        OCRepresentation clockRep;
        key = TIMER_HOUR_KEY;
        clockRep.setValue(key, TIMER_HOUR_VALUE);
        key = TIMER_MINUTE_KEY;
        clockRep.setValue(key, TIMER_MINUTE_VALUE);
        key = TIMER_SECOND_KEY;
        clockRep.setValue(key, TIMER_SECOND_VALUE);
        key = TIMER_RESET_KEY;
        clockRep.setValue(key, TIMER_RESET_VALUE);
        value = REGION_KEY;
        key = TIMER_LOCATION_KEY;
        clockRep.setValue(key, value);

        g_acTimerResource->setResourceRepresentation(clockRep);

        result = g_acTimerResource->startResource();
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

        g_acSwingResource = new SampleResource();
        g_acSwingResource->setResourceProperties(AC_SWING_URI, SWING_RESOURCE_TYPE,
        SWING_RESOURCE_INTERFACE);
        if (isSecured)
        {
            g_acSwingResource->setAsSecuredResource();
        }

        OCRepresentation swingRep;
        swingRep.setValue(SWING_STATE_KEY, SWING_STATE_VALUE);
        value = SWING_MOVEMENT_VALUE;
        swingRep.setValue(SWING_MOVEMENT_KEY, value);
        string supportedDirection[2] =
        { "horizontal", "vertical" };
        swingRep.setValue(SWING_SUPPOTED_DIRECTION_KEY, supportedDirection);

        g_acSwingResource->setResourceRepresentation(swingRep);
        g_acSwingResource->setAsReadOnly("x.com.vendor.swing.supported.direction");

        result = g_acSwingResource->startResource();

        if (result == OC_STACK_OK)
        {
            cout << "Air Conditioner Swing Resource created successfully" << endl;
            g_createdResourceList.push_back(g_acSwingResource);
            g_isAirConDeviceCreated = true;
        }
        else
        {
            cout << "Unable to create Air Conditioner Swing resource" << endl;
        }
    }
    else
    {
        cout << "Already Smart Home AirCon Device Resources are  created!!" << endl;
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

        for (unsigned int i = 0; i < g_createdResourceList.size(); i++)
        {
            result = g_createdResourceList[i]->stopResource();
            if (result == OC_STACK_OK)
            {
                cout << "Successfully stopped Resource with URI: "
                        << g_createdResourceList[i]->getUri() << endl;
            }
        }
    }
    else
    {
        cout << "There is no resource available to delete!!" << endl;
    }

    g_createdResourceList.clear();
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
    OCStackResult result = OC_STACK_ERROR;

    if (isMulticast)
    {
        deviceDiscoveryRequest << OC_MULTICAST_PREFIX << OC_RSRVD_DEVICE_URI;
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
            deviceDiscoveryRequest << OC_RSRVD_DEVICE_URI;
            cout << "Discovering Device using Unicast... " << endl;
            cout << "Sending Unicast device discovery to the host: " << host << endl;
        }
        else
        {
            cout << "No server found to send Unicast device discovery" << endl;
            cout << "Sending Multicast Device discovery request..." << endl;
            deviceDiscoveryRequest << OC_MULTICAST_PREFIX << OC_RSRVD_DEVICE_URI;
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

AttributeValue getResourceAttributeValue(string uri, string& key)
{
    AttributeValue value;
    static double temperatureValue = 25;
    static string directionValue = "left";
    static int hourValue = 1;
    static bool swingerValue = true;
    static bool binaryValue = BINARY_SWITCH_VALUE;

    if (!uri.compare(AC_SWITCH_URI) || !uri.compare(AC_SWITCH_URI_CHILD))
    {
        key = string(ON_OFF_KEY);
        value = binaryValue = !binaryValue;
    }
    else if (!uri.compare(AC_TEMPERATURE_URI) || !uri.compare(AC_TEMPERATURE_URI_CHILD))
    {
        key = string(TEMPERATURE_KEY);
        value = temperatureValue =
                temperatureValue > 0 ? (temperatureValue - 26) : (temperatureValue + 26);
    }
    else if (!uri.compare(AC_AIR_FLOW_URI) || !uri.compare(AC_AIR_FLOW_URI_CHILD))
    {
        key = string(DIRECTION_KEY);
        value = directionValue = directionValue.compare("left") ? "left" : "right";
    }
    else if (!uri.compare(AC_TIMER_URI) || !uri.compare(AC_TIMER_URI_CHILD))
    {
        key = string(TIMER_HOUR_KEY);
        value = (hourValue % 2) ? (hourValue * 2) : (hourValue / 2);
    }
    else if (!uri.compare(AC_SWING_URI) || !uri.compare(AC_SWING_URI_CHILD))
    {
        key = string("x.com.vendor.swing.on");
        value = swingerValue = !swingerValue;
    }
    else if (!uri.compare(AC_SWITCH_URI) || !uri.compare(AC_SWITCH_URI_CHILD))
    {
        key = string(BINARY_SWITCH_KEY);
        value = binaryValue = !binaryValue;
    }

    return value;
}

void updateLocalResources()
{

    static bool binaryValue = false;

    if (g_createdResourceList.size() > 0)

    {
        for (auto resource : g_createdResourceList)
        {
            string key = "";
            AttributeValue value;

            value = getResourceAttributeValue(resource->getUri(), key);
            OCRepresentation rep = resource->getRepresentation();
            if (rep.hasAttribute(key))
            {
                rep.setValue(key, value);
                resource->setResourceRepresentation(rep);
                cout << "Successfully updated resource attribute!!" << endl;
                ResourceHelper::getInstance()->printRepresentation(rep);
                resource->notifyObservers(resource);
            }
            else
            {
                cout << "The resource does not have the mentioned attribute" << endl;
            }
        }

    }
    else
    {
        cout << "No resource to Update!!" << endl;
    }
}

void sendGetRequest(ResourceSelectionType type)
{
    vector< shared_ptr< OCResource > > remoteResourceList = getRemoteResourceList(type);
    if (remoteResourceList.size() > 0)
    {
        QueryParamsMap qpMap;
        for (auto resource : remoteResourceList)
        {
            g_hasCallbackArrived = false;
            cout << "Sending Get Request to the resource: " << resource->host() << resource->uri()
                    << endl;
            resource->get(qpMap, onGet, g_qos);
            cout << "GET request sent!!" << endl;
        }

        waitForCallback();

    }
    else
    {
        cout << "No resource to send GET!!" << endl;
    }
}

void sendPutRequest(ResourceSelectionType type)
{

    vector< shared_ptr< OCResource > > remoteResourceList = getRemoteResourceList(type);
    if (remoteResourceList.size() > 0)
    {
        OCRepresentation rep;
        QueryParamsMap qpMap;
        string key = REGION_KEY;
        string value = DEFAULT_REGION;
        rep.setValue(key, value);
        key = POWER_KEY;
        value = DEFAULT_POWER_STATE;
        rep.setValue(key, value);
        key = INTENSITY_KEY;
        value = DEFAULT_INTENSITY;
        rep.setValue(key, value);
        key = MANUFACTURER_KEY;
        value = DEFAULT_MANUFACTURER;
        rep.setValue(key, value);

        for (auto resource : remoteResourceList)
        {
            g_hasCallbackArrived = false;
            cout << "Sending PUT Request to the resource: " << resource->host() << resource->uri()
                    << endl;
            resource->put(rep, qpMap, onPut, g_qos);
            cout << "PUT request sent!!" << endl;
        }

        waitForCallback();

    }
    else
    {
        cout << "No resource to send PUT!!" << endl;
    }
}

void sendPostRequest(ResourceSelectionType type)
{
    vector< shared_ptr< OCResource > > remoteResourceList = getRemoteResourceList(type);
    if (remoteResourceList.size() > 0)
    {
        QueryParamsMap qpMap;
        string key = "";
        AttributeValue value;

        for (auto resource : remoteResourceList)
        {
            OCRepresentation rep;
            g_hasCallbackArrived = false;
            if (ResourceSelectionType::VERTICAL_RESOURCE == type)
            {
                value = getResourceAttributeValue(resource->uri(), key);
            }
            else if (ResourceSelectionType::SYSTEM_RESOURCE == type)
            {
                key = REGION_KEY;
                value = DEFAULT_REGION;
            }

            rep.setValue(key, value);
            cout << "Sending POST Request to the resource: " << resource->host() << resource->uri()
                    << endl;
            resource->post(rep, qpMap, onPost, g_qos);
            cout << "POST request sent!!" << endl;
        }

        waitForCallback();

    }
    else
    {
        cout << "No resource to send POST!!" << endl;
    }
}

void sendBlockwisePost()
{
    for (auto resource : getRemoteResourceList(ResourceSelectionType::VERTICAL_RESOURCE))
    {
        QueryParamsMap qpMap;
        static string bigValue = VERY_BIG_VALUE;
        if (resource->uri().compare(AC_TIMER_URI) == 0)
        {
            g_hasCallbackArrived = false;
            OCRepresentation rep;
            bigValue = string(DEFAULT_REGION) + bigValue;
            AttributeValue value = bigValue ;
            rep.setValue(TIMER_LOCATION_KEY, value);
            cout << "Sending POST Request with Blockwise Payload to the resource: "
                    << resource->host() << resource->uri() << endl;
            resource->post(rep, qpMap, onPost, g_qos);
            cout << "POST with Blockwise Payload request sent!!" << endl;
            break;
        }
    }
}

void sendDeleteRequest(ResourceSelectionType type)
{
    vector< shared_ptr< OCResource > > remoteResourceList = getRemoteResourceList(type);
    if (remoteResourceList.size() > 0)
    {
        for (auto resource : remoteResourceList)
        {
            g_hasCallbackArrived = false;
            cout << "Sending Delete Request to the resource: " << resource->host()
                    << resource->uri() << endl;
            resource->deleteResource(onDelete, g_qos);
            cout << "Delete request sent!!" << endl;
        }

        waitForCallback();

    }
    else
    {
        cout << "No resource to send Delete!!" << endl;
    }
}

void observeResource(ResourceSelectionType type)
{
    vector< shared_ptr< OCResource > > remoteResourceList = getRemoteResourceList(type);
    if (remoteResourceList.size() > 0)
    {
        for (auto resource : remoteResourceList)
        {
            g_hasCallbackArrived = false;
            cout << "Sending Observe Request to the resource: " << resource->host()
                    << resource->uri() << endl;
            resource->observe(ObserveType::Observe, QueryParamsMap(), &onObserve, g_qos);
            cout << "Observe request sent!!" << endl;
        }
        g_isObservingResource = true;
        waitForCallback();
    }
    else
    {
        cout << "No resource to Observe!!" << endl;
    }
}

void cancelObserveResource(ResourceSelectionType type)
{
    vector< shared_ptr< OCResource > > remoteResourceList = getRemoteResourceList(type);
    if (remoteResourceList.size() > 0)
    {
        for (auto resource : remoteResourceList)
        {
            g_hasCallbackArrived = false;
            cout << "Sending Observe Cancel Request to the resource: " << resource->host()
                    << resource->uri() << endl;
            resource->cancelObserve(g_qos);
            if (g_qos == QualityOfService::HighQos)
            {
                cout << "Observe Cancel request sent!!" << endl;
            }
            else
            {
                cout << "Observe Cancel request will be sent when a CON Notification is received!!"
                        << endl;
            }
        }
        g_isObservingResource = false;
        waitForCallback();

    }
    else
    {
        cout << "No resource to Observe!!" << endl;
    }
}

string getHost()
{
    string host = "";

    for (auto resource : g_foundResourceList)
    {
        if (resource->uri().compare(AC_TEMPERATURE_URI) == 0)
        {
            host = resource->host();
            break;
        }
    }
    if (host.compare("") == 0)
    {
        cout << "No remote resource found to send Unicast" << endl;
    }
    return host;
}

vector< shared_ptr< OCResource > > getRemoteResourceList(ResourceSelectionType type)
{
    vector< shared_ptr< OCResource > > desiredResourceList;
    desiredResourceList.clear();
    string remoteHost = getHost();

    for (auto resource : g_foundResourceList)
    {
        if (ResourceSelectionType::VERTICAL_NON_OBSERVABLE_RESOURCE == type)
        {
            if (resource->uri().compare(AC_TIMER_URI) == 0)
            {
                desiredResourceList.clear();
                desiredResourceList.push_back(resource);
                break;
            }
        }
        else if (ResourceSelectionType::VERTICAL_OBSERVABLE_RESOURCE == type)
        {
            if (resource->uri().find(g_commonSmartHomeUri) != string::npos)
            {
                desiredResourceList.push_back(resource);
            }

        }
        else if (ResourceSelectionType::VERTICAL_RESOURCE == type)
        {
            if ((resource->uri().find(g_commonVendorUri) != string::npos)
                    || (resource->uri().find(g_commonSmartHomeUri) != string::npos))
            {
                desiredResourceList.push_back(resource);
            }

        }
        else
        {
            if ((remoteHost.compare(resource->host()) == 0)
                    && ((resource->uri().compare(OC_RSRVD_DEVICE_URI) == 0)
                            || (resource->uri().compare(OC_RSRVD_PLATFORM_URI) == 0)))
            {
                desiredResourceList.push_back(resource);
            }
        }
    }
    return desiredResourceList;
}

void onlyShowMenu()
{
    cout << "Generating Menu, Please wait...." << endl;
    g_resourceHelper->waitInSecond(CALLBACK_WAIT_MIN + CALLBACK_WAIT_MIN);
    cout << "Please Select an option from the menu and press Enter" << endl;
    cout << "\t\t 0. Quit IUT Emulator App" << endl;
    cout << "\t\t 100. Set CON Type Messaging" << endl;
    cout << "\t\t 101. Set NON Type Messaging" << endl;

    if (s_isFirstTime)
    {
        cout << "\t\t 102. Select IP Transport- IPv4" << endl;
        cout << "\t\t 103. Select IP Transport - IPv6" << endl;
        cout << "\t\t 104. Select IP Transport - Both IPv6 & IPv4" << endl;
        cout << "\t\t 105. Select BLE Transport" << endl;
        cout << "\t\t 106. Select EDR Transport" << endl;
        cout << "\t\t 107. Select TCP Transport" << endl;
        cout << "\t\t 108. Select NFC Transport" << endl;
    }
    else
    {
        cout << endl;
        cout << "\t Server Operations:" << endl;
        cout << "\t\t 1. Create Non-Secured Air Conditioner Device" << endl;
        cout << "\t\t 2. Create Secured Air Conditioner Device" << endl;
        cout << "\t\t 3. Delete All Resources" << endl;
        cout << "\t\t 4. Update Local Resource" << endl;
        cout << endl;
        cout << "\t Client Operations:" << endl;
        cout << "\t\t 10. Discover Device - Multicast" << endl;
        cout << "\t\t 11. Discover Platform - Multicast" << endl;
        cout << "\t\t 12. Discover Resources - Multicast" << endl;
        cout << "\t\t 13. Discover Resources - Unicast" << endl;
        cout << "\t\t 14. Discover Resources - Multicast with Query" << endl;
        cout << "\t\t 15. Send GET Request to Vertical Resources" << endl;
        cout << "\t\t 16. Send GET Request to Core Resources" << endl;
        cout << "\t\t 17. Send PUT Request to Vertical Resources" << endl;
        cout << "\t\t 18. Send PUT Request to Core Resources" << endl;
        cout << "\t\t 19. Send POST Request to Vertical Resources" << endl;
        cout << "\t\t 20. Send POST Request to Core Resources" << endl;
        cout << "\t\t 21. Send Delete Request to Vertical Resources" << endl;
        cout << "\t\t 22. Send Delete Request to Core Resources" << endl;
        cout << "\t\t 23. Send Observe Request to Vertical Observable Resources" << endl;
        cout << "\t\t 24. Send Observe Request to Core Resources" << endl;
        cout << "\t\t 25. Send Observe Request to Vertical Non-observable Resources" << endl;
        cout << "\t\t 26. Cancel Observing Resource" << endl;
        cout << "\t\t 27. Send POST Request with Blockwise Payload" << endl;
    }

}

void showMenu()
{
    onlyShowMenu();
    g_hasCallbackArrived = false;
    handleMenu();
}

void handleMenu()
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
    bool isSecured = false;
    cin >> userInput;

    long int choice = strtol(userInput.c_str(), NULL, 10);
    if (choice > 101 && choice < 109)
    {
        s_isFirstTime = false;
    }

    switch (choice)
    {
        case 1:
            createAirConDevice();
            break;

        case 2:
            isSecured = true;
            createAirConDevice(isSecured);
            break;

        case 3:
            deleteResource();
            break;
            break;

        case 4:
            updateLocalResources();
            break;
            break;

        case 10:
            isMulticast = true;
            discoverDevice(isMulticast);
            break;

        case 11:
            discoverPlatform();
            break;

        case 12:
            findAllResources();
            if (g_foundResourceList.size() == 0)
            {
                cout << "No resource found!!" << endl;
            }
            break;

        case 13:
            findAllResources();
            resourceHost = getHost();
            findAllResources(resourceHost);
            if (g_foundResourceList.size() == 0)
            {
                cout << "No resource found!!" << endl;
            }
            break;

        case 14:
            findResource(string(TEMPERATURE_RESOURCE_TYPE));
            if (g_foundResourceList.size() == 0)
            {
                cout << "No resource found!!" << endl;
            }
            break;

        case 15:
            sendGetRequest(ResourceSelectionType::VERTICAL_RESOURCE);
            break;

        case 16:
            sendGetRequest(ResourceSelectionType::SYSTEM_RESOURCE);
            break;

        case 17:
            sendPutRequest(ResourceSelectionType::VERTICAL_RESOURCE);
            break;

        case 18:
            sendPutRequest(ResourceSelectionType::SYSTEM_RESOURCE);
            break;

        case 19:
            sendPostRequest(ResourceSelectionType::VERTICAL_RESOURCE);
            break;

        case 20:
            sendPostRequest(ResourceSelectionType::SYSTEM_RESOURCE);
            break;

        case 21:
            sendDeleteRequest(ResourceSelectionType::VERTICAL_RESOURCE);
            break;

        case 22:
            sendDeleteRequest(ResourceSelectionType::SYSTEM_RESOURCE);
            break;

        case 23:
            observeResource(ResourceSelectionType::VERTICAL_OBSERVABLE_RESOURCE);
            break;

        case 24:
            observeResource(ResourceSelectionType::SYSTEM_RESOURCE);
            break;

        case 25:
            observeResource(ResourceSelectionType::VERTICAL_NON_OBSERVABLE_RESOURCE);
            break;

        case 26:
            cancelObserveResource(ResourceSelectionType::VERTICAL_OBSERVABLE_RESOURCE);
            break;

        case 27:
            sendBlockwisePost();
            break;

        case 100:
            g_qos = QualityOfService::HighQos;
            if (s_isFirstTime)
            {
                cout << "CON Type Message Selected for Server/Client" << endl;
            }
            else
            {
                cout << "CON Type Message Selected for Client" << endl;
            }
            break;

        case 101:
            g_qos = QualityOfService::LowQos;
            if (s_isFirstTime)
            {
                cout << "NON Type Message Selected for Server/Client" << endl;
            }
            else
            {
                cout << "NON Type Message Selected for Client" << endl;
            }
            break;

        case 102:
            g_connectivityType = CT_IP_USE_V4;
            initiateServer();
            break;

        case 103:
            g_connectivityType = CT_IP_USE_V6;
            initiateServer();
            break;

        case 104:
            g_connectivityType = CT_ADAPTER_IP;
            initiateServer();
            break;

        case 105:
            g_connectivityType = CT_ADAPTER_GATT_BTLE;
            initiateServer();
            break;

        case 106:
            g_connectivityType = CT_ADAPTER_RFCOMM_BTEDR;
            initiateServer();
            break;

        case 107:
            g_connectivityType = CT_ADAPTER_TCP;
            initiateServer();
            break;

        case 108:
            g_connectivityType = CT_ADAPTER_NFC;
            initiateServer();
            break;

        case 0:
            if (g_createdResourceList.size() > 0)
            {
                deleteResource();
            }
            g_createdResourceList.clear();
            g_foundResourceList.clear();
            return;
            break;

        default:
            cout << "Invalid Input. Please input your choice again" << endl;
    }

    showMenu();
}
