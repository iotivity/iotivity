/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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
#include <stdlib.h>
#include <stdio.h>

#ifdef __LINUX__
#include <execinfo.h>
#endif

#include <signal.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "OCPlatform.h"
#include "OCApi.h"
#include "ResourceHelper.h"
#include "CommonUtil.h"

using namespace OC;
using namespace std;

static string udpPortPrefix = "coap";
static string tcpPortPrefix = "tcp";
vector< shared_ptr< OCResource > > g_foundResourceList;
map< string, int > g_tcpPortList;
map< string, int > g_udpPortList;
vector< string > g_localIpList;

QualityOfService g_qos = QualityOfService::LowQos;
OCConnectivityType g_connectivityType = CT_DEFAULT;
ResourceHelper* g_resourceHelper = NULL;
static mutex s_mutex;

void onResourceFound(shared_ptr< OCResource > );
OCStackResult initiateServer(void);
void findAllResources(string host = "", string query = "");
void waitForCallback(void);

void handler(int sig)
{
    void *array[10];
    size_t size;

#ifdef __LINUX__
    size = backtrace(array, 10);
#endif

    fprintf(stderr, "Error: signal %d:\n", sig);

#ifdef __LINUX__
    backtrace_symbols_fd(array, size, STDERR_FILENO);
#endif

    exit(1);
}

int main(int argc, char* argv[])
{
    signal(SIGSEGV, handler);
    g_foundResourceList.clear();
    g_localIpList.clear();
    g_tcpPortList.clear();
    g_udpPortList.clear();
    g_resourceHelper = ResourceHelper::getInstance();
    g_connectivityType = CT_DEFAULT;
    g_qos = QualityOfService::HighQos;

    cout << "Serial Killer App started successfully" << endl << endl;
    g_localIpList.push_back(CommonUtil::getLocalIP(InternetProtocolVersion::IPv4));
    g_localIpList.push_back(CommonUtil::getLocalIP(InternetProtocolVersion::IPv6));

    OCStackResult result = initiateServer();
    if (result != OC_STACK_OK)
    {
        return -1;
    }

    findAllResources();
    waitForCallback();

    for (auto tcpPort : g_tcpPortList)
    {
        cout << "found tcp port: " << tcpPort.first << endl;
        CommonUtil::killPID(CommonUtil::getPIDFromPort(tcpPort.first));
    }
    for (auto udpPort : g_udpPortList)
    {
        cout << "found udp port: " << udpPort.first << endl;

        CommonUtil::killPID(CommonUtil::getPIDFromPort(udpPort.first));
    }

    cout << "Signing off from serial killer duty .... Done!!" << endl;

    return 0;
}

void onResourceFound(shared_ptr< OCResource > resource)
{
    s_mutex.lock();

    if (resource)
    {
        string host = resource->host();
        g_foundResourceList.push_back(resource);
        if (host.find(udpPortPrefix) != string::npos && (host.find(g_localIpList.at(0)) != string::npos || host.find(g_localIpList.at(1)) != string::npos ))
        {
            g_udpPortList[host.substr(host.rfind(":") + 1, string::npos)]++;
        }
        if (host.find(tcpPortPrefix) != string::npos && (host.find(g_localIpList.at(0)) != string::npos || host.find(g_localIpList.at(1)) != string::npos ))
        {
            g_tcpPortList[host.substr(host.rfind(":") + 1, string::npos)]++;
        }
    }
    else
    {
        cout << "Found resource is invalid!!" << endl;
    }

    s_mutex.unlock();
}

void waitForCallback()
{
    g_resourceHelper->waitInSecond(CALLBACK_WAIT_MIN + CALLBACK_WAIT_MIN);
}

OCStackResult initiateServer()
{
    OCStackResult result = OC_STACK_OK;
    PlatformConfig cfg
    { OC::ServiceType::InProc, OC::ModeType::Client, g_connectivityType, g_connectivityType, g_qos };
    OCPlatform::Configure(cfg);
    result = OCPlatform::start();

    if (result != OC_STACK_OK)
    {
        cout << "Unable to start IoTivity Client" << endl;
    }
    else
    {
        cout << "IoTivity Client started successfully" << endl;
    }

    return result;
}

void findAllResources(string host, string query)
{
    g_foundResourceList.clear();
    std::ostringstream requestURI;

    try
    {
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
        cout << "Finding Open Resource Servers...." << endl;

    }
    catch (OCException &e)
    {
        cerr << "Exception in main: " << e.what();
    }
    waitForCallback();
}
