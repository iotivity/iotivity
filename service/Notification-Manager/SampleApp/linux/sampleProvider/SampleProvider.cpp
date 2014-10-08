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

#include <functional>

#include <pthread.h>

#include "OCPlatform.h"
#include "OCApi.h"

#include <memory>

using namespace OC;
using namespace std;

int g_Observation = 0;

void entityHandler(std::shared_ptr< OCResourceRequest > request ,
        std::shared_ptr< OCResourceResponse > response);

class LightResource
{
public:
    bool m_state;
    int m_power;

    int m_temp;
    int m_humid;

    std::string m_uri;
    OCResourceHandle m_resourceHandle;

public:
    LightResource() :
            m_state(false), m_power(0), m_temp(0), m_humid(0), m_uri("/a/TempHumSensor")
    {
    }

    void createResource(OC::OCPlatform& platform)
    {
        std::string resourceURI = "/a/NM/TempHumSensor";
        std::string resourceTypeName = "NotificationManager.Hosting";
        std::string resourceInterface = DEFAULT_INTERFACE;

        m_uri = resourceURI;

        uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;

        OCStackResult result = platform.registerResource(m_resourceHandle , resourceURI ,
                resourceTypeName , resourceInterface , &entityHandler , resourceProperty);

        if(OC_STACK_OK != result)
        {
            cout << "Resource creation was unsuccessful\n";
        }
    }

    OCResourceHandle getHandle()
    {
        return m_resourceHandle;
    }

    void setRepresentation(AttributeMap& attributeMap)
    {
        cout << "\t\t\t" << "Received representation: " << endl;
        cout << "\t\t\t\t" << "temp: " << attributeMap["temperature"][0] << endl;
        cout << "\t\t\t\t" << "humid: " << attributeMap["humidity"][0] << endl;
        m_temp = std::stoi(attributeMap["temperature"][0]);
        m_humid = std::stoi(attributeMap["humidity"][0]);
    }

    OCRepresentation getRepresentation()
    {
        OCRepresentation temp;

        temp.setUri(m_uri);

        std::vector< std::string > interfaces;

        temp.setResourceInterfaces(interfaces);

        std::vector< std::string > types;

        temp.setResourceTypes(types);

        AttributeMap attributeMap;

        AttributeValues tempVal;
        AttributeValues humidVal;

        tempVal.push_back(to_string(m_temp));
        humidVal.push_back(to_string(m_humid));

        attributeMap["temperature"] = tempVal;
        attributeMap["humidity"] = humidVal;

        temp.setAttributeMap(attributeMap);

        return temp;
    }

    void getRepresentation(AttributeMap& attributeMap)
    {
        AttributeValues tempVal;
        AttributeValues humidVal;

        tempVal.push_back(to_string(m_temp));
        humidVal.push_back(to_string(m_humid));

        attributeMap["temperature"] = tempVal;
        attributeMap["humidity"] = humidVal;
    }
};

LightResource myResource;

void *ChangeLightRepresentation(void *param)
{
    if(g_Observation)
    {

        cout << endl;
        cout << "========================================================" << endl;
        cout << "HUMTepm updated to : " << myResource.m_temp << endl;
        cout << "Notifying observers with resource handle: " << myResource.getHandle() << endl;

        cout << endl;
        cout << "========================================================" << endl;
        cout << "Send data : \n";
        cout << "Attribute Name: Temp\tvalue: " << myResource.m_temp << endl;
        cout << "Attribute Name: Humid\tvalue: " << myResource.m_humid << endl;

        OCStackResult result = OCPlatform::notifyObservers(myResource.getHandle());

        if(OC_STACK_NO_OBSERVERS == result)
        {
            cout << "No More observers, stopping notifications" << endl;
            g_Observation = 0;
        }
    }

    return NULL;
}

void entityHandler(std::shared_ptr< OCResourceRequest > request ,
        std::shared_ptr< OCResourceResponse > response)
{

    if(request)
    {
        std::string requestType = request->getRequestType();
        RequestHandlerFlag requestFlag = request->getRequestHandlerFlag();

        if(requestFlag == RequestHandlerFlag::InitFlag)
        {
        }
        else if(requestFlag == RequestHandlerFlag::RequestFlag)
        {
            if(requestType == "GET")
            {
                QueryParamsMap queryParamsMap = request->getQueryParameters();

                for(QueryParamsMap::iterator it = queryParamsMap.begin() ;
                        it != queryParamsMap.end() ; it++)
                {
                }

                OCRepresentation rep;
                rep = myResource.getRepresentation();

                if(response)
                {
                    response->setErrorCode(200);

                    auto findRes = queryParamsMap.find("if");

                    if(findRes != queryParamsMap.end())
                    {
                        response->setResourceRepresentation(rep , findRes->second);
                    }
                    else
                    {
                        response->setResourceRepresentation(rep , DEFAULT_INTERFACE);
                    }
                }
            }
            else if(requestType == "PUT")
            {
                cout << "\t\t\trequestType : PUT\n";

                QueryParamsMap queryParamsMap = request->getQueryParameters();

                AttributeMap attributeMap = request->getAttributeRepresentation();

                myResource.setRepresentation(attributeMap);

                myResource.getRepresentation(attributeMap);

                if(response)
                {
                    response->setErrorCode(200);
                    response->setResourceRepresentation(attributeMap);
                }
            }
            else if(requestType == "POST")
            {
            }
            else if(requestType == "DELETE")
            {
            }
        }
        else if(requestFlag == RequestHandlerFlag::ObserverFlag)
        {
            pthread_t threadId;

            cout << "========================================================" << endl;
            cout << "Receive ObserverFlag : Start Observe\n";
            cout << "========================================================" << endl;
            g_Observation = 1;

            static int startedThread = 0;
            if(!startedThread)
            {
                pthread_create(&threadId , NULL , ChangeLightRepresentation , (void *) NULL);
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

    PlatformConfig cfg(ServiceType::InProc, ModeType::Server,
            "134.134.161.33", 56832, QualityOfService::NonConfirmable);

    int number = 0;

    try
    {
        OCPlatform platform(cfg);

        myResource.createResource(platform);

        while(true)
        {
            bool end = false;
            cout << endl;
            cout << "========================================================" << endl;
            cout << "1. Temp is up" << endl;
            cout << "2. Temp is down" << endl;
            cout << "3. This Program will be ended." << endl;
            cout << "========================================================" << endl;
            cin >> number;

            switch(number)
            {
                case 1:
                {
                    cout << "Temp is up!" << endl;
                    myResource.m_temp += 10;
                    ChangeLightRepresentation((void *) NULL);

                    break;
                }
                case 2:
                {
                    cout << "Temp is down!" << endl;
                    myResource.m_temp -= 10;
                    ChangeLightRepresentation((void *) NULL);

                    break;
                }
                case 3:
                {
                    cout << "Bye!" << endl;
                    end = true;
                    break;
                }
                default:
                {
                    cout << "You type wrong number. Try again!" << endl;
                    break;
                }
            }
            if(end == true)
            {
                break;
            }
        }
    }
    catch(OCException e)
    {
    }
}
