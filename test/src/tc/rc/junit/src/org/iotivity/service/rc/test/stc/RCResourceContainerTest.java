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

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.iotivity.service.rc.test.helper.RCHelper;

import static org.iotivity.service.rc.test.helper.RCCommonUtil.*;

import org.iotivity.service.resourcecontainer.RcsBundleInfo;
import org.iotivity.service.resourcecontainer.RcsResourceContainer;

import android.util.Log;
import android.test.InstrumentationTestCase;

public class RCResourceContainerTest extends InstrumentationTestCase {
	private RCHelper m_RCHelper;
	private RcsResourceContainer m_ResourceContainer;
	private String configFile;
	private String sdCardPath;
	private String hueBundlePath;

	private static boolean isCopied = false;
	private static final int m_count = 10;
	private static final int m_waitSec = 1;

	private static final String m_BmiBundleSensorId = "oic.bundle.BMISensor";
	private static final String s_VERSION = "Version";
	private static final String s_INVALID_PATH = "./invalidBundle.so";
	private static final String s_INVALID_ID = "Invalid";

	protected void setUp() throws Exception {
		super.setUp();

		m_RCHelper = RCHelper.getInstance();

		isCopied = m_RCHelper.copyFilesToDevice(getInstrumentation()
				.getTargetContext());

		sdCardPath = m_RCHelper.getFilesPath(getInstrumentation()
				.getTargetContext()) + PATH_INDICATOR;

		configFile = sdCardPath + RESOURCE_CONTAINER_CONFIG_XML;
		hueBundlePath = sdCardPath + HUE_BUNDLE_JAR;

		if (!isCopied) {
			Log.d(TAG, "Can't copy the files from assets/lib folder");
		} else {
			Log.d(TAG, "Successfully copied files from assets/lib folder");
		}

		m_ResourceContainer = new RcsResourceContainer(getInstrumentation()
				.getTargetContext());
	}

	protected void tearDown() throws Exception {
		super.tearDown();
	}

	/**
	 * @since 2015-10-29
	 * @see None
	 * @objective Test startContainer and stopContainer function multiple times
	 *            with valid configuration file
	 * @target void startContainer(string configFile)
	 * @target void stopContainer(string configFile)
	 * @test_data ResourceContainerConfig
	 * @pre_condition Get container instance
	 * @procedure 1. Perform startContainer() with config file API 2. Perform
	 *            stopContainer() API 3. Repeat step 1~2 10 times
	 * @post_condition None
	 * @expected Should not crashed
	 **/
	public void testStartStopContainerMultipleTimes_SRC_VLCC_P() {
		for (int i = 0; i < m_count; i++) {
			try {
				m_ResourceContainer.startContainer(configFile);
				m_RCHelper.waitInSecond(m_waitSec);
				m_ResourceContainer.stopContainer();
			} catch (Exception e) {
				fail("Exception occurred when stopping container multiple times. "
						+ e.getLocalizedMessage());
			}
		}
	}

	/**
	 * @since 2015-10-30
	 * @see None
	 * @objective Test startContainer and 'stopContainer' function multiple
	 *            times without 'ResourceContainerConfig'
	 * @target void startContainer(string configFile)
	 * @target void stopContainer(string configFile)
	 * @test_data none
	 * @pre_condition Get container instance
	 * @procedure 1. Perform startContainer() with empty string API 2. Perform
	 *            stopContainer() API 3. Repeat step 1~2 10 times
	 * @post_condition None
	 * @expected Should not crashed
	 **/
	public void testStopContainerMultipleTimes_ESV_N() {
		for (int i = 0; i < m_count; i++) {
			try {
				m_ResourceContainer.startContainer(EMPTY_STRING);
				m_ResourceContainer.stopContainer();

				fail("Successfully called stopContainer() while the start container without config file");
			} catch (Exception e) {
			}
		}
	}

	/**
	 * @since 2015-10-30
	 * @see void startContainer(string configFile)
	 * @see void stopContainer(string configFile)
	 * @objective Test 'listBundles' function with positive way
	 * @target List<RcsBundleInfo> listBundles()
	 * @test_data ResourceContainerConfig
	 * @test_data oic.bundle.BMISensor
	 * @pre_condition 1. Get container instance 2. Perform StartContainer() API
	 * @procedure Perform listBundles() API and get and ckeck the bundle with
	 *            predefined id
	 * @post_condition Perform stopContainer
	 * @expected Container can list up bundles.
	 **/
	public void testListBundlesForResourceContainerConfig_SRC_P() {
		try {
			m_ResourceContainer.startContainer(configFile);
			RcsBundleInfo bundleInfo = m_ResourceContainer.listBundles().get(0);

			assertEquals("Got " + bundleInfo.getID()
					+ " Expected: oic.bundle.BMISensor", bundleInfo.getID(),
					m_BmiBundleSensorId);

			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
			fail("Exception occurs at ListBundlesForResourceContainerConfig_P. "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2015-10-30
	 * @see None
	 * @objective Test 'addBundle' function multiple times with valid data
	 * @target void addBundle(String bundleId, String bundleUri, String
	 *         bundlePath, String activator, Map<String, String> params)
	 * @test_data Bundle information
	 * @pre_condition Get container instance
	 * @procedure Perform addBundle() API
	 * @post_condition None
	 * @expected Container can add bundle
	 **/
	public void testAddBundleWithCustomBundleInfo_SRC_P() {
		Map<String, String> params = new HashMap<String, String>();
		try {
			int bundleSize = m_ResourceContainer.listBundles().size();

			m_ResourceContainer.addBundle(HUE_BUNDLE_ID, HUE_BUNDLE_URI,
					hueBundlePath, HUE_BUNDLE_ACTIVATOR, params);
			if ((m_ResourceContainer.listBundles().size() == bundleSize)
					&& (m_ResourceContainer.listBundles().size() != (bundleSize + 1))) {
				fail("Does not get the added bundle using addBundle.");
			}
		} catch (Exception e) {
			fail("Exception occurred inside testAddBundleWithCustomBundleInfo_P. "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2015-10-30
	 * @see None
	 * @objective Test 'startBundle' and 'stopBundle' function multiple times
	 *            with positive way
	 * @target void startBundle(string bundleId)
	 * @test_data Bundle information
	 * @pre_condition Get container instance
	 * @procedure 1. Perform startBundle() API 2. Perform stopBundle() API 3.
	 *            Repeat 1~2 10 times
	 * @post_condition None
	 * @expected Container can add bundle
	 **/
	public void testStartAndStopBundleMultipleTimes_SRC_VLCC_P() {
		m_ResourceContainer.startContainer(configFile);

		RcsBundleInfo bundleInfo = m_ResourceContainer.listBundles().get(0);

		for (int i = 0; i < m_count; i++) {
			try {
				m_ResourceContainer.startBundle(bundleInfo.getID());
			} catch (Exception e) {
				fail("Exception occurred at startBundle() when adding multiple times. "
						+ e.getLocalizedMessage());
				return;
			}

			try {
				m_ResourceContainer.stopBundle(bundleInfo.getID());
			} catch (Exception e) {
				fail("Exception occurred at stopBundle() when adding multiple times. "
						+ e.getLocalizedMessage());
				return;
			}
		}
	}

	/**
	 * @since 2015-10-30
	 * @see void void addBundle(String bundleId, String bundleUri, String
	 *      bundlePath, String activator, Map<String, String> params)
	 * @objective Test 'startBundle' function with positive way
	 * @target void startBundle(string bundleId)
	 * @test_data Bundle information
	 * @pre_condition Get container instance
	 * @procedure 1. Add custom bundle 2. Perform startBundle() API
	 * @post_condition None
	 * @expected Should not crashed
	 **/
	public void testStartBundleWithoutResourceContainerConfig_SRC_P() {
		try {
			Map<String, String> params = new HashMap<String, String>();
			params.put(s_VERSION, HUE_BUNDLE_VERSION);
			m_ResourceContainer.addBundle(HUE_BUNDLE_ID, HUE_BUNDLE_ID,
					hueBundlePath, HUE_BUNDLE_ACTIVATOR, params);

		} catch (Exception e) {
			fail("Exception occurred inside StartBundleWithoutResourceContainerConfig_P"
					+ e.getLocalizedMessage());
		}

		try {
			m_ResourceContainer.startBundle(HUE_BUNDLE_ID);
		} catch (Exception e) {
			fail("Exception occurred at startBundle()."
					+ e.getLocalizedMessage());
			return;
		}

	}

	/**
	 * @since 2015-10-30
	 * @see None
	 * @objective Test 'removeBundle' function with negative way
	 * @target void removeBundle(string bundleId)
	 * @test_data None
	 * @pre_condition 1. Get container instance 2. Add bundle 3. Remove bundle
	 * @procedure Perform removeBundle() API
	 * @post_condition None
	 * @expected Container can remove bundle
	 **/
	public void testRemoveBundleWhichAlreadyRemoved_ESV_N() {
		try {
			Map<String, String> params = new HashMap<String, String>();
			params.put(s_VERSION, HUE_BUNDLE_VERSION);

			m_ResourceContainer.addBundle(HUE_BUNDLE_ID, HUE_BUNDLE_ID,
					hueBundlePath, HUE_BUNDLE_ACTIVATOR, params);
			m_ResourceContainer.removeBundle(HUE_BUNDLE_ID);
		} catch (Exception e) {
			fail("Exception occurred at precontion. " + e.getLocalizedMessage());
		}

		try {
			m_ResourceContainer.removeBundle(HUE_BUNDLE_ID);
		} catch (Exception e) {
			fail("Exception occurred when removeBundle. "
					+ e.getLocalizedMessage());
		}

		try {
			m_ResourceContainer.removeBundle(s_INVALID_ID);
		} catch (Exception e) {
			fail("Exception occurred when removeBundle with Invalid Path. "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2015-10-30
	 * @see None
	 * @objective Test 'startBundle' and 'addBundle' function with negative way
	 * @target void addBundle(String bundleId, String bundleUri, String
	 *         bundlePath, String activator, Map<String, String> params)
	 * @target void startBundle(String bundleId)
	 * @test_data BundleID
	 * @pre_condition Get container instance
	 * @procedure 1. AddBundle with invalid bundle path 2. Perform startBundle()
	 *            API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testStartBundleWithInvalidBundlePath_ESV_N() {
		try {
			Map<String, String> params = new HashMap<String, String>();
			params.put(s_VERSION, HUE_BUNDLE_VERSION);

			m_ResourceContainer.addBundle(HUE_BUNDLE_ID, HUE_BUNDLE_ID,
					s_INVALID_PATH, HUE_BUNDLE_ACTIVATOR, params);

		} catch (Exception e) {
			fail("Exception occurred inside adding invalid bundle path. "
					+ e.getLocalizedMessage());
		}

		try {
			m_ResourceContainer.startBundle(HUE_BUNDLE_ID);
		} catch (Exception e) {
			fail("Exception occurred inside adding invalid bundle path and start bundle. "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2015-11-02
	 * @see None
	 * @objective Test 'getPath' function with positive way
	 * @target string getPath()
	 * @test_data None
	 * @pre_condition Perform build() API
	 * @procedure Perform getPath() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testGetBundlePath_SRC_P() {
		try {
			m_ResourceContainer.startContainer(configFile);
			RcsBundleInfo bundleInfo = m_ResourceContainer.listBundles().get(0);

			if (bundleInfo != null) {
				String bundleID = bundleInfo.getID();

				if (bundleID.length() == 0) {
					fail("getID string is empty");
				}
			} else {
				fail("Precondition failed. Bundle info is null");
			}

			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
			fail("Exception occurred inside GetBundleID_P. "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2015-11-02
	 * @see None
	 * @objective Test 'getVersion' function with positive way
	 * @target string getVersion()
	 * @test_data None
	 * @pre_condition Perform build() API
	 * @procedure Perform getVersion() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testGetBundleVersion_SRC_P() {
		try {
			m_ResourceContainer.startContainer(configFile);
			String version = m_ResourceContainer.listBundles().get(0)
					.getVersion();
			if (version.isEmpty()) {
				fail("Bundle Version is empty after initialize");
			}
		} catch (Exception e) {
			fail("Exception occurred inside GetBundleVersion_P. "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-03-01
	 * @see None
	 * @objective Test 'removeResourceConfig' function with positive way [VLCC]
	 * @target void removeResourceConfig(string bundleId)
	 * @test_data None
	 * @pre_condition None
	 * @procedure 1. Get container instance 2. Add resource 3. Perform
	 *            removeResourceConfig() API 4. Repeat steps 1~3 multiple times
	 * @post_condition None
	 * @expected Container can remove resource in resourceConfig
	 **/
	public void testRemoveResourceConfig_VLCC_P() {
		int bundleSize, newBundleSize;
		for (int i = 0; i < m_count; i++) {
			try {
				Map<String, String> params = new HashMap<String, String>();
				params.put(s_VERSION, HUE_BUNDLE_VERSION);

				m_ResourceContainer.addResourceConfig(HUE_BUNDLE_ID,
						HUE_BUNDLE_ID, params);
				bundleSize = m_ResourceContainer.listBundles().size();

				m_RCHelper.waitInSecond(2);

				m_ResourceContainer.removeResourceConfig(HUE_BUNDLE_ID,
						HUE_BUNDLE_ID);
				newBundleSize = m_ResourceContainer.listBundles().size();

				assertEquals(bundleSize, newBundleSize);
			} catch (Exception e) {
				fail("Exception occurred inside RemoveResourceConfig_VLCC_P. "
						+ e.getLocalizedMessage());
			}
		}
	}

	/**
	 * @since 2016-03-01
	 * @see None
	 * @objective Test 'removeResourceConfig' function with negative way [USTC]
	 * @target void removeResourceConfig(string bundleId)
	 * @test_data None
	 * @pre_condition 1. Get container instance
	 * @procedure 1.Perform removeResourceConfig() API 2.Perform
	 *            addResourceConfig() API
	 * @post_condition None
	 * @expected No crash should occurs
	 **/
	public void testRemoveResourceConfig_USTC_N() {
		try {
			int bundleSize, newBundleSize;
			Map<String, String> params = new HashMap<String, String>();
			params.put(s_VERSION, HUE_BUNDLE_VERSION);

			m_ResourceContainer.removeResourceConfig(HUE_BUNDLE_ID,
					HUE_BUNDLE_ID);
			bundleSize = m_ResourceContainer.listBundles().size();

			m_RCHelper.waitInSecond(2);

			m_ResourceContainer.addResourceConfig(HUE_BUNDLE_ID, HUE_BUNDLE_ID,
					params);
			newBundleSize = m_ResourceContainer.listBundles().size();

			assertEquals(bundleSize, newBundleSize);

		} catch (Exception e) {
			fail("Exception occurred inside RemoveResourceConfig_VLCC_P. "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-03-01
	 * @see public List<RcsBundleInfo> startContainer(String configFile)
	 * @see public List<String> listBundleResources(String bundleId)
	 * @see public void removeResourceConfig(String bundleId, String
	 *      resourceUri)
	 * @see public void stopContainer()
	 * @objective Test 'addResourceConfig' function with positive way [SLCC]
	 * @target void addResourceConfig(string bundleId, string
	 *         resourceUri,std::map<string, string> params)
	 * @test_data Bundle information
	 * @pre_condition Start Container and get container instance
	 * @procedure Perform addResourceConfig() API and check total number of
	 *            resources
	 * @post_condition removeResourceConfig(),stopContainer()
	 * @expected Container can add resource in resourceConfig
	 **/
	public void testAddResourceConfig_SLCC_P() {

		List<String> resources = new ArrayList<String>();
		Map<String, String> params = new HashMap<String, String>();
		params.put("resourceType", DI_SENSOR_RESOURCE_TYPE);
		try {
			m_ResourceContainer.startContainer(configFile);
			resources = m_ResourceContainer.listBundleResources(DI_BUNDLE_ID);

			m_ResourceContainer.addResourceConfig(DI_BUNDLE_ID,
					DI_BUNDLE_RESOURCE1_NAME, params);
			assertEquals("addResourceConfig doest not work properly..",
					resources.size() + 1, m_ResourceContainer
							.listBundleResources(DI_BUNDLE_ID).size());

			m_ResourceContainer.removeResourceConfig(DI_BUNDLE_ID,
					DI_BUNDLE_RESOURCE1_URI);
			m_RCHelper.waitInSecond(10);
			assertTrue("removeResourceConfig doest not work properly..",
					m_ResourceContainer.listBundleResources(DI_BUNDLE_ID)
							.size() != (resources.size()));

			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
			fail("Exception occurred inside AddResourceConfig_P. "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-03-02
	 * @see public List<RcsBundleInfo> startContainer(String configFile)
	 * @see public List<RcsBundleInfo> listBundles()
	 * @see public void removeBundle(String bundleId)
	 * @see public void stopContainer()
	 * @objective Test 'addBundle' function with positive way [SLCC]
	 * @target void addBundle(String bundleId, String bundleUri, String
	 *         bundlePath, String activator, Map<String, String> params)
	 * @test_data Bundle information
	 * @pre_condition startContainer()
	 * @procedure 1. Get container instance 2. Perform addBundle() API
	 * @post_condition removeBundle(),stopContainer()
	 * @expected Container can add bundle
	 **/
	public void testAddBundleWithCustomBundleInfo_SLCC_P() {
		Map<String, String> params = new HashMap<String, String>();
		List<RcsBundleInfo> bundleList = new ArrayList<RcsBundleInfo>();

		m_ResourceContainer.startContainer(configFile);
		bundleList = m_ResourceContainer.listBundles();

		try {
			m_ResourceContainer.addBundle(HUE_BUNDLE_ID, HUE_BUNDLE_ID,
					hueBundlePath, HUE_BUNDLE_ACTIVATOR, params);
			assertTrue("Bundle is not added properly..", m_ResourceContainer
					.listBundles().size() == (bundleList.size() + 1));

			RcsBundleInfo bundleInfo = bundleList.get(0);

			m_ResourceContainer.removeBundle(HUE_BUNDLE_ID);
			m_RCHelper.waitInSecond(2);

			assertTrue(
					"Bundle is not removed properly..it's activated after remove..",
					!bundleInfo.isActivated());

			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
			fail("Exception occurred inside AddBundle_P. "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-03-02
	 * @see None
	 * @objective Test 'StopContainer' function with positive way [VLCC]
	 * @target void StopContainer(void)
	 * @test_data ResourceContainerConfig
	 * @pre_condition 1. Get container instance 2. Perform StartContainer() API
	 * @procedure Perform StopContainer() API
	 * @post_condition None
	 * @expected Container can stop container.
	 **/
	public void testStartAndStopContainer_VLCC_P() {
		try {
			for (int i = 0; i < m_count; i++) {
				m_ResourceContainer.startContainer(configFile);
				m_ResourceContainer.stopContainer();
			}
		} catch (Exception e) {
			fail("Can't stop container because of the exception: "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-03-01
	 * @see public List<RcsBundleInfo> startContainer(String configFile)
	 * @see public List<RcsBundleInfo> listBundles()
	 * @see public void removeBundle(String bundleId)
	 * @see public void startBundle(String bundleId)
	 * @objective Test 'stopBundle' function with positive way [ITLC]
	 * @target void stopBundle(string bundleId)
	 * @test_data ResourceContainerConfig
	 * @pre_condition void startContainer()
	 * @procedure 1. Get container instance 2. Perform StartContainer() API 3.
	 *            Get bundle list 4. Perform stopBundle() API 5. Repeat steps
	 *            1~4 multiple times
	 * @post_condition void stopContainer()
	 * @expected Container can stop bundle
	 **/
	public void testStartAndStopBundle_ITLC_P() {
		try {
			m_ResourceContainer.startContainer(configFile);
			List<RcsBundleInfo> bundleList = m_ResourceContainer.listBundles();

			assertNotNull("Bundle list is empty..", bundleList);
			assertTrue("Number of bundle is less than or equal zero..",
					bundleList.size() > 0);

			for (int i = 0; i < m_count; i++) {
				RcsBundleInfo bundleInfo = bundleList.get(0);
				m_ResourceContainer.startBundle(bundleInfo.getID());
				m_RCHelper.waitInSecond(2);
				m_ResourceContainer.stopBundle(bundleInfo.getID());
			}

			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
			fail("Got exception at StopBundle_P. Exception: "
					+ e.getLocalizedMessage());
		}
	}
}
