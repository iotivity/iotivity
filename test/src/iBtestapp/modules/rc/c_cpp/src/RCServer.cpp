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

#include "RCServer.h"

using namespace std;
using namespace OIC;
using namespace OIC::Service;

RCServer* RCServer::s_RCServer = NULL;
bool RCServer::g_bTestAppContainerStarted = false;
bool RCServer::g_bTestBundleStarted = false;
std::string RCServer::m_errorMessage = "";
std::mutex RCServer::s_mutexRcServer;
int RCServer::g_intBundleStartCounter = 0;

RCServer::RCServer()
{

}

RCServer* RCServer::getInstance(void)
{
    if (s_RCServer == NULL)
    {
        s_mutexRcServer.lock();

        s_RCServer = new RCServer();

        s_mutexRcServer.unlock();
    }

    return s_RCServer;
}

RCServer::~RCServer()
{
    if (s_RCServer != NULL)
    {
        delete s_RCServer;
        s_RCServer = NULL;
    }
}

void RCServer::rcStartContainer(RCSResourceContainer *container, std::string configPath)
{
    try
    {
        if (!g_bTestAppContainerStarted)
        {
            container->startContainer(configPath);
            g_bTestAppContainerStarted = true;
            if (!configPath.empty())
            {
                m_errorMessage =
                        "Resource container has been started successfully with default configuration...\n";
            }
            else
            {
                m_errorMessage = "Resource container has been started successfully...\n";
            }

            cout << m_errorMessage << endl;
        }
        else
        {
            cout << "Resource container has been started already..." << endl;
        }
    }
    catch (std::exception &ex)
    {
        cout << "Exception occurred :  " << ex.what() << endl;
    }
}

void RCServer::rcStopContainer(RCSResourceContainer *container)
{
    try
    {
        if (!checkIsContainerStarted(container))
            return;

        container->stopContainer();
        g_bTestAppContainerStarted = false;
        g_intBundleStartCounter = 0;
        m_errorMessage += "Resource container has been stopped successfully...\n";
        cout << m_errorMessage << endl;
    }
    catch (std::exception &ex)
    {
        cout << "Exception occurred :  " << ex.what() << endl;
    }
}

void RCServer::rcAddBundle(RCSResourceContainer *container, const std::string &bundleId,
        const std::string &bundleUri, const std::string &bundlePath, const std::string &activator,
        std::map< std::string, std::string > bundleParams)
{
    try
    {
        if (!checkIsContainerStarted(container))
            return;

        container->addBundle(bundleId, bundleUri, bundlePath, activator, bundleParams);

        string strTemp = "Bundle ";
        strTemp += "(";
        strTemp += bundleId;
        strTemp += ")";
        strTemp += " has been added successfully having\n";
        strTemp += "Uri: " + bundleUri + "\n";
        strTemp += "Path : " + bundlePath + "\n";

        m_errorMessage += strTemp;

        cout << m_errorMessage << endl;
    }
    catch (std::exception &ex)
    {
        cout << "Exception occurred :  " << ex.what() << endl;
    }
}

void RCServer::rcRemoveBundle(RCSResourceContainer *container, const std::string &bundleId)
{
    try
    {
        if (!checkIsContainerStarted(container))
            return;

        if (checkBundleRegistered(container, bundleId))
        {
            container->removeBundle(bundleId);
            (g_intBundleStartCounter < 0 ? 0 : g_intBundleStartCounter--);

            string strTemp = "Bundle ";
            strTemp += "(";
            strTemp += bundleId;
            strTemp += ")";
            strTemp += " has been removed successfully...\n";

            m_errorMessage += strTemp;

            cout << m_errorMessage << endl;
        }
        else
        {
            cout << "Bundle \'" << bundleId << "\' is not registered." << endl;
        }
    }
    catch (std::exception &ex)
    {
        cout << "Exception occurred :  " << ex.what() << endl;
    }
}

void RCServer::rcStartBundle(RCSResourceContainer *container, const std::string &bundleId)
{
    try
    {
        if (!checkIsContainerStarted(container))
            return;

        if (checkBundleRegistered(container, bundleId) && (g_intBundleStartCounter <= 2))
        {
            container->startBundle(bundleId);
            g_bTestBundleStarted = true;
            g_intBundleStartCounter++;

            string strTemp = "Bundle ";
            strTemp += "(";
            strTemp += bundleId;
            strTemp += ")";
            strTemp += " has been started successfully...\n";
            m_errorMessage += strTemp;

            cout << m_errorMessage << endl;
        }
        else
        {
            cout << "Bundle \'" << bundleId << "\' is not registered." << endl;
        }
    }
    catch (std::exception &ex)
    {
        cout << "Exception occurred :  " << ex.what() << endl;
    }
}

void RCServer::rcStopBundle(RCSResourceContainer *container, const std::string &bundleId)
{
    try
    {
        if (!checkIsContainerStarted(container))
            return;
        if (!checkIsBundleStarted(container, bundleId))
            return;

        container->stopBundle(bundleId);
        g_bTestBundleStarted = false;
        g_intBundleStartCounter--;

        string strTemp = "Bundle ";
        strTemp += "(";
        strTemp += bundleId;
        strTemp += ")";
        strTemp += " has been stopped successfully...\n";

        m_errorMessage += strTemp;
        cout << m_errorMessage << endl;
    }
    catch (std::exception &ex)
    {
        cout << "Exception occurred :  " << ex.what() << endl;
    }
}

void RCServer::rcAddBundleResource(RCSResourceContainer *container, const std::string &bundleId,
        const std::string &resourceUri, std::map< std::string, std::string > resourceParams,
        bool flag)
{
    try
    {
        if (!checkIsContainerStarted(container))
            return;
        if (!checkIsBundleStarted(container, bundleId))
            return;

        container->addResourceConfig(bundleId, resourceUri, resourceParams);
        m_errorMessage += "Resources(" + resourceUri + ")"
                + " has been added successfully to Bundle ID : " + bundleId + "\n";
        if (flag)
            cout << m_errorMessage << endl;
    }
    catch (std::exception &ex)
    {
        cout << "Exception occurred :  " << ex.what() << endl;
    }
}

void RCServer::rcRemoveBundleResource(RCSResourceContainer *container, const std::string &bundleId,
        const std::string &resourceUri)
{
    try
    {
        if (!checkIsContainerStarted(container))
            return;
        if (checkResourceRegistered(container, bundleId, resourceUri) && g_bTestBundleStarted)
        {
            container->removeResourceConfig(bundleId, resourceUri);
            m_errorMessage += "Resources(" + resourceUri + ")"
                    + " has been removed successfully from Bundle ID : " + bundleId + "\n";
            cout << m_errorMessage << endl;
        }
    }
    catch (std::exception &ex)
    {
        cout << "Exception occurred :  " << ex.what() << endl;
    }
}

void RCServer::displayCurrentBundleList(RCSResourceContainer *container)
{
    std::list < std::unique_ptr< RCSBundleInfo > > bundleList;

    if (!checkIsContainerStarted(container))
        return;

    bundleList = container->listBundles();

    if (bundleList.size() <= 0)
    {
        cout << "Sorry.Bundle list is empty..." << endl;
        return;
    }

    int i = 0;
    string bundleListString = "";
    bundleListString += "size of bundleList : ";
    bundleListString += std::to_string(bundleList.size());
    bundleListString += "\n\n";

    for (auto &bundleinfo : bundleList)
    {
        i++;
        bundleListString += "Bundle : ";
        bundleListString += std::to_string(i);
        bundleListString += " -: \n";
        bundleListString += "ID : " + bundleinfo->getID();
        bundleListString += "\n";
        bundleListString += "Lib Path: " + bundleinfo->getPath();
        bundleListString += "\n";

        cout << "-- " << bundleListString << endl;
    }

    cout << endl;
}

void RCServer::displayCurrentResourceList(RCSResourceContainer *container, std::string bundleId)
{
    std::list < std::string > resourceList;

    if (!checkIsContainerStarted(container))
        return;

    if (checkBundleRegistered(container, bundleId))
    {
        resourceList = container->listBundleResources(bundleId);

        if (resourceList.size() <= 0)
        {
            cout << "Sorry.Bundle Resource list is empty..." << endl;
            return;
        }

        string resourceListString = "";

        for (auto &bundleResource : resourceList)
        {
            resourceListString += bundleResource + "\n";
        }
        cout << "-- " << resourceListString << std::endl;
        cout << endl;
    }
    else
    {
        cout << "Bundle \'" << bundleId << "\' is not registered." << endl;
    }
}

/**********************Supporting functions *************************************/
/********************************************************************************/

bool RCServer::checkIsBundleStarted(RCSResourceContainer *container, std::string bundleId)
{
    if (checkBundleRegistered(container, bundleId) && (g_intBundleStartCounter <= 2))
    {
        return true;
    }
    else
    {
        cout << RESOURCE_BUNDLE_IS_NOT_STARTED_MSG << endl;
        return false;
    }
}

bool RCServer::checkIsContainerStarted(RCSResourceContainer *container)
{
    if (container && g_bTestAppContainerStarted)
    {
        return true;
    }
    else
    {
        cout << RESOURCE_CONTAINER_IS_NOT_STARTED_MSG << endl;
        return false;
    }
}

bool RCServer::checkBundleRegistered(RCSResourceContainer *container, std::string bundleId)
{
    std::list < std::unique_ptr< RCSBundleInfo > > bundleList;

    if (!checkIsContainerStarted(container))
        return;

    bundleList = container->listBundles();

    for (auto &bundle : bundleList)
    {
        if (bundle->getID().compare(bundleId) == 0)
            return true;
    }

    return false;
}

bool RCServer::checkResourceRegistered(RCSResourceContainer *container, std::string bundleId,
        std::string resourceUri)
{
    std::list < std::string > resourceList;

    if (g_bTestAppContainerStarted && checkBundleRegistered(container, bundleId))
    {
        resourceList = container->listBundleResources(bundleId);

        for (auto &resource : resourceList)
        {
            if (resource.compare(resourceUri) == 0)
                return true;
        }
    }

    std::cout << "Resource \'" << resourceUri << "\' is not registered." << std::endl;
    return false;
}
