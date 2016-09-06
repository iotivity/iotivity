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

#include "RIHelper.h"

#define KEY_PLATFORM_ID "pi"
#define KEY_MANUFACTURER_NAME "mnmn"
#define KEY_MANUFACTURER_URL "mnml"
#define KEY_MODEL_NO "mnmo"
#define KEY_MANUFACTURE_DATE "mndt"
#define KEY_PLATFORM_VERSION "mnpv"
#define KEY_OS_VERSION "mnos"
#define KEY_HW_VERSION "mnhw"
#define KEY_FIRMWARE_VERSION "mnfv"
#define KEY_SUPPORT_URL "mnsl"
#define KEY_SYSTEM_TIME "st"

class RICppIntegrationTest_stc: public ::testing::Test
{
protected:
    RIHelper *m_RIHelper;
    const std::string m_DeviceDiscoveryURI = "/oic/d";
    const std::string m_PlatformDiscoveryURI = "/oic/p";
    const std::string m_ResourceInterface = DEFAULT_INTERFACE;
    typedef std::map< OCResourceIdentifier, std::shared_ptr< OCResource > > m_discoveredResourceMap;
    m_discoveredResourceMap m_discoveredResources;
    std::shared_ptr< OCResource > m_temperatureResource;
    std::mutex m_curResourceLock;
    OCResourceHandle m_ResourceHandle;
    const string m_TemperatureUri = TEMPERATURE_URI;
    const string m_TemperatureType = TEMPERATURE_TYPE;
    OCResourceHandle m_temperatureHandle;bool m_foundResourceCheck = false;bool m_deviceDiscoveryCheck =
            false;bool m_PlatformDiscoveryCheck = false;bool m_foundAllPlatformInfo = true;bool m_foundAllDeviceInfo =
            true;bool isGetCorrect = false;bool isPutCorrect = false;bool isPostCorrect = false;bool isDeleteCorrect =
            false;bool isObserveCorrect = false;
    int m_temp = 20;
    int m_hour = 20;
    std::string m_temperatureResourceQuery = "/oic/res?rt=oic.r.temperature";

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();
        m_RIHelper = RIHelper::getInstance();
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
    }

public:
    void onDeviceInfoReceived(const OCRepresentation& rep)
    {
        m_deviceDiscoveryCheck = true;
        IOTIVITYTEST_LOG(INFO, "Device Information received----->");
        std::string value;
        std::string values[] =
        { "di", "Device ID        ", "n", "Device name      ", "lcv", "Spec version url ", "dmv",
                "Data Model Version ", };

        for (unsigned int i = 0; i < sizeof(values) / sizeof(values[0]); i += 2)
        {
            if (rep.getValue(values[i], value))
            {
                if (values[i] == "n")
                {
                    if (value != DEVICE_NAME)
                    {
                        m_foundAllDeviceInfo = false;
                    }
                }
                IOTIVITYTEST_LOG(INFO, "%s : %s", values[i + 1].c_str(), value.c_str());
            }
        }
    }

    void onPlatformInfoReceived(const OCRepresentation& rep)
    {
        m_PlatformDiscoveryCheck = true;
        IOTIVITYTEST_LOG(INFO, "Platform Information received ---->");
        std::string value;
        std::string values[] =
        { "pi", "Platform ID                    ", "mnmn", "Manufacturer name              ",
                "mnml", "Manufacturer url               ", "mnmo",
                "Manufacturer Model No          ", "mndt", "Manufactured Date              ",
                "mnpv", "Manufacturer Platform Version  ", "mnos",
                "Manufacturer OS version        ", "mnhw", "Manufacturer hardware version  ",
                "mnfv", "Manufacturer firmware version  ", "mnsl",
                "Manufacturer support url       ", "st", "Manufacturer system time       " };

        for (unsigned int i = 0; i < sizeof(values) / sizeof(values[0]); i += 2)
        {
            if (rep.getValue(values[i], value))
            {
                IOTIVITYTEST_LOG(INFO, "%s : %s", values[i + 1].c_str(), value.c_str());

                if ((values[i].compare(KEY_PLATFORM_ID)) == 0)
                {
                    if (value != PLATFORM_ID)
                    {
                        m_foundAllPlatformInfo = false;
                        IOTIVITYTEST_LOG(INFO, "%s did not match", values[i].c_str());
                    }
                }
                else if ((values[i].compare(KEY_MANUFACTURER_NAME)) == 0)
                {
                    if (value != MANUFACTURER_NAME)
                    {
                        m_foundAllPlatformInfo = false;
                        IOTIVITYTEST_LOG(INFO, "%s did not match", values[i].c_str());
                    }
                }
                else if ((values[i].compare(KEY_MANUFACTURER_URL)) == 0)
                {
                    if (value != MANUFACTURER_URL)
                    {
                        m_foundAllPlatformInfo = false;
                        IOTIVITYTEST_LOG(INFO, "%s did not match", values[i].c_str());
                    }
                }
                else if ((values[i].compare(KEY_MODEL_NO)) == 0)
                {
                    if (value != MODEL_NO)
                    {
                        m_foundAllPlatformInfo = false;
                        IOTIVITYTEST_LOG(INFO, "%s did not match", values[i].c_str());
                    }
                }
                else if ((values[i].compare(KEY_MANUFACTURE_DATE)) == 0)
                {
                    if (value != MANUFACTURE_DATE)
                    {
                        m_foundAllPlatformInfo = false;
                        IOTIVITYTEST_LOG(INFO, "%s did not match", values[i].c_str());
                    }
                }
                else if ((values[i].compare(KEY_PLATFORM_VERSION)) == 0)
                {
                    if (value != PLATFORM_VERSION)
                    {
                        m_foundAllPlatformInfo = false;
                        IOTIVITYTEST_LOG(INFO, "%s did not match", values[i].c_str());
                    }
                }
                else if ((values[i].compare(KEY_OS_VERSION)) == 0)
                {
                    if (value != OS_VERSION)
                    {
                        m_foundAllPlatformInfo = false;
                        IOTIVITYTEST_LOG(INFO, "%s did not match", values[i].c_str());
                    }
                }
                else if ((values[i].compare(KEY_HW_VERSION)) == 0)
                {
                    if (value != HW_VERSION)
                    {
                        m_foundAllPlatformInfo = false;
                        IOTIVITYTEST_LOG(INFO, "%s did not match", values[i].c_str());
                    }
                }
                else if ((values[i].compare(KEY_FIRMWARE_VERSION)) == 0)
                {
                    if (value != FIRMWARE_VERSION)
                    {
                        m_foundAllPlatformInfo = false;
                        IOTIVITYTEST_LOG(INFO, "%s did not match", values[i].c_str());
                    }
                }
                else if ((values[i].compare(KEY_SUPPORT_URL)) == 0)
                {
                    if (value != SUPPORT_URL)
                    {
                        m_foundAllPlatformInfo = false;
                        IOTIVITYTEST_LOG(INFO, "%s did not match", values[i].c_str());
                    }
                }
                else if ((values[i].compare(KEY_SYSTEM_TIME)) == 0)
                {
                    if (value != SYSTEM_TIME)
                    {
                        m_foundAllPlatformInfo = false;
                        IOTIVITYTEST_LOG(INFO, "%s did not match", values[i].c_str());
                    }
                }
            }
        }
    }

    void foundResource(std::shared_ptr< OCResource > resource)
    {
        IOTIVITYTEST_LOG(INFO, "In foundResource");
        std::string resourceURI;
        std::string hostAddress;

        try
        {
            m_foundResourceCheck = true;
            std::lock_guard< std::mutex > lock(m_curResourceLock);

            // Do some operations with resource object.
            if (resource)
            {
                IOTIVITYTEST_LOG(INFO, "DISCOVERED Resource:");
                // Get the resource URI
                resourceURI = resource->uri();
                IOTIVITYTEST_LOG(INFO, "\ttURI of the resource: %s", resourceURI.c_str());

                // Get the resource host address
                hostAddress = resource->host();
                IOTIVITYTEST_LOG(INFO, "\tHost address of the resource: %s", hostAddress.c_str());

                // Get the resource types
                IOTIVITYTEST_LOG(INFO, "\tList of resource types:");
                for (std::string resourceTypes : resource->getResourceTypes())
                {
                    IOTIVITYTEST_LOG(INFO, "\t\t\t%s", resourceTypes.c_str());
                    if (resourceTypes == m_TemperatureType)
                    {
                        IOTIVITYTEST_LOG(INFO, "Found temperature resource");
                        m_temperatureResource = resource;
                    }
                }
            }
            else
            {
                // Resource is invalid
                IOTIVITYTEST_LOG(INFO, "Resource is invalid");
            }
        }
        catch (std::exception& e)
        {
            IOTIVITYTEST_LOG(DEBUG, "Exception in foundResource: %s", e.what());
        }
    }

    void onGet(const HeaderOptions& /*headerOptions*/, const OCRepresentation& rep, const int eCode)
    {
        try
        {
            if (eCode == OC_STACK_OK)
            {
                int temperature;
                int hour;
                IOTIVITYTEST_LOG(INFO, "GET request was successful");
                rep.getValue("temperature", temperature);
                rep.getValue("hour", hour);

                IOTIVITYTEST_LOG(INFO, "\thour: %d", hour);
                IOTIVITYTEST_LOG(INFO, "\ttemperature: %d", temperature);

                if (temperature == m_RIHelper->m_temp && hour == m_RIHelper->m_hour)
                {
                    isGetCorrect = true;
                }
            }
            else
            {
                IOTIVITYTEST_LOG(INFO, "onGet Response error: %d", eCode);
            }
        }
        catch (std::exception& e)
        {
            IOTIVITYTEST_LOG(INFO, "Exception in onGet: %s", eCode);
        }
    }

    void onPut(const HeaderOptions& /*headerOptions*/, const OCRepresentation& rep, const int eCode)
    {
        try
        {
            if (eCode == OC_STACK_OK || eCode == OC_STACK_RESOURCE_CHANGED)
            {
                int temperature;
                int hour;
                IOTIVITYTEST_LOG(INFO, "PUT request was successful");
                rep.getValue("temperature", temperature);
                rep.getValue("hour", hour);

                IOTIVITYTEST_LOG(INFO, "\thour: %d", hour);
                IOTIVITYTEST_LOG(INFO, "\ttemperature: %d", temperature);

                if (temperature == m_temp && hour == m_hour)
                {
                    isPutCorrect = true;
                }
            }
            else
            {
                IOTIVITYTEST_LOG(INFO, "onPut Response error: %d", eCode);
            }
        }
        catch (std::exception& e)
        {
            IOTIVITYTEST_LOG(INFO, "Exception in onPut: %s", eCode);
        }
    }

    void onPost(const HeaderOptions& /*headerOptions*/, const OCRepresentation& rep,
            const int eCode)
    {
        try
        {
            if (eCode == OC_STACK_OK || eCode == OC_STACK_RESOURCE_CHANGED)
            {
                int temperature;
                int hour;
                IOTIVITYTEST_LOG(INFO, "POST request was successful");
                rep.getValue("temperature", temperature);
                rep.getValue("hour", hour);

                IOTIVITYTEST_LOG(INFO, "\thour: %d", hour);
                IOTIVITYTEST_LOG(INFO, "\ttemperature: %d", temperature);

                if (temperature == m_temp && hour == m_hour)
                {
                    isPostCorrect = true;
                }
            }
            else
            {
                IOTIVITYTEST_LOG(INFO, "onPut Response error: %d", eCode);
            }
        }
        catch (std::exception& e)
        {
            IOTIVITYTEST_LOG(INFO, "Exception in onPut: %s", eCode);
        }
    }

    void onDelete(const HeaderOptions &headerOptions, const int eCode)
    {
        if (eCode == 0 || eCode == 2)
        {
            IOTIVITYTEST_LOG(INFO, "Delete request was successful");
            isDeleteCorrect = true;
        }
        else
        {
            IOTIVITYTEST_LOG(INFO, "onDelete Response error: %d", eCode);
        }
    }
};

/**
 * @since 2016-08-04
 * @see void Configure (const PlatformConfig &config)
 * @objective Test  registerResource() API and findResource() API positively to check
 *      if registered resource is found and foundResource callback is called
 * @target OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                       std::string& resourceURI,
 *                       const std::string& resourceTypeName,
 *                       const std::string& resourceInterface,
 *                       EntityHandler entityHandler,
 *                       uint8_t resourceProperty)
 * @target OCStackResult findResource(const std::string& host, const std::string& resourceURI,
 *                   OCConnectivityType connectivityType, FindCallback resourceHandler)
 * @test_data     1. OCResourceHandle pointer to the created resource
 *                2. resourceURI "/a/temperature"
 *                3. resourceTypeName "oic.r.temperature"
 *                4. resourceInterface DEFAULT_INTERFACE
 *                5. EntityHandler callback to be called
 *                6. resourceProperty indicates the property of the resource
 *                7. host Host IP Address of the resource
 *                8. resourceURI OC_RSRVD_WELL_KNOWN_URI
 *                9. OCConnectivityType CT_DEFAULT
 *                10. FindCallback callback to be called when found resource
 * @pre_condition Configure platform for client server mode
 * @procedure     1. Perform registerResource() API
 *                2. Perform findResource() API
 *                3. Check if callback is called
 *                4. Check if temperature resource is found
 * @post_condition None
 * @expected Resource should be found successfully and callback should be called
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICppIntegrationTest_stc, CreateAndFindResource_SQV_CV_P)
{
    try
    {
        m_temperatureHandle = m_RIHelper->registerResource(m_TemperatureUri, m_TemperatureType,
                m_ResourceInterface);
        ASSERT_NE(m_temperatureHandle,NULL) << "Register Resource failed";

        std::ostringstream requestURI;
        requestURI << OC_RSRVD_WELL_KNOWN_URI;
        IOTIVITYTEST_LOG(INFO, "Finding Resource...");
        ASSERT_EQ(OC_STACK_OK,
                OCPlatform::findResource("", requestURI.str(), CT_DEFAULT, std::bind(&RICppIntegrationTest_stc::foundResource, this, PH::_1), QualityOfService::HighQos))
        << "findResource does not return success";
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if(!m_foundResourceCheck)
        {
            SET_FAILURE("onFoundResource Callback is not Invoked!");
            return;
        }
        if(m_temperatureResource == nullptr)
        {
            SET_FAILURE("Temperature resource was not found");
        }
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occured. Exception is " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-08-04
 * @see void Configure (const PlatformConfig &config)
 * @objective Test  registerResource() API and findResource() API (with resource type) positively to check
 *      if foundResource callback is called
 * @target OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                       std::string& resourceURI,
 *                       const std::string& resourceTypeName,
 *                       const std::string& resourceInterface,
 *                       EntityHandler entityHandler,
 *                       uint8_t resourceProperty)
 * @target OCStackResult findResource(const std::string& host, const std::string& resourceURI,
 *                   OCConnectivityType connectivityType, FindCallback resourceHandler)
 * @test_data     1. OCResourceHandle pointer to the created resource
 *                2. resourceURI "/a/temperature"
 *                3. resourceTypeName "oic.r.temperature"
 *                4. resourceInterface DEFAULT_INTERFACE
 *                5. EntityHandler callback to be called
 *                6. resourceProperty indicates the property of the resource
 *                7. host Host IP Address of the resource
 *                8. resourceURI "/oic/res/?rt=oic.r.temperature"
 *                9. OCConnectivityType CT_DEFAULT
 *                10. FindCallback callback to be called when found resource
 * @pre_condition Configure platform for client server mode
 * @procedure     1. Perform registerResource() API
 *                2. Perform findResource() API with resource type in query
 *                3. Check if callback is called
 *                4. Check if temperature resource is found
 * @post_condition None
 * @expected Resource should be found successfully and callback should be called
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICppIntegrationTest_stc, CreateAndFindResourceUsingResourceType_SQV_CV_P)
{
    try
    {
        m_temperatureHandle = m_RIHelper->registerResource(m_TemperatureUri, m_TemperatureType,
                m_ResourceInterface);
        ASSERT_NE(m_temperatureHandle,NULL) << "Register Resource failed";

        IOTIVITYTEST_LOG(INFO, "Finding Temperature Resource...");
        ASSERT_EQ(OC_STACK_OK,
                OCPlatform::findResource("", m_temperatureResourceQuery, CT_DEFAULT, std::bind(&RICppIntegrationTest_stc::foundResource, this, PH::_1)))
        << "findResource does not return success";
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if(!m_foundResourceCheck)
        {
            SET_FAILURE("onFoundResource Callback is not Invoked!");
            return;
        }
        if(m_temperatureResource == nullptr)
        {
            SET_FAILURE("Temperature resource was not found");
        }
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occured. Exception is " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-08-04
 * @see void Configure (const PlatformConfig &config)
 * @objective Test  registerResource() API and findResource() API (with resource type) for two times to check
 *      if foundResource callback is called
 * @target OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                       std::string& resourceURI,
 *                       const std::string& resourceTypeName,
 *                       const std::string& resourceInterface,
 *                       EntityHandler entityHandler,
 *                       uint8_t resourceProperty)
 * @target OCStackResult findResource(const std::string& host, const std::string& resourceURI,
 *                   OCConnectivityType connectivityType, FindCallback resourceHandler)
 * @test_data     1. OCResourceHandle pointer to the created resource
 *                2. resourceURI "/a/temperature"
 *                3. resourceTypeName "oic.r.temperature"
 *                4. resourceInterface DEFAULT_INTERFACE
 *                5. EntityHandler callback to be called
 *                6. resourceProperty indicates the property of the resource
 *                7. host Host IP Address of the resource
 *                8. resourceURI "/oic/res/?rt=oic.r.temperature"
 *                9. OCConnectivityType CT_DEFAULT
 *                10. FindCallback callback to be called when found resource
 * @pre_condition Configure platform for client server mode
 * @procedure     1. Perform registerResource() API
 *                2. Perform findResource() API with resource type in query
 *                3. Check if callback is called
 *                4. Check if temperature resource is found
 *                5. Perform findResource() API again with resource type in query
 *                6. Check if callback is called
 *                7. Check if temperature resource is found
 * @post_condition None
 * @expected Resource should be found successfully and callback should be called for both times
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICppIntegrationTest_stc, CreateAndFindResourceTwice_VLCC_P)
{
    try
    {
        m_temperatureHandle = m_RIHelper->registerResource(m_TemperatureUri, m_TemperatureType,
                m_ResourceInterface);
        ASSERT_NE(m_temperatureHandle,NULL) << "Register Resource failed";

        IOTIVITYTEST_LOG(INFO, "Finding Temperature Resource...");
        ASSERT_EQ(OC_STACK_OK,
                OCPlatform::findResource("", m_temperatureResourceQuery, CT_DEFAULT, std::bind(&RICppIntegrationTest_stc::foundResource, this, PH::_1)))
        << "findResource does not return success";
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if(!m_foundResourceCheck)
        {
            SET_FAILURE("onFoundResource Callback is not Invoked at the first attempt!");
            return;
        }
        if(m_temperatureResource == nullptr)
        {
            SET_FAILURE("Temperature resource was not found");
            return;
        }

        m_foundResourceCheck = false;
        m_temperatureResource = nullptr;

        IOTIVITYTEST_LOG(INFO, "Finding Resource for second time...");
        ASSERT_EQ(OC_STACK_OK,
                OCPlatform::findResource("", m_temperatureResourceQuery, CT_DEFAULT, std::bind(&RICppIntegrationTest_stc::foundResource, this, PH::_1)))
        << "findResource does not return success";
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if(!m_foundResourceCheck)
        {
            SET_FAILURE("onFoundResource Callback is not Invoked at the second attempt!");
            return;
        }
        if(m_temperatureResource == nullptr)
        {
            SET_FAILURE("Temperature resource was not found");
            return;
        }
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occured. Exception is " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-08-04
 * @see void Configure (const PlatformConfig &config)
 * @objective Test  registerResource() API and findResource() API (with resource type and with QOS) to check
 *      if foundResource callback is called
 * @target OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                       std::string& resourceURI,
 *                       const std::string& resourceTypeName,
 *                       const std::string& resourceInterface,
 *                       EntityHandler entityHandler,
 *                       uint8_t resourceProperty)
 * @target OCStackResult findResource(const std::string& host, const std::string& resourceURI,
 *                   OCConnectivityType connectivityType, FindCallback resourceHandler,QualityOfService QoS)
 * @test_data     1. OCResourceHandle pointer to the created resource
 *                2. resourceURI "/a/temperature"
 *                3. resourceTypeName "oic.r.temperature"
 *                4. resourceInterface DEFAULT_INTERFACE
 *                5. EntityHandler callback to be called
 *                6. resourceProperty indicates the property of the resource
 *                7. host Host IP Address of the resource
 *                8. resourceURI "/oic/res/?rt=oic.r.temperature"
 *                9. OCConnectivityType CT_DEFAULT
 *                10. FindCallback callback to be called when found resource
 *                11. QualityOfService HighQos
 * @pre_condition Configure platform for client server mode
 * @procedure     1. Perform registerResource() API
 *                2. Perform findResource() API with resource type in query and QOS
 *                3. Check if callback is called
 *                4. Check if temperature resource is found
 * @post_condition None
 * @expected Resource should be found successfully and callback should be called
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICppIntegrationTest_stc, CreateAndFindResourceWithQos_SQV_CV_P)
{
    try
    {
        m_temperatureHandle = m_RIHelper->registerResource(m_TemperatureUri, m_TemperatureType,
                m_ResourceInterface);
        ASSERT_NE(m_temperatureHandle,NULL) << "Register Resource failed";

        IOTIVITYTEST_LOG(INFO, "Finding Temperature Resource...");

        ASSERT_EQ(OC_STACK_OK,
                OCPlatform::findResource("", m_temperatureResourceQuery, CT_DEFAULT, std::bind(&RICppIntegrationTest_stc::foundResource, this, PH::_1), QualityOfService::HighQos))
        << "findResource does not return success";
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if(!m_foundResourceCheck)
        {
            SET_FAILURE("onFoundResource Callback is not Invoked!");
            return;
        }
        if(m_temperatureResource == nullptr)
        {
            SET_FAILURE("Temperature resource was not found");
        }
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occured. Exception is " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-08-04
 * @see void Configure (const PlatformConfig &config)
 * @objective Test  registerResource() API and findResource() API (with resource type and QOS) for two times to check
 *      if foundResource callback is called
 * @target OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                       std::string& resourceURI,
 *                       const std::string& resourceTypeName,
 *                       const std::string& resourceInterface,
 *                       EntityHandler entityHandler,
 *                       uint8_t resourceProperty)
 * @target OCStackResult findResource(const std::string& host, const std::string& resourceURI,
 *                   OCConnectivityType connectivityType, FindCallback resourceHandler,QualityOfService QoS)
 * @test_data     1. OCResourceHandle pointer to the created resource
 *                2. resourceURI "/a/temperature"
 *                3. resourceTypeName "oic.r.temperature"
 *                4. resourceInterface DEFAULT_INTERFACE
 *                5. EntityHandler callback to be called
 *                6. resourceProperty indicates the property of the resource
 *                7. host Host IP Address of the resource
 *                8. resourceURI "/oic/res/?rt=oic.r.temperature"
 *                9. OCConnectivityType CT_DEFAULT
 *                10. FindCallback callback to be called when found resource
 *                11. QualityOfService HighQos
 * @pre_condition Configure platform for client server mode
 * @procedure     1. Perform registerResource() API
 *                2. Perform findResource() API with resource type in query and QOS
 *                3. Check if callback is called
 *                4. Check if temperature resource is found
 *                5. Perform findResource() API again with resource type in query
 *                6. Check if callback is called
 *                7. Check if temperature resource is found
 * @post_condition None
 * @expected Resource should be found successfully and callback should be called for both times
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICppIntegrationTest_stc, CreateAndFindResourceWithQosTwice_VLCC_P)
{
    try
    {
        m_temperatureHandle = m_RIHelper->registerResource(m_TemperatureUri, m_TemperatureType,
                m_ResourceInterface);
        ASSERT_NE(m_temperatureHandle,NULL) << "Register Resource failed";

        IOTIVITYTEST_LOG(INFO, "Finding Resource with Qos...");
        ASSERT_EQ(OC_STACK_OK,
                OCPlatform::findResource("", m_temperatureResourceQuery, CT_DEFAULT, std::bind(&RICppIntegrationTest_stc::foundResource, this, PH::_1), QualityOfService::HighQos))
        << "findResource does not return success";
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if(!m_foundResourceCheck)
        {
            SET_FAILURE("onFoundResource Callback is not Invoked!");
            return;
        }
        if(m_temperatureResource == nullptr)
        {
            SET_FAILURE("Temperature resource was not found");
            return;
        }

        IOTIVITYTEST_LOG(INFO, "Finding Resource with Qos for second time...");
        ASSERT_EQ(OC_STACK_OK,
                OCPlatform::findResource("", m_temperatureResourceQuery, CT_DEFAULT, std::bind(&RICppIntegrationTest_stc::foundResource, this, PH::_1)))
        << "findResource does not return success";
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if(!m_foundResourceCheck)
        {
            SET_FAILURE("onFoundResource Callback is not Invoked!");
            return;
        }
        if(m_temperatureResource == nullptr)
        {
            SET_FAILURE("Temperature resource was not found");
        }

    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occured. Exception is " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-08-04
 * @see None
 * @objective Test registerDeviceInfo() API and getDeviceInfo() API to check if
 *      registered device information is get
 * @target OCStackResult registerDeviceInfo(const OCDeviceInfo deviceInfo)
 * @target OCStackResult getDeviceInfo(const std::string& host, const std::string& deviceURI,
 *                   OCConnectivityType connectivityType, FindDeviceCallback deviceInfoHandler)
 * @test_data   1. OCDeviceInfo structure containing all the device specific information
 *              2. host Host IP Address
 *              3. deviceURI "/oic/d"
 *              4. connectivityType CT_DEFAULT
 *              5. FindDeviceCallback callback to be called
 * @pre_condition Configure platform for client server mode
 * @procedure   1. Perform registerDeviceInfo() API
 *              2. Perform getDeviceInfo() API
 *              3. Check if onDeviceInfoReceived callback is called
 *              4. Check if registered information is received
 * @post_condition None
 * @expected Received device info should be matched with Set device info
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICppIntegrationTest_stc, RegisterAndGetDeviceInfo_GSRV_CV_P)
{
    try
    {
        OCDeviceInfo deviceInfo = m_RIHelper->getDeviceInfo();
        ASSERT_EQ(OC_STACK_OK,OCPlatform::registerDeviceInfo(deviceInfo)) << "registerDeviceInfo does not return success";

        std::ostringstream deviceDiscoveryRequest;
        deviceDiscoveryRequest << OC_MULTICAST_PREFIX << m_DeviceDiscoveryURI;
        IOTIVITYTEST_LOG(INFO, "Querying for device information...");
        ASSERT_EQ(OC_STACK_OK,
                OCPlatform::getDeviceInfo("", deviceDiscoveryRequest.str(), CT_DEFAULT, std::bind(&RICppIntegrationTest_stc::onDeviceInfoReceived, this, PH::_1)))
        << "getDeviceInfo does not return success";
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if(!m_deviceDiscoveryCheck)
        {
            SET_FAILURE("onDeviceInfoReceived Callback is not Invoked!");
            return;
        }
        if(!m_foundAllDeviceInfo)
        {
            SET_FAILURE("All the set device info does not match with received info");
        }
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occured. Exception is " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-08-04
 * @see None
 * @objective Test registerDeviceInfo() API and getDeviceInfo() API (with Qos) to check if
 *      registered device information is get
 * @target OCStackResult registerDeviceInfo(const OCDeviceInfo deviceInfo)
 * @target OCStackResult getDeviceInfo(const std::string& host, const std::string& deviceURI,
 *                   OCConnectivityType connectivityType, FindDeviceCallback deviceInfoHandler,
 *                   QualityOfService QoS)
 * @test_data   1. OCDeviceInfo structure containing all the device specific information
 *              2. host Host IP Address
 *              3. deviceURI "/oic/d"
 *              4. connectivityType CT_DEFAULT
 *              5. FindDeviceCallback callback to be called
 *              6. QualityOfService HighQos
 * @pre_condition Configure platform for client server mode
 * @procedure   1. Perform registerDeviceInfo() API
 *              2. Perform getDeviceInfo() API(with Qos)
 *              3. Check if onDeviceInfoReceived callback is called
 *              4. Check if registered information is received
 * @post_condition None
 * @expected Received device info should be matched with Set device info
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICppIntegrationTest_stc, RegisterAndGetDeviceInfoWithQos_GSRV_CV_P)
{
    try
    {
        OCDeviceInfo deviceInfo = m_RIHelper->getDeviceInfo();
        ASSERT_EQ(OC_STACK_OK,OCPlatform::registerDeviceInfo(deviceInfo)) << "registerDeviceInfo does not return success";

        std::ostringstream deviceDiscoveryRequest;
        deviceDiscoveryRequest << OC_MULTICAST_PREFIX << m_DeviceDiscoveryURI;
        IOTIVITYTEST_LOG(INFO, "Querying for device information...");
        ASSERT_EQ(OC_STACK_OK,
                OCPlatform::getDeviceInfo("", deviceDiscoveryRequest.str(), CT_DEFAULT, std::bind(&RICppIntegrationTest_stc::onDeviceInfoReceived, this, PH::_1),
                        QualityOfService::HighQos)) << "getDeviceInfo does not return success";
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if(!m_deviceDiscoveryCheck)
        {
            SET_FAILURE("onDeviceInfoReceived Callback is not Invoked!");
            return;
        }
        if(!m_foundAllDeviceInfo)
        {
            SET_FAILURE("All the set device info does not match with received info");
        }
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occured. Exception is " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-08-04
 * @see None
 * @objective Test registerPlatformInfo() API and getPlatformInfo() API to check if
 *      registered platform information is get
 * @target OCStackResult registerPlatformInfo(const OCPlatformInfo platformInfo)
 * @target OCStackResult getPlatformInfo(const std::string& host, const std::string& platformURI,
 *                   OCConnectivityType connectivityType, FindPlatformCallback platformInfoHandler)
 * @test_data   1. OCPlatformInfo structure containing all the platform specific information
 *              2. host Host IP Address
 *              3. platformURI "/oic/p"
 *              4. connectivityType CT_DEFAULT
 *              5. FindPlatformCallback callback to be called
 * @pre_condition Configure platform for client server mode
 * @procedure   1. Perform registerPlatformInfo() API
 *              2. Perform getPlatformInfo() API
 *              3. Check if onPlatformInfoReceived callback is called
 *              4. Check if registered information is received
 * @post_condition None
 * @expected Received platform info should be matched with Set platform info
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICppIntegrationTest_stc, RegisterAndGetPlatformInfo_GSRV_CV_P)
{
    try
    {
        ASSERT_EQ(OC_STACK_OK, OCPlatform::registerPlatformInfo(m_RIHelper->getPlatformInfo())) << "Platform Info is not registered";

        std::ostringstream platformDiscoveryRequest;
        platformDiscoveryRequest << OC_MULTICAST_PREFIX << m_PlatformDiscoveryURI;

        IOTIVITYTEST_LOG(INFO, "Querying for platform information...");
        ASSERT_EQ(OC_STACK_OK,
                OCPlatform::getPlatformInfo("", platformDiscoveryRequest.str(), CT_DEFAULT, std::bind(&RICppIntegrationTest_stc::onPlatformInfoReceived, this, PH::_1)
                        ,QualityOfService::HighQos)) << "getPlatformInfo does not return success";
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if(!m_PlatformDiscoveryCheck)
        {
            SET_FAILURE("onPlatformInfoReceived Callback is not Invoked!");
        }
        if(!m_foundAllPlatformInfo)
        {
            SET_FAILURE("All the set platform informations is not received correctly");
        }
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occured. Exception is " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-08-04
 * @see None
 * @objective Test registerPlatformInfo() API and getPlatformInfo() API(with Qos) to check if
 *      registered platform information is get
 * @target OCStackResult registerPlatformInfo(const OCPlatformInfo platformInfo)
 * @target OCStackResult getPlatformInfo(const std::string& host, const std::string& platformURI,
 *                   OCConnectivityType connectivityType, FindPlatformCallback platformInfoHandler,
 *                   QualityOfService QoS)
 * @test_data   1. OCPlatformInfo structure containing all the platform specific information
 *              2. host Host IP Address
 *              3. platformURI "/oic/p"
 *              4. connectivityType CT_DEFAULT
 *              5. FindPlatformCallback callback to be called
 *              6. QualityOfService HighQos
 * @pre_condition Configure platform for client server mode
 * @procedure   1. Perform registerPlatformInfo() API(with Qos)
 *              2. Perform getPlatformInfo() API
 *              3. Check if onPlatformInfoReceived callback is called
 *              4. Check if registered information is received
 * @post_condition None
 * @expected Received platform info should be matched with Set platform info
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICppIntegrationTest_stc, RegisterAndGetPlatformInfoWithQos_SQV_CV_P)
{
    try
    {
        ASSERT_EQ(OC_STACK_OK, OCPlatform::registerPlatformInfo(m_RIHelper->getPlatformInfo())) << "Platform Info is not registered";

        std::ostringstream platformDiscoveryRequest;
        platformDiscoveryRequest << OC_MULTICAST_PREFIX << m_PlatformDiscoveryURI;

        IOTIVITYTEST_LOG(INFO, "Querying for platform information...");
        ASSERT_EQ(OC_STACK_OK,
                OCPlatform::getPlatformInfo("", platformDiscoveryRequest.str(), CT_DEFAULT, std::bind(&RICppIntegrationTest_stc::onPlatformInfoReceived, this, PH::_1)
                        ,QualityOfService::HighQos)) << "getPlatformInfo does not return success";
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if(!m_PlatformDiscoveryCheck)
        {
            SET_FAILURE("onPlatformInfoReceived Callback is not Invoked!");
        }
        if(!m_foundAllPlatformInfo)
        {
            SET_FAILURE("All the set platform informations is not received correctly");
        }
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occured. Exception is " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-08-04
 * @see void Configure (const PlatformConfig &config)
 * @see OCStackResult findResource(const std::string& host, const std::string& resourceURI,
 *                   OCConnectivityType connectivityType, FindCallback resourceHandler)
 * @objective Test registerResource() API and get() API positively to check
 *      if get request is done successfully
 * @target OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                       std::string& resourceURI,
 *                       const std::string& resourceTypeName,
 *                       const std::string& resourceInterface,
 *                       EntityHandler entityHandler,
 *                       uint8_t resourceProperty)
 * @target OCStackResult get(const QueryParamsMap& queryParametersMap, GetCallback attributeHandler)
 * @test_data     1. OCResourceHandle pointer to the created resource
 *                2. resourceURI "/a/temperature"
 *                3. resourceTypeName "oic.r.temperature"
 *                4. resourceInterface DEFAULT_INTERFACE
 *                5. EntityHandler callback to be called
 *                6. resourceProperty indicates the property of the resource
 *                7. queryParametersMap map which can have the query parameter name and value
 *                8. GetCallback callback to be called
 * @pre_condition Configure platform for client server mode
 * @procedure     1. Perform registerResource() API
 *                2. Perform findResource() API with resource type in query
 *                3. Check if callback is called
 *                4. Check if temperature resource is found
 *                5. Perform get() API on the found temperature resource
 *                6. Check if server can get the get request and send response correctly
 *                7. Check if client's onGet callback is called and resource attributes are found correctly in response
 * @post_condition None
 * @expected onGet callback is called and response is found correctly
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICppIntegrationTest_stc, CreateResourceAndSendGetRequest_SQV_CV_P)
{
    try
    {
        m_temperatureHandle = m_RIHelper->registerResource(m_TemperatureUri, m_TemperatureType,
                m_ResourceInterface);
        ASSERT_NE(m_temperatureHandle,NULL) << "Register Resource failed";

        IOTIVITYTEST_LOG(INFO, "Finding Temperature Resource...");
        ASSERT_EQ(OC_STACK_OK,
                OCPlatform::findResource("", m_temperatureResourceQuery, CT_DEFAULT, std::bind(&RICppIntegrationTest_stc::foundResource, this, PH::_1)))
        << "findResource does not return success";
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if(!m_foundResourceCheck)
        {
            SET_FAILURE("onFoundResource Callback is not Invoked!");
        }
        if(m_temperatureResource)
        {
            QueryParamsMap test;
            ASSERT_EQ(OC_STACK_OK, m_temperatureResource->get(test, std::bind(&RICppIntegrationTest_stc::onGet, this, PH::_1, PH::_2, PH::_3)))
            << "Get does not return success";
            CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        }
        else
        {
            SET_FAILURE("Temperature resource was not found");
            return;
        }
        if(!m_RIHelper->isServerOk)
        {
            SET_FAILURE(m_RIHelper->getFailure());
            return;
        }
        if(!isGetCorrect)
        {
            SET_FAILURE("onGet Callback values are not correct");
        }
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occured. Exception is " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-08-04
 * @see void Configure (const PlatformConfig &config)
 * @see OCStackResult findResource(const std::string& host, const std::string& resourceURI,
 *                   OCConnectivityType connectivityType, FindCallback resourceHandler)
 * @objective Test registerResource() API and get() API (with Qos) positively to check
 *      if get request is done successfully
 * @target OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                       std::string& resourceURI,
 *                       const std::string& resourceTypeName,
 *                       const std::string& resourceInterface,
 *                       EntityHandler entityHandler,
 *                       uint8_t resourceProperty)
 * @target OCStackResult get(const QueryParamsMap& queryParametersMap, GetCallback attributeHandler)
 * @test_data     1. OCResourceHandle pointer to the created resource
 *                2. resourceURI "/a/temperature"
 *                3. resourceTypeName "oic.r.temperature"
 *                4. resourceInterface DEFAULT_INTERFACE
 *                5. EntityHandler callback to be called
 *                6. resourceProperty indicates the property of the resource
 *                7. queryParametersMap map which can have the query parameter name and value
 *                8. GetCallback callback to be called
 * @pre_condition Configure platform for client server mode
 * @procedure     1. Perform registerResource() API
 *                2. Perform findResource() API with resource type in query
 *                3. Check if callback is called
 *                4. Check if temperature resource is found
 *                5. Perform get() API(with Qos) on the found temperature resource
 *                6. Check if server can get the get request and send response correctly
 *                7. Check if client's onGet callback is called and resource attributes are found correctly in response
 * @post_condition None
 * @expected onGet callback is called and response is found correctly
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICppIntegrationTest_stc, CreateResourceAndSendGetRequestWithQos_SQV_CV_P)
{
    try
    {
        m_temperatureHandle = m_RIHelper->registerResource(m_TemperatureUri, m_TemperatureType,
                m_ResourceInterface);
        ASSERT_NE(m_temperatureHandle,NULL) << "Register Resource failed";

        IOTIVITYTEST_LOG(INFO, "Finding Temperature Resource...");
        ASSERT_EQ(OC_STACK_OK,
                OCPlatform::findResource("", m_temperatureResourceQuery, CT_DEFAULT, std::bind(&RICppIntegrationTest_stc::foundResource, this, PH::_1)))
        << "findResource does not return success";
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if(!m_foundResourceCheck)
        {
            SET_FAILURE("onFoundResource Callback is not Invoked!");
        }
        if(m_temperatureResource)
        {
            QueryParamsMap test;
            ASSERT_EQ(OC_STACK_OK, m_temperatureResource->get(test, std::bind(&RICppIntegrationTest_stc::onGet, this, PH::_1, PH::_2, PH::_3)
                            ,QualityOfService::HighQos))
            << "Get does not return success";
            CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        }
        else
        {
            SET_FAILURE("Temperature resource was not found");
            return;
        }
        if(!m_RIHelper->isServerOk)
        {
            SET_FAILURE(m_RIHelper->getFailure());
            return;
        }
        if(!isGetCorrect)
        {
            SET_FAILURE("onGet Callback values are not correct");
        }
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occured. Exception is " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-08-04
 * @see void Configure (const PlatformConfig &config)
 * @see OCStackResult findResource(const std::string& host, const std::string& resourceURI,
 *                   OCConnectivityType connectivityType, FindCallback resourceHandler)
 * @objective Test registerResource() API and put() API positively to check
 *      if put request is done successfully
 * @target OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                       std::string& resourceURI,
 *                       const std::string& resourceTypeName,
 *                       const std::string& resourceInterface,
 *                       EntityHandler entityHandler,
 *                       uint8_t resourceProperty)
 * @target OCStackResult put(const OCRepresentation& representation,
 *                        const QueryParamsMap& queryParametersMap, PutCallback attributeHandler)
 * @test_data     1. OCResourceHandle pointer to the created resource
 *                2. resourceURI "/a/temperature"
 *                3. resourceTypeName "oic.r.temperature"
 *                4. resourceInterface DEFAULT_INTERFACE
 *                5. EntityHandler callback to be called
 *                6. resourceProperty indicates the property of the resource
 *                7. representation representation with attribute's names and value
 *                8. queryParametersMap map which can have the query parameter name and value
 *                9. PutCallback callback to be called
 * @pre_condition Configure platform for client server mode
 * @procedure     1. Perform registerResource() API
 *                2. Perform findResource() API with resource type in query
 *                3. Check if callback is called
 *                4. Check if temperature resource is found
 *                5. Perform put() API on the found temperature resource
 *                6. Check if server can get the put request and send response correctly
 *                7. Check if client's onPut callback is called and resource attributes are found correctly in response
 * @post_condition None
 * @expected onPut callback is called and response is found correctly
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICppIntegrationTest_stc, CreateResourceAndSendPutRequest_SQV_CV_P)
{
    try
    {
        m_temperatureHandle = m_RIHelper->registerResource(m_TemperatureUri, m_TemperatureType,
                m_ResourceInterface);
        ASSERT_NE(m_temperatureHandle,NULL) << "Register Resource failed";

        IOTIVITYTEST_LOG(INFO, "Finding Temperature Resource...");
        ASSERT_EQ(OC_STACK_OK,
                OCPlatform::findResource("", m_temperatureResourceQuery, CT_DEFAULT, std::bind(&RICppIntegrationTest_stc::foundResource, this, PH::_1)))
        << "findResource does not return success";
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if(!m_foundResourceCheck)
        {
            SET_FAILURE("onFoundResource Callback is not Invoked!");
        }
        if(m_temperatureResource)
        {
            QueryParamsMap test;
            OCRepresentation rep;
            rep.setValue("temperature",m_temp);
            rep.setValue("hour",m_hour);

            ASSERT_EQ(OC_STACK_OK, m_temperatureResource->put(rep,test,std::bind(&RICppIntegrationTest_stc::onPut, this, PH::_1, PH::_2, PH::_3)))
            << "Put does not return success";
            CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        }
        else
        {
            SET_FAILURE("Temperature resource was not found");
            return;
        }
        if(!m_RIHelper->isServerOk)
        {
            SET_FAILURE(m_RIHelper->getFailure());
            return;
        }
        if(!isPutCorrect)
        {
            SET_FAILURE("onPut Callback values are not correct");
        }
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occured. Exception is " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-08-04
 * @see void Configure (const PlatformConfig &config)
 * @see OCStackResult findResource(const std::string& host, const std::string& resourceURI,
 *                   OCConnectivityType connectivityType, FindCallback resourceHandler)
 * @objective Test registerResource() API and put() API(with Qos) positively to check
 *      if put request is done successfully
 * @target OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                       std::string& resourceURI,
 *                       const std::string& resourceTypeName,
 *                       const std::string& resourceInterface,
 *                       EntityHandler entityHandler,
 *                       uint8_t resourceProperty)
 * @target OCStackResult put(const OCRepresentation& representation,
 *                        const QueryParamsMap& queryParametersMap, PutCallback attributeHandler)
 * @test_data     1. OCResourceHandle pointer to the created resource
 *                2. resourceURI "/a/temperature"
 *                3. resourceTypeName "oic.r.temperature"
 *                4. resourceInterface DEFAULT_INTERFACE
 *                5. EntityHandler callback to be called
 *                6. resourceProperty indicates the property of the resource
 *                7. representation representation with attribute's names and value
 *                8. queryParametersMap map which can have the query parameter name and value
 *                9. PutCallback callback to be called
 * @pre_condition Configure platform for client server mode
 * @procedure     1. Perform registerResource() API
 *                2. Perform findResource() API with resource type in query
 *                3. Check if callback is called
 *                4. Check if temperature resource is found
 *                5. Perform put() API(with Qos) on the found temperature resource
 *                6. Check if server can get the put request and send response correctly
 *                7. Check if client's onPut callback is called and resource attributes are found correctly in response
 * @post_condition None
 * @expected onPut callback is called and response is found correctly
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICppIntegrationTest_stc, CreateResourceAndSendPutRequestWithQos_SQV_CV_P)
{
    try
    {
        m_temperatureHandle = m_RIHelper->registerResource(m_TemperatureUri, m_TemperatureType,
                m_ResourceInterface);
        ASSERT_NE(m_temperatureHandle,NULL) << "Register Resource failed";

        IOTIVITYTEST_LOG(INFO, "Finding Temperature Resource...");
        ASSERT_EQ(OC_STACK_OK,
                OCPlatform::findResource("", m_temperatureResourceQuery, CT_DEFAULT, std::bind(&RICppIntegrationTest_stc::foundResource, this, PH::_1)))
        << "findResource does not return success";
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if(!m_foundResourceCheck)
        {
            SET_FAILURE("onFoundResource Callback is not Invoked!");
        }
        if(m_temperatureResource)
        {
            QueryParamsMap test;
            OCRepresentation rep;
            rep.setValue("temperature",m_temp);
            rep.setValue("hour",m_hour);

            ASSERT_EQ(OC_STACK_OK, m_temperatureResource->put(rep,test,std::bind(&RICppIntegrationTest_stc::onPut, this, PH::_1, PH::_2, PH::_3)
                            ,QualityOfService::HighQos))
            << "Put does not return success";
            CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        }
        else
        {
            SET_FAILURE("Temperature resource was not found");
            return;
        }
        if(!m_RIHelper->isServerOk)
        {
            SET_FAILURE(m_RIHelper->getFailure());
            return;
        }
        if(!isPutCorrect)
        {
            SET_FAILURE("onPut Callback values are not correct");
        }
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occured. Exception is " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-08-04
 * @see void Configure (const PlatformConfig &config)
 * @see OCStackResult findResource(const std::string& host, const std::string& resourceURI,
 *                   OCConnectivityType connectivityType, FindCallback resourceHandler)
 * @objective Test registerResource() API and post() API positively to check
 *      if post request is done successfully
 * @target OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                       std::string& resourceURI,
 *                       const std::string& resourceTypeName,
 *                       const std::string& resourceInterface,
 *                       EntityHandler entityHandler,
 *                       uint8_t resourceProperty)
 * @target OCStackResult post(const OCRepresentation& representation,
 *                        const QueryParamsMap& queryParametersMap, PostCallback attributeHandler)
 * @test_data     1. OCResourceHandle pointer to the created resource
 *                2. resourceURI "/a/temperature"
 *                3. resourceTypeName "oic.r.temperature"
 *                4. resourceInterface DEFAULT_INTERFACE
 *                5. EntityHandler callback to be called
 *                6. resourceProperty indicates the property of the resource
 *                7. representation representation with attribute's names and value
 *                8. queryParametersMap map which can have the query parameter name and value
 *                9. PostCallback callback to be called
 * @pre_condition Configure platform for client server mode
 * @procedure     1. Perform registerResource() API
 *                2. Perform findResource() API with resource type in query
 *                3. Check if callback is called
 *                4. Check if temperature resource is found
 *                5. Perform post() API on the found temperature resource
 *                6. Check if server can get the post request and send response correctly
 *                7. Check if client's onPost callback is called and resource attributes are found correctly in response
 * @post_condition None
 * @expected onPost callback is called and response is found correctly
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICppIntegrationTest_stc, CreateResourceAndSendPostRequest_SQV_CV_P)
{
    try
    {
        m_temperatureHandle = m_RIHelper->registerResource(m_TemperatureUri, m_TemperatureType,
                m_ResourceInterface);
        ASSERT_NE(m_temperatureHandle,NULL) << "Register Resource failed";

        IOTIVITYTEST_LOG(INFO, "Finding Temperature Resource...");
        ASSERT_EQ(OC_STACK_OK,
                OCPlatform::findResource("", m_temperatureResourceQuery, CT_DEFAULT, std::bind(&RICppIntegrationTest_stc::foundResource, this, PH::_1)))
        << "findResource does not return success";
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if(!m_foundResourceCheck)
        {
            SET_FAILURE("onFoundResource Callback is not Invoked!");
        }
        if(m_temperatureResource)
        {
            QueryParamsMap test;
            OCRepresentation rep;
            rep.setValue("temperature",m_temp);
            rep.setValue("hour",m_hour);

            ASSERT_EQ(OC_STACK_OK, m_temperatureResource->post(rep,test,std::bind(&RICppIntegrationTest_stc::onPost, this, PH::_1, PH::_2, PH::_3)))
            << "Post does not return success";
            CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        }
        else
        {
            SET_FAILURE("Temperature resource was not found");
            return;
        }
        if(!m_RIHelper->isServerOk)
        {
            SET_FAILURE(m_RIHelper->getFailure());
            return;
        }
        if(!isPostCorrect)
        {
            SET_FAILURE("onPost Callback values are not correct");
        }
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occured. Exception is " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-08-04
 * @see void Configure (const PlatformConfig &config)
 * @see OCStackResult findResource(const std::string& host, const std::string& resourceURI,
 *                   OCConnectivityType connectivityType, FindCallback resourceHandler)
 * @objective Test registerResource() API and post() API(with Qos) positively to check
 *      if post request is done successfully
 * @target OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                       std::string& resourceURI,
 *                       const std::string& resourceTypeName,
 *                       const std::string& resourceInterface,
 *                       EntityHandler entityHandler,
 *                       uint8_t resourceProperty)
 * @target OCStackResult post(const OCRepresentation& representation,
 *                        const QueryParamsMap& queryParametersMap, PostCallback attributeHandler)
 * @test_data     1. OCResourceHandle pointer to the created resource
 *                2. resourceURI "/a/temperature"
 *                3. resourceTypeName "oic.r.temperature"
 *                4. resourceInterface DEFAULT_INTERFACE
 *                5. EntityHandler callback to be called
 *                6. resourceProperty indicates the property of the resource
 *                7. representation representation with attribute's names and value
 *                8. queryParametersMap map which can have the query parameter name and value
 *                9. PostCallback callback to be called
 * @pre_condition Configure platform for client server mode
 * @procedure     1. Perform registerResource() API
 *                2. Perform findResource() API with resource type in query
 *                3. Check if callback is called
 *                4. Check if temperature resource is found
 *                5. Perform post() API(with Qos) on the found temperature resource
 *                6. Check if server can get the post request and send response correctly
 *                7. Check if client's onPost callback is called and resource attributes are found correctly in response
 * @post_condition None
 * @expected onPost callback is called and response is found correctly
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICppIntegrationTest_stc, CreateResourceAndSendPostRequestWithQos_SQV_CV_P)
{
    try
    {
        m_temperatureHandle = m_RIHelper->registerResource(m_TemperatureUri, m_TemperatureType,
                m_ResourceInterface);
        ASSERT_NE(m_temperatureHandle,NULL) << "Register Resource failed";

        IOTIVITYTEST_LOG(INFO, "Finding Temperature Resource...");
        ASSERT_EQ(OC_STACK_OK,
                OCPlatform::findResource("", m_temperatureResourceQuery, CT_DEFAULT, std::bind(&RICppIntegrationTest_stc::foundResource, this, PH::_1)))
        << "findResource does not return success";
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if(!m_foundResourceCheck)
        {
            SET_FAILURE("onFoundResource Callback is not Invoked!");
        }
        if(m_temperatureResource)
        {
            QueryParamsMap test;
            OCRepresentation rep;
            rep.setValue("temperature",m_temp);
            rep.setValue("hour",m_hour);

            ASSERT_EQ(OC_STACK_OK, m_temperatureResource->post(rep,test,std::bind(&RICppIntegrationTest_stc::onPost, this, PH::_1, PH::_2, PH::_3)
                            ,QualityOfService::HighQos))
            << "Post does not return success";
            CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        }
        else
        {
            SET_FAILURE("Temperature resource was not found");
            return;
        }
        if(!m_RIHelper->isServerOk)
        {
            SET_FAILURE(m_RIHelper->getFailure());
            return;
        }
        if(!isPostCorrect)
        {
            SET_FAILURE("onPost Callback values are not correct");
        }
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occured. Exception is " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-08-04
 * @see void Configure (const PlatformConfig &config)
 * @see OCStackResult findResource(const std::string& host, const std::string& resourceURI,
 *                   OCConnectivityType connectivityType, FindCallback resourceHandler)
 * @objective Test registerResource() API and delete() API positively to check
 *      if delete request is done successfully
 * @target OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                       std::string& resourceURI,
 *                       const std::string& resourceTypeName,
 *                       const std::string& resourceInterface,
 *                       EntityHandler entityHandler,
 *                       uint8_t resourceProperty)
 * @target OCStackResult deleteResource(DeleteCallback deleteHandler)
 * @target OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @test_data     1. OCResourceHandle pointer to the created resource
 *                2. resourceURI "/a/temperature"
 *                3. resourceTypeName "oic.r.temperature"
 *                4. resourceInterface DEFAULT_INTERFACE
 *                5. EntityHandler callback to be called
 *                6. resourceProperty indicates the property of the resource
 *                7. DeleteCallback callback to be called
 * @pre_condition Configure platform for client server mode
 * @procedure     1. Perform registerResource() API
 *                2. Perform findResource() API with resource type in query
 *                3. Check if callback is called
 *                4. Check if temperature resource is found
 *                5. Perform delete() API on the found temperature resource
 *                6. Check if server can get the delete request and send response correctly
 *                7. Call unregisterResource() API inside server's entity handler
 *                8. Check if client's onDelete callback is called
 *                9. Call findResource() API again with resource type in query
 *                10. Check if resource is not found now(after delete operation)
 * @post_condition None
 * @expected onDelete callback is called and response is found correctly and resource is not found after delete operation
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICppIntegrationTest_stc, CreateResourceAndSendDeleteRequest_SQV_CV_P)
{
    try
    {
        m_temperatureHandle = m_RIHelper->registerResource(m_TemperatureUri, m_TemperatureType,
                m_ResourceInterface);
        ASSERT_NE(m_temperatureHandle,NULL) << "Register Resource failed";

        IOTIVITYTEST_LOG(INFO, "Finding Temperature Resource...");
        ASSERT_EQ(OC_STACK_OK,
                OCPlatform::findResource("", m_temperatureResourceQuery, CT_DEFAULT, std::bind(&RICppIntegrationTest_stc::foundResource, this, PH::_1)))
        << "findResource does not return success";
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if(!m_foundResourceCheck)
        {
            SET_FAILURE("onFoundResource Callback is not Invoked!");
        }
        if(m_temperatureResource)
        {
            ASSERT_EQ(OC_STACK_OK, m_temperatureResource->deleteResource(std::bind(&RICppIntegrationTest_stc::onDelete, this, PH::_1, PH::_2)))
            << "Delete does not return success";
            CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        }
        else
        {
            SET_FAILURE("Temperature resource was not found");
            return;
        }
        if(!m_RIHelper->isServerOk)
        {
            SET_FAILURE(m_RIHelper->getFailure());
            return;
        }
        if(!isDeleteCorrect)
        {
            SET_FAILURE("onDelete Callback values are not correct");
            return;
        }

        m_temperatureResource = nullptr;

        IOTIVITYTEST_LOG(INFO, "Trying to Find Resource Again...");
        ASSERT_EQ(OC_STACK_OK,
                OCPlatform::findResource("", m_temperatureResourceQuery, CT_DEFAULT, std::bind(&RICppIntegrationTest_stc::foundResource, this, PH::_1)));
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if(m_temperatureResource)
        {
            SET_FAILURE("Resource should not be found after delete operation");
        }
        else
        {
            IOTIVITYTEST_LOG(INFO, "Resource not found as it was deleted");
        }
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occured. Exception is " + std::string(e.what()));
    }
}
#endif

/**
 * @since 2016-08-04
 * @see void Configure (const PlatformConfig &config)
 * @see OCStackResult findResource(const std::string& host, const std::string& resourceURI,
 *                   OCConnectivityType connectivityType, FindCallback resourceHandler)
 * @objective Test registerResource() API and delete() API(with Qos) positively to check
 *      if delete request is done successfully
 * @target OCStackResult registerResource(OCResourceHandle& resourceHandle,
 *                       std::string& resourceURI,
 *                       const std::string& resourceTypeName,
 *                       const std::string& resourceInterface,
 *                       EntityHandler entityHandler,
 *                       uint8_t resourceProperty)
 * @target OCStackResult deleteResource(DeleteCallback deleteHandler)
 * @target OCStackResult unregisterResource(const OCResourceHandle& resourceHandle)
 * @test_data     1. OCResourceHandle pointer to the created resource
 *                2. resourceURI "/a/temperature"
 *                3. resourceTypeName "oic.r.temperature"
 *                4. resourceInterface DEFAULT_INTERFACE
 *                5. EntityHandler callback to be called
 *                6. resourceProperty indicates the property of the resource
 *                7. DeleteCallback callback to be called
 * @pre_condition Configure platform for client server mode
 * @procedure     1. Perform registerResource() API
 *                2. Perform findResource() API with resource type in query
 *                3. Check if callback is called
 *                4. Check if temperature resource is found
 *                5. Perform delete() API(with Qos) on the found temperature resource
 *                6. Check if server can get the delete request and send response correctly
 *                7. Call unregisterResource() API inside server's entity handler
 *                8. Check if client's onDelete callback is called
 *                9. Call findResource() API again with resource type in query
 *                10. Check if resource is not found now(after delete operation)
 * @post_condition None
 * @expected onDelete callback is called and response is found correctly and resource is not found after delete operation
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(RICppIntegrationTest_stc, CreateResourceAndSendDeleteRequestWithQos_SQV_CV_P)
{
    try
    {
        m_temperatureHandle = m_RIHelper->registerResource(m_TemperatureUri, m_TemperatureType,
                m_ResourceInterface);
        ASSERT_NE(m_temperatureHandle,NULL) << "Register Resource failed";

        IOTIVITYTEST_LOG(INFO, "Finding Temperature Resource...");
        ASSERT_EQ(OC_STACK_OK,
                OCPlatform::findResource("", m_temperatureResourceQuery, CT_DEFAULT, std::bind(&RICppIntegrationTest_stc::foundResource, this, PH::_1)))
        << "findResource does not return success";
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if(!m_foundResourceCheck)
        {
            SET_FAILURE("onFoundResource Callback is not Invoked!");
        }
        if(m_temperatureResource)
        {
            ASSERT_EQ(OC_STACK_OK, m_temperatureResource->deleteResource(std::bind(&RICppIntegrationTest_stc::onDelete, this, PH::_1, PH::_2)
                            ,QualityOfService::HighQos))
            << "Delete does not return success";
            CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        }
        else
        {
            SET_FAILURE("Temperature resource was not found");
            return;
        }
        if(!m_RIHelper->isServerOk)
        {
            SET_FAILURE(m_RIHelper->getFailure());
            return;
        }
        if(!isDeleteCorrect)
        {
            SET_FAILURE("onDelete Callback values are not correct");
            return;
        }

        m_temperatureResource = nullptr;

        IOTIVITYTEST_LOG(INFO, "Trying to Find Resource Again...");
        ASSERT_EQ(OC_STACK_OK,
                OCPlatform::findResource("", m_temperatureResourceQuery, CT_DEFAULT, std::bind(&RICppIntegrationTest_stc::foundResource, this, PH::_1)))
        << "findResource does not return success";
        CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);
        if(m_temperatureResource)
        {
            SET_FAILURE("Resource should not be found after delete operation");
        }
        else
        {
            IOTIVITYTEST_LOG(INFO, "Resource not found as it was deleted");
        }
    }
    catch (exception &e)
    {
        SET_FAILURE("Exception occured. Exception is " + std::string(e.what()));
    }
}
#endif
