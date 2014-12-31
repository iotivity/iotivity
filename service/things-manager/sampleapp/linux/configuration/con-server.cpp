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

///
/// This sample shows how one could create a resource (collection) with children.
///

#include <functional>
#include <thread>

#include "OCPlatform.h"
#include "OCApi.h"
#include "ThingsManager.h"
#include "ConfigurationCollection.h"
#include "DiagnosticsCollection.h"
#include "FactorySetCollection.h"

using namespace OC;
using namespace OIC;

const int SUCCESS_RESPONSE = 0;
int g_Steps = 0;
int isWaiting = 0;

// Default system configuration value's variables
// The variable's names should be same as the names of "extern" variables defined in
// "ConfigurationCollection.h"
std::string defaultRegionValue;
std::string defaultTimeValue;
std::string defaultCurrentTimeValue;
std::string defaultNetworkValue;
std::string defaultIPAddressValue;
std::string defaultSecurityValue;
std::string defaultModeValue;
std::string defaultConfigurationValue;
std::string defaultFactorySetValue;

static ThingsManager* g_thingsmanager;

// Forward declaring the entityHandler (Configuration)
bool prepareResponseForResource(std::shared_ptr< OCResourceRequest > request);
OCStackResult sendResponseForResource(std::shared_ptr< OCResourceRequest > pRequest);
OCEntityHandlerResult entityHandlerForResource(std::shared_ptr< OCResourceRequest > request);

ConfigurationCollection *myConfigurationCollection;
DiagnosticsCollection *myDiagnosticsCollection;
FactorySetCollection *myFactorySetCollection;

typedef std::function< void(OCRepresentation&) > putFunc;
typedef std::function< OCRepresentation(void) > getFunc;

getFunc getGetFunction(std::string uri)
{
    getFunc res = NULL;

    if (uri == myConfigurationCollection->getTimeUri())
    {
        res = std::bind(&ConfigurationCollection::getTimeRepresentation, myConfigurationCollection);
    }
    else if (uri == myConfigurationCollection->getConfigurationUri())
    {
        res = std::bind(&ConfigurationCollection::getConfigurationRepresentation,
                myConfigurationCollection);
    }
    else if (uri == myConfigurationCollection->myTimeCollection->getCurrentTimeUri())
    {
        res = std::bind(&TimeCollection::getCurrentTimeRepresentation,
                myConfigurationCollection->myTimeCollection);
    }
    else if (uri == myConfigurationCollection->getRegionUri())
    {
        res = std::bind(&ConfigurationCollection::getRegionRepresentation,
                myConfigurationCollection);
    }
    else if (uri == myDiagnosticsCollection->getFactoryResetUri())
    {
        res = std::bind(&DiagnosticsCollection::getFactoryResetRepresentation,
                myDiagnosticsCollection);
    }
    else if (uri == myDiagnosticsCollection->getRebootUri())
    {
        res = std::bind(&DiagnosticsCollection::getRebootRepresentation, myDiagnosticsCollection);
    }

    return res;
}

putFunc getPutFunction(std::string uri)
{
    putFunc res = NULL;

    if (uri == myConfigurationCollection->getRegionUri())
    {
        res = std::bind(&ConfigurationCollection::setRegionRepresentation,
                myConfigurationCollection, std::placeholders::_1);
    }
    else if (uri == myConfigurationCollection->myTimeCollection->getCurrentTimeUri())
    {
        res = std::bind(&TimeCollection::setCurrentTimeRepresentation,
                myConfigurationCollection->myTimeCollection, std::placeholders::_1);
    }
    else if (uri == myDiagnosticsCollection->getFactoryResetUri())
    {
        res = std::bind(&DiagnosticsCollection::setFactoryResetRepresentation,
                myDiagnosticsCollection, std::placeholders::_1);
    }
    else if (uri == myDiagnosticsCollection->getRebootUri())
    {
        res = std::bind(&DiagnosticsCollection::setRebootRepresentation, myDiagnosticsCollection,
                std::placeholders::_1);
    }

    return res;
}

// This function prepares a response for any incoming request to Light resource.
bool prepareResponseForResource(std::shared_ptr< OCResourceRequest > request)
{
    std::cout << "\tIn Server CPP prepareResponseForResource:\n";
    bool result = false;
    if (request)
    {
        // Get the request type and request flag
        std::string requestType = request->getRequestType();
        int requestFlag = request->getRequestHandlerFlag();

        if (requestFlag == RequestHandlerFlag::InitFlag)
        {
            std::cout << "\t\trequestFlag : Init\n";

            // entity handler to perform resource initialization operations
        }
        else if (requestFlag == RequestHandlerFlag::RequestFlag)
        {
            std::cout << "\t\trequestFlag : Request\n";

            // If the request type is GET
            if (requestType == "GET")
            {
                std::cout << "\t\t\trequestType : GET\n";
                // GET operations are directly handled while sending the response
                // in the sendLightResponse function
                result = true;
            }
            else if (requestType == "PUT")
            {
                std::cout << "\t\t\trequestType : PUT\n";
                putFunc putFunction;
                OCRepresentation rep = request->getResourceRepresentation();

                putFunction = getPutFunction(request->getResourceUri());

                // Do related operations related to PUT request
                putFunction(rep);
                result = true;
            }
            else if (requestType == "POST")
            {
                // POST request operations
            }
            else if (requestType == "DELETE")
            {
                // DELETE request operations
            }
        }
        else if (requestFlag == RequestHandlerFlag::ObserverFlag)
        {
            std::cout << "\t\trequestFlag : Observer\n";
        }
    }
    else
    {
        std::cout << "Request invalid" << std::endl;
    }

    return result;
}

OCStackResult sendResponseForResource(std::shared_ptr< OCResourceRequest > pRequest)
{
    auto pResponse = std::make_shared< OC::OCResourceResponse >();

    // Check for query params (if any)
    QueryParamsMap queryParamsMap = pRequest->getQueryParameters();

    pResponse->setRequestHandle(pRequest->getRequestHandle());
    pResponse->setResourceHandle(pRequest->getResourceHandle());

    getFunc getFunction;
    getFunction = getGetFunction(pRequest->getResourceUri());

    OCRepresentation rep;
    rep = getFunction();

    auto findRes = queryParamsMap.find("if");

    if (findRes != queryParamsMap.end())
    {
        pResponse->setResourceRepresentation(rep, findRes->second);
    }
    else
    {
        pResponse->setResourceRepresentation(rep, DEFAULT_INTERFACE);
    }

    pResponse->setErrorCode(200);
    pResponse->setResponseResult(OC_EH_OK);

    return OCPlatform::sendResponse(pResponse);
}

OCEntityHandlerResult entityHandlerForResource(std::shared_ptr< OCResourceRequest > request)
{
    std::cout << "\tIn Server CPP (entityHandlerForResource) entity handler:\n";
    OCEntityHandlerResult ehResult = OC_EH_ERROR;

    if (prepareResponseForResource(request))
    {
        if (OC_STACK_OK == sendResponseForResource(request))
        {
            ehResult = OC_EH_OK;
        }
        else
        {
            std::cout << "sendResponse failed." << std::endl;
        }
    }
    else
    {
        std::cout << "PrepareResponse failed." << std::endl;
    }
    return ehResult;
}

// callback handler on GET request
void onBootstrap(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode)
{
    if (eCode == SUCCESS_RESPONSE)
    {
        std::cout << "\n\nGET request was successful" << std::endl;
        std::cout << "\tResource URI: " << rep.getUri() << std::endl;

        defaultRegionValue = rep.getValue< std::string >("regionValue");
        defaultTimeValue = rep.getValue< std::string >("timeValue");
        defaultCurrentTimeValue = rep.getValue< std::string >("currentTimeValue");
        defaultNetworkValue = rep.getValue< std::string >("networkValue");
        defaultIPAddressValue = rep.getValue< std::string >("IPAddressValue");
        defaultSecurityValue = rep.getValue< std::string >("securityValue");
        defaultModeValue = rep.getValue< std::string >("modeValue");
        defaultConfigurationValue = rep.getValue< std::string >("configurationValue");
        defaultFactorySetValue = rep.getValue< std::string >("factorySetValue");

        std::cout << "\tregionValue : " << defaultRegionValue << std::endl;
        std::cout << "\ttimeValue : " << defaultTimeValue << std::endl;
        std::cout << "\tcurrentTimeValue : " << defaultCurrentTimeValue << std::endl;
        std::cout << "\tnetworkValue : " << defaultNetworkValue << std::endl;
        std::cout << "\tIPAddressValue : " << defaultIPAddressValue << std::endl;
        std::cout << "\tsecurityValue : " << defaultSecurityValue << std::endl;
        std::cout << "\tmodeValue : " << defaultModeValue << std::endl;
        std::cout << "\tconfigurationValue : " << defaultConfigurationValue << std::endl;
        std::cout << "\tfactorySetValue : " << defaultFactorySetValue << std::endl;

    }
    else
    {
        std::cout << "onGET Response error: " << eCode << std::endl;
        std::exit(-1);
    }
    isWaiting = 0;
}

int main()
{
    //**************************************************************
    // STEP 0
    // Create PlatformConfig object
    PlatformConfig cfg
    { OC::ServiceType::InProc, OC::ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos };

    OCPlatform::Configure(cfg);
    g_thingsmanager = new ThingsManager();
    //**************************************************************

    if (getuid() != 0)
    {
        std::cout << "NOTE: You may gain the root privilege (e.g, reboot)\n";
        std::cout << "NOTE: Now, you don't have it.\n";
    }

    try
    {
        // Perform app tasks
        while (true)
        {

            if (isWaiting > 0)
                continue;

            isWaiting = 0;

            std::cout << endl << endl << "(0) Quit" << std::endl;
            std::cout << "(1) Bootstrap" << std::endl;
            std::cout << "(2) Create Configuration Resources" << std::endl;

            cin >> g_Steps;
            //
            if (g_Steps == 0)
                break;
            else if (g_Steps == 1)
            {
                if( g_thingsmanager->doBootstrap(&onBootstrap) == OC_STACK_OK)
                    isWaiting = 1;
                else
                    std::cout << "A callback pointer of the function is NULL." << std::endl;
            }
            else if (g_Steps == 2)
            {
                myConfigurationCollection = new ConfigurationCollection();
                myConfigurationCollection->createResources(&entityHandlerForResource);

                myDiagnosticsCollection = new DiagnosticsCollection();
                myDiagnosticsCollection->createResources(&entityHandlerForResource);

                myFactorySetCollection = new FactorySetCollection();
                myFactorySetCollection->createResources(&entityHandlerForResource);
                myDiagnosticsCollection->factoryReset = std::function < void()
                        > (std::bind(&ConfigurationCollection::factoryReset,
                                myConfigurationCollection));
                isWaiting = 1;
            }
        }
    }
    catch (OCException e)
    {
        std::cout << "Exception in main: " << e.what();
    }

    // No explicit call to stop the platform.
    // When OCPlatform destructor is invoked, internally we do platform cleanup
}

