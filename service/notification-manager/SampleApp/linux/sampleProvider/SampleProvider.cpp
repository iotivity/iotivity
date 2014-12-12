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
#include "OCResourceResponse.h"

#include <memory>

using namespace OC;
using namespace std;

int g_Observation = 0;

pthread_cond_t m_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;

OCEntityHandlerResult entityHandler(std::shared_ptr< OCResourceRequest > request ,
                                    std::shared_ptr< OCResourceResponse > response);

class TempHumidResource
{
public:

    int m_temp;
    int m_humid;

    std::string m_uri;
    OCResourceHandle m_resourceHandle;

    OCRepresentation m_Rep;

public:
    TempHumidResource() :
            m_temp(0), m_humid(0), m_uri("/a/TempHumSensor")
    {
    }

    void createResource()
    {
        std::string resourceURI = "/a/NM/TempHumSensor";
        std::string resourceTypeName = "NotificationManager.Hosting";
        std::string resourceInterface = DEFAULT_INTERFACE;

        m_uri = resourceURI;

        uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;

        OCStackResult result = OCPlatform::registerResource(m_resourceHandle , resourceURI ,
                resourceTypeName , resourceInterface , &entityHandler , resourceProperty);

        if(OC_STACK_OK != result)
        {
            cout << "Resource creation was unsuccessful\n";
        }
    }

    OCStackResult createResource1()
	{
		std::string resourceURI = "/a/NM/TempHumSensor1"; // URI of the resource
		std::string resourceTypeName = "NotificationManager.Hosting"; // resource type name. In this case, it is light
		std::string resourceInterface = DEFAULT_INTERFACE; // resource interface.

		// OCResourceProperty is defined ocstack.h
		uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;

		OCResourceHandle resHandle;

		// This will internally create and register the resource.
		OCStackResult result = OCPlatform::registerResource(
									resHandle, resourceURI, resourceTypeName,
									resourceInterface, &entityHandler, resourceProperty);

		if (OC_STACK_OK != result)
		{
			cout << "Resource creation was unsuccessful\n";
		}

		return result;
	}

    OCResourceHandle getHandle()
    {
        return m_resourceHandle;
    }

    OCRepresentation post(OCRepresentation& rep)
    {
        static int first = 1;

        // for the first time it tries to create a resource
        if(first)
        {
            first = 0;

            if(OC_STACK_OK == createResource1())
            {
                OCRepresentation rep1;
                rep1.setValue("createduri", std::string("/a/light1"));

                return rep1;
            }
        }

        // from second time onwards it just puts
        put(rep);
        return get();
    }

    void put(OCRepresentation& rep)
	{
		try {
			if (rep.getValue("temperature", m_temp))
			{
				cout << "\t\t\t\t" << "temperature: " << m_temp << endl;
			}
			else
			{
				cout << "\t\t\t\t" << "temperature not found in the representation" << endl;
			}

			if (rep.getValue("humidity", m_humid))
			{
				cout << "\t\t\t\t" << "humidity: " << m_humid << endl;
			}
			else
			{
				cout << "\t\t\t\t" << "humidity not found in the representation" << endl;
			}
		}
		catch (exception& e)
		{
			cout << e.what() << endl;
		}

	}

    OCRepresentation get()
	{
    	cout << "resource get\n";
		m_Rep.setValue("temperature", m_temp);
		m_Rep.setValue("humidity", m_humid);

		cout << "resource get : done\n";

		return m_Rep;
	}

};

TempHumidResource myResource;

void *ChangeLightRepresentation(void *param)
{
	cout << "ChangeLigthRepresentation Enter\n";
	while(1){
		cout << "pthread_cond_wait\n";
		pthread_cond_wait(&m_cond, &m_mutex);
		cout << "pthread_cond_start\n";
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

			OCStackResult result = OCPlatform::notifyAllObservers(myResource.getHandle());
			cout << "Notify Success\n";

			if(OC_STACK_NO_OBSERVERS == result)
			{
				cout << "No More observers, stopping notifications" << endl;
				g_Observation = 0;
			}
		}
		cout << "ChangeLigthRepresentation Out\n";

	}
    return NULL;
}

OCEntityHandlerResult entityHandler(std::shared_ptr< OCResourceRequest > request ,
        std::shared_ptr< OCResourceResponse > response)
{
	cout << "Sample Provider entityHandler\n";
    if(request)
    {
    	cout << "flag : request\n";
        std::string requestType = request->getRequestType();
        int requestFlag = request->getRequestHandlerFlag();

        if(requestFlag == RequestHandlerFlag::InitFlag)
        {
        	cout << "\t\trequestFlag : Init\n";
        }

        if(requestFlag == RequestHandlerFlag::RequestFlag)
        {
        	cout << "\t\trequestFlag : Request\n";
            if(requestType == "GET")
            {
            	cout << "\t\trequestType : GET\n";
            	try
            	{
					if(response)
					{
						OCRepresentation rep = myResource.get();
						cout << rep.getJSONRepresentation() << endl;
						response->setErrorCode(200);
						response->setResourceRepresentation(rep, DEFAULT_INTERFACE);
					}
					else
					{
						cout << "response is null\n";
					}
            	} catch(exception& e)
            	{
            		cout << e.what() << endl;
            	}
            }
            else if(requestType == "PUT")
            {
                cout << "\t\t\trequestType : PUT\n";

                OCRepresentation rep = request->getResourceRepresentation();
                myResource.put(rep);

                if(response)
                {
                    response->setErrorCode(200);
                    response->setResourceRepresentation(myResource.get());
                }
            }
            else if(requestType == "POST")
            {
            }
            else if(requestType == "DELETE")
            {
            }
        }
        else if(requestFlag & RequestHandlerFlag::ObserverFlag)
        {
        	pthread_t threadId;

            cout << request->getResourceUri() << endl;
            cout << request->getResourceRepresentation().getUri() << endl;

            cout << "========================================================" << endl;
            cout << "Receive ObserverFlag : Start Observe\n";
            cout << "========================================================" << endl;
            g_Observation = 1;

            cout << "\t\trequestFlag : Observer\n";
            static int startedThread = 0;
            if(!startedThread)
            {
            	cout << "\t\tpthrerad_create\n";
                pthread_create(&threadId , NULL , ChangeLightRepresentation , (void *) NULL);
                startedThread = 1;
            }
        }
    }
    else
    {
        std::cout << "Request invalid" << std::endl;
    }

    return OC_EH_OK;
}

int main()
{

	PlatformConfig cfg {
	        OC::ServiceType::InProc,
	        OC::ModeType::Server,
	        "0.0.0.0",
	        0,
	        OC::QualityOfService::LowQos
	    };

	OCPlatform::Configure(cfg);

    int number = 0;

    try
    {

        myResource.createResource();

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
                    pthread_cond_signal(&m_cond);
                    cout << "ChangeLightRepresentation Done!" << endl;
                    break;
                }
                case 2:
                {
                    cout << "Temp is down!" << endl;
                    myResource.m_temp -= 10;
                    pthread_cond_signal(&m_cond);
                    cout << "ChangeLightRepresentation Done!" << endl;
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
    catch(exception& e)
    {
    	cout << "main exception  : " << e.what() << endl;
    }
}
