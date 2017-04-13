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
 *
 ******************************************************************/
#include "CSCsdkCloudHelper.h"
#include "CSCsdkCloudClientHelper.h"
#include "CSCsdkUtilityHelper.h"


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
    if (eCode == SUCCESS_RESPONSE || eCode == OC_STACK_OK)
    {
        cout << "Response: GET request was successful" << endl;

//        g_resourceHelper->printIncomingRepresentation(rep);

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

void onPublish(const OCRepresentation &, const int &eCode)
{
    cout << "Publish resource response received, code: " << eCode << endl;
    g_hasCallbackArrived = true;
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

    }
    else
    {
        cout << "Already Smart Home AirCon Device Resources are  created!!" << endl;
    }

}

void publishResource() {
    g_hasCallbackArrived = false;
    ResourceHandles resourceHandles;
    resourceHandles.push_back(g_acSwitchResource->getResourceHandle());
    resourceHandles.push_back(g_acTemperatureResource->getResourceHandle());

    int result = RDClient::Instance().publishResourceToRD(CloudCommonUtil::getDefaultHostAddess().c_str(), OCConnectivityType::CT_ADAPTER_TCP,
             resourceHandles,
             &onPublish);

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

AttributeValue getResourceAttributeValue(string uri, string& key)
{
    AttributeValue value;
    static double temperatureValue = 25;
    static string directionValue = "left";
    static int hourValue = 1;
    static bool swingerValue = true;
    static bool binaryValue = false;

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

    return value;
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
        OCRepresentation rep;
        QueryParamsMap qpMap;
        string key = "";
        AttributeValue value;

        for (auto resource : remoteResourceList)
        {
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


