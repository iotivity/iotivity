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

vector< SampleResource * > g_createdResourceList;
vector< shared_ptr< OCResource > > g_foundResourceList;
vector< OCResourceHandle > g_handleList;
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
SampleResource *g_acChildLockResource;

bool g_hasCallbackArrived = false;
bool g_isObservingResource = false;
bool g_isAirConDeviceCreated = false;

QualityOfService g_qos = QualityOfService::LowQos;
OCConnectivityType g_connectivityType = CT_DEFAULT;
bool g_isSecuredServer = false;
bool g_isSecuredClient = false;
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
void sendGetRequest();
void sendGetRequestWithQuery(string, string);
void sendPutRequestUpdate(void);
void sendPostRequestUpdateUserInput(void);
void sendPutRequestCreate(void);
void sendPostRequestCreate(void);
void sendDeleteRequest(void);
void waitForCallback(void);
void observeResource(void);
void cancelObserveResource(void);
void cancelObservePassively(void);
int selectResource(void);
int selectLocalResource(void);
AttributeValue getAttributeValueFromUser(void);
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

//        g_resourceHelper->printIncomingRepresentation(rep);

        vector < string > interfacelist = rep.getResourceInterfaces();

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
            vector < OCRepresentation > children = rep.getChildren();

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
        PlatformConfig cfg
        { OC::ServiceType::InProc, OC::ModeType::Both, g_connectivityType, g_connectivityType,
                g_qos, &ps };
        result = SampleResource::constructServer(cfg);
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
        cout << "Creating AirCon Device Resources!!" << endl;
        SampleResource::setDeviceInfo("Vendor Smart Home AirCon Device", Device_TYPE_AC);

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
        setlocale(LC_ALL, "");
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

        g_acChildLockResource = new SampleResource();
        g_acChildLockResource->setResourceProperties(AC_CHILD_LOCK_URI, CHILD_LOCK_RESOURCE_TYPE,
        CHILD_LOCK_RESOURCE_INTERFACE);
        g_acChildLockResource->setAsObservableResource();
        if (isSecured)
        {
            g_acChildLockResource->setAsSecuredResource();
        }

        OCRepresentation childLockRep;
        key = CHILD_LOCK_KEY;
        childLockRep.setValue(key, CHILD_LOCK_VALUE);
        g_acChildLockResource->setResourceRepresentation(childLockRep);

        result = g_acChildLockResource->startResource();

        if (result == OC_STACK_OK)
        {
            cout << "Air Conditioner Timer Resource created successfully" << endl;
            g_createdResourceList.push_back(g_acChildLockResource);
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

void sendGetRequest()
{
    int selection = selectResource();
    if (selection != -1)
    {
        QueryParamsMap qpMap;
        shared_ptr < OCResource > targetResource = g_foundResourceList.at(selection);
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

        vector < string > resourceTypes;
        string key = URI_KEY;
        string value = NEW_RESOURCE_URI;
        rep.setUri(value);
        value = SWITCH_RESOURCE_TYPE;
        resourceTypes.push_back(value);
        rep.setResourceTypes(resourceTypes);
        key = BINARY_SWITCH_KEY;
        rep.setValue(key, BINARY_SWITCH_VALUE);

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
            g_createdResourceList.at(selection)->notifyObservers(
                    g_createdResourceList.at(selection));
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

        vector < string > resourceTypes;
        string key = URI_KEY;
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
        shared_ptr < OCResource > resource = g_foundResourceList.at(selection);
        resource->deleteResource(&onDelete);
        cout << "DELETE request sent!!" << endl;
        waitForCallback();
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

        shared_ptr < OCResource > resource = g_foundResourceList.at(selection);
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

            shared_ptr < OCResource > resource = g_foundResourceList.at(selection);
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
            shared_ptr < OCResource > resource = g_foundResourceList.at(selection);
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
            cout << "\t\t" << i << ". " << g_foundResourceList.at(i - 1)->uniqueIdentifier()
                    << endl;
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
        cout << endl;
        cout << "\t Client Operations:" << endl;
        cout << "\t\t 10. Find Resource using Interface Query" << endl;
        cout << "\t\t 11. Find Specific Type Of Resource" << endl;
        cout << "\t\t 12. Find All Resources" << endl;
        cout << "\t\t 13. Find Resource using Interface Query - Unicast" << endl;
        cout << "\t\t 14. Find Specific Type Of Resource - Unicast" << endl;
        cout << "\t\t 15. Find All Resources - Unicast" << endl;
        cout << "\t\t 16. Send GET Request" << endl;
        cout << "\t\t 17. Send GET Request with query" << endl;
        cout << "\t\t 18. Send PUT Request - Create Resource" << endl;
        cout << "\t\t 19. Send PUT Request - Complete Update" << endl;
        cout << "\t\t 20. Send POST Request - Partial Update - User Input" << endl;
        cout << "\t\t 21. Send POST Request - Create Sub-Ordinate Resource" << endl;
        cout << "\t\t 22. Send Delete Request" << endl;
        cout << "\t\t 23. Observe Resource - Retrieve Request with Observe" << endl;
        cout << "\t\t 24. Cancel Observing Resource" << endl;
        cout << "\t\t 25. Cancel Observing Resource Passively" << endl;
        cout << "\t\t 26. Discover Device - Multicast" << endl;
        cout << "\t\t 27. Discover Platform - Multicast" << endl;
        cout << "\t\t 28. Update Local Resource" << endl;
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
    if ((choice > 28 && choice < 100) || choice < 0 || (choice > 3 && choice < 10) || choice > 108)
    {
        cout << "Invalid Input. Please input your choice again" << endl;
    }
    else
    {
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
                cout << "Please type query(key=value), then press Enter: ";
                cin >> userInterfaceType;
                findAllResources(resourceHost, userInterfaceType);
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
                sendGetRequest();
                break;

            case 17:
                cout << "Please type query key, then press Enter: ";
                cin >> queryKey;
                cout << "Please type query value, then press Enter: ";
                cin >> queryValue;
                sendGetRequestWithQuery(queryKey, queryValue);
                break;

            case 18:
                sendPutRequestCreate();
                break;

            case 19:
                sendPutRequestUpdate();
                break;

            case 20:
                sendPostRequestUpdateUserInput();
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
                isMulticast = true;
                discoverDevice(isMulticast);
                break;

            case 27:
                discoverPlatform();
                break;

            case 28:
                updateLocalResource();
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
        }
    }
    showMenu();
}
