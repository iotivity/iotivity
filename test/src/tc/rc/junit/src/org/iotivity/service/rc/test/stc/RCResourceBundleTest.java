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
package org.iotivity.service.rc.test.stc;

import java.util.Map;

import org.iotivity.service.rc.test.helper.RCHelper;
import static org.iotivity.service.rc.test.helper.RCCommonUtil.*;

import org.iotivity.service.resourcecontainer.*;

import android.content.Context;
import android.test.InstrumentationTestCase;
import android.util.Log;

public class RCResourceBundleTest extends InstrumentationTestCase {
	private RCHelper m_RCHelper;
	private RcsResourceContainer m_ResourceContainer;
	private static boolean isCopied = false;
	private Map<String, String> params;
	private Context m_Context;
	private String configFile;
	private String emptyConfigFile;
	private String sdCardPath;

	protected void setUp() throws Exception {
		m_Context = getInstrumentation().getTargetContext();
		m_RCHelper = RCHelper.getInstance();

		isCopied = m_RCHelper.copyFilesToDevice(m_Context);

		sdCardPath = m_RCHelper.getFilesPath(m_Context) + PATH_INDICATOR;

		configFile = sdCardPath + RESOURCE_CONTAINER_CONFIG_XML;
		emptyConfigFile = sdCardPath + RESOURCE_CONTAINER_EMPTY_CONFIG_XML;

		if (!isCopied) {
			Log.d(TAG, "Can't copy the files from assets/lib folder");
		} else {
			Log.d(TAG, "Successfully copied files from assets/lib folder");
		}

		m_ResourceContainer = new RcsResourceContainer(m_Context);
	}

	/**
	 * @since 2015-09-22
	 * @see StartContainer
	 * @see AddBundle
	 * @objective Test addBundle & startBundle function with hue bundle
	 * @target void addBundle(string bundleId, string bundleUri, string
	 *         bundlePath, std::map<string, string> params)
	 * @target void startBundle(string bundleId)
	 * @test_data ResourceContainerConfig
	 * @pre_condition 1. Get container instance 2. start container
	 * @procedure Call startBundle API
	 * @post_condition Stop container
	 * @expected Container can start hue bundle.
	 **/
	public void testStartHueBundleFromEmptyContainer_SRC_P() {

		m_ResourceContainer.startContainer(emptyConfigFile);
		m_ResourceContainer.addBundle(HUE_BUNDLE_ID, EMPTY_STRING, sdCardPath
				+ HUE_BUNDLE_JAR, HUE_BUNDLE_NAME, params);

		if (!m_RCHelper.isBundleInAddedList(m_ResourceContainer, HUE_BUNDLE_ID)) {
			fail("Add hue bundle failed for empty bundle");
			return;
		}

		if (!m_RCHelper.startBundle(m_ResourceContainer, HUE_BUNDLE_ID))
			fail("Start HueBundle failed.");

		m_ResourceContainer.stopContainer();

	}

	/**
	 * @since 2015-09-22
	 * @see StartContainer
	 * @see AddBundle
	 * @objective Test addBundle & startBundle function with di bundle
	 * @target void addBundle(string bundleId, string bundleUri, string
	 *         bundlePath, std::map<string, string> params)
	 * @target void startBundle(string bundleId)
	 * @test_data ResourceContainerConfig
	 * @pre_condition 1. Get container instance 2. start container
	 * @procedure Call startBundle API
	 * @post_condition Stop container
	 * @expected Container can start di bundle.
	 **/
	public void testStartDIBundleFromEmptyContainer_SRC_P() {

		m_ResourceContainer.startContainer(emptyConfigFile);
		m_ResourceContainer.addBundle(DI_BUNDLE_ID, EMPTY_STRING, sdCardPath
				+ DI_SENSOR_SO_NAME, DI_SENSOR_NAME, params);

		if (!m_RCHelper.isBundleInAddedList(m_ResourceContainer, DI_BUNDLE_ID)) {
			fail("Add di bundle failed for empty bundle");
		}

		if (!m_RCHelper.startBundle(m_ResourceContainer, DI_BUNDLE_ID))
			fail("start di bundle failed for empty bundle");

		m_ResourceContainer.stopContainer();

	}

	/**
	 * @since 2015-09-22
	 * @see StartContainer
	 * @see AddBundle
	 * @objective Test addBundle & startBundle function with bmi bundle
	 * @target void addBundle(string bundleId, string bundleUri, string
	 *         bundlePath, std::map<string, string> params)
	 * @target void startBundle(string bundleId)
	 * @test_data ResourceContainerConfig
	 * @pre_condition 1. Get container instance 2. start container
	 * @procedure Call startBundle API
	 * @post_condition Stop container
	 * @expected Container can start bmi bundle.
	 **/
	public void testStartBMIBundleFromEmptyContainer_SRC_P() {

		m_ResourceContainer.startContainer(emptyConfigFile);
		m_ResourceContainer.addBundle(BMI_BUNDLE_ID, EMPTY_STRING, sdCardPath
				+ BMI_SO_NAME, BMI_NAME, params);

		if (!m_RCHelper.isBundleInAddedList(m_ResourceContainer, BMI_BUNDLE_ID)) {
			fail("Add bmi bundle failed for empty bundle");
			return;
		}

		if (!m_RCHelper.startBundle(m_ResourceContainer, BMI_BUNDLE_ID))
			fail("start bmi bundle failed for empty bundle");

		m_ResourceContainer.stopContainer();

	}

	/**
	 * @since 2015-09-22
	 * @see StartContainer
	 * @see Add Bundle
	 * @see StartBundle
	 * @objective Test addBundle, startBundle & stopBundle function with hue
	 *            bundle
	 * @target void addBundle(string bundleId, string bundleUri, string
	 *         bundlePath, std::map<string, string> params)
	 * @target void startBundle(string bundleId)
	 * @target void stopBundle(string bundleId)
	 * @test_data ResourceContainerConfig
	 * @pre_condition 1. Get container instance 2. start container
	 * @procedure Call stopBundle API
	 * @post_condition StopContainer
	 * @expected Container can stop hue bundle.
	 **/
	public void testStopHueBundle_SRC_P() {

		m_ResourceContainer.startContainer(configFile);

		m_ResourceContainer.addBundle(HUE_BUNDLE_ID, HUE_BUNDLE_URI, sdCardPath
				+ HUE_BUNDLE_JAR, HUE_BUNDLE_ACTIVATOR, params);

		if (!m_RCHelper.isBundleInAddedList(m_ResourceContainer, HUE_BUNDLE_ID)) {
			fail("Hue bundle is not added");
			return;
		}

		if (!m_RCHelper.startBundle(m_ResourceContainer, HUE_BUNDLE_ID)) {
			fail("Hue is not started");
			return;
		}

		if (!m_RCHelper.stopBundle(m_ResourceContainer, HUE_BUNDLE_ID))
			fail("Hue is not stopped");

		m_ResourceContainer.stopContainer();

	}

	/**
	 * @since 2015-09-22
	 * @see StartContainer
	 * @see Add Bundle
	 * @see StartBundle
	 * @objective Test addBundle, startBundle & stopBundle function with di
	 *            bundle
	 * @target void addBundle(string bundleId, string bundleUri, string
	 *         bundlePath, std::map<string, string> params)
	 * @target void startBundle(string bundleId)
	 * @target void stopBundle(string bundleId)
	 * @test_data ResourceContainerConfig
	 * @pre_condition 1. Get container instance 2. start container
	 * @procedure Call stopBundle API
	 * @post_condition none
	 * @expected Container can stom di bundle.
	 **/
	public void testStopDIBundle_SRC_P() {

		m_ResourceContainer.startContainer(configFile);

		if (!m_RCHelper.isBundleInAddedList(m_ResourceContainer, DI_BUNDLE_ID)) {
			fail("DI is not added");
			return;
		}

		if (!m_RCHelper.startBundle(m_ResourceContainer, DI_BUNDLE_ID)) {
			fail("DI is not started");
			return;
		}

		if (!m_RCHelper.stopBundle(m_ResourceContainer, DI_BUNDLE_ID))
			fail("DI is not stopped");

		m_ResourceContainer.stopContainer();

	}

	/**
	 * @since 2015-09-22
	 * @see StartContainer
	 * @see Add Bundle
	 * @see StartBundle
	 * @objective Test addBundle, startBundle & stopBundle function with bmi
	 *            bundle
	 * @target void addBundle(string bundleId, string bundleUri, string
	 *         bundlePath, std::map<string, string> params)
	 * @target void startBundle(string bundleId)
	 * @target void stopBundle(string bundleId)
	 * @test_data ResourceContainerConfig
	 * @pre_condition 1. Get container instance 2. start container
	 * @procedure Call stopBundle API
	 * @post_condition StopContainer
	 * @expected Container can stop bmi bundle.
	 **/
	public void testStopBMIBundle_SRC_P() {

		m_ResourceContainer.startContainer(configFile);

		if (!m_RCHelper.isBundleInAddedList(m_ResourceContainer, BMI_BUNDLE_ID)) {
			fail("BMI is not started");
			return;
		}

		if (!m_RCHelper.startBundle(m_ResourceContainer, BMI_BUNDLE_ID)) {
			fail("BMI is not started");
			return;
		}

		if (!m_RCHelper.stopBundle(m_ResourceContainer, BMI_BUNDLE_ID))
			fail("BMI is not stopped");

		m_ResourceContainer.stopContainer();

	}

	/**
	 * @since 2015-09-22
	 * @see StartContainer
	 * @objective Test removeBundle function with hue bundle
	 * @target void addBundle(string bundleId, string bundleUri, string
	 *         bundlePath, std::map<string, string> params)
	 * @target void startBundle(string bundleId)
	 * @target void removeBundle(string bundleId)
	 * @test_data ResourceContainerConfig
	 * @pre_condition 1. Get container instance 2. Add bundle
	 * @procedure Call removeBundle() API
	 * @post_condition StopContainer
	 * @expected Container can remove hue bundle
	 **/
	public void testRemoveHueBundle_SRC_P() {
		m_ResourceContainer.startContainer(configFile);

		m_ResourceContainer.addBundle(HUE_BUNDLE_ID, HUE_BUNDLE_URI, sdCardPath
				+ HUE_BUNDLE_JAR, HUE_BUNDLE_ACTIVATOR, params);

		if (!m_RCHelper.isBundleInAddedList(m_ResourceContainer, HUE_BUNDLE_ID)) {
			fail("Hue is not added");
		}

		if (!m_RCHelper.removeBundle(m_ResourceContainer, HUE_BUNDLE_ID)) {
			fail("Hue is not removed");
		}

		m_ResourceContainer.stopContainer();
	}

	/**
	 * @since 2015-09-22
	 * @see StartContainer
	 * @objective Test removeBundle function with di bundle
	 * @target void removeBundle(string bundleId)
	 * @test_data ResourceContainerConfig
	 * @pre_condition 1. Get container instance 2. Add bundle
	 * @procedure Call removeBundle() API
	 * @post_condition StopContainer
	 * @expected Container can remove di bundle
	 **/
	public void testRemoveDIBundle_SRC_P() {

		m_ResourceContainer.startContainer(configFile);

		if (!m_RCHelper.isBundleInAddedList(m_ResourceContainer, DI_BUNDLE_ID)) {
			fail("DI is not added");
		}

		if (!m_RCHelper.removeBundle(m_ResourceContainer, DI_BUNDLE_ID)) {
			fail("Can't Remove DI");
		}

		if (m_RCHelper.isBundleInAddedList(m_ResourceContainer, DI_BUNDLE_ID)) {
			fail("DI is not removed");
		}

		m_ResourceContainer.stopContainer();

	}

	/**
	 * @since 2015-09-22
	 * @see StartContainer
	 * @objective Test removeBundle function with bmi bundle
	 * @target void removeBundle(string bundleId)
	 * @test_data ResourceContainerConfig
	 * @pre_condition 1. Get container instance 2. Add bundle
	 * @procedure Call removeBundle() API
	 * @post_condition StopContainer
	 * @expected Container can remove bmi bundle
	 **/
	public void testRemoveBMIBundle_SRC_P() {

		m_ResourceContainer.startContainer(configFile);

		m_ResourceContainer.addBundle(HUE_BUNDLE_ID, EMPTY_STRING, sdCardPath
				+ HUE_BUNDLE_JAR, HUE_BUNDLE_NAME, params);

		if (!m_RCHelper.isBundleInAddedList(m_ResourceContainer, BMI_BUNDLE_ID)) {
			fail("BMI is not added");
		}

		if (!m_RCHelper.removeBundle(m_ResourceContainer, BMI_BUNDLE_ID)) {
			fail("Can't remove BMI bundle");
		}

		m_ResourceContainer.stopContainer();

	}

	/**
	 * @since 2015-09-22
	 * @see StartContainer
	 * @objective Test removeBundle function with hue bundle
	 * @target void removeBundle(string bundleId)
	 * @test_data ResourceContainerConfig
	 * @pre_condition 1. Get container instance 2. Add bundle 3. start hue
	 *                bundle
	 * @procedure Call removeBundle() API
	 * @post_condition StopContainer
	 * @expected Container can remove hue bundle after stopped bundle.
	 **/
	public void testRemoveHueBundleWhileRunning_ESV_N() {
		m_ResourceContainer.startContainer(configFile);

		m_ResourceContainer.addBundle(HUE_BUNDLE_ID, EMPTY_STRING, sdCardPath
				+ HUE_BUNDLE_JAR, HUE_BUNDLE_NAME, params);

		if (!m_RCHelper.isBundleInAddedList(m_ResourceContainer, HUE_BUNDLE_ID)) {
			fail("Hue is not added");
		}

		if (!m_RCHelper.startBundle(m_ResourceContainer, HUE_BUNDLE_ID)) {
			fail("Hue is not stared");
		}

		if (!m_RCHelper.removeBundle(m_ResourceContainer, HUE_BUNDLE_ID)) {
			fail("Hue is not removed");
		}

		m_ResourceContainer.stopContainer();
	}

	/**
	 * @since 2015-09-22
	 * @see StartContainer
	 * @objective Test removeBundle function with di bundle
	 * @target void removeBundle(string bundleId)
	 * @test_data ResourceContainerConfig
	 * @pre_condition 1. Get container instance 2. Add bundle 3. start di bundle
	 * @procedure Call removeBundle() API
	 * @post_condition StopContainer
	 * @expected Container can remove di bundle after stopped bundle.
	 **/
	public void testRemoveDIBundleWhileRunning_USTC_N() {

		m_ResourceContainer.startContainer(configFile);

		if (!m_RCHelper.isBundleInAddedList(m_ResourceContainer, DI_BUNDLE_ID)) {
			fail("DI is not added");
		}

		if (!m_RCHelper.startBundle(m_ResourceContainer, DI_BUNDLE_ID)) {
			fail("DI is not started");
		}

		if (!m_RCHelper.removeBundle(m_ResourceContainer, DI_BUNDLE_ID)) {
			fail("Can't remove DI bundle");
		}

		if (m_RCHelper.isBundleInAddedList(m_ResourceContainer, DI_BUNDLE_ID)) {
			fail("DI is not removed");
		}

		m_ResourceContainer.stopContainer();
	}

	/**
	 * @since 2015-09-22
	 * @see StartContainer
	 * @objective Test removeBundle function with bmi bundle
	 * @target void removeBundle(string bundleId)
	 * @test_data ResourceContainerConfig
	 * @pre_condition 1. Get container instance 2. start bmi bundle
	 * @procedure Call removeBundle() API
	 * @post_condition StopContainer
	 * @expected Container can remove bmi bundle after stopped bundle.
	 **/
	public void testRemoveBMIBundleWhileRunning_USTC_N() {
		m_ResourceContainer.startContainer(configFile);

		if (!m_RCHelper.isBundleInAddedList(m_ResourceContainer, BMI_BUNDLE_ID)) {
			fail("BMI is not added");
		}

		if (!m_RCHelper.startBundle(m_ResourceContainer, BMI_BUNDLE_ID)) {
			fail("BMI is not started");
		}

		if (!m_RCHelper.removeBundle(m_ResourceContainer, BMI_BUNDLE_ID))
			fail("Can't remove BMI Bundle");

		if (m_RCHelper.isBundleInAddedList(m_ResourceContainer, BMI_BUNDLE_ID)) {
			fail("BMI is not removed");
		}

		m_ResourceContainer.stopContainer();
	}

	/**
	 * @since 2016-03-12
	 * @see getBundleResource
	 * @objective Test 'setURI' function with hue bundle uri
	 * @target void setURI(String uri)
	 * @target String getURI()
	 * @test_data BUNDLE_URI
	 * @pre_condition BundleResource class should be initialize with attribute.
	 * @procedure Perform 1. setURI() API 2. getURI() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testSetGetBundleResourceURI_GSRV_P() {
		BundleResource bundleResource = m_RCHelper.getBundleResource(m_Context);
		try {
			bundleResource.setURI(HUE_BUNDLE_URI);

			assertEquals("Didn't get same value as set method  setURI() API.",
					HUE_BUNDLE_URI, bundleResource.getURI());
		} catch (Exception e) {
			fail("Exception occur inside testSetGetBundleResourceURI_GSRV_P");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see getBundleResource
	 * @objective Test 'setResourceType' function with temperature sensor
	 * @target void setResourceType(String type)
	 * @target String getResourceType()
	 * @test_data TEMPERATURE_SENSOR_TYPE
	 * @pre_condition BundleResource class should be initialize with attribute.
	 * @procedure 1. Perform setResourceType() API 2. Perform getResourceType()
	 *            API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testSetGetBundleResourceType_GSRV_P() {
		BundleResource bundleResource = m_RCHelper.getBundleResource(m_Context);
		try {
			bundleResource.setResourceType(TEMPERATURE_SENSOR_TYPE);

			assertEquals(
					"Didn't get same value as set method for setResourceType() API.",
					TEMPERATURE_SENSOR_TYPE, bundleResource.getResourceType());
		} catch (Exception e) {
			fail("Exception occur inside testSetGetBundleResourceType_GSRV_P");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see getBundleResource
	 * @objective Test 'setAddress' function with hue bundle uri
	 * @target void setAddress(String address)
	 * @target String getAddress()
	 * @test_data HUE_BUNDLE_URI
	 * @pre_condition BundleResource class should be initialize with attribute.
	 * @procedure 1. Perform setAddress() API 2. Perform getAddress() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testSetGetBundleResourceAddress_GSRV_P() {
		BundleResource bundleResource = m_RCHelper.getBundleResource(m_Context);
		try {
			bundleResource.setAddress(HUE_BUNDLE_URI);

			assertEquals(
					"Didn't get same value as set method for setAddress() API.",
					HUE_BUNDLE_URI, bundleResource.getAddress());
		} catch (Exception e) {
			fail("Exception occur inside testSetGetBundleResourceAddress_GSRV_P");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see getBundleResource
	 * @objective Test 'setName' function with temperature sensor name
	 * @target void setName(String name)
	 * @target String getName()
	 * @test_data HUE_BUNDLE_URI
	 * @pre_condition BundleResource class should be initialize with attribute.
	 * @procedure 1. Perform setName() API 2. Perform getName() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testSetGetBundleResourceName_GSRV_P() {
		BundleResource bundleResource = m_RCHelper.getBundleResource(m_Context);
		try {
			bundleResource.setName(TEMPERATURE_SENSOR_NAME);

			assertEquals(
					"Didn't get same value as set method for setName() API.",
					TEMPERATURE_SENSOR_NAME, bundleResource.getName());
		} catch (Exception e) {
			fail("Exception occur inside testSetGetBundleResourceName_GSRV_P");
		}
	}
}
