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
 *      LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/

#include "ESMediatorHelper.h"
#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
//#define OC_RSRVD_ES_RES_TYPE_PROV "oic.wk.prov"
#define JSON_DB_PATH "./oic_svr_db_client.dat"
using namespace std;

bool ESMediatorHelper::s_isCallbackInvoked = false;
bool ESMediatorHelper::s_isDevicePropProvCallbackCalled = false;
bool ESMediatorHelper::s_isProvisionDevicePropSuccess = false;
bool ESMediatorHelper::s_isDeviceConfigCallbackCalled = false;
bool ESMediatorHelper::s_isConfigDevicePropSuccess = false;
bool ESMediatorHelper::s_isGetStatusCalled = false;
bool ESMediatorHelper::s_isGetStatusCallbackCalled = false;
bool ESMediatorHelper::s_isCloudPropProvCallbackCalled = false;
bool ESMediatorHelper::s_isProvisionCloudPropSuccess = false;
bool ESMediatorHelper::s_isSecurityPropProvCallbackCalled = false;
bool ESMediatorHelper::s_isProvisionSecurityPropSuccess = false;
std::shared_ptr< RemoteEnrollee > ESMediatorHelper::s_remoteEnrollee = nullptr;
std::shared_ptr< OC::OCResource > ESMediatorHelper::s_curResource = nullptr;
std::condition_variable ESMediatorHelper::s_cond;
int timeout = 10;

ESMediatorHelper::ESMediatorHelper()
{
}

void ESMediatorHelper::waitForResponse(int time)
{
    void* ret = NULL;
    timeout = time;
    pthread_t callbackListiningThread;
    pthread_create(&callbackListiningThread, NULL, listeningFunc, NULL);
    pthread_join(callbackListiningThread, &ret);
}

void* ESMediatorHelper::listeningFunc(void *arg)
{
    int second = 0;
    OCStackResult result = OC_STACK_ERROR;
    IOTIVITYTEST_LOG(INFO, "Waiting For the Response...........................\n");

    while (!ESMediatorHelper::s_isCallbackInvoked)
    {
        result = OCProcess();
        if (result != OC_STACK_OK)
        {
            IOTIVITYTEST_LOG(INFO, "OCStack stop error");
        }
        CommonUtil::waitInSecond(1);
        if (++second == timeout)
        {
            IOTIVITYTEST_LOG(INFO, "\nTimeout For Response!Please Try Again\n\n");
            break;
        }
    }

    IOTIVITYTEST_LOG(INFO, "Callback is successfully called");
    pthread_exit(NULL);
}

void ESMediatorHelper::provisionSecurityStatusCallback(
        std::shared_ptr< SecProvisioningStatus > secProvisioningStatus)
{
    ESMediatorHelper::waitForResponse(5);
    if (secProvisioningStatus->getESResult() != ES_OK)
    {
        IOTIVITYTEST_LOG(INFO, "provisionSecurity is failed.");
    }
    else
    {
        s_isSecurityPropProvCallbackCalled = true;
        IOTIVITYTEST_LOG(INFO, "provisionSecurity is success.");
        IOTIVITYTEST_LOG(INFO, "Device UUID : %d", secProvisioningStatus->getDeviceUUID().c_str());
    }

}

void ESMediatorHelper::provisionSecurityProperty()
{
    try
    {
        s_remoteEnrollee->provisionSecurity(provisionSecurityStatusCallback);
        s_isProvisionSecurityPropSuccess = true;
        ESMediatorHelper::waitForResponse(5);
    }
    catch (OCException &e)
    {
        IOTIVITYTEST_LOG(INFO, "Exception during ProvisionSecurityPropSuccess call");
        return;
    }
    CommonUtil::waitInSecond(5);
}

void ESMediatorHelper::cloudProvisioningStatusCallback(
        std::shared_ptr< CloudPropProvisioningStatus > provStatus)
{
    s_isCloudPropProvCallbackCalled = true;
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

void ESMediatorHelper::provisionCloudProperty()
{
    if (!s_remoteEnrollee)
    {
        return;
    }

    CloudProp cloudProp;
    cloudProp.setCloudProp("authCode", "authProvider", "ciServer");
    cloudProp.setCloudID("f002ae8b-c42c-40d3-8b8d-1927c17bd1b3");

    try
    {
        s_remoteEnrollee->provisionCloudProperties(cloudProp, cloudProvisioningStatusCallback);
        s_isProvisionCloudPropSuccess = true;
        
    }
    catch (OCException &e)
    {
        IOTIVITYTEST_LOG(INFO, "Exception during provisionCloudProperties call");
        return;
    }
    CommonUtil::waitInSecond(5);
}

void ESMediatorHelper::printStatus(EnrolleeStatus status)
{
    IOTIVITYTEST_LOG(INFO, "\tProvStatus : %d", status.getProvStatus());
    IOTIVITYTEST_LOG(INFO, "\tLastErrCode : %d", status.getLastErrCode());
}

void ESMediatorHelper::getStatusCallback(std::shared_ptr< GetEnrolleeStatus > getEnrolleeStatus)
{

    if (getEnrolleeStatus->getESResult() != ES_OK)
    {
        IOTIVITYTEST_LOG(INFO, "getStatus is failed.");
        return;
    }
    else
    {
        ESMediatorHelper esMediatorHelper;
        s_isGetStatusCallbackCalled = true;
        IOTIVITYTEST_LOG(INFO, "getStatus is success.");
        esMediatorHelper.printStatus(getEnrolleeStatus->getEnrolleeStatus());
        esMediatorHelper.waitForResponse(5);
    }
}

void ESMediatorHelper::getStatus()
{
    if (!s_remoteEnrollee)
    {
        return;
    }

    try
    {
        s_remoteEnrollee->getStatus(getStatusCallback);
        s_isGetStatusCalled = true;
        ESMediatorHelper::waitForResponse(5);
    }
    catch (OCException &e)
    {
        IOTIVITYTEST_LOG(INFO, "Exception during getConfiguration call");
        return;
    }
    CommonUtil::waitInSecond(5);
}

void ESMediatorHelper::printConfiguration(EnrolleeConf conf)
{
    IOTIVITYTEST_LOG(INFO, "===========================================");
    IOTIVITYTEST_LOG(INFO, "\tDevice Name : %s ", conf.getDeviceName().c_str());

    for (auto it : conf.getWiFiModes())
    {
        IOTIVITYTEST_LOG(INFO, "\tSupported WiFi modes : %d", it);
    }

    IOTIVITYTEST_LOG(INFO, "\tSupported WiFi freq : %d ", static_cast< int >(conf.getWiFiFreq()));
    IOTIVITYTEST_LOG(INFO, "\tCloud accessibility: %d", conf.isCloudAccessible());
    IOTIVITYTEST_LOG(INFO, "===========================================");
}

void ESMediatorHelper::getConfigurationCallback(
        std::shared_ptr< GetConfigurationStatus > getConfigurationStatus)
{

    ESMediatorHelper esMediatorHelper;
    if (getConfigurationStatus->getESResult() != ES_OK)
    {
        IOTIVITYTEST_LOG(INFO, "GetConfigurationStatus is failed.");
        return;
    }
    else
    {
        s_isDeviceConfigCallbackCalled = true;
        IOTIVITYTEST_LOG(INFO, "GetConfigurationStatus is success.");
        esMediatorHelper.printConfiguration(getConfigurationStatus->getEnrolleeConf());
        //ESMediatorHelper::waitForResponse(5);
    }
}

void ESMediatorHelper::getConfiguration()
{
    if (!s_remoteEnrollee)
    {
        return;
    }

    try
    {
        s_remoteEnrollee->getConfiguration(getConfigurationCallback);
        s_isConfigDevicePropSuccess = true;
        ESMediatorHelper::waitForResponse(5);
    }
    catch (OCException &e)
    {
        IOTIVITYTEST_LOG(INFO, "Exception during getConfiguration call");
        return;
    }
    CommonUtil::waitInSecond(5);
}

void ESMediatorHelper::deviceProvisioningStatusCallback(
        std::shared_ptr< DevicePropProvisioningStatus > provStatus)
{

    if (provStatus->getESResult() != ES_OK)
    {
        IOTIVITYTEST_LOG(INFO, "Device Provisioning is failed.");
        return;
    }
    else
    {
        s_isDevicePropProvCallbackCalled = true;
        ESMediatorHelper::waitForResponse(5);
        IOTIVITYTEST_LOG(INFO, "Device Provisioning is success.");
    }
}

void ESMediatorHelper::provisionDeviceProperty()
{
    if (!s_remoteEnrollee)
    {
        return;
    }

    DeviceProp devProp;
    devProp.setWiFiProp("Iotivity_SSID", "Iotivity_PWD", WPA2_PSK, TKIP_AES);
    devProp.setDevConfProp("Bangla", "Bangladesh", "Location");

    try
    {
        s_remoteEnrollee->provisionDeviceProperties(devProp,
                &ESMediatorHelper::deviceProvisioningStatusCallback);
        ESMediatorHelper::s_isProvisionDevicePropSuccess = true;
        ESMediatorHelper::waitForResponse(5);
    }
    catch (OCException &e)
    {
        IOTIVITYTEST_LOG(INFO, "Exception during provisionDeviceProperties call");
        return;
    }
    CommonUtil::waitInSecond(5);
}

// Callback to found resources
void ESMediatorHelper::foundResource(std::shared_ptr< OC::OCResource > resource)
{
    std::string resourceURI;
    std::string hostAddress;
    try
    {
        // Do some operations with resource object.
        if (resource && !s_curResource
                && resource->getResourceTypes().at(0) == OC_RSRVD_ES_RES_TYPE_PROV)
        {
            IOTIVITYTEST_LOG(INFO, "DISCOVERED Resource:");
            // Get the resource URI
            resourceURI = resource->uri();
            IOTIVITYTEST_LOG(INFO, "\tURI of the resource: %s", resourceURI.c_str());

            // Get the resource host address
            hostAddress = resource->host();
            IOTIVITYTEST_LOG(INFO, "\tHost address of the resource:  %s", hostAddress.c_str());

            // Get the resource types
            IOTIVITYTEST_LOG(INFO, "\tList of resource types: ");
            for (auto &resourceTypes : resource->getResourceTypes())
            {
                IOTIVITYTEST_LOG(INFO, "\t\t %s", resourceTypes.c_str());
            }

            // Get the resource interfaces
            IOTIVITYTEST_LOG(INFO, "\tList of resource interfaces: ");
            for (auto &resourceInterfaces : resource->getResourceInterfaces())
            {
                IOTIVITYTEST_LOG(INFO, "\t\t %s", resourceInterfaces.c_str());
            }

            if (s_curResource == nullptr)
            {
                s_remoteEnrollee = EasySetup::getInstance()->createRemoteEnrollee(resource);
                if (!s_remoteEnrollee)
                {
                    IOTIVITYTEST_LOG(INFO, "RemoteEnrollee object is failed for some reasons!");
                }
                else
                {
                    s_curResource = resource;
                    IOTIVITYTEST_LOG(INFO, "RemoteEnrollee object is successfully created!");
                    s_cond.notify_all();
                }
            }
            ESMediatorHelper::s_isCallbackInvoked = true;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in foundResource: " << e.what() << std::endl;
    }
}

static FILE* client_open(const char *UNUSED_PARAM, const char *mode)
{
    (void) UNUSED_PARAM;
    return fopen(JSON_DB_PATH, mode);
}

void ESMediatorHelper::findEnrolleeResource()
{
    std::ostringstream requestURI;
    OCPersistentStorage ps
    { client_open, fread, fwrite, fclose, unlink };

    PlatformConfig config
    { OC::ServiceType::InProc, ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos, &ps };

    OCPlatform::Configure(config);

    requestURI << OC_RSRVD_WELL_KNOWN_URI << "?rt=" << OC_RSRVD_ES_RES_TYPE_PROV;

    OCPlatform::findResource("", requestURI.str(), CT_DEFAULT, &ESMediatorHelper::foundResource);

    IOTIVITYTEST_LOG(INFO, "Finding Resource... ");

    ESMediatorHelper::waitForResponse(5);
}
