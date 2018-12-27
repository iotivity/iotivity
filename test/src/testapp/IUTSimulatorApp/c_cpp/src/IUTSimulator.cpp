/******************************************************************
*
* Copyright 2018 Samsung Electronics All Rights Reserved.
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

#include <vector>
#include <iostream>
#include <initializer_list>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <regex>

#ifdef __LINUX__
#include <execinfo.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <signal.h>
#include <stdlib.h>

#include <iomanip>
#include <thread>
#include <chrono>

#include "ocstack.h"
#include "octypes.h"

#include "experimental/logger.h"

#include "IUTSimulatorUtils.h"
#include "SampleResource.h"
#include "MntResource.h"
#include "NmonResource.h"
#include "SampleCollection.h"
#include "SampleResourceFactory.h"
#include "bloodpressure0.h"
#include "bloodpressure1.h"
#include "bloodpressure2.h"
#include "ResourceHelper.h"
#include "cloudresource.h"
#include "OCPlatform.h"
#include "ocpayload.h"
#include "OCApi.h"
#include "pinoxmcommon.h"
#include "cautilinterface.h"
#include "cacommon.h"
#include "oic_malloc.h"

#include "easysetup.h"
#include "escommon.h"
#include "ESEnrolleeCommon.h"

#ifdef HAVE_WINDOWS_H
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#endif

#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif

#include "RDClient.h"
#include "rd_server.h"

using namespace OC;
using namespace std;

#define TAG "OCF_IUT"


// Local configuration for WES
static const char WES_ENROLLER_SSID[] = "TEST_AP_NAME";/// Update this to correct value as per setup
static const char WES_ENROLLER_PASSWORD[] = "TEST_AP_PASSWORD"; /// Update this to correct value as per setup
static const WIFI_AUTHTYPE WES_ENROLLER_AUTH_TYPE = WPA2_PSK;
static const WIFI_ENCTYPE WES_ENROLLER_ENC_TYPE = AES;
///////////

vector< shared_ptr< SampleResource > > g_createdResourceList;
vector< shared_ptr< OCResource > > g_foundResourceList;
vector< shared_ptr< OCResource > > g_foundCollectionList;
shared_ptr< OCResource > g_introspectionResource;
vector< string> g_rdServerHosts;
vector< OCResourceHandle > g_handleList;
OCResourceHandle g_childHandle = NULL;
OCResourceHandle g_collectionHandle = NULL;
OCRepresentation g_resourceRepresentation;
OCConnectivityType g_ipVer = CT_DEFAULT;
string g_ocfVer = "ocf.1.0.0";

SampleCollection *g_extraLightCollection;
SampleCollection *g_vendorCollectionResource;
SampleCollection *g_sampleCollection;

#define MAXLEN_STRING 100
#define DEFAULT_HANDLER true
#define USERPROPERTY_KEY_INT "x.user.property.int"
#define USERPROPERTY_KEY_STR "x.user.property.str"

bool g_hasCallbackArrived = false;
bool g_isObservingResource = false;
bool g_isGroupCreated = false;
bool g_isSampleCollectionCreated = false;
bool g_isExtraDeviceCreated = false;
bool g_isSecuredClient = false;
bool g_isSecuredServer = false;
bool g_retainOldDB = false;
bool isDeviceTypeSet = false;

std::ofstream g_RDPPINLogFileStream;
int g_RDPPINSeqNumber = 0;

QualityOfService g_qos = QualityOfService::LowQos;
ResourceHelper *g_resourceHelper;
ESProvisioningCallbacks g_esCallbacks;

typedef struct UserProperties_t
{
    int userValue_int;  /**< User-specific property in WiFi resource **/
    char userValue_str[MAXLEN_STRING]; /**< User-specific property in DevConf resource **/
} UserProperties;

UserProperties g_userProperties;

static mutex s_mutex;
static const char CRED_FILE_SERVER[] = "oic_svr_db_server.dat";
static const char CRED_FILE_CLIENT[] = "oic_svr_db_client.dat";
static const char INTROSPECTION_SWAGGER_FILE[] = "introspection_swagger.dat";
static const string JUST_WORKS_DI = "12345678-1234-1234-1234-123456789012";
static const string RANDOM_PIN_DI = "87654321-4321-4321-4321-CBA987654321";
static const string MFG_DI = "4d617566-6163-7475-7265-724365727430";
static const int CLIENT_MODE = 1;
static const int SERVER_MODE = 2;
static int g_modeType;
static int g_securityType;
static const char sourceFileNames[][100] =
{ "None", "../../oic_svr_db_server_justworks.dat", "../../oic_svr_db_server_randompin.dat",
        "../../oic_svr_db_server_mfg.dat" };
string g_di = "";
string g_collectionName = GROUP_NAME;

void onObserve(const HeaderOptions headerOptions, const OCRepresentation &rep, const int &eCode,
        const int &sequenceNumber);
void onDelete(const HeaderOptions &headerOptions, const int eCode);
void onPost(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);
void onPut(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);
void onGet(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);
void onDeviceInfoReceived(const OCRepresentation& rep);
void onPlatformInfoReceived(const OCRepresentation& rep);
void onCollectionFound(shared_ptr< OCResource > collection);
void onResourceFound(shared_ptr< OCResource > resource);

void onDevConfProvReceived(ESDevConfData *eventData);
void onWiFiConfProvReceived(ESWiFiConfData *eventData);

void onResourceRemoved(const int& eCode);
void onResourcePublished(const OCRepresentation& rep, const int& eCode);

void handleMenu(int argc, char* argv[]);
void showMenu(int argc, char* argv[]);
void selectMenu(int choice);
void createTvDevice(bool secured = false);
void createAirConDevice(bool secured = false);
void createExtraDevice(bool secured = false);
void createSingleAirConResource(bool secured = false);
void createResourceForIntrospection(bool secured = false);
void createVendorDefinedDevice(bool secured = false);
void createResource(void);
void createSecuredResource(void);
void createInvisibleResource(void);
void createResourceWithUrl(void);
void createManyLightResources(void);
void publishResourcesToRD(void);
void updateResourcesToRD(void);
void deleteResourcesFromRD(void);
void deleteCreatedResource(void);
void findCollection(string);
void findResource(string resourceType, string host = "");
void findAllResources(string host = "", string query = "");
void discoverIntrospection(bool isMulticast = true);
void discoverDevice(bool);
void discoverPlatform(bool isMulticast = true);
void sendGetRequest();
void sendGetRequestWithQuery(string, string);
void sendPutRequestUpdate(void);
void sendPostRequestUpdate(void);
void sendPostRequestUpdateUserInput(void);
void sendPutRequestCreate(void);
void sendPostRequestCreate(void);
void sendDeleteRequest(void);
void waitForCallback(void);
void observeResource(void);
void cancelObserveResource(void);
void cancelObservePassively(void);
void createGroup(string);
void deleteGroup(void);
void updateGroup(void);
void updateLocalResource(void);
void prepareForWES(void);
void setDeviceWESInfo(void);
void SetUserProperties(const UserProperties *prop);
void ReadUserdataCb(OCRepPayload* payload,char* resourceType, void** userdata);
void WriteUserdataCb(OCRepPayload* payload,char* resourceType);
void SetCallbackForUserdata(void);
string getHost();
FILE* server_fopen(const char*, const char*);
FILE* client_fopen(const char*, const char*);
void updateLec(void);
void openRDPLogFile(void);
void closeRDPLogFile(void);

uint8_t isSecure(bool secured)
{
    if (secured) return OC_SECURE;
    else return 0;
}

void openRDPLogFile()
{
    time_t t = time(0);
    struct tm *now = localtime(&t);

    char time_str [150];
    strftime(time_str, 150, "%Y-%m-%d-%H-%M-%S.log", now);

    std::string file_name = "iut_RDP_PIN_tracker_log_";
    file_name = file_name + time_str;

    g_RDPPINLogFileStream.open(file_name.c_str());

    if(g_RDPPINLogFileStream.is_open())
    {
         std::cout << "Success to create g_RDPPINLogFileStream" << std::endl;
    }

    g_RDPPINLogFileStream << "Start Time:" << time_str << std::endl;
    g_RDPPINLogFileStream << "===================================" << std::endl;
}

void closeRDPLogFile()
{
    g_RDPPINLogFileStream.close();
}

std::string getTimeNow()
{
    time_t t = time(0);
    struct tm *now = localtime(&t);

    char time_str [150];
    strftime(time_str, 150, "[%Y-%m-%d-%H-%M-%S]", now);

    std::string currTimeStr(time_str);
    return currTimeStr;
}

void handler(int sig)
{
    void *array[10];
    size_t size;

#ifdef __LINUX__
    // get void*'s for all entries on the stack
    size = backtrace(array, 10);
#endif

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);

#ifdef __LINUX__
    backtrace_symbols_fd(array, size, STDERR_FILENO);
#endif

    exit(1);
}

void SetUserProperties(const UserProperties *prop)
{
    if(prop != NULL)
    {
        strncpy(g_userProperties.userValue_str, prop->userValue_str, MAXLEN_STRING);
        g_userProperties.userValue_int = prop->userValue_int;
    }
}

void ReadUserdataCb(OCRepPayload* payload, char*resourceType,void** userdata)
{
    cout << "ReadUserdataCb IN" << endl;

    if(payload != NULL)
    {
        if(strstr(resourceType, OC_RSRVD_ES_RES_TYPE_WIFICONF))
        {
            int64_t value = -1;
            if (OCRepPayloadGetPropInt(payload, USERPROPERTY_KEY_INT, &value))
            {
                if(*userdata != NULL)
                {
                    *userdata = (void*)OICMalloc(sizeof(UserProperties));
                    ((UserProperties*)(*userdata))->userValue_int = value;
                }

                cout << "[User specific property]" << USERPROPERTY_KEY_INT << ":" <<  value;
                g_userProperties.userValue_int = value;
            }
        }
    }

    cout << "ReadUserdataCb OUT" << endl;
}

void WriteUserdataCb(OCRepPayload* payload,char* resourceType)
{
    cout << "WriteUserdataCb IN" << endl;

    if(payload != NULL)
    {
        if(strstr(resourceType,OC_RSRVD_ES_RES_TYPE_WIFICONF))
        {
             OCRepPayloadSetPropInt(payload,USERPROPERTY_KEY_INT,g_userProperties.userValue_int);
        }

        if(strstr(resourceType,OC_RSRVD_ES_RES_TYPE_DEVCONF))
        {
             OCRepPayloadSetPropString(payload,USERPROPERTY_KEY_STR,g_userProperties.userValue_str);
        }
     }
     cout<<"WriteUserdataCb OUT" << endl;
}

void SetCallbackForUserdata()
{
     ESSetCallbackForUserdata(&ReadUserdataCb,&WriteUserdataCb);
}


void replaceDatFile(int modeType, int securityType)
{
    int num;
    char buffer[1000];

    FILE *source;

    source = fopen(sourceFileNames[securityType], "rb");
    if (source == NULL)
    {
        cout << "Unable to read source file " << endl;
        exit(-1);
    }
    else
    {
        cout << "Successfully open " << sourceFileNames[securityType] << endl;
    }

    FILE *destFile = NULL;
    if (modeType == CLIENT_MODE)
    {
        destFile = fopen(CRED_FILE_CLIENT, "wb");
        cout << "Copy to" << CRED_FILE_CLIENT << endl;
    }
    else if (modeType == SERVER_MODE)
    {
        destFile = fopen(CRED_FILE_SERVER, "wb");
        cout << "Copy to " << CRED_FILE_SERVER << endl;
    }
    else
    {
        cout << "No mode found" << endl;
    }

    if (NULL == destFile)
    {
        fclose(source);
        cout << "Unable to write dat file" << endl;
        exit(-1);
    }

    while ((num = fread(buffer, sizeof(char), sizeof(buffer), source)) != 0)
    {
        fwrite(buffer, sizeof(char), num, destFile);
    }

    cout << "Dat File copy successfully" << endl;

    fclose(destFile);
    fclose(source);
}

void generatePinCB(char* pin, size_t pinSize)
{
    cout << "generatePinCB [IN]" << endl;
    if (NULL == pin || pinSize <= 0)
    {
        cout << "Invalid PIN" << endl;
    }
    else
    {
        g_RDPPINSeqNumber++;

        cout << "============================" << endl;
        cout << "PIN CODE Sequence Number: " << g_RDPPINSeqNumber << " Time: " << getTimeNow() << endl;
        cout << "    PIN CODE : " << string(pin) << endl;
        cout << "============================" << endl;

        g_RDPPINLogFileStream << "============================" << endl;
        g_RDPPINLogFileStream << "PIN CODE Sequence Number: " << g_RDPPINSeqNumber << " Time: " << getTimeNow() << endl;
        g_RDPPINLogFileStream << "    PIN CODE : " << string(pin) << endl;
        g_RDPPINLogFileStream << "============================" << endl;
        g_RDPPINLogFileStream.flush();
    }
    cout << "generatePinCB [OUT]" << endl;
}

int main(int argc, char* argv[])
{
    signal(SIGSEGV, handler);

    g_createdResourceList.clear();
    g_foundResourceList.clear();
    g_handleList.clear();
    g_resourceHelper = ResourceHelper::getInstance();

    OCStackResult result = OC_STACK_OK;

    if (argc > 1)
    {
        try
        {
            int optionSelected = stoi(argv[1]);

            if (optionSelected == 1)
            {
                cout << "Using CON Server" << endl;
                g_qos = QualityOfService::HighQos;
            }
            else if (optionSelected == 0)
            {
                cout << "Using NON Server" << endl;
            }
            else
            {
                cout << "Supplied quality of service is invalid. Using default server type: NON"
                        << endl;
            }
        }
        catch (exception&)
        {
            cout << "Invalid input argument. Using default QoS: NON" << endl;
        }
    }
    else
    {
        std::cout << "No QoS supplied. Using default: NON" << std::endl;
    }
    if (argc > 2)
    {
        try
        {
            int optionSelected = stoi(argv[2]);

            if (optionSelected == 6)
            {
                cout << "Using IP version: IPv6" << endl;
                g_ipVer = CT_IP_USE_V6;
            }
            else if (optionSelected == 4)
            {
                cout << "Using IP version: IPv4" << endl;
                g_ipVer = CT_IP_USE_V4;
            }
            else
            {
                cout << "Supplied IP version is not a valid IP version. Using default: IPv6" << endl;
            }
        }
        catch (exception&)
        {
            cout << "Invalid input argument. Using default: IPv6" << endl;
        }
    }
    else
    {
        cout << "No IP version supplied. Using default: IPv6" << endl;
    }

    if (argc > 3)
    {
        try
        {
            int optionSelected = stoi(argv[3]);
            g_securityType = optionSelected % 10;

            if (optionSelected / 10 == 1)
            {
                cout << "Using fresh secured client...." << endl;
                g_isSecuredClient = true;
                g_modeType = CLIENT_MODE;
            }
            else if (optionSelected / 10 == 2)
            {
                cout << "Using fresh secured server...." << endl;
                g_isSecuredServer = true;
                g_modeType = SERVER_MODE;
            }
            else if (optionSelected / 10 == 3)
            {
                cout << "Retaining old secured client...." << endl;
                g_isSecuredClient = true;
                g_modeType = CLIENT_MODE;
                g_retainOldDB = true;
            }
            else if (optionSelected / 10 == 4)
            {
                cout << "Retaining old secured server...." << endl;
                g_isSecuredServer = true;
                g_modeType = SERVER_MODE;
                g_retainOldDB = true;
            }
            else
            {
                cout << "Using unsecured server & client...." << endl;
            }

            if (g_isSecuredClient || g_isSecuredServer)
            {
                if (g_securityType == 1 )
                {
                    cout << "Supported Security Mode: justworks" << endl;
                    g_di = JUST_WORKS_DI;
                }
                else if (g_securityType == 2 )
                {
#ifdef __SECURED__
                    SetGeneratePinCB(generatePinCB);
#endif
                    cout << "Supported Security Mode: randompin" << endl;
                    g_di = RANDOM_PIN_DI;

                    openRDPLogFile();
                }
                else if (g_securityType == 3 )
                {
                    cout << "Supported Security Mode: manufacturing certificate" << endl;
                    g_di = MFG_DI;
                }
                else
                {
                    cout << "Unsupported Security Mode Specified, Using Default Mode: justworks" << endl;
                    g_di = JUST_WORKS_DI;
                    g_securityType = 1;
                }

                if (!g_retainOldDB)
                {
                    replaceDatFile(g_modeType, g_securityType);
                }
            }
        }
        catch (std::exception&)
        {
            cout << "Invalid input argument. Using default: unsecured server & client" << endl;
        }
    }
    else
    {
        cout << "Security mode not specified. Using default: unsecured server & client" << endl;
    }

    if (argc > 4)
    {
        string ocfArg = string(argv[4]);
        if (std::regex_match(ocfArg, std::regex("\\d+\\.\\d+\\.\\d+")))
        {
            cout << "Using OCF version: " << ocfArg << endl;
            g_ocfVer = "ocf." + ocfArg;
        }
        else
        {
            cout << "Invalid input argument. Using default: 1.0.0" << endl;
        }
    }
    else
    {
        std::cout << "No OCF version supplied. Using default: 1.0.0" << std::endl;
    }

    if (g_isSecuredClient)
    {
        vector<string> clientDeviceTypes;
        clientDeviceTypes.push_back(DEFAULT_DEVIE_TYPE);

        OCPersistentStorage ps =
        { client_fopen, fread, fwrite, fclose, unlink };
        PlatformConfig cfg
        { OC::ServiceType::InProc, OC::ModeType::Both, g_ipVer, g_ipVer, g_qos, &ps };
        result = SampleResource::constructServer(cfg);
        SampleResource::setDeviceInfo("Smart Home Client Device", clientDeviceTypes, g_ocfVer);
    }
    else if (g_isSecuredServer)
    {
        OCPersistentStorage ps =
        { server_fopen, fread, fwrite, fclose, unlink };
        PlatformConfig cfg
        { OC::ServiceType::InProc, OC::ModeType::Both, g_ipVer, g_ipVer, g_qos, &ps };
        result = SampleResource::constructServer(cfg);
    }
    else
    {
        OCPersistentStorage ps =
        { server_fopen, fread, fwrite, fclose, unlink };
        PlatformConfig cfg
        { OC::ServiceType::InProc, OC::ModeType::Both, g_ipVer, g_ipVer, g_qos, &ps };
        result = SampleResource::constructServer(cfg);
    }

    if (result != OC_STACK_OK)
    {
        cout << "Unable to start Iotivity servers" << endl;
        return 1;
    }
    else
    {
        cout << "Iotivity Server started successfully" << endl;
    }

#ifdef RD_SERVER
    if (OCRDStart() == OC_STACK_OK)
    {
        cout << "RD Server started successfully..." << endl;
    }
    else
    {
        cout << "Failed to start RD Server..." << endl;
    }
#endif

    cout << "IUT Simulator started successfully" << endl;
    cout << endl;

    for (int i = 5; i < argc; i++)
    {
        int choice = atoi(argv[i]);
        selectMenu(choice);
    }

    showMenu(argc, argv);

    cout << "Iotivity Server stopped successfully" << endl;
    cout << "Quitting IUT Simulator App.... Done!!" << endl;

    return 0;
}

void ESWorkerThreadRoutine(string ssid, string pwd)
{
    cout << "ESWorkerThreadRoutine IN" << endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    cout << "ESWorkerThreadRoutine Woke up from sleep" << endl;

    cout << "ssid: " << ssid << endl;
    cout << "password: " << pwd << endl;

#ifdef __LINUX__

    cout << "#1. Stopping Soft AP" << endl;

    string outputString = g_resourceHelper->executeCommand("sudo service hostapd stop");
    cout << "outputString 1:" << outputString << endl;
    outputString = g_resourceHelper->executeCommand("sudo nmcli nm wifi on");
    cout << "outputString 2:" << outputString << endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));

    string nmcliCommand = "nmcli d wifi connect ";
    nmcliCommand += ssid.c_str();
    nmcliCommand += " password ";
    nmcliCommand += pwd.c_str();

    cout << "executing command:" << nmcliCommand << endl;

    outputString = g_resourceHelper->executeCommand(nmcliCommand.c_str());
    cout << "outputString 3:" << outputString << endl;
    if (outputString == "")
    {
        ESSetErrorCode(ES_ERRCODE_NO_ERROR);
    }
#endif

    cout << "ESWorkerThreadRoutine OUT" << endl;
}

void onWiFiConfProvReceived(ESWiFiConfData *eventData)
{
    cout << "WiFiConfProvCbInApp IN" << endl;

    if(eventData == NULL)
    {
        cout << "ESWiFiConfData is NULL" << endl;
        return ;
    }

    cout << "SSID : " << eventData->ssid << endl;
    cout << "Password : " << eventData->pwd << endl;
    cout << "AuthType : " << eventData->authtype << endl;
    cout << "EncType : " << eventData->enctype << endl;

    cout << "Authentication Type : " << eventData->authtype << endl;

    if(0 != strcmp(eventData->ssid, WES_ENROLLER_SSID))
    {
        cout << "SSID Received is Invalid" << endl;
        ESSetErrorCode((ESErrorCode)1);

        cout << "WiFiConfProvCbInApp OUT 1" << endl;
        return;
    }

    if(0 != strcmp(eventData->pwd, WES_ENROLLER_PASSWORD))
    {
        cout << "Password Received is Invalid" << endl;
        ESSetErrorCode((ESErrorCode)2);

        cout << "WiFiConfProvCbInApp OUT 2" << endl;
        return;
    }

    if(WES_ENROLLER_AUTH_TYPE != eventData->authtype)
    {
        cout << "AUTH Type Received is Invalid" << endl;
        ESSetErrorCode((ESErrorCode)8);

        cout << "WiFiConfProvCbInApp OUT 3" << endl;
        return;
    }

    if(WES_ENROLLER_ENC_TYPE != eventData->enctype)
    {
        cout << "ENC Type Received is Invalid" << endl;
        ESSetErrorCode((ESErrorCode)9);

        cout << "WiFiConfProvCbInApp OUT 4" << endl;
        return;
    }

    cout << "" << endl;
    cout << "===========================" << endl;
    cout << "Connect To Network --> [" << eventData->ssid << "]" << endl;
    cout << "ALSO SWITCH OFF SOFT AP Network" << endl;
    cout << "===========================" << endl;
    cout << "" << endl;

    ESSetErrorCode(ES_ERRCODE_NO_ERROR);

    string ssid(eventData->ssid);
    string password(eventData->pwd);

    std::thread esThread(ESWorkerThreadRoutine, ssid, password);
    esThread.detach();

    cout << "WiFiConfProvCbInApp OUT" << endl;
}

void onDevConfProvReceived(ESDevConfData *eventData)
{
    cout << "DevConfProvCbInApp IN" << endl;

    if(eventData == NULL)
    {
        cout << "ESDevConfData is NULL" << endl;
        return ;
    }

    cout << "DevConfProvCbInApp OUT" << endl;
}

void onResourceFound(shared_ptr< OCResource > resource)
{
    s_mutex.lock();
    cout << "Found a resource!!" << endl;

    if (resource)
    {
        g_foundResourceList.push_back(resource);
        cout << "uri of the found resource is " << resource->uri() << endl;
        g_hasCallbackArrived = true;

        cout << "Host of found resource: ";
        cout << resource->host() << endl;
        cout << "di( OCResource.sid() ) of found resource is = ";
        cout << resource->sid() << endl;
        cout << "unique identifier of found resource is = ";
        cout << resource->uniqueIdentifier() << endl;

        string resourceType = resource->getResourceTypes().front();
        if(resourceType.compare(OC_RSRVD_RESOURCE_TYPE_INTROSPECTION) == 0)
        {
            g_introspectionResource = resource;
        }

        if (resourceType == OC_RSRVD_RESOURCE_TYPE_RDPUBLISH || resourceType == OC_RSRVD_RESOURCE_TYPE_RD)
        {
            vector<string> allHosts = resource->getAllHosts();
            if (allHosts.size() > 0)
            {
                for (string host : allHosts)
                {
                    g_rdServerHosts.push_back(host);
                }
            }
            else
            {
                g_rdServerHosts.push_back(resource->host());
            }

        }

    }
    else
    {
        cout << "Found resource is invalid!!" << endl;
    }

    s_mutex.unlock();
}

void onCollectionFound(shared_ptr< OCResource > collection)
{
    cout << "Found a Collection!!" << endl;

    if (collection)
    {
        g_foundCollectionList.push_back(collection);
        cout << "uri of the found collection is " << collection->uri() << endl;
        g_hasCallbackArrived = true;

        cout << "Host of found resource: " << collection->host() << endl;
        cout << "sid of found resource is = " << collection->sid() << endl;
        cout << "unique identifier of found resource is = " << collection->uniqueIdentifier()
                << endl;
        collection->get("", DEFAULT_INTERFACE, QueryParamsMap(), onGet);
        collection->get("", BATCH_INTERFACE, QueryParamsMap(), onGet);
    }

}

void onPlatformInfoReceived(const OCRepresentation& rep)
{
    cout << "\nPlatform Information received ---->\n";
    string value;
    string values[] =
    { "pi", "Platform ID                    ", "mnmn", "Manufacturer name              ", "mnml",
            "Manufacturer url               ", "mnmo", "Manufacturer Model No          ", "mndt",
            "Manufactured Date              ", "mnpv", "Manufacturer Platform Version  ", "mnos",
            "Manufacturer OS version        ", "mnhw", "Manufacturer hardware version  ", "mnfv",
            "Manufacturer firmware version  ", "mnsl", "Manufacturer support url       ", "st",
            "Manufacturer system time       " };

    for (unsigned int i = 0; i < sizeof(values) / sizeof(values[0]); i += 2)
    {
        if (rep.getValue(values[i], value))
        {
            std::cout << values[i + 1] << " : " << value << std::endl;
        }
    }
    g_resourceHelper->printRepresentation(rep);
    g_hasCallbackArrived = true;
}

void onDeviceInfoReceived(const OCRepresentation& rep)
{
    cout << "\nDevice Information received ---->\n";
    string value;
    string values[] =
    { "di", "Device ID        ", NAME_KEY, "Device name      ", "lcv", "Spec version url ", "dmv",
            "Data Model Model ", };

    for (unsigned int i = 0; i < sizeof(values) / sizeof(values[0]); i += 2)
    {
        if (rep.getValue(values[i], value))
        {
            std::cout << values[i + 1] << " : " << value << std::endl;
        }
    }

    g_resourceHelper->printRepresentation(rep);
    g_hasCallbackArrived = true;
}

// callback handler on GET request
void onGet(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    if (eCode == SUCCESS_RESPONSE || eCode == OC_STACK_OK)
    {
        cout << "Response: GET request was successful" << endl;

        vector< string > interfaceList = rep.getResourceInterfaces();

        bool isCollection = false;
        for (auto resourceInterface : interfaceList)
        {
            if (resourceInterface.compare(BATCH_INTERFACE) == 0)
            {
                isCollection = true;
                break;
            }
        }

        if (isCollection)
        {
            vector< OCRepresentation > children = rep.getChildren();

            cout << "\nChild Resource List of Collectection:" << endl;
            for (auto child : children)
            {
                cout << "\tURI :: " << child.getUri() << endl;
            }
            cout << endl;
        }
        else
        {
            g_resourceRepresentation = rep;
            cout << "The GET Response has the following representation:" << endl;
            g_resourceHelper->printRepresentation(rep);
        }

        if(rep.hasAttribute(OC_RSRVD_INTROSPECTION_URL_INFO))
        {
            vector<OCRepresentation> urlInfo;
            rep.getValue(string(OC_RSRVD_INTROSPECTION_URL_INFO), urlInfo);
            string url;
            urlInfo.front().getValue(string(OC_RSRVD_INTROSPECTION_URL), url);
            url = url.substr(url.find_first_of("/") + 2);
            url = url.substr(url.find_first_of("/") );
            cout << "URI to send is: " << url << endl;

            vector<string> resourceTypes;
            resourceTypes.push_back(OC_RSRVD_RESOURCE_TYPE_INTROSPECTION_PAYLOAD);

            OCResource::Ptr payloadResource = OCPlatform::constructResourceObject(g_introspectionResource->host(),
                    url, g_introspectionResource->connectivityType(),
                    true, resourceTypes, g_introspectionResource->getResourceInterfaces());

            payloadResource->get(QueryParamsMap(), onGet, g_qos);
            cout << "GET request sent to introspection payload resource!!" << endl;
            waitForCallback();
        }
    }
    else
    {
        cout << "onGET Response error: " << eCode << endl;
    }
    g_hasCallbackArrived = true;
}

// callback handler on PUT request
void onPut(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    if (eCode == SUCCESS_RESPONSE || eCode == OC_STACK_RESOURCE_CHANGED
            || eCode == OC_STACK_RESOURCE_CREATED)
    {
        cout << "Response: PUT request was successful" << endl;
        cout << "The PUT response has the following representation:" << endl;
        g_resourceHelper->printRepresentation(rep);
    }
    else
    {
        cout << "onPUT Response error: " << eCode << endl;
    }
    g_hasCallbackArrived = true;
}

// callback handler on POST request
void onPost(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode)
{
    if (eCode == SUCCESS_RESPONSE || eCode == OC_STACK_RESOURCE_CHANGED
            || eCode == OC_STACK_RESOURCE_CREATED)
    {
        cout << "Response: POST request was successful" << endl;
        cout << "The POST Response has the following representation:" << endl;
        g_resourceHelper->printRepresentation(rep);
    }
    else
    {
        cout << "onPOST Response error: " << eCode << endl;
    }
    g_hasCallbackArrived = true;

}

// callback handler on DELETE request
void onDelete(const HeaderOptions &headerOptions, const int eCode)
{
    if (eCode == SUCCESS_RESPONSE || eCode == OC_STACK_RESOURCE_DELETED)
    {
        cout << "Response: DELETE request was successful" << endl;
    }
    else
    {
        cout << "onDELETE Response error: " << eCode << endl;
    }
    g_hasCallbackArrived = true;
}

void onObserve(const HeaderOptions headerOptions, const OCRepresentation &rep, const int &eCode,
        const int &sequenceNumber)
{
    try
    {
        if (eCode == SUCCESS_RESPONSE || eCode == OC_STACK_OK)
        {
            std::cout << "OBSERVE RESULT:" << std::endl;
            std::cout << "\tSequenceNumber: " << sequenceNumber << std::endl;
            ResourceHelper::getInstance()->printRepresentation(rep);
        }
    }
    catch (std::exception &e)
    {
        std::cout << "Exception: " << e.what() << " in onObserve" << std::endl;
    }
    g_hasCallbackArrived = true;

}


void onResourceRemoved(const int& eCode)
{
    cout << "Received Delete Resource Response From RD" << endl;
    try
    {
        if (OC_STACK_RESOURCE_DELETED == eCode)
        {
            cout << "Resource delete was successful" << endl;
        }
        else
        {
            cout << "onDelete Response error: " << eCode << endl;
        }
    }
    catch(std::exception& e)
    {
        cout << "Exception: " << e.what() << " in onDelete" << endl;
    }
}

void onResourcePublished(const OCRepresentation& rep, const int& eCode)
{
    cout << endl <<"Received Publish Resource Response From RD" << endl;

    try
    {
        if (OC_STACK_RESOURCE_CHANGED == eCode)
        {
            cout << "=========== Published Resource ===========" << endl;
            if (rep.hasAttribute("di"))
            {
                cout << " di: " << rep.getValue<std::string>("di") << endl;
            }

            // Published Resource is included as the child resource.
            std::vector<OCRepresentation> children = rep.getChildren();

            for (auto oit = children.begin(); oit != children.end(); ++oit)
            {
                string m_href;
                oit->getValue("href", m_href);
                cout << "   href : " << m_href << endl;

                cout << "   resource type : " << endl;
                for(const std::string& type : oit->getResourceTypes())
                {
                    cout << "       " <<  type << "\n";
                }

                cout << "   resource interface : " << endl;
                for(const std::string& type : oit->getResourceInterfaces())
                {
                    cout << "       " <<  type << "\n";
                }

                int m_ins;
                oit->getValue("ins", m_ins);
                cout << "   ins : " << m_ins << endl;
            }
            cout << "=========================================" << endl;
        }
        else
        {
            cout << "onPublish Response error: " << eCode << endl;
        }
    }
    catch(std::exception& e)
    {
        cout << "Exception: " << e.what() << " in onPublish" << endl;
    }
}

void waitForCallback()
{
    int elapsedSecond = 0;
    while (g_hasCallbackArrived == false)
    {
        g_resourceHelper->waitInSecond(CALLBACK_WAIT_MIN);
        elapsedSecond++;
        if (elapsedSecond > CALLBACK_WAIT_MAX)
        {
            break;
        }
    }
}

FILE* server_fopen(const char *path, const char *mode)
{
    if (0 == strcmp(path, OC_INTROSPECTION_FILE_NAME))
    {
        return fopen(INTROSPECTION_SWAGGER_FILE, mode);
    }
#ifdef __SECURED__
    else if (0 == strcmp(path, OC_SECURITY_DB_DAT_FILE_NAME))
    {
        return fopen(CRED_FILE_SERVER, mode);
    }
#endif
    return fopen(path, mode);
}

FILE* client_fopen(const char *path, const char *mode)
{
    if (0 == strcmp(path, OC_INTROSPECTION_FILE_NAME))
    {
        return fopen(INTROSPECTION_SWAGGER_FILE, mode);
    }
#ifdef __SECURED__
    else if (0 == strcmp(path, OC_SECURITY_DB_DAT_FILE_NAME))
    {
        return fopen(CRED_FILE_CLIENT, mode);
    }
#endif
    return fopen(path, mode);
}

void createResourceWithUrl()
{
    if (g_childHandle == NULL)
    {
        cout << "Creating Resource with complete URL" << endl;
        findResource(RESOURCE_TYPE_LIGHT);
        if (!g_foundResourceList.empty())
        {
            OCPlatform::registerResource(g_childHandle, g_foundResourceList.at(0));
        }
    }
    else
    {
        cout << "Resource with complete URL already created!!" << endl;
    }
}

bool startResource(const std::shared_ptr<SampleResource>& resource)
{
    if (!IUTSimulatorUtils::getResourceByURI(g_createdResourceList, resource->getUri()))
    {
        cout << "Creating " << resource->getUri() << " Resource!" << endl;

        if (resource->startResource() == OC_STACK_OK)
        {
            cout << resource->getUri() << " Resource created successfully!" << endl;
            g_createdResourceList.push_back(resource);

            return true;
        }
        else
        {
            cout << "Unable to create " << resource->getUri() << " Resource!" << endl;
        }
    }
    else
    {
        cout << "Resource with URI \"" << resource->getUri() << "\" is already created!" << endl;
    }

    return false;
}

std::shared_ptr<SampleResource> startResource(
    const std::string& resourceUri,
    const std::string& resourceType,
    const std::string& resourceInterface,
    const uint8_t& resourceProperty,
    const ResponseTimeType& responseTimeType = ResponseTimeType::NORMAL,
    const HandlerType& handlerType = HandlerType::SAMPLE)
{
    if (!IUTSimulatorUtils::getResourceByURI(g_createdResourceList, resourceUri))
    {
        cout << "Creating " << resourceUri << " Resource!" << endl;
        std::shared_ptr< SampleResource > resource = SampleResourceFactory::createResource(
            resourceUri,
            resourceType,
            resourceInterface,
            resourceProperty
        );
        resource->setResponseTimeType(responseTimeType);

        if (resource->startResource(resourceProperty, handlerType) == OC_STACK_OK)
        {
            cout << resourceUri << " Resource created successfully!" << endl;
            g_createdResourceList.push_back(resource);

            return resource;
        }
        else
        {
            cout << "Unable to create " << resourceUri << " Resource!" << endl;
        }
    }
    else
    {
        cout << "Resource with URI \"" << resourceUri<< "\" is already created!" << endl;
    }

    return nullptr;
}

std::shared_ptr<SampleResource> startResource(
    const std::string& resourceUri,
    const std::string& resourceType,
    const std::string& resourceInterface,
    const uint8_t& resourceProperty,
    const HandlerType& handlerType)
{
    return startResource(resourceUri, resourceType, "", resourceProperty, ResponseTimeType::NORMAL, handlerType);
}

std::shared_ptr<SampleResource> startResource(
    const std::string& resourceUri,
    const std::string& resourceType,
    const uint8_t& resourceProperty,
    const ResponseTimeType& responseTimeType = ResponseTimeType::NORMAL,
    const HandlerType& handlerType = HandlerType::SAMPLE)
{
    return startResource(resourceUri, resourceType, "", resourceProperty, responseTimeType, handlerType);
}

std::shared_ptr<SampleResource> startResource(
    const std::string& resourceUri,
    const std::string& resourceType,
    const uint8_t& resourceProperty,
    const HandlerType& handlerType)
{
    return startResource(resourceUri, resourceType, "", resourceProperty, ResponseTimeType::NORMAL, handlerType);
}

std::shared_ptr< SampleResource > startTemperatureSensorResource(const std::string& sensorUri, const uint8_t& sensorResourceProperty)
{
    std::shared_ptr< SampleResource > temperatureSensorResource = std::make_shared< SampleResource >(
        sensorUri,
        TEMPERATURE_RESOURCE_TYPE,
        TEMPERATURE_SENSOR_INTERFACE,
        sensorResourceProperty,
        IUTSimulatorUtils::createRepresentation({
            {UNITS_KEY, string(UNITS_VALUE)},
            {TEMPERATURE_KEY, DEFAULT_TEMPERATURE_VALUE}
        })
    );
    temperatureSensorResource->setAsReadOnly(UNITS_KEY);
    temperatureSensorResource->setAsReadOnly(TEMPERATURE_KEY);

    if(startResource(temperatureSensorResource))
    {
        return temperatureSensorResource;
    }
    else
    {
        return nullptr;
    }
}

std::shared_ptr< SampleResource > startTimerResourceWithLongLocation(const std::string& resourceUri, const uint8_t& resourceProperty)
{
    std::shared_ptr< SampleResource > timerResource = SampleResourceFactory::createResource(
        resourceUri,
        TIMER_RESOURCE_TYPE,
        resourceProperty
    );
    timerResource->updateRepresentation(TIMER_LOCATION_KEY, string(VERY_BIG_VALUE));
    timerResource->setResponseTimeType(ResponseTimeType::SLOW);

    if(startResource(timerResource))
    {
        return timerResource;
    }
    else
    {
        return nullptr;
    }
}

std::shared_ptr< SampleResource > startVendorSwingResource(const std::string& resourceUri, const uint8_t& resourceProperty)
{
    std::shared_ptr< SampleResource > vendorSwingResource = SampleResourceFactory::createResource(
        resourceUri,
        SWING_RESOURCE_TYPE,
        resourceProperty
    );
    vendorSwingResource->setResourceRepresentation(
        IUTSimulatorUtils::createRepresentation({
            {"x.com.vendor.swing.on", false},
            {"x.com.vendor.swing.blade.movement.direction", string("horizontal")},
            {"x.com.vendor.swing.supported.direction", vector<string>{"horizontal", "vertical"}}
        })
    );
    vendorSwingResource->setAsReadOnly("x.com.vendor.swing.supported.direction");

    if(startResource(vendorSwingResource))
    {
        return vendorSwingResource;
    }
    else
    {
        return nullptr;
    }
}

std::shared_ptr< SampleResource > startExtraLightResource(const std::string& resourceUri, const uint8_t& resourceProperty)
{
    std::shared_ptr< SampleResource > extraLightResource = SampleResourceFactory::createResource(
        resourceUri,
        Device_TYPE_LIGHT,
        resourceProperty
    );
    extraLightResource->updateRepresentation(DEVICE_ID_KEY, g_di);

    if(startResource(extraLightResource))
    {
        return extraLightResource;
    }
    else
    {
        return nullptr;
    }
}

void createCollectionWithBatch(bool secured)
{
    cout << "Creating Batch Collection!!" << endl;
    uint8_t resourceProperty = OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE | isSecure(secured);
    uint8_t resourcePropertyHidden = OC_ACTIVE | OC_OBSERVABLE | isSecure(secured);

    if (g_isSampleCollectionCreated == false)
    {
        g_sampleCollection = new SampleCollection();
        g_sampleCollection->setResourceProperties(COLLECTION_URI, GROUP_TYPE_DEFAULT, LINK_INTERFACE);
        g_sampleCollection->setDI(g_di);
        g_sampleCollection->setIPVer(g_ipVer);
        g_sampleCollection->setSecured(true);
        g_sampleCollection->setName("Sample Collection Device");

        if (g_sampleCollection->startResource(resourceProperty, HandlerType::DEFAULT) == OC_STACK_OK)
        {
            cout << "Batch collection created successfully" << endl;
            g_sampleCollection->addResourceInterface(DEFAULT_INTERFACE);
            g_sampleCollection->addResourceInterface(BATCH_INTERFACE);
            g_sampleCollection->addChild(
                startResource(COLLECTION_SWITCH_URI, SWITCH_RESOURCE_TYPE, resourcePropertyHidden, HandlerType::DEFAULT)
            );
            g_isSampleCollectionCreated = true;
        }
        else
        {
            cout << "Unable to create Batch Collection" << endl;
        }
    }
    else
    {
        cout << "Already Batch Collection is created!!" << endl;
    }
}

void createResource()
{
    cout << "createResource called!!" << endl;

    startResource(LIGHT_1_URI, RESOURCE_TYPE_LIGHT, OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE);
    startResource(FAN_1_URI, RESOURCE_TYPE_FAN, OC_ACTIVE | OC_DISCOVERABLE);
}

void createSecuredResource()
{
    cout << "createSecuredResource called!!" << endl;

    startResource(LIGHT_SECURED_URI, RESOURCE_TYPE_LIGHT, OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);
    startResource(FAN_SECURED_URI, RESOURCE_TYPE_FAN, OC_ACTIVE | OC_DISCOVERABLE | OC_SECURE);
}

void createInvisibleResource()
{
    cout << "createInvisibleResource called!!" << endl;

    startResource(
        LIGHT_INVISIBLE_URI,
        RESOURCE_TYPE_LIGHT,
        string(ACTUATOR_INTERFACE) + " " + string(DEFAULT_INTERFACE),
        OC_ACTIVE | OC_OBSERVABLE | OC_EXPLICIT_DISCOVERABLE
    );
    startResource(FAN_INVISIBLE_URI, RESOURCE_TYPE_FAN, OC_ACTIVE);
}

void createSingleAirConResource(bool secured)
{
    cout << "Creating Single Air Conditioner Device Resource!!" << endl;
    uint8_t resourceProperty = OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE | isSecure(secured);

    startResource(AC_SWITCH_URI, SWITCH_RESOURCE_TYPE, resourceProperty);

    SampleResource::setDeviceInfo(ENGLISH_NAME_VALUE, vector<string>{Device_TYPE_LIGHT, Device_TYPE_VENDOR}, g_ocfVer);
}

void createTvDevice(bool secured)
{
    cout << "Creating TV Device Resources!!" << endl;
    uint8_t resourceProperty = OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE | isSecure(secured);

    startResource(TV_SWITCH_URI, SWITCH_RESOURCE_TYPE, resourceProperty);
    startResource(TV_MEDIA_SOURCE_LIST_URI, MEDIA_SOURCE_LIST_RESOURCE_TYPE, resourceProperty, ResponseTimeType::SLOW);
    startResource(TV_AUDIO_URI, AUDIO_RESOURCE_TYPE, resourceProperty);

    SampleResource::setDeviceInfo("Vendor Smart Home TV Device", Device_TYPE_TV, g_ocfVer);
}

void createResourceForIntrospection(bool secured)
{
    cout << "Creating AirCon Device Resources!!" << endl;
    uint8_t resourceProperty = OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE | isSecure(secured);

    startResource(AC_SWITCH_URI, SWITCH_RESOURCE_TYPE, resourceProperty);
    startResource(AC_TEMPERATURE_URI, TEMPERATURE_RESOURCE_TYPE, resourceProperty)->setSensorTwin(
        startTemperatureSensorResource(AC_SENSOR_URI, resourceProperty)
    );
    startResource(AC_DIMMING_URI, DIMMING_RESOURCE_TYPE, resourceProperty);
    startResource(AC_CHROMA_URI, CHROMA_RESOURCE_TYPE, resourceProperty);

    SampleResource::setDeviceInfo(ENGLISH_NAME_VALUE, vector<string>{Device_TYPE_AC, Device_TYPE_VENDOR}, g_ocfVer);
}

void createAirConDevice(bool secured)
{
    cout << "Creating AirCon Device Resources!!" << endl;
    uint8_t resourceProperty = OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE | isSecure(secured);
    uint8_t resourcePropertyHidden = OC_ACTIVE | OC_OBSERVABLE | isSecure(secured);

    startResource(AC_SWITCH_URI, SWITCH_RESOURCE_TYPE, resourceProperty);
    startResource(AC_SWITCH_URI_CHILD, SWITCH_RESOURCE_TYPE, resourcePropertyHidden);

    startResource(AC_TEMPERATURE_URI, TEMPERATURE_RESOURCE_TYPE, resourceProperty);
    startResource(AC_TEMPERATURE_URI_CHILD, TEMPERATURE_RESOURCE_TYPE, resourcePropertyHidden);

    setlocale(LC_ALL, "");
    startResource(AC_AIR_FLOW_URI, AIR_FLOW_RESOURCE_TYPE, resourceProperty);
    startResource(AC_AIR_FLOW_URI_CHILD, AIR_FLOW_RESOURCE_TYPE, resourcePropertyHidden);

    setlocale(LC_ALL, "");
    startTimerResourceWithLongLocation(AC_TIMER_URI, resourceProperty);
    startTimerResourceWithLongLocation(AC_TIMER_URI_CHILD, resourcePropertyHidden);

    startResource(AC_SWING_URI, SWING_RESOURCE_TYPE, OC_ACTIVE | OC_DISCOVERABLE);
    startResource(AC_SWING_URI_CHILD, SWING_RESOURCE_TYPE, resourcePropertyHidden);

    startResource(AC_CON_URI, CON_RESOURCE_TYPE, OC_ACTIVE | OC_DISCOVERABLE | isSecure(secured));

    SampleResource::setDeviceInfo(ENGLISH_NAME_VALUE, vector<string>{Device_TYPE_AC, Device_TYPE_VENDOR}, g_ocfVer);
}

void createExtraDevice(bool secured)
{
    cout << "Creating Extra Collection Device Resources!!" << endl;
    uint8_t resourceProperty = OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE | isSecure(secured);

    startResource(EXTRA_SWITCH_URI, SWITCH_RESOURCE_TYPE, resourceProperty);
    startExtraLightResource(EXTRA_LIGHT_URI, resourceProperty);

    if (g_isExtraDeviceCreated == false)
    {
        g_extraLightCollection = new SampleCollection();
        g_extraLightCollection->setResourceProperties(EXTRA_COLLECTION_URI, GROUP_TYPE_DEFAULT, LINK_INTERFACE);
        g_extraLightCollection->setResourceRepresentation(
            IUTSimulatorUtils::createRepresentation({
                {NAME_KEY, string(ENGLISH_NAME_VALUE)},
                {DEVICE_ID_KEY, g_di},
                {ICV_KEY, string(CORE_SPEC_VERSION)},
                {DMV_KEY, string(RESOURCE_TYPE_SPEC_VERSION) + "," + string(SMART_HOME_SPEC_VERSION)},
                {PIID_KEY, string(PLATFORM_ID)}
        }));
        g_extraLightCollection->setDI(g_di);
        g_extraLightCollection->setIPVer(g_ipVer);
        g_extraLightCollection->setSecured(true);
        g_extraLightCollection->setName(ENGLISH_NAME_VALUE);

        if (g_extraLightCollection->startResource(resourceProperty) == OC_STACK_OK)
        {
            cout << "Extra Light Resource created successfully" << endl;
            g_extraLightCollection->addResourceType(Device_TYPE_LIGHT);
            g_extraLightCollection->addResourceInterface(READ_ONLY_INTERFACE);
            g_extraLightCollection->addResourceInterface(DEFAULT_INTERFACE);
            g_extraLightCollection->addChild(
                startResource(EXTRA_BRIGHTNESS_URI, BRIGHTNESS_RESOURCE_TYPE, resourceProperty)
            );
            g_isExtraDeviceCreated = true;
        }
        else
        {
            cout << "Unable to create Extra Collection" << endl;
        }
    }

    SampleResource::setDeviceInfo(ENGLISH_NAME_VALUE);
}

void createVendorDefinedDevice(bool secured)
{
    cout << "Creating AirCon Device Resources!!" << endl;
    uint8_t resourceProperty = OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE | isSecure(secured);
    uint8_t resourcePropertyHidden = OC_ACTIVE | OC_OBSERVABLE | isSecure(secured);

    setlocale(LC_ALL, "");
    startResource(AC_TIMER_URI, TIMER_RESOURCE_TYPE, resourceProperty, ResponseTimeType::SLOW);
    startResource(AC_TIMER_URI_CHILD, TIMER_RESOURCE_TYPE, resourcePropertyHidden, ResponseTimeType::SLOW);

    startVendorSwingResource(AC_SWING_URI, resourceProperty);
    startVendorSwingResource(AC_SWING_URI, resourcePropertyHidden);

    SampleResource::setDeviceInfo("Vendor Defined System Server", Device_TYPE_VENDOR, g_ocfVer);
}

void createManyLightResources()
{
    cout << "Creating many Light Resources!!" << endl;
    int lightCount = 2;

    for (int i = 0; i < MAX_LIGHT_RESOURCE_COUNT; i++)
    {
        startResource("/device/light-" + to_string(lightCount++), RESOURCE_TYPE_LIGHT, OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE);
    }
}

void publishResourcesToRD()
{
    ResourceHandles handlesToPublish;
    for (std::shared_ptr< SampleResource > resource : g_createdResourceList)
    {
        handlesToPublish.push_back(resource->getResourceHandle());
    }

    if (g_rdServerHosts.size() > 0)
    {
        for ( string rdHost : g_rdServerHosts)
        {
            cout << "Publishingg all resources to RD Host: " << rdHost << endl;
            RDClient::Instance().publishResourceToRD( rdHost, g_ipVer, handlesToPublish, &onResourcePublished);
        }
    }
    else
    {
        cout << "No RD server available" << endl;
    }
}

void updateResourcesToRD()
{
    ResourceHandles handlesToPublish;
    g_createdResourceList.at(0)->addResourceType("x.org.vendor.type.random");
    handlesToPublish.push_back(g_createdResourceList.at(0)->getResourceHandle());

    if (g_rdServerHosts.size() > 0)
    {
        for ( string rdHost : g_rdServerHosts)
        {
            cout << "Publishingg all resources to RD Host: " << rdHost << endl;
            RDClient::Instance().publishResourceToRD( rdHost, g_ipVer, handlesToPublish,
                    &onResourcePublished);
        }
    }
    else
    {
        cout << "No RD server available" << endl;
    }
}

void deleteResourcesFromRD()
{
    ResourceHandles handlesToDelete;
    for (std::shared_ptr< SampleResource >  resource : g_createdResourceList)
    {
        handlesToDelete.push_back(resource->getResourceHandle());
    }

    if (g_rdServerHosts.size() > 0)
    {
        for ( string rdHost : g_rdServerHosts)
        {
            cout << "Publishingg all resources to RD Host: " << rdHost << endl;
            RDClient::Instance().deleteResourceFromRD( rdHost, g_ipVer, handlesToDelete, &onResourceRemoved);
        }
    }
    else
    {
        cout << "No RD server available" << endl;
    }
}

void createGroup(string groupType)
{
    if (g_isGroupCreated)
    {
        cout << "Group already created!!" << endl;
    }
    else
    {
        try
        {
            string resourceURI = COLLECTION_RESOURCE_URI;
            string resourceInterface = BATCH_INTERFACE;
            uint8_t collectionProperty = OC_ACTIVE | OC_DISCOVERABLE;
            if (g_isSecuredServer)
            {
                collectionProperty = collectionProperty | OC_SECURE;
            }

            g_vendorCollectionResource = new SampleCollection();
            g_vendorCollectionResource->setResourceProperties(resourceURI, groupType, LINK_INTERFACE);
            g_vendorCollectionResource->setResourceRepresentation(IUTSimulatorUtils::createRepresentation());
            g_vendorCollectionResource->setDI(g_di);
            g_vendorCollectionResource->setIPVer(g_ipVer);
            g_vendorCollectionResource->setSecured(true);
            g_vendorCollectionResource->setName(ENGLISH_NAME_VALUE);

            if (g_vendorCollectionResource->startResource(collectionProperty) == OC_STACK_OK)
            {
                cout << "Successfully Created Group!!" << endl;
                g_vendorCollectionResource->addResourceType(GROUP_TYPE_AIRCON_VENDOR);
                g_vendorCollectionResource->addResourceInterface(BATCH_INTERFACE);
                g_vendorCollectionResource->addResourceInterface(DEFAULT_INTERFACE);
                g_isGroupCreated = true;
            }
            else
            {
                cout << "Unable to create Extra Collection" << endl;
            }

            if (g_createdResourceList.size() > 0)
            {
                for (shared_ptr< SampleResource > resource : g_createdResourceList)
                {
                    if (resource->getUri().find("Children") != string::npos)
                    {
                        cout << "Joining resource " << resource->getUri()
                                << " to vendor handled group" << endl;
                        g_vendorCollectionResource->addChild(resource);
                    }
                    else if (resource->getUri().find("Child") != string::npos)
                    {
                        cout << "Joining resource " << resource->getUri()
                                << " to vendor handled group" << endl;
                    }
                }

            }
        }
        catch (OCException& e)
        {
            oclog() << "Exception in main: " << e.what();
            g_isGroupCreated = false;
            cout << "Unable to Create Group!!" << endl;
        }

    }

}

void deleteGroup()
{
    if (g_isGroupCreated)
    {
        try
        {
            OCStackResult result = OCPlatform::unregisterResource(g_collectionHandle);
            if (result == OC_STACK_OK)
            {
                cout << "Successfully Deleted Group!!" << endl;
                g_isGroupCreated = false;
            }
            else
            {
                cout << "Unable to delete group!!" << endl;
            }
        }
        catch (exception& e)
        {
            std::cout << "Exception occurred while deleting group, exception is :  " << e.what()
                    << std::endl;
        }

    }
    else
    {
        cout << "No group exists to delete!!" << endl;
    }

    while (g_handleList.size() > 0)
    {
        int index = g_handleList.size() - 1;
        OCPlatform::unregisterResource(g_handleList[index]);
        g_handleList.pop_back();
    }
}

void deleteCreatedResource()
{
    while (g_handleList.size() > 0)
    {
        int index = g_handleList.size() - 1;
        OCPlatform::unregisterResource(g_handleList[index]);
        g_handleList.pop_back();
    }

    g_createdResourceList.clear();
}

void findResource(string resourceType, string host)
{
    g_foundResourceList.clear();
    std::ostringstream requestURI;

    try
    {
        // makes it so that all boolean values are printed as 'true/false' in this stream
        std::cout.setf(std::ios::boolalpha);
        // Find all resources
        requestURI << OC_RSRVD_WELL_KNOWN_URI << resourceType;

        OCPlatform::findResource(host, requestURI.str(), OCConnectivityType::CT_DEFAULT,
                &onResourceFound, g_qos);
        cout << "Finding Resource...." << endl;

    }
    catch (OCException &e)
    {
        oclog() << "Exception in main: " << e.what();
    }
    waitForCallback();
}

void findCollection(string collectionType)
{
    g_foundCollectionList.clear();
    ostringstream requestURI;

    try
    {
        // makes it so that all boolean values are printed as 'true/false' in this stream
        std::cout.setf(std::ios::boolalpha);
        // Find all colletion
        requestURI << OC_RSRVD_WELL_KNOWN_URI << "?rt=" << collectionType;

        OCPlatform::findResource("", requestURI.str(), OCConnectivityType::CT_DEFAULT,
                &onCollectionFound, g_qos);
        cout << "Finding Collection...." << endl;

    }
    catch (OCException &e)
    {
        oclog() << "Exception when finding collection: " << e.what();
    }
    waitForCallback();
}

void findAllResources(string host, string query)
{
    g_foundResourceList.clear();
    std::ostringstream requestURI;

    try
    {
        // makes it so that all boolean values are printed as 'true/false' in this stream
        std::cout.setf(std::ios::boolalpha);
        // Find all resources
        if (query.compare("") == 0)
        {
            requestURI << OC_RSRVD_WELL_KNOWN_URI;
        }
        else
        {
            requestURI << OC_RSRVD_WELL_KNOWN_URI << "?" << query;
        }

        OCPlatform::findResource(host, requestURI.str(), OCConnectivityType::CT_DEFAULT,
                &onResourceFound, g_qos);
        cout << "Finding Resource...." << endl;

    }
    catch (OCException &e)
    {
        oclog() << "Exception in main: " << e.what();
    }
    waitForCallback();
}

void discoverIntrospection(bool isMulticast)
{
    string host = "";
    g_hasCallbackArrived = false;
    ostringstream introspectionDiscoveryRequest;
    if (isMulticast)
    {
        introspectionDiscoveryRequest << "rt=" << OC_RSRVD_RESOURCE_TYPE_INTROSPECTION;
        cout << "Discovering Introspection using Multicast... " << endl;
    }
    findAllResources(host, introspectionDiscoveryRequest.str());

    if (g_foundResourceList.size() == 0)
    {
        cout << "No resource found!!" << endl;
    }
}

void discoverDevice(bool isMulticast)
{
    string host = "";
    g_hasCallbackArrived = false;
    ostringstream deviceDiscoveryRequest;
    string deviceDiscoveryURI = "/oic/d";
    OCStackResult result = OC_STACK_ERROR;

    if (isMulticast)
    {
        deviceDiscoveryRequest << OC_MULTICAST_PREFIX << deviceDiscoveryURI;
        cout << "Discovering Device using Multicast... " << endl;
    }
    else
    {
        if (g_foundResourceList.size() == 0)
        {
            cout << "Finding OIC Servers to send Unicast Discovery Request" << endl;
            findAllResources();
        }

        g_hasCallbackArrived = false;
        if (g_foundResourceList.size() > 0)
        {
            host = g_foundResourceList.at(0)->host();
            deviceDiscoveryRequest << deviceDiscoveryURI;
            cout << "Discovering Device using Unicast... " << endl;
            cout << "Sending Unicast device discovery to the host: " << host << endl;
        }
        else
        {
            cout << "No server found to send Unicast device discovery" << endl;
            cout << "Sending Multicast Device discovery request..." << endl;
            deviceDiscoveryRequest << OC_MULTICAST_PREFIX << deviceDiscoveryURI;
        }

    }

    try
    {
        result = OCPlatform::getDeviceInfo(host, deviceDiscoveryRequest.str(),
                OCConnectivityType::CT_DEFAULT, &onDeviceInfoReceived);

        if (result == OC_STACK_OK)
        {
            cout << "Device discovery done successfully" << endl;
            waitForCallback();
        }
        else
        {
            cout << "Device discovery failed." << endl;
        }

    }
    catch (OCException& e)
    {
        cerr << "Exception occurred while discovering device, reason is: " << e.reason() << endl;
    }

}
void discoverPlatform(bool isMulticast)
{
    string host = "";
    ostringstream platformDiscoveryRequest;
    string platformDiscoveryURI = "/oic/p";

    if (isMulticast)
    {
        platformDiscoveryRequest << OC_MULTICAST_PREFIX << platformDiscoveryURI;
        cout << "Discovering Platform using Multicast... " << endl;

    }
    else
    {
        if (g_foundResourceList.size() == 0)
        {
            findAllResources();
            g_hasCallbackArrived = false;
        }

        host = g_foundResourceList.at(0)->host();
        platformDiscoveryRequest << platformDiscoveryURI;
        cout << "Discovering Platform using Unicast... " << endl;
    }

    try
    {
        OCStackResult ret = OC_STACK_ERROR;
        ret = OCPlatform::getPlatformInfo(host, platformDiscoveryRequest.str(),
                OCConnectivityType::CT_DEFAULT, &onPlatformInfoReceived);

        cout << "Platform discovery ";
        if (ret == OC_STACK_OK)
        {
            cout << "done." << endl;
            waitForCallback();
        }
        else
        {
            cout << "failed." << endl;
        }

    }
    catch (OCException& e)
    {
        cerr << "Failure in main thread: " << e.reason() << endl;
    }
}

void sendGetRequest()
{
    std::shared_ptr< OCResource > selectedResource = IUTSimulatorUtils::getResourceFromUser(g_foundResourceList);

    if (selectedResource)
    {
        IUTSimulatorUtils::setResourceHostEndpointFromUser(selectedResource, g_isSecuredClient);
        cout << "Sending Get Request to the resource with: " << selectedResource->host() << selectedResource->uri() << endl;

        if (selectedResource->getResourceTypes().front().compare(OC_RSRVD_RESOURCE_TYPE_INTROSPECTION) == 0)
        {
            g_introspectionResource->setHost(selectedResource->host());
        }

        QueryParamsMap qpMap;
        selectedResource->get(qpMap, onGet, g_qos);
        cout << "GET request sent!!" << endl;
    }
}

void sendGetRequestWithQuery(string key, string value)
{
    std::shared_ptr< OCResource > selectedResource = IUTSimulatorUtils::getResourceFromUser(g_foundResourceList);

    if (selectedResource)
    {
        QueryParamsMap qpMap;
        qpMap[key] = value;
        selectedResource->get(qpMap, onGet, g_qos);
        cout << "GET request sent!!" << endl;
        waitForCallback();

    }
    else
    {
        cout << "No resource to send GET!!" << endl;
    }
}

void sendPutRequestUpdate()
{
    std::shared_ptr< OCResource > selectedResource = IUTSimulatorUtils::getResourceFromUser(g_foundResourceList);

    if (selectedResource)
    {
        cout << "Sending Complete Update Message(PUT)..." << endl;

        OCRepresentation representation =
            IUTSimulatorUtils::createRepresentation({
                {"region", string("Rajshahi, Bangladesh")},
                {POWER_KEY, string("on")},
                {INTENSITY_KEY, string("50")},
                {MANUFACTURER_KEY, string("SRBD")}
        });

        selectedResource->put(representation, QueryParamsMap(), &onPut, g_qos);
        cout << "PUT request sent!!" << endl;
        waitForCallback();
    }
    else
    {
        cout << "No resource to send PUT!!" << endl;
    }
}

void sendPutRequestCreate()
{
    std::shared_ptr< OCResource > selectedResource = IUTSimulatorUtils::getResourceFromUser(g_foundResourceList);

    if (selectedResource)
    {
        cout << "Sending Create Resource Message(PUT)..." << endl;

        OCRepresentation representation =
            IUTSimulatorUtils::createRepresentation({
                {POWER_KEY, string("off")},
                {INTENSITY_KEY, string("0")}
        });
        representation.setUri("/new-device/created-light-1");
        representation.setResourceTypes(vector<string>{"core.light"});

        selectedResource->put(representation, QueryParamsMap(), &onPut, g_qos);
        cout << "PUT request sent!!" << endl;
        waitForCallback();

    }
    else
    {
        cout << "No resource to send PUT!!" << endl;
    }
}

void sendPostRequestUpdate()
{
    std::shared_ptr< OCResource > selectedResource = IUTSimulatorUtils::getResourceFromUser(g_foundResourceList);

    if (selectedResource)
    {
        cout << "Sending Partial Update Message(POST)..." << endl;

        OCRepresentation representation =
            IUTSimulatorUtils::createRepresentation({
                {REGION_KEY, string("Rajshahi, Bangladesh")},
                {INTENSITY_KEY, 100}
        });

        selectedResource->post(representation, QueryParamsMap(), &onPost, g_qos);
        cout << "POST request sent!!" << endl;
        waitForCallback();

    }
    else
    {
        cout << "No resource to send POST!!" << endl;
    }
}

void updateGroup()
{
    cout << "Please select only Collection resource" << endl;
    std::shared_ptr< OCResource > selectedResource = IUTSimulatorUtils::getResourceFromUser(g_foundResourceList);

    if (selectedResource)
    {
        cout << "Sending Partial Update Message(POST) to Collection..." << endl;

        OCRepresentation representation =
            IUTSimulatorUtils::createRepresentation({
                {RESOURCE_TYPE_KEY, string(RESOURCE_TYPE_ROOM)},
                {INTERFACE_KEY, string("oic.if.baseline oic.if.b oic.mi.grp")},
                {URI_KEY, string("/core/a/collection")}
        });
        representation.setChildren(
            vector< OCRepresentation > {
                IUTSimulatorUtils::createRepresentation({
                    {INTENSITY_KEY, 100},
                    {RESOURCE_TYPE_KEY, string("core.light")},
                    {INTERFACE_KEY, string(ACTUATOR_INTERFACE)},
                    {URI_KEY, string("/device/light-1")}
                })
            }
        );

        selectedResource->post(RESOURCE_TYPE_ROOM, GROUP_INTERFACE, representation, QueryParamsMap(), &onPost, g_qos);
        cout << "POST request sent!!" << endl;
        waitForCallback();

    }
}

void updateLec()
{
    ESErrorCode errCode = ES_ERRCODE_NO_ERROR;

    cout << "Please select ErrorCode as reported by Enrollee and press Enter: " << endl;
    cout << "\t\t 0. Cancel update" << endl;
    cout << "\t\t 1. WiFi's SSID is not found" << endl;
    cout << "\t\t 2. WiFi's password is wrong" << endl;
    cout << "\t\t 3. IP Address not allocated" << endl;
    cout << "\t\t 4. No Internet Connection" << endl;
    cout << "\t\t 5. Timeout occured" << endl;
    cout << "\t\t 6. Cloud server is not reachable" << endl;
    cout << "\t\t 7. No response is arrived from cloud server" << endl;
    cout << "\t\t 8. Delivered authcode is not valid" << endl;
    cout << "\t\t 9. Given access token is not valid due to its expiration" << endl;
    cout << "\t\t 10. Refresh of expired access token is failed" << endl;
    cout << "\t\t 11. Target device is not discovered in cloud server" << endl;
    cout << "\t\t 12. Target user does not exist in cloud server" << endl;
    cout << "\t\t 13. Unsupported WiFi frequency" << endl;

    switch(IUTSimulatorUtils::getSelectionFromUser("ErrorCode", 0, 13))
    {
        case 0:
                return;

    /**
     * Error Code that given WiFi's SSID is not found
     */
        case 1:
                errCode = ES_ERRCODE_SSID_NOT_FOUND;
                break;

    /**
     * Error Code that given WiFi's Password is wrong
     */
        case 2:
                errCode = ES_ERRCODE_PW_WRONG;
                break;
    /**
     * Error Code that IP address is not allocated
     */
        case 3:
                errCode = ES_ERRCODE_IP_NOT_ALLOCATED;
                break;

    /**
     * Error Code that there is no Internet connection
     */
        case 4:
                errCode = ES_ERRCODE_NO_INTERNETCONNECTION;
                break;

    /**
     * Error Code that Timeout occured
     */
        case 5:
                errCode = ES_ERRCODE_TIMEOUT;
                break;

    /**
     * Error Code that cloud server is not reachable due to wrong URL of cloud server, for example.
     */
        case 6:
                errCode = ES_ERRCODE_FAILED_TO_ACCESS_CLOUD_SERVER;
                break;

    /**
     * Error Code that no response is arrived from cloud server
     */
       case 7:
               errCode = ES_ERRCODE_NO_RESPONSE_FROM_CLOUD_SERVER;
                break;

    /**
     * Error Code that a delivered authcode is not valid.
     */
       case 8:
                errCode = ES_ERRCODE_INVALID_AUTHCODE;
                break;

    /**
     * Error Code that a given access token is not valid due to its expiration, for example.
     */
       case 9:
                errCode = ES_ERRCODE_INVALID_ACCESSTOKEN;
                break;

    /**
     * Error Code that a refresh of expired access token is failed due to some reasons.
     */
       case 10:
                errCode = ES_ERRCODE_FAILED_TO_REFRESH_ACCESSTOKEN;
                break;

    /**
     * Error Code that a target device is not discovered in cloud server
     */
       case 11:
                errCode = ES_ERRCODE_FAILED_TO_FIND_REGISTERED_DEVICE_IN_CLOUD;
                break;

    /**
     * Error Code that a target user does not exist in cloud server.
     */
       case 12:
                errCode = ES_ERRCODE_FAILED_TO_FIND_REGISTERED_USER_IN_CLOUD;
                break;

    /**
     * Error Code that an enrollee can not connect to a target WiFi AP because the AP resides in
     * an unsupported WiFi frequency.
     */
       case 13:
                errCode = ES_ERRCODE_UNSUPPORTED_WIFI_FREQUENCY;
                break;

       default:
                errCode = ES_ERRCODE_UNKNOWN;
                break;
    }
    ESSetErrorCode(errCode);
}

void updateLocalResource()
{
    std::shared_ptr< SampleResource > selectedResource = IUTSimulatorUtils::getResourceFromUser(g_createdResourceList);

    if (selectedResource)
    {
        cout << "Please provide Attribute Key: ";
        string attributeKey = IUTSimulatorUtils::getInputFromUser<string>();

        OCRepresentation representation = selectedResource->getRepresentation();
        if (representation.hasAttribute(attributeKey))
        {
            representation.setValue(attributeKey, IUTSimulatorUtils::getAttributeValueFromUser());
            selectedResource->setResourceRepresentation(representation);
            cout << "Successfully updated resource attribute!!" << endl;
            ResourceHelper::getInstance()->printRepresentation(representation);
            selectedResource->notifyObservers();
        }
        else
        {
            cout << "The resource does not have the mentioned attribute" << endl;
        }

    }
    else
    {
        cout << "No resource to Update!!" << endl;
    }
}

void updateLocalResourceAutomatically()
{
    static bool binaryValue = false;
    static double temperatureValue = 25;
    static string directionValue = "left";
    static int hourValue = 1;
    static bool swingerValue = true;

    std::shared_ptr< SampleResource > selectedResource = IUTSimulatorUtils::getResourceFromUser(g_createdResourceList);
    if (selectedResource)
    {
        string attributeKey = "";
        AttributeValue attributValue;

        string uri = selectedResource->getUri();

        if (!uri.compare(AC_SWITCH_URI) || !uri.compare(AC_SWITCH_URI_CHILD))
        {
            attributeKey = string(ON_OFF_KEY);
            attributValue = binaryValue = !binaryValue;
        }
        else if (!uri.compare(AC_TEMPERATURE_URI) || !uri.compare(AC_TEMPERATURE_URI_CHILD)  || !uri.compare(AC_SENSOR_URI))
        {
            attributeKey = string(TEMPERATURE_KEY);
            attributValue = temperatureValue =
                    temperatureValue > 0 ? (temperatureValue - 26) : (temperatureValue + 26);
        }
        else if (!uri.compare(AC_AIR_FLOW_URI) || !uri.compare(AC_AIR_FLOW_URI_CHILD))
        {
            attributeKey = string(DIRECTION_KEY);
            attributValue = directionValue = directionValue.compare("left") ? "left" : "right";
        }
        else if (!uri.compare(AC_TIMER_URI) || !uri.compare(AC_TIMER_URI_CHILD))
        {
            attributeKey = string("x.com.vendor.timer.hour");
            attributValue = (hourValue % 2) ? (hourValue * 2) : (hourValue / 2);
        }
        else if (!uri.compare(AC_SWING_URI) || !uri.compare(AC_SWING_URI_CHILD))
        {
            attributeKey = string(SWING_STATE_KEY);
            attributValue = swingerValue = !swingerValue;
        }

        OCRepresentation representation = selectedResource->getRepresentation();
        if (representation.hasAttribute(attributeKey))
        {
            representation.setValue(attributeKey, attributValue);
            selectedResource->setResourceRepresentation(representation);
            cout << "Successfully updated resource attribute!!" << endl;
            ResourceHelper::getInstance()->printRepresentation(representation);
            selectedResource->notifyObservers();
        }
        else
        {
            cout << "The resource does not have the mentioned attribute" << endl;
        }

    }
    else
    {
        cout << "No resource to Update!!" << endl;
    }
}

void sendPostRequestUpdateUserInput()
{
    std::shared_ptr< OCResource > selectedResource = IUTSimulatorUtils::getResourceFromUser(g_foundResourceList);

    if (selectedResource)
    {
        IUTSimulatorUtils::setResourceHostEndpointFromUser(selectedResource, g_isSecuredClient);

        cout << "Please provide Attribute Key: ";
        OCRepresentation representation;
        string key = IUTSimulatorUtils::getInputFromUser<string>();
        AttributeValue value = IUTSimulatorUtils::getAttributeValueFromUser();
        representation.setValue(key, value);

        g_hasCallbackArrived = false;

        cout << "Sending Partial Update Message(POST)..." << endl;
        QueryParamsMap query;
        query[INTERFACE_KEY] = DEFAULT_INTERFACE;
        selectedResource->post(representation, query, &onPost, g_qos);
        cout << "POST request sent!!" << endl;
        waitForCallback();
    }
    else
    {
        cout << "No resource to send POST!!" << endl;
    }
}

void sendPostRequestCreate()
{
    std::shared_ptr< OCResource > selectedResource = IUTSimulatorUtils::getResourceFromUser(g_foundResourceList);

    if (selectedResource)
    {
        cout << "Sending Subordinate Resource Create Message(POST)..." << endl;

        OCRepresentation representation =
            IUTSimulatorUtils::createRepresentation({
                {POWER_KEY, string("off")},
                {INTENSITY_KEY, string("0")}
        });
        representation.setUri("/subordinate-device/subordinate-light-1");
        representation.setResourceTypes(vector<string>{"core.light"});

        selectedResource->post(representation, QueryParamsMap(), &onPost, g_qos);
        cout << "POST request sent!!" << endl;
        waitForCallback();
    }
    else
    {
        cout << "No resource to send POST!!" << endl;
    }
}

void sendPostCloudConfiguration()
{
    std::shared_ptr< OCResource > cloudConfigurationResource = IUTSimulatorUtils::getResourceByURI(g_foundResourceList, COAP_CLOUD_CONF_URI);

    if (cloudConfigurationResource)
    {
        OCRepresentation representation =
            IUTSimulatorUtils::createRepresentation({
                {CIS_KEY, string(CIS_VALUE)},
                {SID_KEY, string(SID_VALUE)},
                {AT_KEY, string(AT_VALUE)},
                {APN_KEY, string(APN_VALUE)}
        });

        cloudConfigurationResource->post(representation, QueryParamsMap(), &onPost, g_qos);
        cout << "POST Cloud Configuration request sent!" << endl;
        waitForCallback();
    }
    else
    {
        cout << "Could not find " << COAP_CLOUD_CONF_URI << " resource, perhaps you need to find all resources first." << endl;
    }
}

void sendDeleteRequest()
{
    std::shared_ptr< OCResource > selectedResource = IUTSimulatorUtils::getResourceFromUser(g_foundResourceList);

    if (selectedResource)
    {
        cout << "Sending Delete Request..." << endl;

        selectedResource->deleteResource(&onDelete);
        cout << "DELETE request sent!!" << endl;
        waitForCallback();
        if (selectedResource->uri().compare("/device/fan-1") == 0)
        {
            g_foundResourceList.clear();
            g_createdResourceList.clear();
        }
    }
    else
    {
        cout << "No resource to send DELETE!!" << endl;
    }
}

void observeResource()
{
    std::shared_ptr< OCResource > selectedResource = IUTSimulatorUtils::getResourceFromUser(g_foundResourceList);

    if (selectedResource)
    {
        cout << "Observing resource..." << endl;

        IUTSimulatorUtils::setResourceHostEndpointFromUser(selectedResource, g_isSecuredClient);
        cout << "Sending Get Request to the resource with: " << selectedResource->host() << selectedResource->uri() << endl;

        selectedResource->observe(ObserveType::Observe, QueryParamsMap(), &onObserve, g_qos);
        cout << "Observe request sent!!" << endl;
        g_isObservingResource = true;
        waitForCallback();
    }
    else
    {
        cout << "No resource to Observe!!" << endl;
    }
}

void cancelObserveResource()
{
    std::shared_ptr< OCResource > selectedResource = IUTSimulatorUtils::getResourceFromUser(g_foundResourceList);

    if (selectedResource)
    {
        if (g_isObservingResource)
        {
            cout << "Canceling Observe resource..." << endl;

            IUTSimulatorUtils::setResourceHostEndpointFromUser(selectedResource, g_isSecuredClient);
            cout << "Sending Get Request to the resource with: " << selectedResource->host() << selectedResource->uri() << endl;

            selectedResource->cancelObserve(g_qos);
            cout << "Cancel Observe request sent!!" << endl;
            g_isObservingResource = false;
            g_resourceHelper->waitInSecond(CALLBACK_WAIT_DEFAULT);
        }
        else
        {
            cout << "No resource is being Observed currently!!" << endl;
        }
    }
    else
    {
        cout << "No resource to cancel Observe!!" << endl;
    }
}

void cancelObservePassively()
{
    std::shared_ptr< OCResource > selectedResource = IUTSimulatorUtils::getResourceFromUser(g_foundResourceList);

    if (selectedResource)
    {
        if (g_isObservingResource)
        {
            cout << "Canceling Observe passively..." << endl;

            // Currently, there is no api to cancel observe passively
            cout << "Cancel Observe request not sent!! Currently there is no API!!" << endl;
        }
        else
        {
            cout << "No resource is being Observed currently!!" << endl;
        }
    }
    else
    {
        cout << "No resource to cancel Observe!!" << endl;
    }
}

void setDeviceWESInfo()
{
    cout << "SetDeviceInfo IN" << endl;
    ESDeviceProperty deviceProperty =
    {
        {
            {WIFI_11G, WIFI_11N, WIFI_11AC},3,
            {WIFI_24G, WIFI_5G},2,
            {WPA_PSK, WPA2_PSK},2,
            {AES,TKIP_AES},2
        },
        { "Test Device"}
    };

    // Set user properties if needed
    char userValue_str[] = "user_str";
    g_userProperties.userValue_int = 0;

    strncpy(g_userProperties.userValue_str, userValue_str, strlen(userValue_str) + 1);
    SetUserProperties(&g_userProperties);

    if(ESSetDeviceProperty(&deviceProperty) == ES_ERROR)
    {
        cout << "ESSetDeviceProperty Error" << endl;
    }

    cout << "SetDeviceInfo OUT" << endl;
}

void prepareForWES()
{
    cout << "Getting ready for Wifi Easy SetuUp" << endl;
    cout << "Please wait patiently..." << endl;

    ESResourceMask resourcemMask = (ESResourceMask) (ES_WIFICONF_RESOURCE | ES_DEVCONF_RESOURCE);

    g_esCallbacks.WiFiConfProvCb = &onWiFiConfProvReceived;
    g_esCallbacks.DevConfProvCb = &onDevConfProvReceived;

    if(ESInitEnrollee(g_isSecuredServer, resourcemMask, g_esCallbacks) != ES_OK)
    {
        cout << "Unable to initiate device to be Wifi Easy SetUp ready!!" << endl;
    }
    else
    {
        cout << "Device is now ready for Wifi Easy SetUp" << endl;
    }

}

void sendPingMessage()
{
    string address;
    uint16_t port;

    cout << "Please provide IP address (with interface after '%' if you are using IPv6) to which send ping to." << endl;
    IUTSimulatorUtils::getInputFromUser(address);
    if (!IUTSimulatorUtils::isValidIpAddress(address.substr(0, address.find("%")), g_ipVer))
    {
        cout << "Invalid IP address." << endl;
        return;
    }
    cout << "Please provide port." << endl;
    IUTSimulatorUtils::getInputFromUser(port);

    OCDevAddr devAddr;
    devAddr.adapter = OCTransportAdapter::OC_ADAPTER_TCP;
    devAddr.flags = OC_IP_USE_V4;
    if (g_ipVer == CT_IP_USE_V6)
        devAddr.flags = OC_IP_USE_V6;
    strncpy(devAddr.addr, address.c_str(), MAX_ADDR_STR_SIZE);
    devAddr.port = port;
    devAddr.ifindex = 0;
    strncpy(devAddr.remoteId, JUST_WORKS_DI.c_str(), MAX_IDENTITY_SIZE);


    OCCallbackData cbData;
    cbData.context = NULL;
    cbData.cb = [](void* ctx,OCDoHandle hd,OCClientResponse* clientResponse) -> OCStackApplicationResult {
        std::cout << "Pong received! addr: " << clientResponse->devAddr.addr << std::endl;
        return OC_STACK_DELETE_TRANSACTION;
    };
    cbData.cd = [](void* ctx) {};

    std::cout << "Sending the ping message now to address: " << address << std::endl;
    OCSendPingMessage(&devAddr, false, &cbData);
}

string getHost()
{
    std::shared_ptr< OCResource > selectedResource = IUTSimulatorUtils::getResourceFromUser(g_foundResourceList);

    if (selectedResource)
    {
        return selectedResource->host();
    }
    else
    {
        string ip = "";
        string port = "";

        cout << "Please enter the IP of the Resource host, then press Enter: ";
        IUTSimulatorUtils::getInputFromUser(ip);
        cout << "Please enter the port of the Resource host, then press Enter: ";
        IUTSimulatorUtils::getInputFromUser(port);

        return ip + ":" + port;
    }
}

void showMenu(int argc, char* argv[])
{
    cout << "Please Select an option from the menu and press Enter" << endl;
    cout << "\t\t " << setw(3) << "0" << ". Quit IUT Emulator App" << endl;
    cout << "\t Server Operations:" << endl;
    cout << "\t\t " << setw(3) << "1" << ". Create Normal Resource" << endl;
    cout << "\t\t " << setw(3) << "2" << ". Create Invisible Resource" << endl;
    cout << "\t\t " << setw(3) << "3" << ". Create Resource With Complete URL" << endl;
    cout << "\t\t " << setw(3) << "4" << ". Create Secured Resource" << endl;
    cout << "\t\t " << setw(3) << "5" << ". Create " << MAX_LIGHT_RESOURCE_COUNT << " Light Resources" << endl;
    cout << "\t\t " << setw(3) << "6" << ". Create Group Resource" << endl;
    cout << "\t\t " << setw(3) << "7" << ". Delete Created Resources" << endl;
    cout << "\t\t " << setw(3) << "8" << ". Delete Created Group" << endl;
    cout << "\t Client Operations:" << endl;
    cout << "\t\t " << setw(3) << "9" << ". Find Introspection" << endl;
    cout << "\t\t " << setw(3) << "10" << ". Find Resource using Interface Query" << endl;
    cout << "\t\t " << setw(3) << "11" << ". Find Specific Type Of Resource" << endl;
    cout << "\t\t " << setw(3) << "12" << ". Find All Resources" << endl;
    cout << "\t\t " << setw(3) << "13" << ". Find All Resources using Baseline Query - Unicast" << endl;
    cout << "\t\t " << setw(3) << "14" << ". Find Specific Type Of Resource - Unicast" << endl;
    cout << "\t\t " << setw(3) << "15" << ". Find All Resources - Unicast" << endl;
    cout << "\t\t " << setw(3) << "16" << ". Join Found Resource To The Group" << endl;
    cout << "\t\t " << setw(3) << "17" << ". Send GET Request" << endl;
    cout << "\t\t " << setw(3) << "18" << ". Send GET Request with query" << endl;
    cout << "\t\t " << setw(3) << "19" << ". Send PUT Request - Create Resource" << endl;
    cout << "\t\t " << setw(3) << "20" << ". Send PUT Request - Complete Update" << endl;
    cout << "\t\t " << setw(3) << "21" << ". Send POST Request - Partial Update - Default" << endl;
    cout << "\t\t " << setw(3) << "22" << ". Send POST Request - Partial Update - User Input" << endl;
    cout << "\t\t " << setw(3) << "23" << ". Send POST Request - Create Sub-Ordinate Resource" << endl;
    cout << "\t\t " << setw(3) << "24" << ". Send Delete Request" << endl;
    cout << "\t\t " << setw(3) << "25" << ". Observe Resource - Retrieve Request with Observe" << endl;
    cout << "\t\t " << setw(3) << "26" << ". Cancel Observing Resource" << endl;
    cout << "\t\t " << setw(3) << "27" << ". Cancel Observing Resource Passively" << endl;
    cout << "\t\t " << setw(3) << "28" << ". Discover Device - Unicast" << endl;
    cout << "\t\t " << setw(3) << "29" << ". Discover Device - Multicast" << endl;
    cout << "\t\t " << setw(3) << "30" << ". Discover Platform - Multicast" << endl;
    cout << "\t\t " << setw(3) << "31" << ". Find Group" << endl;
    cout << "\t\t " << setw(3) << "32" << ". Join Found Resource To Found Group" << endl;
    cout << "\t\t " << setw(3) << "33" << ". Update Collection" << endl;
    cout << "\t\t " << setw(3) << "34" << ". Update Local Resource Manually" << endl;
    cout << "\t\t " << setw(3) << "35" << ". Update Local Resource Automatically" << endl;
    cout << "\t\t " << setw(3) << "36" << ". Set Quality of Service - CON(Confirmable)" << endl;
    cout << "\t\t " << setw(3) << "37" << ". Set Quality of Service - NON(Non-Confirmable)" << endl;
    cout << "\t\t " << setw(3) << "38" << ". Reset Secure Storage" << endl;
    cout << "\t\t " << setw(3) << "39" << ". Send Post Request for Cloud Configuration" << endl;
    cout << "\t Smart Home Vertical Resource Creation:" << endl;
    cout << "\t\t " << setw(3) << "101" << ". Create Smart TV Device" << endl;
    cout << "\t\t " << setw(3) << "102" << ". Create Air Conditioner Device" << endl;
#ifdef __SECURED__
    cout << "\t\t " << setw(3) << "103" << ". Create Secured Smart TV Device" << endl;
    cout << "\t\t " << setw(3) << "104" << ". Create Secured Air Conditioner Device" << endl;
    cout << "\t\t " << setw(3) << "105" << ". Create Secured Air Conditioner Single Resource" << endl;
    cout << "\t\t " << setw(3) << "106" << ". Create Secured Vendor Defined Resource" << endl;
#endif
    cout << "\t\t " << setw(3) << "107" << ". Create Air Conditioner Single Resource" << endl;
    cout << "\t\t " << setw(3) << "108" << ". Create  Vendor Defined Resource" << endl;
    cout << "\t\t " << setw(3) << "109" << ". Prepare for Wifi Easy Setup" << endl;
    cout << "\t\t " << setw(3) << "110" << ". Publish Created Resources To RD" << endl;
    cout << "\t\t " << setw(3) << "111" << ". Update Published Resources To RD" << endl;
    cout << "\t\t " << setw(3) << "112" << ". Delete Published Resources From RD" << endl;
    cout << "\t\t " << setw(3) << "113" << ". Create Extra Device" << endl;
    cout << "\t\t " << setw(3) << "114" << ". Update Last Error Code" << endl;
    cout << "\t\t " << setw(3) << "115" << ". Create Complex Device For Introspection" << endl;
    cout << "\t\t " << setw(3) << "116" << ". Create Binary Switch Resource" << endl;
    cout << "\t\t " << setw(3) << "117" << ". Create Network Monitoring and Maintaince Resources" << endl;
    cout << "\t\t " << setw(3) << "118" << ". Create Sample Batch collection" << endl;
    cout << "\t\t " << setw(3) << "119" << ". Create Atomic Measurement Resource - Blood Pressure" << endl;
    cout << "\t\t " << setw(3) << "120" << ". Cloud Configuration - Initialize cloud configuration resource" << endl;
    cout << "\t\t " << setw(3) << "121" << ". Cloud Configuration - Sign out" << endl;
    cout << "\t\t " << setw(3) << "122" << ". Cloud Configuration - Deregister and reinitialize cloud configuration resource" << endl;
    cout << "\t\t " << setw(3) << "123" << ". Create Configuration Resource" << endl;
    cout << "\t\t " << setw(3) << "124" << ". Send Ping Message" << endl;
    cout << "\t\t " << setw(3) << "125" << ". Create Air Purifier Device" << endl;
    cout << "\t\t " << setw(3) << "126" << ". Create Blood Pressure Monitor Device" << endl;

    g_hasCallbackArrived = false;
    handleMenu(argc, argv);
}

void selectMenu(int choice)
{
    string userResourceType;
    string userInterfaceType;
    string resourceHost = "";
    string collectionType = "";
    string queryKey = "";
    string queryValue = "";
    AttributeValue attrVal;
    OCRepresentation linkRep;
    bool isMulticast = false;

    switch (choice)
    {
        case 1:
            createResource();
            break;

        case 2:
            createInvisibleResource();
            break;

        case 3:
            createResourceWithUrl();
            break;

        case 4:
            createSecuredResource();
            break;

        case 5:
            createManyLightResources();
            break;

        case 6:
            createGroup(GROUP_TYPE_DEFAULT);
            break;

        case 7:
            deleteCreatedResource();
            break;

        case 8:
            deleteGroup();
            break;

        case 9:
            discoverIntrospection(true);
            break;

        case 10:
            cout << "Please type query(key=value), then press Enter: ";
            IUTSimulatorUtils::getInputFromUser(userInterfaceType);
            resourceHost = "";
            findAllResources(resourceHost, userInterfaceType);
            if (g_foundResourceList.size() == 0)
            {
                cout << "No resource found!!" << endl;
            }
            break;

        case 11:
            cout << "Please type the Resource Type to find, then press Enter: ";
            IUTSimulatorUtils::getInputFromUser(userResourceType);
            userResourceType = "?rt=" + userResourceType;
            findResource(userResourceType);
            if (g_foundResourceList.size() == 0)
            {
                cout << "No resource found!!" << endl;
            }
            break;

        case 12:
            findAllResources();
            if (g_foundResourceList.size() == 0)
            {
                cout << "No resource found!!" << endl;
            }
            break;

        case 13:
            resourceHost = getHost();
            findResource("?if=oic.if.baseline", resourceHost);
            if (g_foundResourceList.size() == 0)
            {
                cout << "No resource found!!" << endl;
            }
            break;

        case 14:
            resourceHost = getHost();
            cout << "Please type the Resource Type to find, then press Enter: ";
            IUTSimulatorUtils::getInputFromUser(userResourceType);
            userResourceType = "?rt=" + userResourceType;
            findResource(userResourceType, resourceHost);
            if (g_foundResourceList.size() == 0)
            {
                cout << "No resource found!!" << endl;
            }
            break;

        case 15:
            resourceHost = getHost();
            findAllResources(resourceHost);
            if (g_foundResourceList.size() == 0)
            {
                cout << "No resource found!!" << endl;
            }
            break;

        case 16:
            break;

        case 17:
            sendGetRequest();
            break;

        case 18:
            cout << "Please type query key, then press Enter: ";
            IUTSimulatorUtils::getInputFromUser(queryKey);
            cout << "Please type query value, then press Enter: ";
            IUTSimulatorUtils::getInputFromUser(queryValue);
            sendGetRequestWithQuery(queryKey, queryValue);
            break;

        case 19:
            sendPutRequestCreate();
            break;

        case 20:
            sendPutRequestUpdate();
            break;

        case 21:
            sendPostRequestUpdate();
            break;

        case 22:
            sendPostRequestUpdateUserInput();
            break;

        case 23:
            sendPostRequestCreate();
            break;

        case 24:
            sendDeleteRequest();
            break;

        case 25:
            observeResource();
            break;

        case 26:
            try
            {
                cancelObserveResource();
            }
            catch (exception& e)
            {
                cout << "Unable to cancel observing resource." << endl;
                cout << "Error is: " << e.what() << endl;
            }

            break;

        case 27:
            cancelObservePassively();
            break;

        case 28:
            isMulticast = false;
            discoverDevice(isMulticast);
            break;

        case 29:
            isMulticast = true;
            discoverDevice(isMulticast);
            break;

        case 30:
            discoverPlatform();
            break;

        case 31:
            collectionType = GROUP_TYPE_DEFAULT;
            findCollection(collectionType);
            if (g_foundCollectionList.size() == 0)
            {
                cout << "No collection found!!" << endl;
            }
            break;

        case 32:
            collectionType = GROUP_TYPE_DEFAULT;
            if (g_foundCollectionList.size() == 0)
            {
                cout << "No Group available, starting find Group." << endl;
                findCollection(collectionType);
                if (g_foundCollectionList.size() == 0)
                {
                    cout << "Still no Resource found, quitting join resource to group." << endl;
                    break;
                }
                else
                {
                    g_hasCallbackArrived = false;
                }
            }

            g_foundResourceList.at(0)->get(QueryParamsMap(), onGet);
            waitForCallback();

            if (g_resourceRepresentation.hasAttribute("links"))
            {
                cout << "link is available." << endl;
                g_resourceRepresentation.getAttributeValue("links", attrVal);
                linkRep["links"] = attrVal;
            }
            else
            {
                cout << "link is not available. creating link..." << endl;
                linkRep.setValue("links",
                        (string) "[{\"href\":\"/device/light-1\",\"rt\":\"core.light\",\"if\":\"oic.if.baseline\",\"ins\":\"123\",\"p\":{\"bm\":3}}]");
            }

            g_foundResourceList.at(0)->post(collectionType, LINK_INTERFACE, linkRep,
                    QueryParamsMap(), onPost, g_qos);

            break;

        case 33:
            updateGroup();
            break;

        case 34:
            updateLocalResource();
            break;

        case 35:
            updateLocalResourceAutomatically();
            break;

        case 36:
            g_qos = QualityOfService::HighQos;
            cout << "CON Type Message Selected for Client" << endl;
            break;

        case 37:
            g_qos = QualityOfService::LowQos;
            cout << "NON Type Message Selected for Client" << endl;
            break;

        case 38:
            replaceDatFile(g_modeType, g_securityType);
            cout << "Replaced Secure Storage" << endl;
            break;

        case 39:
            sendPostCloudConfiguration();
            break;

        case 101:
            createTvDevice();
            break;

        case 102:
            createAirConDevice();
            break;

#ifdef __SECURED__
        case 103:
            createTvDevice(true);
            break;

        case 104:
            createAirConDevice(true);
            break;

        case 105:
            createSingleAirConResource(true);
            break;

        case 106:
            createVendorDefinedDevice(true);
            break;
#endif

        case 107:
            createSingleAirConResource();
            break;

        case 108:
            createVendorDefinedDevice();
            break;

        case 109:
            setDeviceWESInfo();
            SetCallbackForUserdata();
            prepareForWES();
            break;

        case 110:
            publishResourcesToRD();
            break;

        case 111:
            updateResourcesToRD();
            break;

        case 112:
            deleteResourcesFromRD();
            break;

        case 113:
            createExtraDevice(true);
            break;

        case 114:
            updateLec();
            break;

        case 115:
            createResourceForIntrospection(g_isSecuredServer);
            break;

        case 116:
            startResource(
                AC_SWITCH_URI,
                SWITCH_RESOURCE_TYPE,
                OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE | isSecure(g_isSecuredServer)
            );
            break;

        case 117:
            startResource(
                MN_MAINTENANCE_URI,
                MAINTENANCE_RESOURCE_TYPE,
                OC_ACTIVE | OC_DISCOVERABLE | isSecure(g_isSecuredServer)
            );
            startResource(
                NMON_NETWORK_MONITORING_URI,
                NETWORK_MONITORING_RESOURCE_TYPE,
                OC_ACTIVE | OC_DISCOVERABLE | isSecure(g_isSecuredServer)
            );
            break;

        case 118:
            createCollectionWithBatch(g_isSecuredServer);
            break;

        case 119:
            createBP0Resource();
            createBP1Resource();
            createBP2Resource();
            break;

        case 120:
            InitCloudResource();
            break;

        case 121:
            CloudsSignOut();
            break;

        case 122:
            DeleteCloudAccount();
            DeInitCloudResource();
            InitCloudResource();
            break;

        case 123:
            startResource(
                CON_URI,
                CON_RESOURCE_TYPE,
                uint8_t(OC_ACTIVE | OC_DISCOVERABLE | isSecure(g_isSecuredServer))
            );
            break;

        case 124:
            sendPingMessage();
            break;

        case 125:
            if (!isDeviceTypeSet)
            {
                SampleResource::setDeviceInfo(AP_ENGLISH_NAME_VALUE, Device_TYPE_AP, g_ocfVer);
                isDeviceTypeSet = true;
            }
            else
            {
                cout << "Device type is already set." << endl;
            }
            break;

        case 126:
            if (!isDeviceTypeSet)
            {
                SampleResource::setDeviceInfo("Blood Pressure Monitor Device", Device_TYPE_BPM, g_ocfVer);
                isDeviceTypeSet = true;
            }
            else
            {
                cout << "Device type is already set." << endl;
            }
            break;

        case 0:
            if (g_createdResourceList.size() > 0)
            {
                deleteCreatedResource();
            }
            if (g_isGroupCreated)
            {
                deleteGroup();
            }
            if (g_childHandle != NULL)
            {
                OCPlatform::unregisterResource(g_childHandle);
            }
            g_createdResourceList.clear();
            g_foundResourceList.clear();

            closeRDPLogFile();
            exit(0);

        default:
            cout << "Invalid Input. Please input your choice again" << endl;
    }
}

void handleMenu(int argc, char* argv[])
{
    selectMenu(IUTSimulatorUtils::getInputFromUser<int>());
    showMenu(0, NULL);
}