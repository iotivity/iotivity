//******************************************************************
//
// Copyright 2015 Han Joo Chae, Jae Ho Jeon (Seoul National University) All Rights Reserved.
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

#include <WirelessSensorApp.h>

#include <ctime>
#include <iostream>


using namespace OC;
using namespace std;

OCEntityHandlerResult entityHandler(std::shared_ptr< OCResourceRequest > request);

/*
 * TempResourceFunctions
 */

void WirelessResource::registerResource()
{
    uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;

    // This will internally create and register the resource.
    OCStackResult result = OC::OCPlatform::registerResource(m_resourceHandle, m_resourceUri,
                           m_resourceTypes[0], m_resourceInterfaces[0], &entityHandler, resourceProperty);

    if (OC_STACK_OK != result)
    {
        cout << "Resource creation was unsuccessful\n";
    }
}

OCResourceHandle WirelessResource::getHandle()
{
    return m_resourceHandle;
}
void WirelessResource::setResourceRepresentation(OCRepresentation &rep) { 

    std::string tempWireless; 
    rep.getValue("wireless", tempWireless); 
    m_wireless = tempWireless;

    cout << "\t\t\t" << "Received representation: " << endl; 
    cout << "\t\t\t\t" << "wireless" << m_wireless << endl; 
}

OCRepresentation WirelessResource::getResourceRepresentation()
{
    m_resourceRep.setValue("wireless", m_wireless); 
    return m_resourceRep;
}

// Create the instance of the TemphumidResource class
WirelessResource g_myResource;

std::string generate_hex() 
{
   char str[3] = {0};
   const char *hex_digits = "0123456789ABCDEF"; 
   str[0] = hex_digits[(rand() % 16)];
   str[1] = hex_digits[(rand() % 16)]; 
   std::string res = str;
   return res; 
}

std::string generate_bssid() 
{
  std::string res;
  int i; 
  for (i = 0; i < 5; i++) res += generate_hex() + ":"; 
  res += generate_hex(); 
  return res;
}

std::string concat_bssid(std::vector<std::string> & bssid_arr, int start, int end)
{
  std::string res; 
  for (;start < end; start++) res += bssid_arr[start] + ",";

  res += bssid_arr[start];
  return res; 
}

void generate_bssid_arr(std::vector<std::string> & bssid_arr)
{
  bssid_arr.push_back("05:17:A5:A3:FD:B6");
  bssid_arr.push_back("60:14:1E:E7:BB:63");
  bssid_arr.push_back("72:43:BB:67:02:A9");
  bssid_arr.push_back("3E:17:1B:D5:19:36");
  bssid_arr.push_back("82:CB:B0:3E:A3:D5");

  bssid_arr.push_back("BA:98:FD:3B:6D:24");
  bssid_arr.push_back("7B:7D:6C:52:1F:9B");
  bssid_arr.push_back("4D:55:9C:9C:90:EA");
  bssid_arr.push_back("91:75:7A:0F:8D:0A");
  bssid_arr.push_back("07:9A:62:BE:38:56");

  bssid_arr.push_back("CE:22:CD:8D:38:F1");
  bssid_arr.push_back("6B:94:E7:1D:C9:04");
  bssid_arr.push_back("CC:33:10:32:E7:EA");
  bssid_arr.push_back("9D:63:05:5F:C9:D5");
  bssid_arr.push_back("A7:59:FD:98:C1:7D");

  bssid_arr.push_back("0A:B4:4D:9F:58:32");
  bssid_arr.push_back("5D:76:DD:96:06:39");
  bssid_arr.push_back("47:28:95:92:BC:59");
  bssid_arr.push_back("8A:76:EC:43:9A:6E");
  bssid_arr.push_back("2B:4F:29:79:2A:46");
}

std::string get_random_bssid(std::vector<std::string> & bssid_arr) 
{
  int i = rand() % 4; 
  return concat_bssid(bssid_arr, 5 * i, 5 * i + 4);
}

bool is_moving() 
{
  time_t t = time(0);
  return ((t % 10) < 5);
} 

void *TestSensorVal(void *param)
{
    (void)param;

    int nSleep_time = 1;

    std::vector<std::string> bssid_arr;
    generate_bssid_arr(bssid_arr);

    std::cout << "[WirelessSensorAPP] ::" << __func__ << " is called."
              << std::endl;

    // This function continuously monitors for the changes
    while (1)
    {
        sleep(nSleep_time);

        //if (is_moving())
        //{

        //}

        //else {

        //}

        g_myResource.m_wireless = get_random_bssid(bssid_arr);

        cout << "\n wireless updated to : " << g_myResource.m_wireless << endl;
        cout << "Notifying observers with resource handle: " << g_myResource.getHandle()
             << endl;

        OCStackResult result = OCPlatform::notifyAllObservers(g_myResource.getHandle());
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

    return 0;
}

