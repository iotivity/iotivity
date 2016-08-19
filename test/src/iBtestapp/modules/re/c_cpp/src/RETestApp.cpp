//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include "REUtility.h"

struct CloseApp
{
};

shared_ptr< RCSResourceObject > m_TempSensorRes;
shared_ptr< RCSResourceObject > m_TempActuatorRes;
shared_ptr< RCSResourceObject > m_BinSwitchResource;

shared_ptr< RCSRemoteResourceObject > m_Resource;

vector< shared_ptr< RCSRemoteResourceObject > > m_ResourceList;

bool m_IsResourceCreated = false;
bool m_IsResourceFound = false;

static int s_TemperatureVal = 0;

static mutex s_mutex;

// This function is for Configuration as both client and server
void configServerClient()
{
    PlatformConfig config
    { OC::ServiceType::InProc, ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos };

    OCPlatform::Configure(config);
}

// Print Menu Items
void showMainMenu()
{
    cout << endl;
    cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;

    cout << "*** [SERVER] ***" << endl;

    cout << setw(2) << CREATE_RESOURCE << ".  Create Resource" << endl;
    cout << setw(2) << SERVER_SET_ATTRIBUTES << ".  Set Attributes" << endl;
    cout << setw(2) << SERVER_GET_ATTRIBUTES << ".  Get Attributes" << endl;
    cout << endl;

    cout << "*** [CLIENT] ***" << endl;

    cout << setw(2) << DISCOVER_RESOURCE_WITHOUT_QUERY << ".  Discover Resource without query"
            << endl;
    cout << setw(2) << DISCOVER_RESOURCE_WITH_TYPE_QUERY
            << ".  Discover Resource with Resource Type" << endl;
    cout << setw(2) << DISCOVER_RESOURCE_WITH_TYPES_QUERY
            << ".  Discover Resource with Resource Types" << endl;
    cout << setw(2) << DISCOVER_NON_DISCOVERABLE_RESOURCE << ".  Discover Non-Discoverable Resource"
            << endl;
    cout << setw(2) << START_MONITORING << ".  Start Monitoring" << endl;
    cout << setw(2) << STOP_MONITORING << ".  Stop Monitoring" << endl;
    cout << setw(2) << GET_STATE << ".  Get State" << endl;
    cout << setw(2) << SET_REMOTE_ATTRIBUTES << ".  Set Remote Attributes" << endl;
    cout << setw(2) << GET_REMOTE_ATTRIBUTES << ".  Get Remote Attributes" << endl;
    cout << setw(2) << START_CACHING << ".  Start Caching" << endl;
    cout << setw(2) << STOP_CACHING << ".  Stop Caching" << endl;
    cout << setw(2) << GET_CACHED_ATTRIBUTES << ".  Get Cached Attributes" << endl;
    cout << setw(2) << GET_CACHED_STATE << ".  Get Cached State" << endl;
    cout << endl;

    cout << setw(2) << EXIT << ".  Exit" << endl;

    cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;
    cout << endl;

    cout << "\tPlease Choose a Menu: ";
}

// This function is for wait method with seconds
void waitInSecond(int second)
{
    sleep(second);
}

// Print Attributes Method
void showAttributes(const RCSResourceAttributes& attrs)
{
    cout << endl << "*** [ATTRIBUTES] ***" << endl;

    for (const auto& attr : attrs)
    {
        cout << "Attribute :  " << attr.key() << " = " << attr.value().toString() << endl;
    }
}

// Callback method for Get Request from Client
RCSGetResponse getRequestHandler(const RCSRequest& request, RCSResourceAttributes& attrs)
{
    cout << endl << "Received a GET request from CLIENT for Temperature" << endl;

    {
        RCSResourceObject::LockGuard lock(m_TempActuatorRes);
        cout << endl << "Send default response for URI: " << request.getResourceUri().c_str()
                << endl;
    }

    return RCSGetResponse::defaultAction();
}

//Callback method for Set Request from Client
RCSSetResponse setRequestHandler(const RCSRequest& request, RCSResourceAttributes& attrs)
{
    cout << endl << "Received a SET request from CLIENT" << endl;
    cout << "Send default response for URI: " << request.getResourceUri().c_str() << endl;

    return RCSSetResponse::defaultAction();
}

// Resource Creation method
void createResource()
{
    cout << "Create Resource..." << endl;

    if (m_IsResourceCreated)
    {
        cout << "Resource Already created." << endl;
        return;
    }

    try
    {
        RCSResourceObject::Builder*pBuilderBinSwitch = new RCSResourceObject::Builder(
        BINARY_SWITCH_URI, BINARY_SWITCH_TYPE, DEFAULT_ACTUATOR_INTERFACE);
        pBuilderBinSwitch->setDiscoverable(true);
        pBuilderBinSwitch->setObservable(true);
        m_BinSwitchResource = pBuilderBinSwitch->build();

        RCSResourceObject::Builder*pBuilderTempS = new RCSResourceObject::Builder(
        TEMPERATURE_SENSOR_URI, TEMPERATURE_RES_TYPE, DEFAULT_SENSOR_INTERFACE);
        pBuilderTempS->setDiscoverable(false);
        pBuilderTempS->setObservable(false);
        m_TempSensorRes = pBuilderTempS->build();

        RCSResourceObject::Builder*pBuilderTempA = new RCSResourceObject::Builder(
        TEMPERATURE_ACTUATOR_URI, TEMPERATURE_RES_TYPE, DEFAULT_ACTUATOR_INTERFACE);
        pBuilderTempA->setDiscoverable(true);
        pBuilderTempA->setObservable(true);
        m_TempActuatorRes = pBuilderTempA->build();
        m_TempActuatorRes->setAttribute(TEMPERATURE_TEMP_KEY, s_TemperatureVal);
        m_TempActuatorRes->setAttribute(TEMPERATURE_UNITS_KEY, "C");
        m_TempActuatorRes->setAttribute(TEMPERATURE_RANGE_KEY, "0,30");
        m_TempActuatorRes->setGetRequestHandler(getRequestHandler);
        m_TempActuatorRes->setSetRequestHandler(setRequestHandler);

        m_IsResourceCreated = true;

        cout << "Resource created successfully." << endl;
    }
    catch (exception& e)
    {
        cout << "Resource creation failed, due to " << string(e.what()) << endl;
    }
}

// This is the server method for Set Attributes
void setAttributes()
{
    cout << "Setting Attributes..." << endl;

    if (m_IsResourceCreated)
    {
        if (++s_TemperatureVal > 30)
            s_TemperatureVal = 0;

        try
        {
            m_TempActuatorRes->setAttribute(TEMPERATURE_TEMP_KEY, s_TemperatureVal);

            cout << "Set Attributes Successfully." << endl;
        }
        catch (exception& e)
        {
            cout << "Set Attributes failed, due to " << string(e.what()) << endl;
        }
    }
    else
    {
        cout << "Create Resource First" << endl;
    }
}

// This is the server method for Get Attributes
void getAttributes()
{
    cout << "Getting Attributes..." << endl;

    if (m_IsResourceCreated)
    {
        RCSResourceObject::LockGuard lock(m_TempActuatorRes);
        showAttributes(m_TempActuatorRes->getAttributes());
    }
    else
    {
        cout << "Create Resource First" << endl;
    }
}

void printResourceInfo(shared_ptr< RCSRemoteResourceObject > pResource)
{
    try
    {
        cout << "Resource URI: " << pResource->getUri() << " Address: " << pResource->getAddress()
                << endl;
        cout << "Resource Types:";
        for (auto const &type : pResource->getTypes())
        {
            cout << " " << type;
        }
        cout << "Resource Interfaces:";
        for (const auto &ifs : pResource->getInterfaces())
        {
            cout << " " << ifs;
        }
    }
    catch (exception& e)
    {
        cout << "Resource Info Error..." << endl;
    }
}

// Callback method for Resource Discovery
void onPrimitiveResourceDiscovered(shared_ptr< RCSRemoteResourceObject > pResource)
{
    s_mutex.lock();

    cout << endl << endl << "***onResourceDiscovered***" << endl;

    if (pResource)
    {
        printResourceInfo(pResource);
        m_ResourceList.push_back(pResource);
    }
    else
    {
        cout << "Found resource is null" << endl;
    }

    s_mutex.unlock();
}

bool checkResourceWithUri(string uri)
{
    try
    {
        for (vector< shared_ptr< RCSRemoteResourceObject > >::iterator resIt =
                m_ResourceList.begin(); resIt != m_ResourceList.end(); ++resIt)
        {
            shared_ptr< RCSRemoteResourceObject > resource =
                    (shared_ptr< RCSRemoteResourceObject > ) *resIt;

            if (resource->getUri().compare(uri) == 0)
            {
                return true;
            }
        }
    }
    catch (exception& e)
    {
        return false;
    }

    return false;
}

void selectResource()
{
    m_Resource = nullptr;

    for (vector< shared_ptr< RCSRemoteResourceObject > >::iterator resIt = m_ResourceList.begin();
            resIt != m_ResourceList.end(); ++resIt)
    {
        shared_ptr< RCSRemoteResourceObject > resource =
                (shared_ptr< RCSRemoteResourceObject > ) *resIt;

        if (resource->getUri().compare(TEMPERATURE_ACTUATOR_URI) == 0)
        {
            m_Resource = resource;
            return;
        }
    }
}

// Menu item method for Resource Discovery
void discoverResource()
{
    cout << "Discovering Resource..." << endl;

    m_ResourceList.clear();
    m_Resource = nullptr;

    try
    {
        unique_ptr< RCSDiscoveryManager::DiscoveryTask > discoveryTask =
                RCSDiscoveryManager::getInstance()->discoverResource(RCSAddress::multicast(),
                        &onPrimitiveResourceDiscovered);

        waitInSecond(WAIT_TIME_MAX);

        if (checkResourceWithUri(BINARY_SWITCH_URI)
                && checkResourceWithUri(TEMPERATURE_ACTUATOR_URI))
        {
            cout << endl << "All resource found successfully." << endl;
            selectResource();
        }
        else
            cout << endl << "Could not found proper resources." << endl;

        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        cout << "Exception occurred while finding resource, exception is: " << e.what() << endl;
    }
}

void discoverResourceWithQueryByResTypes()
{
    cout << "Discovering Resource by Resource Types" << endl;

    m_ResourceList.clear();
    m_Resource = nullptr;

    vector< string > resourceTypes;
    resourceTypes.push_back(BINARY_SWITCH_TYPE);
    resourceTypes.push_back(TEMPERATURE_RES_TYPE);

    try
    {
        unique_ptr< RCSDiscoveryManager::DiscoveryTask > discoveryTask =
                RCSDiscoveryManager::getInstance()->discoverResourceByTypes(RCSAddress::multicast(),
                        resourceTypes, &onPrimitiveResourceDiscovered);

        waitInSecond(WAIT_TIME_MAX);

        if (checkResourceWithUri(BINARY_SWITCH_URI)
                && checkResourceWithUri(TEMPERATURE_ACTUATOR_URI))
        {
            cout << endl << "All resource found successfully." << endl;
            selectResource();
        }
        else
            cout << endl << "Could not found proper resources." << endl;

        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        cout << "Exception occurred while finding resource, exception is: " << e.what() << endl;
    }
}

void discoverResourceWithQueryByResType()
{
    cout << "Discovering Resource by Resource Type..." << endl;

    m_ResourceList.clear();
    m_Resource = nullptr;

    try
    {
        unique_ptr< RCSDiscoveryManager::DiscoveryTask > discoveryTask =
                RCSDiscoveryManager::getInstance()->discoverResourceByType(RCSAddress::multicast(),
                TEMPERATURE_RES_TYPE, &onPrimitiveResourceDiscovered);

        waitInSecond(WAIT_TIME_MAX);

        if (checkResourceWithUri(TEMPERATURE_ACTUATOR_URI))
        {
            cout << endl << "Resource found successfully." << endl;
            selectResource();
        }
        else
            cout << endl << "Could not found proper resource." << endl;

        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        cout << "Exception occurred while finding resource, exception is: " << e.what() << endl;
    }
}

void discoverNonDiscoverableResource()
{
    cout << "Discovering Non-Discoverable Resource" << endl;

    m_ResourceList.clear();
    m_Resource = nullptr;

    try
    {
        unique_ptr< RCSDiscoveryManager::DiscoveryTask > discoveryTask =
                RCSDiscoveryManager::getInstance()->discoverResource(RCSAddress::multicast(),
                        &onPrimitiveResourceDiscovered);

        waitInSecond(WAIT_TIME_MAX);

        if (checkResourceWithUri(TEMPERATURE_SENSOR_URI))
            cout << endl << "Found Non-Discoverable Resource" << endl;
        else
            cout << endl << "Could not found Non-Discoverable Resource" << endl;

        discoveryTask->cancel();
    }
    catch (exception& e)
    {
        cout << "Exception occurred while finding resource, exception is: " << e.what() << endl;
    }
}

// Method for printing Remote Resource State
void printRemoteResourceState(ResourceState resourceState)
{
    switch (resourceState)
    {
        case ResourceState::NONE:
            cout << "NONE" << endl;
            break;

        case ResourceState::ALIVE:
            cout << "ALIVE" << endl;
            break;

        case ResourceState::REQUESTED:
            cout << "REQUESTED" << endl;
            break;

        case ResourceState::LOST_SIGNAL:
            cout << "LOST_SIGNAL" << endl;
            break;

        case ResourceState::DESTROYED:
            cout << "DESTROYED" << endl;
            break;

        default:
            cout << "NOT_DEFINED" << endl;
            break;
    }
}

// Callback method for Start Monitoring api
void onStateChangeReceived(ResourceState resourceState)
{
    cout << "State changed to :  ";

    printRemoteResourceState(resourceState);
}

// Most of the task is performed with Resource, firstly client need a resource to monitor/caching.
// This method is to check a resource is discovered and selected or not.
bool isResourceAvailable()
{
    if (!m_Resource)
    {
        cout << "Discover Resource first." << endl;
        return false;
    }

    return true;
}

// Menu item method for Start Monitoring
void startMonitoring()
{
    if (isResourceAvailable())
    {
        if (m_Resource->isMonitoring())
        {
            cout << "Monitoring already started." << endl;
        }
        else
        {
            m_Resource->startMonitoring(&onStateChangeReceived);

            waitInSecond(WAIT_TIME_MIN);

            if (m_Resource->isMonitoring())
            {
                cout << "Monitoring started successfully." << endl;
            }
            else
            {
                cout << "Monitoring is not started yet." << endl;
            }
        }
    }
}

// Menu item method for Stop Monitoring
void stopMonitoring()
{
    if (isResourceAvailable())
    {
        if (m_Resource->isMonitoring())
        {
            m_Resource->stopMonitoring();

            waitInSecond(WAIT_TIME_MIN);

            if (!m_Resource->isMonitoring())
            {
                cout << "Monitoring stopped successfully." << endl;
            }
            else
            {
                cout << "Monitoring is not stopped yet." << endl;
            }
        }
        else
        {
            cout << "Monitoring is not started yet." << endl;
        }
    }
}

// Menu item method for Get Remote Resource State
void getState()
{
    if (isResourceAvailable())
    {
        ResourceState resourceState = m_Resource->getState();

        cout << "Remote Resource State : ";
        printRemoteResourceState(resourceState);
    }
}

// Get/Set Attribute Callback method
void onRemoteAttributesReceived(const RCSResourceAttributes& attributes, int)
{
    cout << "***onAttributesReceived***" << endl;
    showAttributes(attributes);
}

// Menu item method for Set Remote Resource Attributes
void setRemoteAttributes()
{
    cout << "Setting Remote Attributes..." << endl;

    if (isResourceAvailable())
    {
        RCSResourceAttributes resAttributes;

        if (++s_TemperatureVal > 30)
            s_TemperatureVal = 0;

        resAttributes[TEMPERATURE_TEMP_KEY] = s_TemperatureVal;

        m_Resource->setRemoteAttributes(resAttributes, onRemoteAttributesReceived);

        waitInSecond(WAIT_TIME_MIN);
    }
}

// Menu item method for Get Remote Resource Attributes
void getRemoteAttributes()
{
    cout << "Getting Remote Attributes..." << endl;

    if (isResourceAvailable())
    {
        m_Resource->getRemoteAttributes(onRemoteAttributesReceived);

        waitInSecond(WAIT_TIME_MIN);
    }
}

// Start Caching Callback method
void onCacheUpdated(const RCSResourceAttributes& resourceAttributes)
{
    cout << "***onCacheUpdated***" << endl;

    showAttributes(resourceAttributes);
}

// Menu item method for Start Caching
void startCaching()
{
    if (isResourceAvailable())
    {
        if (m_Resource->isCaching())
        {
            cout << "Caching already started." << endl;
        }
        else
        {
            m_Resource->startCaching(&onCacheUpdated);

            waitInSecond(WAIT_TIME_MIN);

            if (m_Resource->isCaching())
            {
                cout << "Caching started successfully." << endl;
            }
            else
            {
                cout << "Caching is not started yet." << endl;
            }
        }
    }
}

// Menu item method for Stop Caching
void stopCaching()
{
    if (isResourceAvailable())
    {
        if (m_Resource->isCaching())
        {
            m_Resource->stopCaching();

            waitInSecond(WAIT_TIME_MIN);

            if (m_Resource->isCaching())
            {
                cout << "Caching stopped successfully." << endl;
            }
            else
            {
                cout << "Caching is not stopped yet." << endl;
            }
        }
        else
        {
            cout << "Caching is not started yet." << endl;
        }
    }
}

// Menu item method for Get Cached Resource Attributes
void getCachedAttributes()
{
    cout << "Getting Cached Attributes" << endl;

    if (isResourceAvailable())
    {
        bool isCachingStarted = false;

        if (!m_Resource->isCaching())
        {
            m_Resource->startCaching();
            isCachingStarted = true;
            waitInSecond(WAIT_TIME_MIN);
        }

        RCSResourceAttributes attrs = m_Resource->getCachedAttributes();

        cout << "Cached Attributes: ";
        showAttributes(attrs);

        if (isCachingStarted)
        {
            m_Resource->stopCaching();
            waitInSecond(WAIT_TIME_MIN);
        }
    }
}

// Print method for Cached State
void printCachedState(CacheState cachedState)
{
    switch (cachedState)
    {
        case CacheState::NONE:
            cout << "NONE" << endl;
            break;

        case CacheState::READY:
            cout << "READY" << endl;
            break;

        case CacheState::UNREADY:
            cout << "UNREADY" << endl;
            break;

        case CacheState::LOST_SIGNAL:
            cout << "LOST_SIGNAL" << endl;
            break;

        default:
            cout << "NOT_DEFINED" << endl;
            break;
    }
}

// Menu item method for Get Cached State
void getCachedState()
{
    if (isResourceAvailable())
    {
        CacheState cachedState = m_Resource->getCacheState();

        cout << "Cached State : ";

        printCachedState(cachedState);
    }
}

// This function is for menu selection
void menuSelection(ApplicationMenu menu)
{

    configServerClient();

    switch (menu)
    {
        case CREATE_RESOURCE:
            createResource();
            break;

        case SERVER_SET_ATTRIBUTES:
            setAttributes();
            break;

        case SERVER_GET_ATTRIBUTES:
            getAttributes();
            break;

        case DISCOVER_RESOURCE_WITHOUT_QUERY:
            discoverResource();
            break;

        case DISCOVER_RESOURCE_WITH_TYPE_QUERY:
            discoverResourceWithQueryByResType();
            break;

        case DISCOVER_RESOURCE_WITH_TYPES_QUERY:
            discoverResourceWithQueryByResTypes();
            break;

        case DISCOVER_NON_DISCOVERABLE_RESOURCE:
            discoverNonDiscoverableResource();
            break;

        case START_MONITORING:
            startMonitoring();
            break;

        case STOP_MONITORING:
            stopMonitoring();
            break;

        case GET_STATE:
            getState();
            break;

        case SET_REMOTE_ATTRIBUTES:
            setRemoteAttributes();
            break;

        case GET_REMOTE_ATTRIBUTES:
            getRemoteAttributes();
            break;

        case START_CACHING:
            startCaching();
            break;

        case STOP_CACHING:
            stopCaching();
            break;

        case GET_CACHED_ATTRIBUTES:
            getCachedAttributes();
            break;

        case GET_CACHED_STATE:
            getCachedState();
            break;

        case EXIT:
            cout << "Quit from RETestApp..." << endl << endl;
            throw CloseApp();
            break;
    }
}

// This function is for taking input from console and processing input as necessary
int userInputProcessing(int max, int min)
{
    int input;

    cin >> input;
    cout << endl;

    if (!cin.fail() && min <= input && input <= max)
        return input;

    cin.clear();
    cin.ignore(numeric_limits< streamsize >::max(), '\n');
    throw runtime_error("Invalid Input, please try again");
}

// This is the main function the RE Test Application
int main(void)
{
    while (true)
    {
        try
        {
            showMainMenu();

            int menuItem = userInputProcessing(ApplicationMenu::GET_CACHED_STATE,
                    ApplicationMenu::EXIT);

            menuSelection(ApplicationMenu(menuItem));
        }
        catch (const exception &e)
        {
            cout << "[ERROR] " << e.what() << endl;
        }
        catch (const CloseApp &)
        {
            break;
        }
    }

    return 0;
}
