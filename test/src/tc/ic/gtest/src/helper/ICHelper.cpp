/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include "ICHelper.h"

using namespace std;
using namespace OC;
using namespace OC::OCPlatform;

std::mutex ICHelper::s_mutex;
ICHelper* ICHelper::s_ICHelper = NULL;

string ICHelper::UID = "";
string ICHelper::ACCESS_TOKEN = "";
string ICHelper::GROUP_ID="";
string ICHelper::GROUP_TYPE="";
std::vector<std::string> ICHelper::DEVICE_ID={};

bool ICHelper::isServerResponsed = false;
bool ICHelper::isSignedUp = false;
static bool ICHelper::isPostRequestSuccess = false;

ResourceHandles ICHelper::g_ResourceHandles;
OC::OCResource::Ptr ICHelper::m_binaryswitchResource;

ICHelper::ICHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[ICHelper] IN");
}

ICHelper* ICHelper::getInstance(void)
{
    if (s_ICHelper == NULL)
    {
        s_mutex.lock();

        s_ICHelper = new ICHelper();

        s_mutex.unlock();
    }

    return s_ICHelper;
}

ICHelper::~ICHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[ICHelper] OUT");
    if (s_ICHelper != NULL)
    {
        delete s_ICHelper;

        s_ICHelper = NULL;
    }
}

void ICHelper::waitForServerResponse()
{
    isServerResponsed = false;

    void* ret = NULL;
    pthread_t serverlistiningThread;
    pthread_create(&serverlistiningThread, NULL, executeThread, NULL);
    pthread_join(serverlistiningThread, &ret);
}

void* ICHelper::executeThread(void *arg)
{
    int second = 0;

    cout
            << "\nWaiting For Server Response..........................................................\n";

    while (!isServerResponsed)
    {
        sleep(1);
        if (++second == CALLBACK_TIMEOUT)
        {
            cout << "\nTimeout For Server Response!Please Try Again\n\n";
            break;
        }
    }

    pthread_exit (NULL);
}

FILE* ICHelper::readResourceDATFile(const char * /*path*/, const char *mode)
{
    return fopen(RESOURCE_SERVER_DAT, mode);
}

void ICHelper::icPrintRepresentation(OCRepresentation rep)
{
    for (auto itr = rep.begin(); itr != rep.end(); ++itr)
    {
        cout << "\t" << itr->attrname() << ":\t" << itr->getValueToString() << endl;
        if (itr->type() == AttributeType::Vector)
        {
            switch (itr->base_type())
            {
                case AttributeType::OCRepresentation:
                    for (auto itr2 : (*itr).getValue< vector< OCRepresentation > >())
                    {
                        icPrintRepresentation(itr2);
                    }
                    break;

                case AttributeType::Integer:
                    for (auto itr2 : (*itr).getValue< vector< int > >())
                    {
                        cout << "\t\t" << itr2 << endl;
                    }
                    break;

                case AttributeType::String:
                    for (auto itr2 : (*itr).getValue< vector< string > >())
                    {
                        cout << "\t\t" << itr2 << endl;
                    }
                    break;

                default:
                    cout << "Unhandled base type " << itr->base_type() << endl;
                    break;
            }
        }
        else if (itr->type() == AttributeType::OCRepresentation)
        {
            icPrintRepresentation((*itr).getValue< OCRepresentation >());
        }
    }
}

bool ICHelper::isResourceRegistered()
{
    try
    {
        OCStackResult ocResult = OC_STACK_ERROR;

        ICAirconditionerResource airConditioner(IC_RESOURCE_URI_AIRCON,
        { IC_RESOURCE_TYPE_AIRCON },
        { DEFAULT_INTERFACE, BATCH_INTERFACE, LINK_INTERFACE });

        ICBinarySwitchResource binarySwitch(IC_RESOURCE_URI_BSWITCH,
        { IC_RESOURCE_TYPE_BSWITCH },
        { DEFAULT_INTERFACE });

        ICTemperatureResource temperature(IC_RESOURCE_URI_TEMP,
        { IC_RESOURCE_TYPE_TEMP },
        { DEFAULT_INTERFACE });

        string uri = airConditioner.getResourceUri();
        string rt = airConditioner.getResourceType()[0];
        string itf = airConditioner.getInterfaces()[0];
        ocResult = OC_STACK_ERROR;
        ocResult = OCPlatform::registerResource(airConditioner.m_handle, uri, rt, itf,
                bind(&ICAirconditionerResource::entityHandler, &airConditioner, placeholders::_1),
                OC_DISCOVERABLE);
        if (ocResult != OC_STACK_OK)
        {
            cout << "ICAirconditionerResource registration is unsuccessful" << endl;
            return false;
        }

        itf = airConditioner.getInterfaces()[1];
        ocResult = OC_STACK_ERROR;
        ocResult = OCPlatform::bindInterfaceToResource(airConditioner.m_handle, itf);
        if (ocResult != OC_STACK_OK)
        {
            cout << "Binding second interface for airConditioner is unsuccessful" << endl;
            return false;
        }

        itf = airConditioner.getInterfaces()[2];
        ocResult = OC_STACK_ERROR;
        ocResult = OCPlatform::bindInterfaceToResource(airConditioner.m_handle, itf);
        if (ocResult != OC_STACK_OK)
        {
            cout << "Binding third interface for airConditioner is unsuccessful" << endl;
            return false;
        }

        uri = binarySwitch.getResourceUri();
        rt = binarySwitch.getResourceType()[0];
        itf = binarySwitch.getInterfaces()[0];
        ocResult = OC_STACK_ERROR;
        ocResult = OCPlatform::registerResource(binarySwitch.m_handle, uri, rt, itf,
                bind(&ICBinarySwitchResource::entityHandler, &binarySwitch, placeholders::_1),OC_OBSERVABLE);
        if (ocResult != OC_STACK_OK)
        {
            cout << "ICBinarySwitchResource registration is unsuccessful" << endl;
            return false;
        }

        uri = temperature.getResourceUri();
        rt = temperature.getResourceType()[0];
        itf = temperature.getInterfaces()[0];
        ocResult = OC_STACK_ERROR;
        ocResult = OCPlatform::registerResource(temperature.m_handle, uri, rt, itf,
                bind(&ICTemperatureResource::entityHandler, &temperature, placeholders::_1),
                OC_OBSERVABLE);
        if (ocResult != OC_STACK_OK)
        {
            cout << "ICTemperatureResource registration is unsuccessful" << endl;
            return false;
        }

        ocResult = OC_STACK_ERROR;
        ocResult = airConditioner.addChildResource(&binarySwitch);
        if (ocResult != OC_STACK_OK)
        {
            cout << "Add ICBinarySwitchResource as a child resource of Air conditioner is unsuccessful" << endl;
            return false;
        }

        ocResult = OC_STACK_ERROR;
        ocResult = airConditioner.addChildResource(&temperature);
        if (ocResult != OC_STACK_OK)
        {
            cout << "Add ICTemperatureResource as a child resource of Air conditioner is unsuccessful" << endl;
            return false;
        }

        cout << "Publishing resources to cloud ";

        ResourceHandles resourceHandles;

        OCDeviceInfo devInfoAirConditioner;
        OCStringLL deviceType;

        deviceType.value = "oic.d.airconditioner";
        deviceType.next = NULL;
        devInfoAirConditioner.deviceName = "FAC_2016";
        devInfoAirConditioner.types = &deviceType;
        devInfoAirConditioner.specVersion = NULL;
        devInfoAirConditioner.dataModelVersions = NULL;
        ocResult = OC_STACK_ERROR;
        ocResult = OCPlatform::registerDeviceInfo(devInfoAirConditioner);
        if (ocResult != OC_STACK_OK)
        {
            cout << "RegisterDeviceInfo is unsuccessful" << endl;
            return false;
        }

        resourceHandles.push_back(airConditioner.m_handle);
        g_ResourceHandles = resourceHandles;
        ICHelper::isServerResponsed = true;
        return true;
    }
    catch(OCException &ex)
    {
        cout << "OCException result string : " << CommonUtil::s_OCStackResultString.at(ex.code());
        return false;
    }
}
