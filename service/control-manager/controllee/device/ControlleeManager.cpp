//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

#include "ControlleeManager.h"
#include "IContext.h"
#include "CDContext.h"
#include "OCApi.h"
#include "ISubscriptionManager.h"
#include "CDResourceHandlerFactory.h"
#include "CDSerializationFactory.h"
#include "ResourceHandlerFactory.h"
#include "SerializableDataFactory.h"
#include "NetUtils.h"
#include "log.h"

using namespace OC;
using namespace std;

#define DEFAULT_PLATFORM_IP "127.0.0.1"
#define DEFAULT_PLATFORM_PORT 9999
#define DEFAULT_PLATFORM_SERVICE_TYPE OC::ServiceType::InProc
#define DEFAULT_PLATFORM_MODE_TYPE OC::ModeType::Both
#define DEFAULT_PLATFORM_QOS OC::QualityOfService::LowQos
#define MODULE_ID 00 // Keep the logger in the trylog macro sane
#define PRESENCE_TTL 30

/*
 * Constructor - Default
 */
ControlleeManager::ControlleeManager()
{
    _controllee = MyDevice::getInstance();
    _context = new CDContext();
    _configuration = new OC::Cm::Configuration();

    if ((_context == NULL) || (_configuration == NULL)
        || (_controllee == NULL))
    {
        CE(cout << "Device creation failed, cannot continue" << endl);
        //Have to exit the application
    }

    if (!init_private())
    {
        CE(cout << "Device Init failed in" << __PRETTY_FUNCTION__ << endl);
        //Have to exit the application
    }

    //Default setting for the OCPlatform
    PlatformConfig ocConfig(DEFAULT_PLATFORM_SERVICE_TYPE,
                            DEFAULT_PLATFORM_MODE_TYPE, DEFAULT_PLATFORM_IP,
                            DEFAULT_PLATFORM_PORT, DEFAULT_PLATFORM_QOS);
    OC::OCPlatform::Configure(ocConfig);

    _status = PlatformInitComplete;

    CE(cout << "CM()" << endl);

}

/*
 * Constructor - Parameterised, with IP as input
 */
ControlleeManager::ControlleeManager(const string &ipAddress)
{
    _controllee = MyDevice::getInstance();
    _context = new CDContext();
    _configuration = new OC::Cm::Configuration();

    if ((_context == NULL) || (_configuration == NULL)
        || (_controllee == NULL))
    {
        CE(cout << "Device creation failed, cannot continue" << endl);
        //Have to exit the application

    }
    if (!init_private())
    {
        CE(cout << "Device Init failed in" << __PRETTY_FUNCTION__ << endl);
        //Have to exit the application
    }

    if (!isValidIPAddress(ipAddress))
    {
        CE(cout << "Malformed IP Address" << endl);
        //Have to exit the application
    }
    PlatformConfig ocConfig(DEFAULT_PLATFORM_SERVICE_TYPE,
                            DEFAULT_PLATFORM_MODE_TYPE, ipAddress, DEFAULT_PLATFORM_PORT,
                            DEFAULT_PLATFORM_QOS);
    OC::OCPlatform::Configure(ocConfig);

    _status = PlatformInitComplete;

}

/*
 * Constructor - Parameterised, with Many input parameters
 */
ControlleeManager::ControlleeManager(const PServiceType &serviceType,
                                     const PModeType &modeType, const string &ipAddress,
                                     const uint16_t &port, const PQualityOfService &qualityOfService)

{
    _controllee = MyDevice::getInstance();
    _context = new OC::CDContext(this);

    _configuration = new OC::Cm::Configuration();

    if ((_context == NULL) || (_configuration == NULL)
        || (_controllee == NULL))
    {
        CE(cout << "Device creation failed, cannot continue" << endl);
        //Have to exit the application

    }
    if (!init_private())
    {
        CE(cout << "Device Init failed in" << __PRETTY_FUNCTION__ << endl);
        //Have to exit the application
    }

    bool valid = validateOCParams(serviceType, modeType, ipAddress, port,
                                  qualityOfService);

    PlatformConfig ocConfig((ServiceType) serviceType, (ModeType) modeType,
                            ipAddress, port, (QualityOfService) qualityOfService);
    OC::OCPlatform::Configure(ocConfig);

    _status = PlatformInitComplete;
}

/*
 * Constructor - Get Instance of CE Manager from context. This is a
 *  warped way of doing it right now. Should re-visit later to untangle the
 *  flow
 */
ControlleeManager *
ControlleeManager::getInstance(OC::Cm::IContext *aContext)
{

    OC::CDContext *_cdContext = dynamic_cast<OC::CDContext *> (aContext);
    if (_cdContext)
    {
        return _cdContext->p_cm;
    }
    else
    {
        return NULL;
    }
}

/*
 * init_private - Private Init helper
 */
bool ControlleeManager::init_private()
{
    CE(cout << "ControlleeManager::init_private() => called" << endl);
    OC::CDContext *_cdContext = (OC::CDContext *) _context;

    if (_configuration != NULL)
    {
        _configuration->setResourceHandlerFactory(
            new CDResourceHandlerFactory());
        _configuration->setSerializableDataFactory(
            new CDSerializationFactory());
    }
    else
    {
        CE(cout << "Configuration NULL in" << __PRETTY_FUNCTION__ << endl);
        return false;
    } CE(cout << "Init done in" << __PRETTY_FUNCTION__ << endl);
    _status = DeviceInitComplete;
    return true;
}

/*
 * init - Should go away in clean up.
 */
bool ControlleeManager::init()
{
    CE(cout << "ControlleeManager::init() => called .. remove does nothign now" << endl);

    return true;
}

/*
 * getConfiguration - Provide the current configuration when requested
 */
OC::Cm::Configuration *
ControlleeManager::getConfiguration()
{
    return _configuration;
}

/*
 * start - Once everything is initilaised, we are ready to start the framework
 */
bool ControlleeManager::start()
{
    CE(cout << "ControlleeManager::start() => called" << endl);
    if (_status != ReadyToGo)
    {
        CE(cout << "Please finish device Init and resource add before starting the framework in" << __PRETTY_FUNCTION__ << endl);
        return false;
    }
    OC::CDContext *_cdContext = dynamic_cast<OC::CDContext *> (_context);
    if (_cdContext)
    {
        _cdContext->p_cm = this;
        _cdContext->init();

        if (_cdContext->p_subscriptionManager != NULL)
        {
            // Initialize Subscription Manager
            string dbPath = _configuration->getSubscriptionDbPath();
            _cdContext->p_subscriptionManager->init(_context, dbPath);
        }
        if (_cdContext->p_server != NULL)
        {
            _cdContext->p_server->initialize(_configuration);
        }
        // Start up Presence
        using namespace OC::OCPlatform;
        startPresence(PRESENCE_TTL);
    }
    else
    {
        return false;
    }
    _status = ReadyToGo;
    return true;
}

/*
 * Stop - Clear the Subscription Mechanism and stop advertising presence
 */
bool ControlleeManager::stop()
{
    CE(printf("ControlleeManager::stop() => called"));
    OC::CDContext *_cdContext = (OC::CDContext *) _context;
    bool bRet = false;

    if ((_cdContext) && (_cdContext->p_subscriptionManager))
    {
        bRet = _cdContext->p_subscriptionManager->deinit();
    }

    using namespace OC::OCPlatform;
    stopPresence();

    return bRet;
}


/*
 * getSubscriptionManager - Get the subscription manager associated with this instance
 */
OC::Cm::Notification::ISubscriptionManager *
ControlleeManager::getSubscriptionManager()
{
    CE(cout << "ControlleeManager::getSubscriptionManager() => called" << endl);
    OC::CDContext *_cdContext = (OC::CDContext *) _context;
    if (_cdContext)
    {
        return _cdContext->p_subscriptionManager;
    }
    else
    {
        return NULL;
    }
}

/*
 * setSubscriptionManager - Set the subscription manager associated with this instance
 */
bool ControlleeManager::setSubscriptionManager(
    OC::Cm::Notification::ISubscriptionManager &subscriptionManager)
{

    if ((&subscriptionManager) == NULL)
    {
        CE(cout << "Invalid input in" << __PRETTY_FUNCTION__ << endl);
        return false;
    }

    OC::CDContext *_cdContext = (OC::CDContext *) _context;
    if (_cdContext)
    {
        _cdContext->p_subscriptionManager = &subscriptionManager;
    }
    return true;
}

/*
 * getMyDevice -Get device for device specific operations
 */
::MyDevice *
ControlleeManager::getMyDevice()
{
    return _controllee;
}

bool ControlleeManager::setConfiguration(OC::Cm::Configuration *config)
{
    if (config == NULL)
    {
        CE(cout << "Invalid input in" << __PRETTY_FUNCTION__ << endl);
        return false;
    }
    if ((_configuration != NULL) && (_configuration != config))
    {
        delete _configuration;
        _configuration = NULL;
    }
    _configuration = config;
    return true;
}

/*
 * addResources -Will add a list of resources and register them with the framework.
 */
ControlleeStatus ControlleeManager::addResources(
    const list<ResourceTypeEnum> &resourceTypes, int &posOfError)
{
    ControlleeStatus status = ControlleeError;
    posOfError = 0;
    if (_status != PlatformInitComplete)
    {
        return status;
    }
    if (resourceTypes.empty())
    {
        return status;
    }
    for (list<ResourceTypeEnum>::const_iterator it = resourceTypes.begin();
         it != resourceTypes.end(); ++it)
    {
        posOfError++;
        string resourceString = getResourceString(*it);
        if (resourceString == "invalid")
        {
            CE(cout << "Invalid resource encountered" << __PRETTY_FUNCTION__ << endl);
            return status;
        }
        if (!(_controllee->setSupportedResourceType(resourceString)))
        {
            CE(cout << "Unsupported resource encountered" << __PRETTY_FUNCTION__ << endl);
            return status;
        }
        OCResourceHandle ocHandle;
        string uri = getURI(resourceString);
        vector<string> oicTypes;
        string ocTypes = getoicTypes(resourceString);
        oicTypes.push_back(ocTypes);
        vector<string> ocInterfaces;
        ocInterfaces.push_back(DEFAULT_INTERFACE);

        OCStackResult result = OCPlatform::registerResource(ocHandle, uri,
                               ocTypes, ocInterfaces[0], &CDServer::handleOCRequest,
                               OC_ACTIVE);

        if (result != OC_STACK_OK)
        {
            CE(cout << "Registering with platform failed" << __PRETTY_FUNCTION__ << endl);
            return status;
        }

        _uriHandleMap.insert(pair<string, OCResourceHandle>(uri, ocHandle));

    }
    _status = ReadyToGo;
    return _status;
}

/*
 * validateOCParams -Helper function to validate the OC parameters
 */
bool ControlleeManager::validateOCParams(const PServiceType &serviceType,
        const PModeType &modeType, const string &ipAddress, const int &port,
        const PQualityOfService &qos)
{
    bool retVal = true;

    if (serviceType >= MaxServiceType)
    {
        retVal = false;
    }
    if (modeType >= MaxModeType)
    {
        retVal = false;
    }
    if (!(isValidIPAddress(ipAddress)))
    {
        retVal = false;
    }
    if ((port < 0) || (port > 65535))
    {
        retVal = false;
    }
    if (qos >= MaxQualityOfService)
    {
        retVal = false;
    }

    return retVal;

}

/*
 * getURI -Modularization to localise URI format changes
 */
string ControlleeManager::getURI(const string &resourceString)
{
    string output = "/";
    string devices = "devices/{id}/";

    if (resourceString == "Capability")
    {
        output += resourceString;
    }
    else
    {
        output += devices;
        output += resourceString;
    }
    return output;

}

/*
 * getoicTypes -Modularization to localise URI format changes
 */
string ControlleeManager::getoicTypes(const string &resourceString)
{
    /**
     *  ToDo
     *  Do we prepend core. to the resource string?  For now assume no.
     string output = "core.";
     output += resourceString;
     **/
    return resourceString;

}

/*
 * isValidIPAddress -Helper to validate IP address
 */
bool ControlleeManager::isValidIPAddress(std::string address)
{
    if (address.empty() == true)
    {
        return false;
    }

    return pal_validateIPAddress(address);
}

/*
 * getContext -Get the context associated with this Controllee Manager.
 */
OC::Cm::IContext *ControlleeManager::getContext()
{
    return _context;
}

ControlleeManager::~ControlleeManager()
{

    CE(cout << "Destructor called" << __PRETTY_FUNCTION__ << endl);
    if (_context != NULL)
    {
        OC::CDContext *_cdContext = (OC::CDContext *) _context;

        if (_cdContext)
        {
            if (_cdContext->p_server != NULL)
            {
                _cdContext->p_server->deinit();
            }
        }
        delete _context;
    }


    if (_configuration != NULL)
    {

        OC::Cm::Server::ResourceHandler::ResourceHandlerFactory *tHandlerfactory =
            _configuration->getResourceHandlerFactory();
        if (tHandlerfactory)
        {
            delete (tHandlerfactory); // NOTE that the handler entity in _configuration will be left dangling as it
            //does not implement a deep copy.
        }
        OC::Cm::Serialization::SerializableDataFactory *tSDfactory =
            _configuration->getSerializableDataFactory();
        if (tSDfactory)
        {
            delete (tSDfactory); // NOTE that the handler entity in _configuration will be left dangling as it
        } //does not implement a deep copy.
        delete _configuration;
    }

}

