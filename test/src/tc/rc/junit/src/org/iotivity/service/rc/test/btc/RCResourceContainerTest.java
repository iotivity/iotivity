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
package org.iotivity.service.rc.test.btc;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static org.iotivity.service.rc.test.helper.RCCommonUtilBTC.*;
import org.iotivity.service.rc.test.helper.RCHelperBTC;

import org.iotivity.service.resourcecontainer.RcsBundleInfo;
import org.iotivity.service.resourcecontainer.RcsResourceContainer;
import org.iotivity.service.resourcecontainer.ResourceConfig;

import android.content.Context;
import android.test.InstrumentationTestCase;
import android.util.Log;

public class RCResourceContainerTest extends InstrumentationTestCase {
	private RcsResourceContainer m_ResourceContainer;
	private RCHelperBTC m_RCHelper;

	private String bundlePath;
	private String configFile;
	private String emptyConfigFile;
	private String sdCardPath;

	public static final String s_INSERT_VERSION_INFO = "Version";
	public static String[] resourceParam;

	public static final int n_ZERO = 0;
	public static final int n_MAX_WAIT = 2;
	public static final int n_NEGATIVE_ONE = -1;

	private static boolean isCopied = false;
	private Context m_Context = null;

	protected void setUp() throws Exception {
		super.setUp();

		m_Context = getInstrumentation().getTargetContext();

		m_RCHelper = RCHelperBTC.getInstance();

		isCopied = m_RCHelper.copyFilesToDevice(m_Context);

		sdCardPath = m_RCHelper.getFilesPath(m_Context);

		configFile = sdCardPath + RESOURCE_CONTAINER_CONFIG_XML;
		emptyConfigFile = sdCardPath + RESOURCE_CONTAINER_EMPTY_CONFIG_XML;
		bundlePath = sdCardPath + HUE_BUNDLE_JAR;

		if (!isCopied) {
			Log.d(TAG, "Can't copy the files from assets/lib folder");
		} else {
			Log.d(TAG, "Successfully copied files from assets/lib folder");
		}

		m_ResourceContainer = new RcsResourceContainer(m_Context);
	}

	protected void tearDown() throws Exception {
		super.tearDown();

		if (m_ResourceContainer != null) {
			m_ResourceContainer = null;
		}
	}

	/**
	 * @since 2015-10-26
	 * @see None
	 * @objective Test 'StartContainer' function with ResourceContainerConfig
	 * @target void startContainer(string configFile)
	 * @test_data ResourceContainerConfig
	 * @pre_condition Get container instance
	 * @procedure Perform StartContainer() API
	 * @post_condition None
	 * @expected Container can gather information from
	 *           ResourceContainerConfig.xml file.
	 **/
	public void testStartContainer_SRC_P() {
		try {
			m_ResourceContainer.startContainer(configFile);
		} catch (Exception e) {
			fail("Can't start container because of the exception: "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2015-10-26
	 * @see None
	 * @objective Test 'StartContainer' function with empty string instead of
	 *            ResourceContainerConfig
	 * @target void startContainer(string configFile)
	 * @test_data None
	 * @pre_condition Get container instance
	 * @procedure Perform StartContainer() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testStartContainer_ESV_N() {
		try {
			m_ResourceContainer.startContainer(EMPTY_STRING);

			fail("Successfully called startContainer with empty config file path");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2015-10-26
	 * @see StartContainer
	 * @objective Test 'StopContainer' function with StartContainer first
	 * @target void StopContainer(void)
	 * @test_data ResourceContainerConfig
	 * @pre_condition 1. Get container instance 2. Perform StartContainer() API
	 * @procedure Perform StopContainer() API
	 * @post_condition None
	 * @expected Container can stop container.
	 **/
	public void testStopContainer_SRC_P() {
		try {
			m_ResourceContainer.startContainer(configFile);
			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
			fail("Can't stop container because of the exception: "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2015-10-26
	 * @see None
	 * @objective Test 'StopContainer' function without StartContainer
	 * @target void StopContainer(void)
	 * @test_data None
	 * @pre_condition Get container instance
	 * @procedure Perform StopContainer() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testStopContainer_ESV_N() {
		try {
			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
			fail("Can't stop container because of the exception: "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2015-10-26
	 * @see StartContainer
	 * @objective Test 'listBundles' function with StartContainer first
	 * @target list<BundleInfo*> listBundles(void)
	 * @test_data ResourceContainerConfig
	 * @pre_condition 1. Get container instance 2. Perform StartContainer() API
	 *                with config file
	 * @procedure Perform listBundles() API
	 * @post_condition Stop container
	 * @expected Container can list up bundles.
	 **/
	public void testListBundles_SRC_P() {
		m_ResourceContainer.startContainer(configFile);
		try {
			List<RcsBundleInfo> bundleList = m_ResourceContainer.listBundles();
		} catch (Exception e) {
			fail("ListBundles occurs  the exception: "
					+ e.getLocalizedMessage());
		}

		m_ResourceContainer.stopContainer();
	}

	/**
	 * @since 2015-10-26
	 * @see StartContainer
	 * @objective Test 'listBundles' function with StartContainer that have
	 *            empty config file
	 * @target list<BundleInfo*> listBundles(void)
	 * @test_data Empty Resource Config
	 * @pre_condition 1. Get container instance 2. Start container with valid
	 *                config file which has bundles
	 * @procedure Perform listBundles() API
	 * @post_condition Stop container
	 * @expected No crash occurs
	 **/
	public void testListBundles_ESV_N() {
		m_ResourceContainer.startContainer(emptyConfigFile);

		try {
			List<RcsBundleInfo> bundleList = m_ResourceContainer.listBundles();

			if (bundleList.size() != n_ZERO) {
				fail("Bundle size must be 0 but got " + bundleList.size());
			}
		} catch (Exception e) {
		}

		m_ResourceContainer.stopContainer();
	}

	/**
	 * @since 2015-10-27
	 * @see StartContainer
	 * @objective Test 'startBundle' function with StartContainer that have
	 *            valid Resource Config file
	 * @target void startBundle(string bundleId)
	 * @test_data ResourceContainerConfig
	 * @pre_condition 1. Get container instance 2. Perform StartContainer() API
	 * @procedure Perform startBundle() API
	 * @post_condition Stop container
	 * @expected Container can start bundle.
	 **/
	public void testStartBundle_SRC_P() {
		try {
			m_ResourceContainer.startContainer(configFile);
			List<RcsBundleInfo> bundleList = m_ResourceContainer.listBundles();

			if (bundleList.size() >= n_ZERO) {
				RcsBundleInfo bundleInfo = bundleList.get(n_ZERO);
				m_ResourceContainer.startBundle(bundleInfo.getID());

				m_RCHelper.waitInSecond(n_MAX_WAIT);

				m_ResourceContainer.stopContainer();
			} else {
				fail("Can't get bundle list");
			}
		} catch (Exception e) {
			fail("Got exception at StartBundle_P. Exception: "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2015-10-27
	 * @see None
	 * @objective Test 'startBundle' function with Empty String
	 * @target void startBundle(string bundleId)
	 * @test_data empty string
	 * @pre_condition Get container instance
	 * @procedure Perform startBundle() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testStartBundle_ESV_N() {
		try {
			m_ResourceContainer.startBundle(EMPTY_STRING);
			fail("Successfully called startBundle with null bundleId");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2015-10-27
	 * @see StartContainer
	 * @see StartBundle
	 * @see Get bundle list
	 * @objective Test 'stopBundle' function with StartContainer and StartBundle
	 *            these have valid data
	 * @target void stopBundle(string bundleId)
	 * @test_data ResourceContainerConfig
	 * @pre_condition 1. Get container instance 2. Perform StartContainer() 3.
	 *                Get bundle list 4. StartBundle
	 * @procedure Perform stopBundle() API
	 * @post_condition StopContainer
	 * @expected Container can stop bundle
	 **/
	public void testStopBundle_SRC_P() {
		try {
			m_ResourceContainer.startContainer(configFile);
			List<RcsBundleInfo> bundleList = m_ResourceContainer.listBundles();

			if (bundleList.size() >= n_ZERO) {
				RcsBundleInfo bundleInfo = bundleList.get(n_ZERO);
				m_ResourceContainer.startBundle(bundleInfo.getID());
				m_RCHelper.waitInSecond(n_MAX_WAIT);
				m_ResourceContainer.stopBundle(bundleInfo.getID());

				m_ResourceContainer.stopContainer();
			} else {
				fail("Can't get bundle list");
			}
		} catch (Exception e) {
			fail("Got exception at StopBundle_P. Exception: "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2015-10-27
	 * @see None
	 * @objective Test 'stopBundle' function with empty string
	 * @target void stopBundle(string bundleId)
	 * @test_data Empty string
	 * @pre_condition Get container instance
	 * @procedure Perform stopBundle() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testStopBundle_ESV_N() {
		try {
			m_ResourceContainer.stopBundle(EMPTY_STRING);

			fail("Successfully called stopBundle with null bundleId");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2015-10-27
	 * @see None
	 * @objective Test 'addBundle' function with data and path
	 * @target void addBundle(String bundleId, String bundleUri, String
	 *         bundlePath, String activator, Map<String, String> params)
	 * @test_data Bundle information
	 * @pre_condition 1. Get container instance 2. Start Container 3. Initiate
	 *                param value
	 * @procedure Perform addBundle() API
	 * @post_condition None
	 * @expected Container should add bundle
	 **/
	public void testAddBundle_SRC_P() {
		try {
			m_ResourceContainer.startContainer(configFile);
			Map<String, String> params = new HashMap<String, String>();
			params.put(s_INSERT_VERSION_INFO, HUE_BUNDLE_VERSION);

			m_ResourceContainer.addBundle(HUE_BUNDLE_ID, HUE_BUNDLE_URI,
					bundlePath, HUE_BUNDLE_ACTIVATOR, params);
		} catch (Exception e) {
			fail("Exception occurred inside AddBundle_P. "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2015-10-27
	 * @see None
	 * @objective Test 'addBundle' function with empty string
	 * @target void addBundle(String bundleId, String bundleUri, String
	 *         bundlePath, String activator, Map<String, String> params)
	 * @test_data None
	 * @pre_condition Get container instance
	 * @procedure Perform addBundle() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAddBundle_ESV_N() {
		try {
			Map<String, String> params = new HashMap<String, String>();

			m_ResourceContainer.addBundle(EMPTY_STRING, EMPTY_STRING,
					EMPTY_STRING, EMPTY_STRING, params);
		} catch (Exception e) {
			fail("Exception occurred inside AddBundle_N. "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2015-10-27
	 * @see None
	 * @objective Test 'removeBundle' function with valid data and path
	 * @target void removeBundle(string bundleId)
	 * @test_data Bundle Info
	 * @pre_condition 1. Get container instance 2. Add bundle
	 * @procedure Perform removeBundle() API
	 * @post_condition None
	 * @expected Container can remove bundle
	 **/
	public void testRemoveBundle_SRC_P() {
		try {
			Map<String, String> params = new HashMap<String, String>();
			params.put(s_INSERT_VERSION_INFO, HUE_BUNDLE_VERSION);

			m_ResourceContainer.addBundle(HUE_BUNDLE_ID, HUE_BUNDLE_URI,
					bundlePath, HUE_BUNDLE_ACTIVATOR, params);

			m_RCHelper.waitInSecond(n_MAX_WAIT);

			m_ResourceContainer.removeBundle(HUE_BUNDLE_ID);
		} catch (Exception e) {
			fail("Exception occurred inside RemoveBundle_P. "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2015-10-27
	 * @see None
	 * @objective Test 'removeBundle' function with empty bundleID
	 * @target void removeBundle(string bundleId)
	 * @test_data empty string
	 * @pre_condition Get container instance
	 * @procedure Perform removeBundle() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRemoveBundle_ESV_N() {
		try {
			m_ResourceContainer.removeBundle(EMPTY_STRING);
			fail("Successfully called removeBundle with empty bundleId");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2015-10-27
	 * @see None
	 * @objective Test 'addResourceConfig' function with valid resource data
	 * @target void addResourceConfig(string bundleId, string resourceUri,
	 *         std::map<string, string> params)
	 * @test_data Bundle information
	 * @pre_condition Get container instance
	 * @procedure Perform addResourceConfig() API
	 * @post_condition None
	 * @expected Container can add resource in resourceConfig
	 **/
	public void testAddResourceConfig_SRC_P() {
		try {
			Map<String, String> params = new HashMap<String, String>();
			params.put(s_INSERT_VERSION_INFO, HUE_BUNDLE_VERSION);

			m_ResourceContainer.addResourceConfig(HUE_BUNDLE_ID,
					HUE_BUNDLE_URI, params);
		} catch (Exception e) {
			fail("Exception occurred inside AddResourceConfig_P. "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2015-10-27
	 * @see None
	 * @objective Test 'addResourceConfig' function with empty string for
	 *            bundleId and resourceUri
	 * @target void addResourceConfig(string bundleId, string resourceUri,
	 *         std::map<string, string> params)
	 * @test_data None
	 * @pre_condition Get container instance
	 * @procedure Perform addResourceConfig() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAddResourceConfig_ESV_N() {
		try {
			Map<String, String> params = new HashMap<String, String>();

			m_ResourceContainer.addResourceConfig(EMPTY_STRING, EMPTY_STRING,
					params);
			fail("Successfully called addResourceConfig with empty bundleId and resUri");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2015-10-27
	 * @see addResourceConfig
	 * @objective Test 'removeResourceConfig' function with BundleId & BundleUri
	 * @target void removeResourceConfig(string bundleId)
	 * @test_data BundleId & BundleUri
	 * @pre_condition 1. Get container instance 2. Add resource
	 * @procedure Perform removeResourceConfig() API
	 * @post_condition None
	 * @expected Container can remove resource in resourceConfig
	 **/
	public void testRemoveResourceConfig_SRC_P() {
		try {
			Map<String, String> params = new HashMap<String, String>();
			params.put(s_INSERT_VERSION_INFO, HUE_BUNDLE_VERSION);

			m_ResourceContainer.addResourceConfig(HUE_BUNDLE_ID,
					HUE_BUNDLE_URI, params);

			m_RCHelper.waitInSecond(n_MAX_WAIT);

			m_ResourceContainer.removeResourceConfig(HUE_BUNDLE_ID,
					HUE_BUNDLE_URI);
		} catch (Exception e) {
			fail("Exception occurred inside RemoveResourceConfig_P. "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2015-10-27
	 * @see None
	 * @objective Test 'removeResourceConfig' function with empty string
	 * @target void removeResourceConfig(string bundleId)
	 * @test_data None
	 * @pre_condition Get container instance
	 * @procedure Perform removeResourceConfig() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRemoveResourceConfig_ESV_N() {
		try {
			m_ResourceContainer
					.removeResourceConfig(EMPTY_STRING, EMPTY_STRING);

			fail("Successfully called removeResourceConfig with empty bundleId, resUri");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2015-10-27
	 * @see StartContainer
	 * @objective Test 'listBundleResources' function with StartContainer first
	 * @target list<string> listBundleResources(void)
	 * @test_data ResourceConfig File
	 * @pre_condition 1. Get container instance 2. Add bundle resource
	 * @procedure Perform listBundleResources() API
	 * @post_condition StopContainer
	 * @expected Container can remove resource in resourceConfig
	 **/
	public void testListBundleResources_SRC_P() {
		m_ResourceContainer.startContainer(configFile);
		RcsBundleInfo bundleInfo = m_ResourceContainer.listBundles()
				.get(n_ZERO);

		if (bundleInfo != null) {
			String bundleID = bundleInfo.getID();

			if (m_ResourceContainer.listBundleResources(bundleID).size() == n_ZERO) {
				fail("bundle resource count is 0.");
			}
		} else {
			fail("Precondition failed. Bundle info is null");
		}

		m_ResourceContainer.stopContainer();
	}

	/**
	 * @since 2015-10-27
	 * @see StartContainer
	 * @objective Test 'listBundleResources' function with StartContainer that
	 *            have empty config file
	 * @target list<string> listBundleResources(void)
	 * @test_data emptyConfigFile
	 * @pre_condition Get container instance
	 * @procedure Perform listBundleResources() API
	 * @post_condition StopContainer
	 * @expected No crash occurs
	 **/
	public void testListBundleResources_N() {
		m_ResourceContainer.startContainer(emptyConfigFile);

		if (m_ResourceContainer.listBundleResources(HUE_BUNDLE_ID).size() != n_ZERO) {
			fail("bundle resource count is 0");
		}

		m_ResourceContainer.stopContainer();
	}

	/**
	 * @since 2016-02-29
	 * @see Get Instance
	 * @objective Test 'StartContainer' function with NULL configFile
	 * @target void startContainer(string configFile)
	 * @test_data None
	 * @pre_condition Get container instance
	 * @procedure Perform StartContainer() API with NULL
	 * @post_condition None
	 * @expected Should not be crashed
	 **/
	public void testStartContainer_NV_N() {
		try {
			m_ResourceContainer.startContainer(null);

			fail("Successfully called startContainer with null bundleId");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-02-29
	 * @see None
	 * @objective Test 'addBundle' function with NULL Resource BundleId
	 * @target void addBundle(String bundleId, String bundleUri, String
	 *         bundlePath, String activator, Map<String, String> params)
	 * @test_data None
	 * @pre_condition 1. Get container instance 2. Initiate param value as null
	 * @procedure Perform addBundle() API
	 * @post_condition None
	 * @expected Should not be crashed
	 **/
	public void testAddBundle_BundleID_NV_N() {
		try {
			Map<String, String> params = new HashMap<String, String>();

			m_ResourceContainer.addBundle(null, HUE_BUNDLE_URI, bundlePath,
					HUE_BUNDLE_ACTIVATOR, params);

			fail("Successfully called addBundle with null bundleId");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-02-29
	 * @see None
	 * @objective Test 'addBundle' function with NULL Resource URI
	 * @target void addBundle(String bundleId, String bundleUri, String
	 *         bundlePath, String activator, Map<String, String> params)
	 * @test_data None
	 * @pre_condition 1. Get container instance 2. Initiate param value as null
	 * @procedure Perform addBundle() API
	 * @post_condition None
	 * @expected Should not be crashed
	 **/
	public void testAddBundle_BundleUri_NV_N() {
		try {
			Map<String, String> params = new HashMap<String, String>();

			m_ResourceContainer.addBundle(HUE_BUNDLE_ID, null, bundlePath,
					HUE_BUNDLE_ACTIVATOR, params);

			fail("Successfully called addBundle with null Bundle Uri");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-02-29
	 * @see None
	 * @objective Test 'addBundle' function with NULL Resource BundlePath
	 * @target void addBundle(String bundleId, String bundleUri, String
	 *         bundlePath, String activator, Map<String, String> params)
	 * @test_data None
	 * @pre_condition 1. Get container instance 2. Initiate param value as null
	 * @procedure Perform addBundle() API
	 * @post_condition None
	 * @expected Should not be crashed
	 **/
	public void testAddBundle_BundlePath_NV_N() {
		try {
			Map<String, String> params = new HashMap<String, String>();

			m_ResourceContainer.addBundle(HUE_BUNDLE_ID, HUE_BUNDLE_URI, null,
					HUE_BUNDLE_ACTIVATOR, params);
			fail("Successfully called addBundle with null Bundle path");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-02-29
	 * @see None
	 * @objective Test 'addBundle' function with NULL resource Activator
	 * @target void addBundle(String bundleId, String bundleUri, String
	 *         bundlePath, String activator, Map<String, String> params)
	 * @test_data None
	 * @pre_condition 1. Get container instance 2. Initiate param value as null
	 * @procedure Perform addBundle() API
	 * @post_condition None
	 * @expected Should not be crashed
	 **/
	public void testAddBundle_BundleActivator_NV_N() {
		try {
			Map<String, String> params = new HashMap<String, String>();

			m_ResourceContainer.addBundle(HUE_BUNDLE_ID, HUE_BUNDLE_URI,
					bundlePath, null, params);
			fail("Successfully called addBundle with null activator");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-02-29
	 * @see None
	 * @objective Test 'removeBundle' function with NULL
	 * @target void removeBundle(string bundleId)
	 * @test_data None
	 * @pre_condition Get container instance
	 * @procedure Perform removeBundle() API with null
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRemoveBundle_NV_N() {
		try {
			m_ResourceContainer.removeBundle(null);
			fail("Successfully called removeBundle() with null.");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-02-29
	 * @see None
	 * @objective Test 'startBundle' function with NULL
	 * @target void startBundle(string bundleId)
	 * @test_data None
	 * @pre_condition Get container instance
	 * @procedure Perform startBundle() API with null
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testStartBundle_NV_N() {
		try {
			m_ResourceContainer.startBundle(null);

			fail("Successfully called startBundle with null bundleId");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-02-29
	 * @see None
	 * @objective Test 'stopBundle' function with NULL
	 * @target void stopBundle(string bundleId)
	 * @test_data None
	 * @pre_condition Get container instance
	 * @procedure Perform stopBundle() API with null
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testStopBundle_NV_N() {
		try {
			m_ResourceContainer.stopBundle(null);

			fail("Successfully called stopBundle with null bundleId");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-02-29
	 * @see None
	 * @objective Test 'addResourceConfig' function with NULL resource BundleId
	 * @target void addResourceConfig(string bundleId, string resourceUri,
	 *         std::map<string, string> params)
	 * @test_data Bundle information
	 * @pre_condition Get container instance
	 * @procedure Perform addResourceConfig() API
	 * @post_condition None
	 * @expected should not be crashed
	 **/
	public void testAddResourceConfig_BundleId_NV_N() {
		try {
			Map<String, String> params = new HashMap<String, String>();

			m_ResourceContainer.addResourceConfig(null, HUE_BUNDLE_URI, params);

			fail("Successfully called addResourceConfig with null bundleId");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-02-29
	 * @see None
	 * @objective Test 'addResourceConfig' function with NULL resource BundleURI
	 * @target void addResourceConfig(string bundleId, string resourceUri,
	 *         std::map<string, string> params)
	 * @test_data Bundle information
	 * @pre_condition Get container instance
	 * @procedure Perform addResourceConfig() API with null
	 * @post_condition None
	 * @expected should not be crashed
	 **/
	public void testAddResourceConfig_BundleUri_NV_N() {
		try {
			Map<String, String> params = new HashMap<String, String>();

			m_ResourceContainer.addResourceConfig(HUE_BUNDLE_ID, null, params);
			fail("Successfully called addResourceConfig with null resourceUri");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-02-29
	 * @see None
	 * @objective Test 'removeResourceConfig' function with NULL resource
	 *            BundleId
	 * @target void removeResourceConfig(string bundleId)
	 * @test_data None
	 * @pre_condition Get container instance
	 * @procedure Perform removeResourceConfig() API with null
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRemoveResourceConfig_BundleId_NV_N() {
		try {
			m_ResourceContainer.removeResourceConfig(null, HUE_BUNDLE_URI);
			fail("Successfully called removeResourceConfig with null bundleId");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-02-29
	 * @see None
	 * @objective Test 'removeResourceConfig' function with NULL resource
	 *            BundleURI
	 * @target void removeResourceConfig(string bundleId, string resourceUri)
	 * @test_data None
	 * @pre_condition Get container instance
	 * @procedure Perform removeResourceConfig() API with null
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRemoveResourceConfig_ResUri_NV_N() {
		try {
			m_ResourceContainer.removeResourceConfig(HUE_BUNDLE_ID, null);
			fail("Successfully called removeResourceConfig with null resUri");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @see getbundleResource
	 * @objective Test 'registerResource' function with startContainer first and
	 *            valid BundleId
	 * @target public void registerResource(java.lang.String bundleId,
	 *         BundleResource resource)
	 * @test_data ConfigFile
	 * @pre_condition Start Container
	 * @procedure Perform registerResource() API with DISCOMFORT_BUNDLE_ID and
	 *            Bundle Resource instance
	 * @post_condition Stop Container
	 * @expected No Exception occurs
	 **/
	public void testRegisterResource_SRC_P() {

		try {
			m_ResourceContainer.startContainer(configFile);
			m_ResourceContainer.registerResource(DI_BUNDLE_ID,
					RCHelperBTC.getBundleResource(m_Context));
			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
			fail("Can't  register resource of the exception: "
					+ e.getLocalizedMessage());
		}

	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'registerResource' function with empty resource BundleId
	 * @target public void registerResource(java.lang.String bundleId,
	 *         BundleResource resource)
	 * @test_data configFile
	 * @pre_condition Start Container
	 * @procedure Perform registerResource() API with DISCOMFORT_BUNDLE_ID = ""
	 * @post_condition Stop Container
	 * @expected Exception Should Occure
	 **/
	public void testRegisterResourceBundleID_ESV_N() {
		try {
			m_ResourceContainer.startContainer(configFile);
			m_ResourceContainer.registerResource(EMPTY_STRING,
					RCHelperBTC.getBundleResource(m_Context));

			fail("Should throw a Exception");
			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
		}

	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'registerResource' function with NULL resource BundleId
	 * @target public void registerResource(java.lang.String bundleId,
	 *         BundleResource resource)
	 * @test_data configFile
	 * @pre_condition Start Container
	 * @procedure Perform registerResource() API with DISCOMFORT_BUNDLE_ID =
	 *            null
	 * @post_condition Stop Container
	 * @expected Exception Should Occure
	 **/
	public void testRegisterResourceBundleID_NV_N() {
		try {
			m_ResourceContainer.startContainer(configFile);

			m_ResourceContainer.registerResource(null,
					RCHelperBTC.getBundleResource(m_Context));

			fail("Should throw a Exception for registerResource() with null bundleId");
			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'registerResource' function with NULL getBundleResource
	 * @target public void registerResource(java.lang.String bundleId,
	 *         BundleResource resource)
	 * @test_data ConfigFile
	 * @pre_condition Start Container
	 * @procedure Perform registerResource() API with BundleResource = null
	 * @post_condition Stop Container
	 * @expected Exception Should Occure
	 **/
	public void testRegisterResourceBundleResource_NV_N() {
		try {
			m_ResourceContainer.startContainer(configFile);
			m_ResourceContainer.registerResource(DI_BUNDLE_ID, null);

			fail("Should throw a Exception");
			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
		}

	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @see RegisterResource
	 * @objective Test 'unregisterResource' function with registerResource first
	 * @target public void unregisterResource(BundleResource resource)
	 * @test_data configFile
	 * @pre_condition 1.Start Container 2.Register Resource
	 * @procedure Perform unregisterResource() API with BundleResource
	 * @post_condition Stop Container
	 * @expected No Exception occurs
	 **/
	public void testUnregisterResource_SRC_P() {
		try {
			m_ResourceContainer.startContainer(configFile);
			m_ResourceContainer.registerResource(DI_BUNDLE_ID,
					RCHelperBTC.getBundleResource(m_Context));
			m_ResourceContainer.unregisterResource(RCHelperBTC
					.getBundleResource(m_Context));
			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
			fail("Can't  unregister resource of the exception: "
					+ e.getLocalizedMessage());
		}

	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @see Register Resource
	 * @objective Test 'unregisterResource' function with NULL
	 * @target public void unregisterResource(BundleResource resource)
	 * @test_data configFile
	 * @pre_condition 1. Start Container 2. Register Resource
	 * @procedure Perform unregisterResource() API with BundleResource = null
	 * @post_condition Stop Container
	 * @expected Exception should occure
	 **/
	public void testUnregisterResourceBundleResource_NV_N() {
		try {
			m_ResourceContainer.startContainer(configFile);
			m_ResourceContainer.registerResource(DI_BUNDLE_ID,
					RCHelperBTC.getBundleResource(m_Context));
			m_ResourceContainer.unregisterResource(null);

			fail("Should throw a Exception");
			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
		}

	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'getConfiguredBundleResources' function with Resource
	 *            BundleId
	 * @target public void getConfiguredBundleResources(BundleResource resource)
	 * @test_data configFile
	 * @pre_condition Start Container
	 * @procedure Perform getConfiguredBundleResources() API with
	 *            DISCOMFORT_BUNDLE_ID
	 * @post_condition Stop Container
	 * @expected Return a List with Size > 0 and No Exception occurs
	 **/
	public void testGetConfiguredBundleResources_SRC_P() {
		try {
			m_ResourceContainer.startContainer(configFile);

			List<ResourceConfig> resourceConfigList = m_ResourceContainer
					.getConfiguredBundleResources(DI_BUNDLE_ID);
			Log.i(TAG, "Size of resource configuration list ="
					+ resourceConfigList.size());

			if (resourceConfigList.size() <= n_ZERO) {
				fail("getConfiguredBundleResources returns a List Size = "
						+ resourceConfigList.size());
			}

			for (int i = n_ZERO; i < resourceConfigList.size(); i++) {
				Log.i(TAG, "resourceConfigList - [" + i + "] "
						+ resourceConfigList.get(i));
			}

			m_ResourceContainer.stopContainer();

		} catch (Exception e) {
			fail("Can't  get configured bundle resources because of the exception: "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'getConfiguredBundleResources' function with Resource
	 *            empty BundleId [ESV]
	 * @target public void getConfiguredBundleResources(BundleResource resource)
	 * @test_data configFile
	 * @pre_condition Start Container
	 * @procedure Perform getConfiguredBundleResources() API with
	 *            DISCOMFORT_BUNDLE_ID = ""
	 * @post_condition Stop Container
	 * @expected Expection should occurs
	 **/
	public void testGetConfiguredBundleResourcesBudleID_ESV_N() {
		try {
			m_ResourceContainer.startContainer(configFile);

			List<ResourceConfig> resConfigList = m_ResourceContainer
					.getConfiguredBundleResources(EMPTY_STRING);

			if (resConfigList.size() > n_ZERO) {
				fail("getConfiguredBundleResources() returns ResourceConfig list while BundleId is empty");
			}

			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
			m_ResourceContainer.stopContainer();
		}
	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'getConfiguredBundleResources' function with NULL
	 *            Resource BundleId
	 * @target public void getConfiguredBundleResources(BundleResource resource)
	 * @test_data configFile
	 * @pre_condition Start Container
	 * @procedure Perform getConfiguredBundleResources() API with
	 *            DISCOMFORT_BUNDLE_ID = null
	 * @post_condition Stop Container
	 * @expected Expection should occurs
	 **/
	public void testGetConfiguredBundleResources_NV_N() {
		try {
			m_ResourceContainer.startContainer(configFile);

			List<ResourceConfig> resConfigList = m_ResourceContainer
					.getConfiguredBundleResources(null);

			if (resConfigList.size() > n_ZERO) {
				fail("getConfiguredBundleResources() returns ResourceConfig list while BundleId is null");
			}

			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
			m_ResourceContainer.stopContainer();
		}
	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'getNumberOfConfiguredResources' function with valid
	 *            Resource BundleId
	 * @target public void getNumberOfConfiguredResources(BundleResource
	 *         resource)
	 * @test_data configFile
	 * @pre_condition Start Container
	 * @procedure Perform getNumberOfConfiguredResources() API with
	 *            DISCOMFORT_BUNDLE_ID
	 * @post_condition Stop Container
	 * @expected Return ResourceCount > 0 and No Exception occurs
	 **/
	public void testGetNumberOfConfiguredResources_SRC_P() {
		try {
			m_ResourceContainer.startContainer(configFile);

			int numberOfConfiguredResources = n_NEGATIVE_ONE;
			numberOfConfiguredResources = m_ResourceContainer
					.getNumberOfConfiguredResources(DI_BUNDLE_ID);

			Log.i(TAG, "numberOfConfiguredResources  is "
					+ numberOfConfiguredResources);

			if (numberOfConfiguredResources <= n_ZERO) {
				fail("numberOfConfiguredResources  is "
						+ numberOfConfiguredResources);
			}

			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
			fail("Can't  get number of configured bundle resources because of the exception: "
					+ e.getLocalizedMessage());
		}

	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'getNumberOfConfiguredResources' function with Empty
	 *            Resource BundleId
	 * @target public void getNumberOfConfiguredResources(BundleResource
	 *         resource)
	 * @test_data configFile
	 * @pre_condition Start Container
	 * @procedure Perform getNumberOfConfiguredResources() API with
	 *            DISCOMFORT_BUNDLE_ID = ""
	 * @post_condition Stop Container
	 * @expected Exception should occurs
	 **/
	public void testGetNumberOfConfiguredResourcesBundleID_ESV_N() {
		try {
			m_ResourceContainer.startContainer(configFile);

			int confRestCount = m_ResourceContainer
					.getNumberOfConfiguredResources(EMPTY_STRING);

			if (confRestCount > n_ZERO) {
				fail("getNumberOfConfiguredResources() returns greater than zero when bundleId is empty");
			}

			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
			m_ResourceContainer.stopContainer();
		}
	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'getNumberOfConfiguredResources' function with NULL
	 *            Resource BundleId
	 * @target public void getNumberOfConfiguredResources(BundleResource
	 *         resource)
	 * @test_data configFile
	 * @pre_condition Start Container
	 * @procedure Perform getNumberOfConfiguredResources() API with
	 *            DISCOMFORT_BUNDLE_ID = null
	 * @post_condition Stop Container
	 * @expected Exception should occurs
	 **/
	public void testGetNumberOfConfiguredResourcesBundleID_NV_N() {
		try {
			m_ResourceContainer.startContainer(configFile);

			int confRestCount = m_ResourceContainer
					.getNumberOfConfiguredResources(null);

			if (confRestCount > n_ZERO) {
				fail("getNumberOfConfiguredResources() returns greater than zero when bundleId is null");
			}

			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
			m_ResourceContainer.stopContainer();
		}
	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'getConfiguredResourceParams' function with postively
	 *            valid Resource BundleId
	 * @target public void getConfiguredResourceParams(BundleResource resource,
	 *         int redID)
	 * @test_data configFile
	 * @pre_condition Start Container
	 * @procedure Perform getNumberOfConfiguredResources() API with
	 *            DISCOMFORT_BUNDLE_ID and resId = 0
	 * @post_condition Stop Container
	 * @expected Return a Strign Array Length > 0 and No Exception occurs
	 **/
	public void testGetConfiguredResourceParams_SRC_P() {
		try {
			m_ResourceContainer.startContainer(configFile);

			resourceParam = m_ResourceContainer.getConfiguredResourceParams(
					DI_BUNDLE_ID, n_ZERO);

			if (resourceParam.length <= n_ZERO) {
				fail("getConfiguredResourceParams returns a string array Size"
						+ resourceParam.length);
			}

			for (int i = n_ZERO; i < resourceParam.length; i++) {
				Log.i(TAG, "resourceParam - [" + i + "] = " + resourceParam[i]);
			}

			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
			fail("Can't  get configured resource parameters because of the exception: "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'getConfiguredResourceParams' function negatively with
	 *            negative redId
	 * @target public void getConfiguredResourceParams(BundleResource resource,
	 *         int redID)
	 * @test_data ConfigFile
	 * @pre_condition Start Container
	 * @procedure Perform getNumberOfConfiguredResources() API with
	 *            DISCOMFORT_BUNDLE_ID and resId = -1
	 * @post_condition Stop Container
	 * @expected Should handle the Exception
	 **/
	public void testGetConfiguredResourceParams_LOBV_N() {
		try {
			m_ResourceContainer.startContainer(configFile);

			resourceParam = m_ResourceContainer.getConfiguredResourceParams(
					DI_BUNDLE_ID, n_NEGATIVE_ONE);

			if (resourceParam.length <= n_ZERO) {
				fail("getConfiguredResourceParams returns a string array Size"
						+ resourceParam.length);
			}

			for (int i = n_ZERO; i < resourceParam.length; i++) {
				Log.i(TAG, "resourceParam - [" + i + "] = " + resourceParam[i]);
			}

			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'getConfiguredResourceParams' function empty Resource
	 *            BundleId
	 * @target public void getConfiguredResourceParams(BundleResource resource,
	 *         int redID)
	 * @test_data configFile
	 * @pre_condition Start Container
	 * @procedure Perform getNumberOfConfiguredResources() API with
	 *            DISCOMFORT_BUNDLE_ID = ""
	 * @post_condition Stop Container
	 * @expected Should handle the Exception
	 **/
	public void testGetConfiguredResourceParamsBundleID_ESV_N() {
		try {
			m_ResourceContainer.startContainer(configFile);

			resourceParam = m_ResourceContainer.getConfiguredResourceParams(
					EMPTY_STRING, n_ZERO);

			if (resourceParam.length < n_ZERO) {
				fail("getConfiguredResourceParams returns a string array Size"
						+ resourceParam.length);
			}

			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
			m_ResourceContainer.stopContainer();
		}

	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'getConfiguredResourceParams' function with NULL Resource
	 *            BundleId
	 * @target public void getConfiguredResourceParams(BundleResource resource,
	 *         int redID)
	 * @test_data configFile
	 * @pre_condition Start Container
	 * @procedure Perform getNumberOfConfiguredResources() API with
	 *            DISCOMFORT_BUNDLE_ID = null
	 * @post_condition Stop Container
	 * @expected Should handle the Exception
	 **/
	public void testGetConfiguredResourceParamsBundleID_NV_N() {
		try {
			m_ResourceContainer.startContainer(configFile);

			m_ResourceContainer.getConfiguredResourceParams(null, n_ZERO);

			resourceParam = m_ResourceContainer.getConfiguredResourceParams(
					null, n_ZERO);

			if (resourceParam.length < n_ZERO) {
				fail("getConfiguredResourceParams returns a string array Size "
						+ resourceParam.length);
			}

			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
			m_ResourceContainer.stopContainer();
		}
	}
}
