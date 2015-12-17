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

#include "ACNESensor.h"

#include <iostream>
#include <map>
#include <string>
#include <mutex>
#include <cstdlib>
#include <pthread.h>
#include <condition_variable>
#include <stdlib.h>
#include "SysTimer.h"
#include "OCPlatform.h"
#include "OCApi.h"

#define TEMP_LOW_THRESHOLD   35.5
#define TEMP_HIGH_THRESHOLD  37.5
#define MOIST_LOW_THRESHOLD  40
#define MOIST_HIGH_THRESHOLD 60
#define OIL_LOW_THRESHOLD    40
#define OIL_HIGH_THRESHOLD   60

using namespace ACNESensorName;
using namespace OC;

typedef std::map<OCResourceIdentifier, std::shared_ptr<OCResource>> DiscoveredResourceMap;

DiscoveredResourceMap discoveredResources;
std::shared_ptr<OCResource> curResource;
static ObserveType OBSERVE_TYPE_TO_USE = ObserveType::Observe;
std::mutex curResourceLock;

ACNESensor::ACNESensor()
{
    m_temp = "";
    m_moist = "";
    m_oil = "";
    m_outTemp = "";
    m_outHumid = "";
    std::cout << "ACNE sensor is constructed and initialized" << std::endl;
}

ACNESensor::~ACNESensor()
{
}

ACNESensor myACNE;

int ACNESensor::executeACNESensorLogic(std::map<std::string, std::string> *pInputData, std::string *pOutput)
{
    ACNEResult result;

    if (pInputData->find("temp") != pInputData->end())
    {
        m_temp = pInputData->at("temp");
    }

    if (pInputData->find("moist") != pInputData->end())
    {
        m_moist = pInputData->at("moist");
    }

    if (pInputData->find("oil") != pInputData->end())
    {
        m_oil = pInputData->at("oil");
    }

    if (pInputData->find("temperature") != pInputData->end())
    {
        m_outTemp = pInputData->at("temperature");
        std::cout << "Temperature inputted: " << m_outTemp << std::endl;
    }

    if (pInputData->find("humidity") != pInputData->end())
    {
        m_outHumid = pInputData->at("humidity");
        std::cout << "Humidity inputted: " << m_outHumid << std::endl;
    }

    if ((result = makeACNE()) != SUCCESS)
    {
        return -1;
    }

    (*pOutput) = "SUCCESS";

    return 0;
}

/*
 * Make result with inputs
 */
ACNEResult ACNESensor::makeACNE()
{
    std::cout << "ACNE Computation Start." << std::endl;
        
    int out_temp = std::stoi(myACNE.m_outTemp);
    int out_humid = std::stoi(myACNE.m_outHumid);
    int skin_temp = std::stoi(myACNE.m_temp);
    int skin_moist = std::stoi(myACNE.m_moist);
    int skin_oil = std::stoi(myACNE.m_oil);

    // check if time difference between two inputs is valid
    if (0)
    {
        std::cout << "[AcneSensor] : OUTOFDATE" << std::endl;
    }
    else
    {
        std::cout << "Input Result" << std::endl;
        std::cout << "Outside Temperature :" << out_temp << std::endl;
        std::cout << "Outside Humidity :" << out_humid << std::endl;
        std::cout << "Skin Temperature :" << skin_temp << std::endl;
        std::cout << "Skin Moisture :" << skin_moist << std::endl;
        std::cout << "Skin Oil :" << skin_oil << std::endl;

        if (skin_temp >= TEMP_HIGH_THRESHOLD)
        {
            std::cout << "Skin is too hot!" << std::endl;
        }
        else if (skin_temp >= TEMP_LOW_THRESHOLD)
        {
            std::cout << "Skin temperature is good." << std::endl;
        }
        else
        {
            std::cout << "Skin is too cold!" << std::endl;
        }

        if (skin_moist >= MOIST_HIGH_THRESHOLD)
        {
            std::cout << "Skin is well-moisturized." << std::endl;
        }
        else if (skin_moist >= MOIST_LOW_THRESHOLD)
        {
            std::cout << "Skin is a little bit moisturized." << std::endl;
        }
        else
        {
            std::cout << "Skin should be moisturized." << std::endl;
        }

        if (skin_oil >= OIL_HIGH_THRESHOLD)
        {
            std::cout << "Skin has too much oil!" << std::endl;
        }
        else if (skin_oil >= OIL_LOW_THRESHOLD)
        {
            std::cout << "Skin has a bit much oil." << std::endl;
        }
        else
        {
            std::cout << "Skin has less oil." << std::endl;
        }

        if (out_humid >= 60)
        {
            std::cout << "Outside is wet." << std::endl;
        }
        else if (out_humid >= 40)
        {
            std::cout << "Outside has well balanced humidity." << std::endl;
        }
        else
        {
            std::cout << "Outside is dry, so you should moisturize your skin." << std::endl;
        }

        if (out_temp >= 30)
        {
            std::cout << "Outside is hot, so you should consider preventing UV." << std::endl;
        }
        else if (out_temp >= 10)
        {
            std::cout << "Outside has well balanced temperature." << std::endl;
        }
        else
        {
            std::cout << "Outside is cold, so you should moisturize your skin." << std::endl;
        }

        std::cout << std::endl;
    }

    return SUCCESS;
}



/**
 * Displaying the ACNE result from inputs
 */
//void ACNESensor::displayACNE(void)
//{
//   double ACNE

//}

void onGet(const HeaderOptions &, const OCRepresentation &rep, const int eCode)
{
    try
    {
        if (eCode == OC_STACK_OK)
        {
            std::cout << "GET request was successful" << std::endl;
            std::cout << "Resource URI: " << rep.getUri() << std::endl;
            
            if (rep.getUri() == "/OutTempHumSensor")
            {
                rep.getValue("temperature", myACNE.m_outTemp);
                rep.getValue("humidity", myACNE.m_outHumid);
                std::cout << "\ttemperature: " << myACNE.m_outTemp << std::endl;
                std::cout << "\thumidity: " << myACNE.m_outHumid << std:: endl;
            }

            if (rep.getUri() == "/MoistSensor")
            {
                rep.getValue("moist", myACNE.m_moist);
                std::cout << "\tmoist: " << myACNE.m_moist << std::endl;
            }

            if (rep.getUri() == "/TempSensor")
            {
                rep.getValue("temp", myACNE.m_temp);
                std::cout << "\ttemp: " << myACNE.m_temp << std::endl;
            }

            if (rep.getUri() == "/OilSensor")
            {
                rep.getValue("oil", myACNE.m_oil);
                std::cout << "\toil: " << myACNE.m_oil << std::endl;
            }

            if (myACNE.m_outTemp.length() > 0 && myACNE.m_outHumid.length() > 0
                && myACNE.m_temp.length() > 0 && myACNE.m_moist.length() > 0 && myACNE.m_oil.length() > 0)
            {
                myACNE.makeACNE();
            }
        }
        else
        {
            std::cout << "onGET Response error: " << eCode << std::endl;
            std::exit(-1);
        }
    }
    catch(std::exception &e)
    {
        std::cout << "Exception: " << e.what() << " in onGet" << std::endl;
    }
}

void getRepresentation(std::shared_ptr<OCResource> resource)
{
    if (resource)
    {
        std::cout << "Getting... " << std::endl;
        QueryParamsMap test;
        resource->get(test, &onGet);
    }
}

// Callback function
void foundResource(std::shared_ptr<OCResource> resource)
{
    std::cout << "In foundResource\n";
    std::string resourceURI;
    std::string hostAddress;

    try
    {
        {
            std::lock_guard<std::mutex> lock(curResourceLock);
            
            if(discoveredResources.find(resource->uniqueIdentifier()) == discoveredResources.end())
            {
                std::cout << "Found resource " << resource->uniqueIdentifier() << " with ID: " << resource->sid() << std::endl;
                discoveredResources[resource->uniqueIdentifier()] = resource;
            }
            else
            {
                std::cout << "Found resource " << resource->uniqueIdentifier() << " again!" << std::endl;
            }
        }

        if (resource)
        {
            std::cout  << "DISCOVERED Resource: " << std::endl;
            resourceURI = resource->uri();
            std::cout << "\tURI of the resource: " << resourceURI << std::endl;

            hostAddress = resource->host();
            std::cout << "\tHost address of the resource: " << hostAddress << std::endl;

            getRepresentation(resource);
        }
/*
        if (myACNE.m_outTemp.length() > 0 && myACNE.m_outHumid.length() > 0
            && myACNE.m_temp.length() > 0 && myACNE.m_moist.length() > 0 && myACNE.m_oil.length() > 0)
        {
            myACNE.makeACNE();
        }
*/
    }
    catch (std::exception &e)
    {
        std::cout << "reason: " << e.what() << std::endl;
    }
}
    
static FILE* client_open(const char*, const char *mode)
{
    return fopen("./oic_svr_db_client.json", mode);
}

int main(int argc, char *argv[])
{
    std::ostringstream requestURI;
    OCPersistentStorage ps { client_open, fread, fwrite, fclose, unlink };

    PlatformConfig cfg
    {
        OC::ServiceType::InProc,
        OC::ModeType::Both,
        "0.0.0.0",
        0,
        OC::QualityOfService::LowQos,
        &ps
    };

    OCPlatform::Configure(cfg);
    try
    {
        std::cout.setf(std::ios::boolalpha);
        requestURI << OC_RSRVD_WELL_KNOWN_URI;
        OCPlatform::findResource("", requestURI.str(), CT_DEFAULT, &foundResource);
        std::cout << "Finding Resource... " << std::endl;

        OCPlatform::findResource("", requestURI.str(), CT_DEFAULT, &foundResource);

        std::cout << "Finding Resource for second time... " << std::endl;

        std::mutex blocker;
        std::condition_variable cv;
        std::unique_lock<std::mutex> lock(blocker);
        cv.wait(lock);
    }
    catch(std::exception &e)
    {
        oclog() << "Exception in main: " << e.what();
    }
    /*
    if (myACNE.m_outTemp.length() > 0 && myACNE.m_outHumid.length() > 0
        && myACNE.m_temp.length() > 0 && myACNE.m_moist.length() > 0 && myACNE.m_oil.length() > 0)
    {
        myACNE.makeACNE();
    }
    */
    return 0;
}

