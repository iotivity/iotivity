/******************************************************************
 *
 * Copyright 2017 Granite River Labs All Rights Reserved.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iostream>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif
#include <mutex>
#include <condition_variable>
#include "oic_string.h"
#include "OCApi.h"
#include "octypes.h"
#include "OCPlatform.h"
#include "OCApi.h"
#include "ocpayload.h"
#include "easysetup_x.h"
#include "easysetup.h"
#include "RemoteEnrollee.h"
#include "OCProvisioningManager.hpp"
#include "experimental/securevirtualresourcetypes.h"
#include "EasySetup.hpp"
#include "ESRichCommon.h"
#define JSON_DB_PATH "./oic_svr_db_client.dat"
#define MAX_SLEEP_TIME 1
#ifdef __cplusplus
extern "C" {
#endif

using namespace OC;
using namespace OIC::Service;
static bool gIsSecured = false;
pthread_t thread_handle;

static std::shared_ptr<RemoteEnrollee> remoteEnrollee = nullptr;
static std::shared_ptr<OC::OCResource> curResource = nullptr;

static std::mutex g_discoverymtx;
static std::condition_variable g_cond;
#ifdef MULTIPLE_OWNER
static char CRED_FILE[] = "oic_svr_db_server_MOT.dat";
#else
static char CRED_FILE[] = "oic_svr_db_server.dat";
#endif

OCPersistentStorage ps;

void *listeningFunc(void * data)
{
    (void)data;
    OCStackResult result = OC_STACK_ERROR;

    while (true)
    {
        result = OCProcess();
        if (result != OC_STACK_OK)
        {
           printf("OCStack stop error");
        }
    }
    return NULL;
}
void ConnectRequestCbInApp(ESConnectRequest *connectRequest)
{
    printf("ConnectRequestCbInApp IN\n");
    if (connectRequest == NULL)
    {
        printf("connectRequest is NULL\n");
        return ;
    }
    for (int i = 0 ; i < connectRequest->numRequest ; ++i)
    {
        printf("connect : %d\n", connectRequest->connect[i]);
    }
    printf("ConnectRequestCbInApp OUT\n");
}
void WiFiConfProvCbInApp(ESWiFiConfData *eventData)
{
    printf("WiFiConfProvCbInApp IN\n");
    if (eventData == NULL)
    {
        printf("ESWiFiConfData is NULL\n");
        return ;
    }
    printf("SSID : %s\n", eventData->ssid);
    printf("Password : %s\n", eventData->pwd);
    printf("AuthType : %d\n", eventData->authtype);
    printf("EncType : %d\n", eventData->enctype);
    if (eventData->userdata != NULL)
    {
        printf("userValue : %d\n", ((UserProperties *)(eventData->userdata))->userValue_int);
    }
    printf("WiFiConfProvCbInApp OUT\n");
}
void DevConfProvCbInApp(ESDevConfData *eventData)
{
    printf("DevConfProvCbInApp IN\n");
    if (eventData == NULL)
    {
        printf("ESDevConfData is NULL\n");
        return ;
    }
    printf("DevConfProvCbInApp OUT\n");
}
void CoapCloudConfProvCbInApp(ESCoapCloudConfData *eventData)
{
    printf("CoapCloudConfProvCbInApp IN\n");
    if (eventData == NULL)
    {
        printf("ESCoapCloudConfData is NULL\n");
        return ;
    }
    //printf("AuthCode : %s\n", eventData->authCode);
    printf("AcessTokenType : %d\n", eventData->accessTokenType);
    printf("AuthProvider : %s\n", eventData->authProvider);
    printf("CI Server : %s\n", eventData->ciServer);
    printf("CoapCloudConfProvCbInApp OUT\n");
}
ESProvisioningCallbacks gCallbacks =
{
    .ConnectRequestCb = &ConnectRequestCbInApp,
    .WiFiConfProvCb = &WiFiConfProvCbInApp,
    .DevConfProvCb = &DevConfProvCbInApp,
    .CoapCloudConfProvCb = &CoapCloudConfProvCbInApp
};
FILE *server_fopen(const char *path, const char *mode)
{
    if (0 == strcmp(path, OC_SECURITY_DB_DAT_FILE_NAME))
    {
        return fopen(CRED_FILE, mode);
    }
    else
    {
        return fopen(path, mode);
    }
}
static FILE *client_open(const char *path, const char *mode)
{
    if (0 == strcmp(path, OC_SECURITY_DB_DAT_FILE_NAME))
    {
        return fopen(JSON_DB_PATH, mode);
    }
    else
    {
        return fopen(path, mode);
    }
}
void clearDislay()
{
    system("clear");
}
void customWait(int seconds)
{
    sleep(seconds);
}
void displayMenu()
{
    printf("\t E. Enrollee \n");
    printf("\t M. Mediator \n");
    printf("\t Q. Quit \n");
}
void EnableSecurity()
{
    printf("Inside EnableSecurity API..\n");
    gIsSecured = true;
    // Initialize Persistent Storage for SVR database
    ps = (OCPersistentStorage) { server_fopen, fread, fwrite, fclose, unlink };
    OCRegisterPersistentStorageHandler(&ps);
}
void StartEasySetup()
{
    printf("StartEasySetup IN\n");
    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        printf("OCStack init error!!\n");
        return;
    }
    ESResourceMask resourcemMask = ES_WIFICONF_RESOURCE | ES_COAPCLOUDCONF_RESOURCE |
                                   ES_DEVCONF_RESOURCE;
    if (ESInitEnrollee(gIsSecured, resourcemMask, gCallbacks) != ES_OK)
    {
        printf("OCStack init error!!\n");
        return;
    }
    printf("ESInitEnrollee Success\n");
    if (pthread_create(&thread_handle, NULL, listeningFunc, NULL))
    {
        printf("Thread creation failed\n");
    }
    printf("StartEasySetup OUT\n");
}
void SetDeviceInfo()
{
    UserProperties g_userProperties;
    printf("SetDeviceInfo IN\n");
    ESDeviceProperty deviceProperty =
    {
    {
    {WIFI_11G, WIFI_11N, WIFI_11AC }, 3,
    { WIFI_24G, WIFI_5G }, 2,
    { WPA_PSK, WPA2_PSK }, 2,
    { AES, TKIP_AES }, 2
    },
    { "Test Device" }};
    // Set user properties if needed
    char userValue_str[] = "user_str";
    g_userProperties.userValue_int = 0;

    strncpy(g_userProperties.userValue_str, userValue_str, strlen(userValue_str));
    SetUserProperties(&g_userProperties);

    if (ESSetDeviceProperty(&deviceProperty) == ES_ERROR)
    {
        printf("ESSetDeviceProperty Error\n");
    }
    printf("SetDeviceInfo OUT\n");

}
void StopEasySetup()
{
    printf("StopEasySetup IN\n");
    if (ESTerminateEnrollee() == ES_ERROR)
    {
        printf("ESTerminateEnrollee Failed!!\n");
        return;
    }
    if (0 != pthread_cancel(thread_handle))
    {
        printf("Thread cancellation failed\n");
        return;
    }
    if (OCStop() != OC_STACK_OK)
    {
        printf("OCStack stop failed!!\n");
        return;
    }
    printf("StopEasySetup OUT\n");
}
void SetCallbackForUserdata()
{
    ESSetCallbackForUserdata(&ReadUserdataCb, &WriteUserdataCb);
}
void printConfiguration(const EnrolleeConf &conf)
{
    cout << "===========================================" << endl;
    cout << "\tProvStatus : " << conf.getProvStatus() << endl;
    cout << "\tLastErrCode : " << conf.getLastErrCode() << endl;
    cout << "\tDevice Name : " << conf.getDeviceName() << endl;

    for (auto it : conf.getWiFiModes())
    {
        cout << "\tSupported WiFi modes : " << it << endl;
    }

    cout << "\tSupported WiFi freq : " << static_cast<int>(conf.getWiFiFreq()) << endl;
    cout << "\tCloud accessibility: " << conf.isCloudAccessible() << endl;
    cout << "===========================================" << endl;
}
void printStatus(const EnrolleeStatus &status)
{
    cout << "===========================================" << endl;
    cout << "\tProvStatus : " << status.getProvStatus() << endl;
    cout << "\tLastErrCode : " << status.getLastErrCode() << endl;
    cout << "===========================================" << endl;
}
ESOwnershipTransferData provisionSecurityStatusCallback(std::shared_ptr<SecProvisioningStatus>
        secProvisioningStatus)
{
    cout << "provisionSecurityStatusCallback IN" << endl;
    cout << "ESResult : " << secProvisioningStatus->getESResult() << std::endl;
    cout << "Device ID : " << secProvisioningStatus->getDeviceUUID() << std::endl;

    if (secProvisioningStatus->getESResult() == ES_SECURE_RESOURCE_IS_DISCOVERED)
    {
#ifdef __WITH_DTLS__
        cout << "Owned Status : " << secProvisioningStatus->isOwnedDevice() << std::endl;
        if (secProvisioningStatus->isOwnedDevice())
        {
            cout << "Owner ID : " << secProvisioningStatus->getOwnerID() << std::endl;
        }
        cout << "OT Method : " << secProvisioningStatus->getSelectedOTMethod() << std::endl;
#ifdef MULTIPLE_OWNER
        cout << "MOT Enabled : " << secProvisioningStatus->isMOTEnabled() << std::endl;
        ESOwnershipTransferData OTData;
        OTData.setMOTMethod(OIC_PRECONFIG_PIN, "12345678");
        cout << "Enter!" << std::endl;
        getchar();
        return OTData;
#endif
#endif
    }
    else if (secProvisioningStatus->getESResult() == ES_OK)
    {
        cout << "provisionSecurity is success." << std::endl;
        curResource = nullptr;
    }
    else
    {
        cout << "provisionSecurity is failed." << endl;
    }

    return {};
}
void provisionSecurity()
{
    if (!remoteEnrollee)
    {
        std::cout << "RemoteEnrollee is null, retry Discovery EnrolleeResource." << endl;
        return;
    }
    try
    {
        remoteEnrollee->provisionSecurity((SecurityProvStatusCbWithOption)provisionSecurityStatusCallback);
    }
    catch (OCException &e)
    {
        std::cout << "Exception during provisionSecurity call" << e.reason();
        return;
    }
}
void getStatusCallback(std::shared_ptr< GetEnrolleeStatus > getEnrolleeStatus)
{
    if (getEnrolleeStatus->getESResult() != ES_OK)
    {
        cout << "getStatus is failed." << endl;
        return;
    }
    else
    {
        cout << "getStatus is success." << endl;
        printStatus(getEnrolleeStatus->getEnrolleeStatus());
    }
}
void getStatus()
{
    if (!remoteEnrollee)
    {
        std::cout << "RemoteEnrollee is null, retry Discovery EnrolleeResource." << endl;
        return;
    }
    try
    {
        remoteEnrollee->getStatus(getStatusCallback);
    }
    catch (OCException &e)
    {
        std::cout << "Exception during getConfiguration call" << e.reason();
        return;
    }
}
void getConfigurationCallback(std::shared_ptr< GetConfigurationStatus > getConfigurationStatus)
{
    if (getConfigurationStatus->getESResult() != ES_OK)
    {
        cout << "GetConfigurationStatus is failed." << endl;
        return;
    }
    else
    {
        cout << "GetConfigurationStatus is success." << endl;
        printConfiguration(getConfigurationStatus->getEnrolleeConf());
    }
}
void getConfiguration()
{
    if (!remoteEnrollee)
    {
        std::cout << "RemoteEnrollee is null, retry Discovery EnrolleeResource." << endl;
        return;
    }

    try
    {
        remoteEnrollee->getConfiguration(getConfigurationCallback);
    }
    catch (OCException &e)
    {
        std::cout << "Exception during getConfiguration call" << e.reason();
        return;
    }
}

void deviceProvisioningStatusCallback(std::shared_ptr< DevicePropProvisioningStatus > provStatus)
{
    if (provStatus->getESResult() != ES_OK)
    {
        cout << "Device Provisioning is failed." << endl;
        return;
    }
    else
    {
        cout << "Device Provisioning is success." << endl;
    }
}
void provisionDeviceProperty()
{
    if (!remoteEnrollee)
    {
        std::cout << "RemoteEnrollee is null, retry Discovery EnrolleeResource." << endl;
        return;
    }
    DeviceProp devProp;
    devProp.setWiFiProp("Iotivity_SSID", "Iotivity_PWD", WPA2_PSK, TKIP_AES);
    try
    {
        remoteEnrollee->provisionDeviceProperties(devProp, deviceProvisioningStatusCallback);
    }
    catch (OCException &e)
    {
        std::cout << "Exception during provisionDeviceProperties call" << e.reason();
        return;
    }
}
void connectRequestStatusCallback(std::shared_ptr< ConnectRequestStatus > requestStatus)
{
    if (requestStatus->getESResult() != ES_OK)
    {
        cout << "Request to connection is failed." << endl;
        return;
    }
    else
    {
        cout << "Request to connection is success." << endl;
    }
}
void requestToConnect()
{
    if (!remoteEnrollee)
    {
        std::cout << "RemoteEnrollee is null, retry Discovery EnrolleeResource." << endl;
        return;
    }
    try
    {
        std::vector<ES_CONNECT_TYPE> types;
        types.push_back(ES_CONNECT_WIFI);
        types.push_back(ES_CONNECT_COAPCLOUD);
        remoteEnrollee->requestToConnect(types, connectRequestStatusCallback);
    }
    catch (OCException &e)
    {
        std::cout << "Exception during provisionDeviceProperties call" << e.reason();
        return;
    }
}
void cloudProvisioningStatusCallback(std::shared_ptr< CloudPropProvisioningStatus > provStatus)
{
    switch (provStatus->getESResult())
    {
        case ES_OK:
            cout << "Cloud Provisioning is success." << endl;
            break;
        case ES_SECURE_RESOURCE_DISCOVERY_FAILURE:
            cout << "Enrollee is not found in a given network." << endl;
            break;
        case ES_ACL_PROVISIONING_FAILURE:
            cout << "ACL provisioning is failed." << endl;
            break;
        case ES_CERT_PROVISIONING_FAILURE:
            cout << "CERT provisioning is failed." << endl;
            break;
        default:
            cout << "Cloud Provisioning is failed." << endl;
            break;
    }
}
void provisionCloudProperty()
{
    if (!remoteEnrollee)
    {
        std::cout << "RemoteEnrollee is null, retry Discovery EnrolleeResource." << endl;
        return;
    }
    CloudProp cloudProp;
    cloudProp.setCloudPropWithAccessToken("accessToken", OAUTH_TOKENTYPE_BEARER, "authProvider",
                                          "ciServer");
    cloudProp.setCloudID("f002ae8b-c42c-40d3-8b8d-1927c17bd1b3");
    cloudProp.setCredID(1);
    try
    {
        remoteEnrollee->provisionCloudProperties(cloudProp, cloudProvisioningStatusCallback);
    }
    catch (OCException &e)
    {
        std::cout << "Exception during provisionCloudProperties call" << e.reason();
        return;
    }
}
void foundUnsecuredResource(std::shared_ptr<OC::OCResource> resource)
{
    std::string resourceURI;
    std::string hostAddress;
    try
    {
        if (resource && !curResource  &&
            resource->getResourceTypes().at(0) == OC_RSRVD_ES_RES_TYPE_EASYSETUP)
        {
            std::cout << "DISCOVERED Resource:" << std::endl;
            resourceURI = resource->uri();
            std::cout << "\tURI of the resource: " << resourceURI << std::endl;
            hostAddress = resource->host();
            std::cout << "\tHost address of the resource: " << hostAddress << std::endl;
            std::cout << "\tList of resource types: " << std::endl;
            for (auto &resourceTypes : resource->getResourceTypes())
            {
                std::cout << "\t\t" << resourceTypes << std::endl;
            }
            std::cout << "\tList of resource interfaces: " << std::endl;
            for (auto &resourceInterfaces : resource->getResourceInterfaces())
            {
                std::cout << "\t\t" << resourceInterfaces << std::endl;
            }

            if (curResource == nullptr)
            {
                remoteEnrollee = EasySetup::getInstance()->createRemoteEnrollee(resource);
                if (!remoteEnrollee)
                {
                    std::cout << "RemoteEnrollee object is failed for some reasons!" << std::endl;
                }
                else
                {
                    curResource = resource;
                    std::cout << "RemoteEnrollee object is successfully created!" << std::endl;
                    g_cond.notify_all();
                }
            }
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception in foundResource: " << e.what() << std::endl;
    }
}
void foundSecuredResource(std::shared_ptr<OC::OCResource> resource)
{
    std::string resourceURI;
    std::string hostAddress;
    try
    {
        if (resource && !curResource  &&
            resource->getResourceTypes().at(0) == OC_RSRVD_ES_RES_TYPE_EASYSETUP)
        {
            std::cout << "DISCOVERED Resource:" << std::endl;
            resourceURI = resource->uri();
            std::cout << "\tURI of the resource: " << resourceURI << std::endl;
            hostAddress = resource->host();
            std::cout << "\tHost address of the resource: " << hostAddress << std::endl;
            std::cout << "\tList of resource endpoints: " << std::endl;
            for (auto &resourceEndpoints : resource->getAllHosts())
            {
                std::cout << "\t\t" << resourceEndpoints << std::endl;
            }
            std::cout << "\tList of resource types: " << std::endl;
            for (auto &resourceTypes : resource->getResourceTypes())
            {
                std::cout << "\t\t" << resourceTypes << std::endl;
            }
            std::cout << "\tList of resource interfaces: " << std::endl;
            for (auto &resourceInterfaces : resource->getResourceInterfaces())
            {
                std::cout << "\t\t" << resourceInterfaces << std::endl;
            }
            if (curResource == nullptr)
            {
                remoteEnrollee = nullptr;

                for (auto &resourceEndpoints : resource->getAllHosts())
                {
                    if (std::string::npos != resourceEndpoints.find("coaps"))
                    {
                        std::string newHost = resourceEndpoints;
                        std::cout << "\tChange host of resource endpoints" << std::endl;
                        std::cout << "\t\t" << "Current host is "
                                  << resource->setHost(newHost) << std::endl;
                        remoteEnrollee = EasySetup::getInstance()->createRemoteEnrollee(resource);
                        break;
                    }
                }
                if (!remoteEnrollee)
                {
                    std::cout << "RemoteEnrollee object is failed for some reasons!" << std::endl;
                }
                else
                {
                    curResource = resource;
                    std::cout << "RemoteEnrollee object is successfully created!" << std::endl;
                    g_cond.notify_all();
                }
            }
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception in foundResource: " << e.what() << std::endl;
    }
}

void discoveryEnrolleeResource(void (*f)(std::shared_ptr<OC::OCResource> resource))
{
    try
    {
        std::ostringstream requestURI;
        requestURI << OC_RSRVD_WELL_KNOWN_URI << "?rt=" << OC_RSRVD_ES_RES_TYPE_EASYSETUP;
        OCPlatform::findResource("", requestURI.str(), CT_DEFAULT, f);
        std::cout << "Finding Resource... " << std::endl;

        std::unique_lock<std::mutex> lck(g_discoverymtx);
        g_cond.wait_for(lck, std::chrono::seconds(5));
    }
    catch (OCException &e)
    {
        std::cout << "Exception in discoveryEnrolleeResource: " << e.what();
    }
}

int main (int argc, char* agrv[])
{
    char option,terminate;
    printf("Easy-Setup Simualtor to Run TC \n");
    displayMenu();
    scanf("%c", &option);
    if(option=='E')
    {
        printf("Simulator as Enrollee IN\n");
        EnableSecurity();
        customWait(MAX_SLEEP_TIME);
        StartEasySetup();
        customWait(MAX_SLEEP_TIME);
        SetDeviceInfo();
        customWait(MAX_SLEEP_TIME);
        SetCallbackForUserdata();
        customWait(MAX_SLEEP_TIME);
    }
    else if(option=='M')
    {
        printf("Simulator as Mediator IN\n");
        OCPersistentStorage ps {client_open, fread, fwrite, fclose, unlink };

        PlatformConfig config
        {
            OC::ServiceType::InProc, ModeType::Both, OC_DEFAULT_ADAPTER, OC::QualityOfService::HighQos, &ps
        };
        OCPlatform::Configure(config);
        // For secured enrollee discovery
        #ifdef __WITH_DTLS__
        discoveryEnrolleeResource(&foundSecuredResource);
        #endif
        // For non-secured enrollee discovery
        #ifndef __WITH_DTLS__
        discoveryEnrolleeResource(&foundUnsecuredResource);
        #endif
        provisionSecurity();
        getConfiguration();
        provisionDeviceProperty();
        requestToConnect();
        provisionCloudProperty();
    }
    else
    {
        printf("Invalid option\n");
    }
    displayMenu();
    scanf("%c", &option);
    printf("Enter to terminate Simualtor\n");
    scanf("%c", &terminate);
}
#ifdef __cplusplus
}
#endif