//******************************************************************
//
// Copyright 2015 Duck Ju Kim (Seoul National University) All Rights Reserved.
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

#include "AcneSensor.h"

#include <iostream>
#include <stdlib.h>
#include "SysTimer.h"

#ifdef __ANDROID__
#include "OCAndroid.h"
#endif

using namespace AcneSensorName;
using namespace OC;
using namespace std;

int g_Observation = 0;

OCEntityHandlerResult entityHandler(std::shared_ptr< OCResourceRequest > request);

AcneSensor::AcneSensor()
{
    // m_humid = 0;
    // m_temp = 0;
    // m_resourceHandle(0);
    m_temperature = "";
    m_humidity = "";
    m_AcneResult = "";
    time(&m_timepstampT);
    time(&m_timepstampH);

    m_resourceUri = "/Global_TempHumSensor";
    m_resourceTypes.push_back(RESOURCE_TYPE_NAME_TEMP);
    m_resourceTypes.push_back(RESOURCE_TYPE_NAME_HUMID);
    m_resourceInterfaces.push_back(OC::DEFAULT_INTERFACE);
    printf("Running thing as %s\n", m_resourceUri.c_str());
    m_resourceRep.setUri(m_resourceUri);
}

AcneSensor::~AcneSensor()
{

}

void AcneSensor::registerResource()
{
    uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;

    OCStackResult result = OC::OCPlatform::registerResource(m_resourceHandle, m_resourceUri, m_resourceTypes[0], m_resourceInterfaces[0], &entityHandler, resourceProperty);

    if (OC_STACK_OK != result)
    {
        cout << "Resource creation was unsuccessful\n";
    }

    result = OCPlatform::bindTypeToResource(m_resourceHandle, m_resourceTypes[1]);

    if (OC_STACK_OK != result)
    {
        cout << "Binding TypeName to Resource was unsuccessful\n";
    }
}

OCResourceHandle AcneSensor::getHandle()
{
    return m_resourceHandle;
}

void AcneSensor::setResourceRepresentation(OCRepresentation &rep)
{
    int tempHumid;
    int tempTemp;

    rep.getValue("humidity", tempTemp);
    rep.getValue("temperature", tempHumid);

    m_humidity = std::to_string(tempHumid);
    m_temperature = std::to_string(tempTemp);

    cout << "\t\t\t" << "Received representation: " << endl;
    cout << "\t\t\t\t" << "temp: " << m_humidity << endl;
    cout << "\t\t\t\t" << "humid: " << m_temperature << endl;
}

OCRepresentation AcneSensor::getResourceRepresentation()
{

    // This representation is temporaily for soft-sensor-management - name, type, vale
    m_resourceRep.setValue("temperature", m_temperature);
    m_resourceRep.setValue("humidity", m_humidity);

    return m_resourceRep;
}

// Create the instance of the TemphumidResource class
AcneSensor g_myResource;

void *TestSensorVal(void *param)
{
    (void)param;

    g_myResource.m_temperature = "27";
    g_myResource.m_humidity = "60";

    // This function continuously monitors for the changes
    while (1)
    {
        sleep(5);

        if (g_Observation)
        {
            // g_myResource.m_temperature += 1;
            // g_myResource.m_humidity -= 1;

            cout << "\ntemp updated to : " << g_myResource.m_temperature << endl;
            cout << "\nhumid updated to : " << g_myResource.m_humidity << endl;
            cout << "Notifying observers with resource handle: " << g_myResource.getHandle()
                 << endl;

            OCStackResult result = OCPlatform::notifyAllObservers(g_myResource.getHandle());

            if (OC_STACK_NO_OBSERVERS == result)
            {
                cout << "No More observers, stopping notifications" << endl;
                g_Observation = 0;
            }
        }
    }
    return NULL;
}

OCEntityHandlerResult entityHandler(std::shared_ptr< OCResourceRequest > request)
{
    cout << "\tIn Server CPP entity handler:\n";

    auto response = std::make_shared<OC::OCResourceResponse>();

    if (request)
    {
        // Get the request type and request flag
        std::string requestType = request->getRequestType();
        int requestFlag = request->getRequestHandlerFlag();

        response->setRequestHandle(request->getRequestHandle());
        response->setResourceHandle(request->getResourceHandle());

        if (requestFlag & RequestHandlerFlag::RequestFlag)
        {
            cout << "\t\trequestFlag : Request\n";

            // If the request type is GET
            if (requestType == "GET")
            {
                cout << "\t\t\trequestType : GET\n";

                // Check for query params (if any)
                // Process query params and do required operations ..

                // Get the representation of this resource at this point and send it as response
                OCRepresentation rep = g_myResource.getResourceRepresentation();

                if (response)
                {
                    // TODO Error Code
                    response->setErrorCode(200);
                    response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
                }
            }
            else if (requestType == "PUT")
            {
                // TODO PUT request operations
            }
            else if (requestType == "POST")
            {
                // TODO POST request operations
            }
            else if (requestType == "DELETE")
            {
                // TODO DELETE request operations
            }
        }

        if (requestFlag & RequestHandlerFlag::ObserverFlag)
        {
            pthread_t threadId;

            cout << "\t\trequestFlag : Observer\n";
            g_Observation = 1;

            static int startedThread = 0;

            if (!startedThread)
            {
                pthread_create(&threadId, NULL, TestSensorVal, (void *) NULL);
                startedThread = 1;
            }
        }
    }
    else
    {
        std::cout << "Request invalid" << std::endl;
    }

    return OCPlatform::sendResponse(response) == OC_STACK_OK ? OC_EH_OK : OC_EH_ERROR;
}

int AcneSensor::executeAcneSensorLogic(std::map<std::string, std::string> *pInputData,
                                     std::string *pOutput)
{
    AcneResult result;

    if (pInputData->find("temperature") != pInputData->end())
    {
        m_temperature = pInputData->at("temperature");
        time(&m_timepstampT);
    }

    if (pInputData->find("humidity") != pInputData->end())
    {
        m_humidity = pInputData->at("humidity");
        time(&m_timepstampH);
    }

    if ((result = makeAcne()) != SUCCESS)
    {
        return -1;
    }

    (*pOutput) = m_AcneResult;

    return 0;
}

/**
 * Outside Temperature Check
 */
AcneResult AcneSensor::makeAcne()
{
    double timediffsecond;
    int AcneResult;
    
    int t_emp = stoi(m_temperature);
    int h_umid = stoi(m_humidity);
    
    timediffsecond = abs(difftime(m_timepstampT, m_timepstampH));

    // Create PlatformConfig object
    PlatformConfig cfg(COAP_SRVTYPE, COAP_MODE, COAP_IP, COAP_PORT, OC::QualityOfService::LowQos);

    try
    {
        OC::OCPlatform::Configure(cfg);

        OC::OCPlatform::startPresence(60);

        g_myResource.registerResource();

        int input = 0;
        cout << "Type any key to terminate" << endl;
        cin >> input;

        OC::OCPlatform::stopPresence();
    }
    catch (std::exception e)
    {
        cout << e.what() << endl;
    }

    // check if time difference between two inputs is valid
    if (timediffsecond > DIFFTIME)
    {
        AcneResult = INVALID;
        cout << "[AcneSensor] :  OUTOFDATE: " << AcneResult << endl;
    }
    else
    {
        cout << "Outside Climate" << endl;
        cout << "Temperature :" << t_emp << ", Humidity :" << h_umid << endl;

        cout << "[result] Weather Alert" << endl;
        if (t_emp >= 40)
        {
            AcneResult = (int)TOOHOT;
            cout << "Danger : Severe Heat Wave!!" << endl;
        }
        else if (t_emp >= 35)
        {
            AcneResult = (int)VERYHOT;
            cout << "Warning : Heat Wave!" << endl;
        }
        else if (t_emp >= 30)
        {
            AcneResult = (int)HOT;
            cout << "Caution : Weak Heat Wave." << endl;
        }
        else if (t_emp >= 15)
        {
            AcneResult = (int)WARM;
            cout << "Safe : Warm for Outgoing." << endl;
        }
        else if (t_emp >= 0)
        {
            AcneResult = (int)COOL;
            cout << "Safe : Cool for Outgoing." << endl;
        }
        else if (t_emp >= -10)
        {
            AcneResult = (int)COLD;
            cout << "Caution : Weak Cold Wave." << endl;
        }
        else if (t_emp >= -15)
        {
            AcneResult = (int)VERYCOLD;
            cout << "Warning : Cold Wave!" << endl;
        }
        else
        {
            AcneResult = (int)TOOCOLD;
            cout << "Danger : Severe Cold Wave!!" << endl;
        }
        
        if (h_umid < 0)
        {
            cout << "Invalid humidity value." << endl;
        }
        else if (h_umid <= 30)
        {
            cout << "Warning : Very Dry Condition." << endl;
        }
        else
        {
            cout << "Great!" << endl;
        }

        cout << endl;
    }

    m_AcneResult = std::to_string(AcneResult);

    return SUCCESS;
}
