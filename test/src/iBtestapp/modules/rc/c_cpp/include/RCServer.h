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

#ifndef INCLUDE_TESTAPP_RCServerHELPER_H
#define INCLUDE_TESTAPP_RCServerHELPER_H

#if defined(__linux__)
#include <unistd.h>
#include <string.h>
#endif

#include "RCSResourceContainer.h"
#include "RCSBundleInfo.h"

#include <iostream>
#include <functional>
#include <limits>
#include <stdexcept>
#include <string>
#include <mutex>
#include <condition_variable>

using namespace OIC::Service;

#define     RESOURCE_CONTAINER_IS_NOT_STARTED_MSG   "Please start resource container before perform this action..."
#define     RESOURCE_BUNDLE_IS_NOT_STARTED_MSG      "Please start resource bundle before perform this action..."
#define     MAX_PATH 2048
#define     RC_MENU_SLEEP                           1

struct BundleIDInformation
{
    std::string bundleID;
    std::string bundleURI;
    std::string bundlePATH;
    std::string bundleACRIVATOR;
    std::string bundleResourceURI;
    std::string bundleResourceType;
    std::string bundleResourceAdress;
    bool isBundleStarted;
    bool isBundleIDAdded;
    bool isBundleResourceAdded;
};

struct ContainerResourceInfo
{
    std::string resourceURI;
    std::string resourceTYPE;
};

const std::string TEST_BUNDLE_ID_HUESAMPLE = "oic.bundle.hueSample";
const std::string TEST_BUNDLE_URI_HUESAMPLE = "/hueSample";
const std::string TEST_BUNDLE_PATH_HUESAMPLE = "libHueBundle.so";
const std::string TEST_BUNDLE_ACTIVATOR_HUESAMPLE = "huesample";
const std::string TEST_RESOURCE_URI_HUESAMPLE = "/hue/light/sample";
const std::string TEST_RESOURCE_TYPE_HUESAMPLE = "oic.r.light";
const std::string TEST_ADDRESS_HUESAMPLE = "http://192.168.0.2/api/newdeveloper/lights/1";

const std::string TEST_BUNDLE_ID_BMISENSOR = "oic.bundle.BMISensor";
const std::string TEST_BUNDLE_URI_BMISENSOR = "/BMISensor";
const std::string TEST_BUNDLE_PATH_BMISENSOR = "libBMISensorBundle.so";
const std::string TEST_BUNDLE_ACTIVATOR_BMISENSOR = "bmisensor";
const std::string TEST_RESOURCE_URI_BMISENSOR = "/softsensor/BMIsensor/1";
const std::string TEST_RESOURCE_TYPE_BMISENSOR = "oic.r.sensor";
const std::string TEST_ADDRESS_BMISENSOR = "";

const std::string TEST_BUNDLE_ID_DEFAULT_BUNDLE_ID = "oic.bundle.discomfortIndexSensor";
const std::string TEST_CONFIG_PATH = "ResourceContainerConfigTestApp.xml";

const std::string TEST_RESOURCE_TYPE_HUMIDITY  = "oic.r.humidity";
const std::string TEST_RESOURCE_TYPE_TEMPERATURE = "oic.r.temperature";
const std::string TEST_RESOURCE_TYPE_WEIGHT = "oic.r.sensor.weight";
const std::string TEST_RESOURCE_TYPE_HEIGHT = "oic.r.sensor.height";

class RCServer
{
private:
    static RCServer* s_RCServer;
    static std::mutex s_mutexRcServer;

public:
    RCServer(void);
    ~RCServer(void);

    static bool g_bTestAppContainerStarted;
    static bool g_bTestBundleStarted;
    static int g_intBundleStartCounter;
    static std::string m_errorMessage;

    /**
     * @brief   function for getting class instance
     * @return  class instance
     */
    static RCServer* getInstance(void);

    /**
     * @brief       This function will check that container is started or not
     */
    bool checkIsContainerStarted(RCSResourceContainer *container);

    /**
     * @brief       This function will check that bundle is started or not
     */
    bool checkIsBundleStarted(RCSResourceContainer *container, std::string bundleId);

    /**
     * @brief       This function will check that a bundle is already registered or not
     */
    bool checkBundleRegistered(RCSResourceContainer *container, std::string bundleId);

    /**
     * @brief       This function will check that a resource is already registered or not
     */
    bool checkResourceRegistered(RCSResourceContainer *container, std::string bundleId,
            std::string resourceUri);

    /**
     * @brief                   Function is for getting status of start resource container
     */
    void rcStartContainer(RCSResourceContainer *container, std::string configPath);

    /**
     * @brief                   This function will stop the resource container
     */
    void rcStopContainer(RCSResourceContainer *container);

    /**
     * @brief       API for adding the bundle to the Container
     *
     * @param       bundleId Id of the Bundle
     * @param       bundleUri Uri of the bundle
     * @param       bundlePath Path of the bundle
     * @param       activator Activation prefix for .so bundles, or activator class name for .jar bundles
     * @param       params  key-value pairs in string form for other Bundle parameters
     */
    void rcAddBundle(RCSResourceContainer *container, const std::string &bundleId,
            const std::string &bundleUri, const std::string &bundlePath,
            const std::string &activator, std::map< std::string, std::string > bundleParams);

    /**
     * @brief       API for removing the bundle from the container
     *
     * @param       bundleId Id of the Bundle
     */
    void rcRemoveBundle(RCSResourceContainer *container, const std::string &bundleId);

    /**
     * @brief       API for starting the bundle.
     *
     * @param       bundleId Id of the Bundle
     */
    void rcStartBundle(RCSResourceContainer *container, const std::string &bundleId);

    /**
     * @brief       API for Stopping the bundle.
     *
     * @param       bundleId Id of the Bundle
     */
    void rcStopBundle(RCSResourceContainer *container, const std::string &bundleId);

    /**
     * @brief       API for adding the Resource configuration information to the bundle
     *
     * @param       bundleId Id of the Bundle
     * @param       resourceUri URI of the resource
     * @param       params key-value pairs in string form for other Bundle parameters
     */
    void rcAddBundleResource(RCSResourceContainer *container, const std::string &bundleId,
            const std::string &resourceUri, std::map< std::string, std::string > resourceParams,
            bool flag);

    /**
     * @brief       API for removing the Resource configuration information from the bundle
     *
     * @param       bundleId Id of the Bundle
     * @param       resourceUri URI of the resource
     */
    void rcRemoveBundleResource(RCSResourceContainer *container, const std::string &bundleId,
            const std::string &resourceUri);

    /**
     * @brief       This function will display current bundle list
     */
    void displayCurrentBundleList(RCSResourceContainer *container);

    /**
     * @brief       This function will display current resource list
     */
    void displayCurrentResourceList(RCSResourceContainer *container, std::string bundleId);
};

#endif
