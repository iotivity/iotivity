/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include <ThingResourceServer.h>

int g_Observation = 0;

void entityHandler(std::shared_ptr< OCResourceRequest > request,
                   std::shared_ptr< OCResourceResponse > response);

/*
 * TempResourceFunctions
 */

void TemphumidResource::registerResource(OC::OCPlatform &platform)
{
    string resourceURI = m_resourceUri; // URI of the resource
    string resourceTypeName = COAP_TYPE_NAME; // resource type name.
    string resourceInterface = DEFAULT_INTERFACE; // resource interface.

    uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;

    // This will internally create and register the resource.
    OCStackResult result = platform.registerResource(m_resourceHandle, resourceURI,
                           resourceTypeName, resourceInterface, &entityHandler, resourceProperty);

    if (OC_STACK_OK != result)
    {
        cout << "Resource creation was unsuccessful\n";
    }
}

OCResourceHandle TemphumidResource::getHandle()
{
    return m_resourceHandle;
}

void TemphumidResource::setRepresentation(AttributeMap &attributeMap)
{
    cout << "\t\t\t" << "Received representation: " << endl;
    cout << "\t\t\t\t" << "temp: " << attributeMap["temp"][0] << endl;
    cout << "\t\t\t\t" << "humid: " << attributeMap["humid"][0] << endl;

    m_temp = std::stoi(attributeMap["temp"][0]);
    m_humid = std::stoi(attributeMap["humid"][0]);
}

void TemphumidResource::getRepresentation(AttributeMap &attributeMap)
{
    attributeMap["0"].push_back("temperature");
    attributeMap["1"].push_back("int");

    attributeMap["2"].push_back(to_string(m_temp));

    attributeMap["3"].push_back("humidity");
    attributeMap["4"].push_back("int");

    attributeMap["5"].push_back(to_string(m_humid));
}

TemphumidResource g_myResource;

void *TestSensorVal(void *param)
{
    g_myResource.m_temp = 27;
    g_myResource.m_humid = 48;

    // This function continuously monitors for the changes
    while (1)
    {
        sleep(5);

        if (g_Observation)
        {
            g_myResource.m_temp += 1;
            g_myResource.m_humid -= 1;

            cout << "\ntemp updated to : " << g_myResource.m_temp << endl;
            cout << "\nhumid updated to : " << g_myResource.m_humid << endl;
            cout << "Notifying observers with resource handle: " << g_myResource.getHandle()
                 << endl;

            OCStackResult result = OCPlatform::notifyObservers(g_myResource.getHandle());

            if (OC_STACK_NO_OBSERVERS == result)
            {
                cout << "No More observers, stopping notifications" << endl;
                g_Observation = 0;
            }
        }
    }
    return NULL;
}

void entityHandler(std::shared_ptr< OCResourceRequest > request,
                   std::shared_ptr< OCResourceResponse > response)
{
    cout << "\tIn Server CPP entity handler:\n";

    if (request)
    {
        // Get the request type and request flag
        std::string requestType = request->getRequestType();
        RequestHandlerFlag requestFlag = request->getRequestHandlerFlag();

        if (requestFlag == RequestHandlerFlag::InitFlag)
        {
            cout << "\t\trequestFlag : Init\n";

            // entity handler to perform resource initialization operations
        }
        else if (requestFlag == RequestHandlerFlag::RequestFlag)
        {
            cout << "\t\trequestFlag : Request\n";

            // If the request type is GET
            if (requestType == "GET")
            {
                cout << "\t\t\trequestType : GET\n";

                // Check for query params (if any)
                QueryParamsMap queryParamsMap = request->getQueryParameters();

                // Process query params and do required operations ..

                // Get the representation of this resource at this point and send it as response
                OCRepresentation rep = request->getResourceRepresentation();
                AttributeMap attributeMap;

                g_myResource.getRepresentation(attributeMap);

                if (response)
                {
                    // TODO Error Code
                    rep.setAttributeMap(attributeMap);
                    response->setErrorCode(200);
                    response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
                }
            }
            else if (requestType == "PUT")
            {
                // PUT request operations
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
}

int main()
{
    // Create PlatformConfig object
    PlatformConfig cfg(COAP_SRVTYPE, COAP_MODE, COAP_IP, COAP_PORT, QualityOfService::NonConfirmable);

    try
    {
        OCPlatform platform(cfg);

        g_myResource.registerResource(platform);

        int input = 0;
        cout << "Type any key to terminate" << endl;
        cin >> input;
    }
    catch (std::exception e)
    {
        cout << e.what() << endl;
    }
}
