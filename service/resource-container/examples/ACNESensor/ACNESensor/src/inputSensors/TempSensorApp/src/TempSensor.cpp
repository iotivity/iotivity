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

#include <TempSensorApp.h>

using namespace OC;
using namespace std;

int g_Observation = 0;

OCEntityHandlerResult entityHandler(std::shared_ptr< OCResourceRequest > request);

/*
 * TempResourceFunctions
 */

void TempResource::registerResource()
{
    uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;

    // This will internally create and register the resource.
    OCStackResult result = OC::OCPlatform::registerResource(m_resourceHandle, m_resourceUri,
                           m_resourceTypes[0], m_resourceInterfaces[0], &entityHandler, resourceProperty);

    if (OC_STACK_OK != result)
    {
        cout << "TempResource creation was unsuccessful\n";
    }
    else
    {
        cout << "TempResource creation is successful" << endl;
    }
}

OCResourceHandle TempResource::getHandle()
{
    return m_resourceHandle;
}

void TempResource::setResourceRepresentation(OCRepresentation &rep)
{
    double tempTemp;

    rep.getValue("temp", tempTemp);

    m_temp = tempTemp;

    cout << "\t" << "Received representation: " << endl;

    cout << "\t" << "temp" << m_temp << endl;

}

OCRepresentation TempResource::getResourceRepresentation()
{
    m_resourceRep.setValue("temp", std::to_string(m_temp));

    cout << "Temp is set to " << m_temp << endl;

    return m_resourceRep;
}

TempResource g_myResource;

void *TestSensorVal(void *param)
{
    (void)param;

    bool bFlag = true;
    int nSleep_time = INTERVAL_FOR_CHECK;
    double nTemp;

    std::cout << "[TempSensorAPP] ::" << __func__ << " is called."
              << std::endl;

    nTemp = INIT_VAL;

    // This function continuously monitors for the changes
    while (1)
    {
        sleep(nSleep_time);

        if (g_Observation)
        {
            if (bFlag)
            {
                nSleep_time = INTERVAL_FOR_MEASUREMENT;
                nTemp += DIFF_VAL;
                g_myResource.m_temp = nTemp;
            }
            else
            {
                nSleep_time = INTERVAL_FOR_CHECK;
            }

            bFlag = bFlag ^ true;

            cout << "\n temp updated to : " << g_myResource.m_temp << endl;
            cout << "Notifying observers with resource handle: " << g_myResource.getHandle()
                 << endl;

            OCStackResult result = OCPlatform::notifyAllObservers(g_myResource.getHandle());


            if (OC_STACK_NO_OBSERVERS == result)
            {
                cout << "No More observers, stopping notifications" << endl;
                g_Observation = 0;
            }
        }
        else
        {
            nSleep_time = INTERVAL_FOR_CHECK ;
        }
    }
    return NULL;
}

OCEntityHandlerResult entityHandler(std::shared_ptr< OCResourceRequest > request)
{
    cout << "\tIn Temp Server CPP entity handler:\n";

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
                // TODO: PUT request operations
            }
            else if (requestType == "POST")
            {
                // TODO: POST request operations
            }
            else if (requestType == "DELETE")
            {
                // TODO: DELETE request operations
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

int main()
{
    // Create PlatformConfig object
    PlatformConfig cfg(OC::ServiceType::InProc, COAP_MODE, COAP_IP, COAP_PORT, OC::QualityOfService::LowQos);

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

    return 0;
}

