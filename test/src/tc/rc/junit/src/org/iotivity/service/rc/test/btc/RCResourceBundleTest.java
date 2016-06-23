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

import static org.iotivity.service.rc.test.helper.RCCommonUtilBTC.*;
import org.iotivity.service.rc.test.helper.RCHelperBTC;

import org.iotivity.service.resourcecontainer.BundleResource;
import org.iotivity.service.resourcecontainer.RcsBundleInfo;
import org.iotivity.service.resourcecontainer.RcsResourceContainer;

import android.content.Context;
import android.test.InstrumentationTestCase;
import android.util.Log;

public class RCResourceBundleTest extends InstrumentationTestCase {
	private RcsResourceContainer m_ResourceContainer;
	private BundleResource m_BundleResource;
	private Context m_Context = null;
	private RCHelperBTC m_RCHelper;
	private static boolean isCopied = false;
	private String configFile;
	private String emptyConfigFile;
	private String sdCardPath;

	public static final int ZERO_VALUE = 0;

	protected void setUp() throws Exception {
		super.setUp();

		m_Context = getInstrumentation().getTargetContext();

		m_RCHelper = RCHelperBTC.getInstance();

		isCopied = m_RCHelper.copyFilesToDevice(m_Context);

		sdCardPath = m_RCHelper.getFilesPath(m_Context);

		configFile = sdCardPath + RESOURCE_CONTAINER_CONFIG_XML;
		emptyConfigFile = sdCardPath + RESOURCE_CONTAINER_EMPTY_CONFIG_XML;

		if (!isCopied) {
			Log.d(TAG, "Can't copy the files from assets/lib folder");
		} else {
			Log.d(TAG, "Successfully copied files from assets/lib folder");
		}

		m_ResourceContainer = new RcsResourceContainer(m_Context);
		m_BundleResource = RCHelperBTC.getBundleResource(m_Context);
	}

	protected void tearDown() throws Exception {
		super.tearDown();

		if (m_ResourceContainer != null) {
			m_ResourceContainer = null;
		}

		if (m_BundleResource != null) {
			m_BundleResource.deactivateResource();
		}
	}

	/**
	 * @since 2015-10-25
	 * @see StartContainer
	 * @objective Test 'getID' function with Resource ConfigFile
	 * @target string getID(void)
	 * @test_data configFile
	 * @pre_condition set valid bundle name using setID API.
	 * @procedure Perform getID() API
	 * @post_condition StopContainer
	 * @expected getID value and bundle name is same.
	 **/
	public void testGetBundleID_SRC_P() {
		try {
			m_ResourceContainer.startContainer(configFile);
			RcsBundleInfo bundleInfo = m_ResourceContainer.listBundles().get(0);

			if (bundleInfo != null) {
				String bundleID = bundleInfo.getID();

				if (bundleID.length() == ZERO_VALUE) {
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
	 * @since 2015-10-25
	 * @see StartContainer
	 * @objective Test 'getID' function with empty Resource ConfigFile
	 * @target string getID(void)
	 * @test_data emptyConfigFile
	 * @pre_condition None
	 * @procedure Perform getID() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testGetBundleID_ESV_N() {
		try {
			m_ResourceContainer.startContainer(emptyConfigFile);
			RcsBundleInfo bundleInfo = m_ResourceContainer.listBundles().get(0);
			String bundleID = bundleInfo.getID();

			if (bundleID.length() > ZERO_VALUE) {
				fail("Got bundleID for empty config file.");
			}
		} catch (Exception e) {
			Log.d(TAG,
					"Exception occurred inside GetBundleID_N. "
							+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2015-10-25
	 * @see StartContainer
	 * @objective Test 'getID' function with Resource ConfigFile
	 * @target string getPath(void)
	 * @test_data configFile
	 * @pre_condition set valid bundle path using setPath API.
	 * @procedure Perform getPath() API
	 * @post_condition StopContainer
	 * @expected getPath value and bundle path is same.
	 **/
	public void testGetBundlePath_SRC_P() {
		try {
			m_ResourceContainer.startContainer(configFile);
			RcsBundleInfo bundleInfo = m_ResourceContainer.listBundles().get(0);

			if (bundleInfo != null) {
				String bundlePath = bundleInfo.getPath();

				if (bundlePath.length() == ZERO_VALUE) {
					fail("getPath string is empty");
				}
			} else {
				fail("Precondition failed. Bundle info is null");
			}

			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
			fail("Exception occurred inside GetBundlePath_P. "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2015-10-25
	 * @see StartContainer
	 * @objective Test 'getPath' function with empty Resource ConfigFile
	 * @target string getPath(void)
	 * @test_data emptyConfigFile
	 * @pre_condition None
	 * @procedure Perform getPath() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testGetBundlePath_ESV_N() {
		try {
			m_ResourceContainer.startContainer(emptyConfigFile);
			RcsBundleInfo bundleInfo = m_ResourceContainer.listBundles().get(0);
			String bundlePath = bundleInfo.getPath();

			if (bundlePath.length() > ZERO_VALUE) {
				fail("Got bundleID for empty config file.");
			}
		} catch (Exception e) {
			Log.d(TAG,
					"Exception occurred inside GetBundlePath_N. "
							+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2015-10-25
	 * @see StartContainer
	 * @objective Test 'getVersion' function with Resource ConfigFile
	 * @target string getVersion()
	 * @test_data configFile
	 * @pre_condition set valid bundle version using setVersion API.
	 * @procedure Perform getVersion() API
	 * @post_condition StopContainer
	 * @expected getVersion value and bundle version is same.
	 **/
	public void testGetBundleVersion_SRC_P() {
		try {
			m_ResourceContainer.startContainer(configFile);

			RcsBundleInfo bundleInfo = m_ResourceContainer.listBundles().get(0);

			if (bundleInfo != null) {
				String version = bundleInfo.getVersion();

				if (version.length() == 0) {
					fail("getVersion string is empty");
				}
			} else {
				fail("Precondition failed. Bundle info is null");
			}

			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
			fail("Exception occurred inside GetBundleVersion_P. " + configFile);
		}
	}

	/**
	 * @since 2015-10-25
	 * @see StartContainer
	 * @objective Test 'getVersion' function with empty Resource ConfigFile
	 * @target string getVersion()
	 * @test_data emptyConfigFile
	 * @pre_condition None
	 * @procedure Perform getVersion() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testGetBundleVersion_ESV_N() {
		try {
			m_ResourceContainer.startContainer(emptyConfigFile);
			RcsBundleInfo bundleInfo = m_ResourceContainer.listBundles().get(0);
			String version = bundleInfo.getVersion();

			if (version.length() > ZERO_VALUE) {
				fail("Got getVersion for empty config file.");
			}
		} catch (Exception e) {
			Log.d(TAG,
					"Exception occurred inside GetBundleVersion_N. "
							+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2015-10-25
	 * @see StartContainer
	 * @objective Test 'getActivatorName' function with Resource ConfigFile
	 * @target string getActivatorName()
	 * @test_data configFile
	 * @pre_condition Set valid bundle version with config file
	 * @procedure Perform getActivatorName() API
	 * @post_condition StopContainer
	 * @expected getActivatorName value successfully.
	 **/
	public void testGetActivatorName_SRC_P() {
		try {
			m_ResourceContainer.startContainer(configFile);
			RcsBundleInfo bundleInfo = m_ResourceContainer.listBundles().get(0);

			if (bundleInfo != null) {
				String activator = bundleInfo.getActivatorName();

				if (activator.length() == ZERO_VALUE) {
					fail("getActivatorName string is empty");
				}
			} else {
				fail("Precondition failed. Bundle info is null");
			}

			m_ResourceContainer.stopContainer();
		} catch (Exception e) {
			fail("Exception occurred inside GetActivatorName_P. "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2015-10-25
	 * @see StartContainer
	 * @objective Test 'getActivatorName' function with empty Resource
	 *            ConfigFile
	 * @target string getActivatorName()
	 * @test_data emptyConfigFile
	 * @pre_condition None
	 * @procedure Perform getActivatorName() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testGetActivatorName_ESV_N() {
		try {
			m_ResourceContainer.startContainer(emptyConfigFile);
			RcsBundleInfo bundleInfo = m_ResourceContainer.listBundles().get(0);
			String activator = bundleInfo.getActivatorName();

			if (activator.length() > ZERO_VALUE) {
				fail("Got getActivatorName for empty config file.");
			}
		} catch (Exception e) {
			Log.d(TAG,
					"Exception occurred inside GetActivatorName_N. "
							+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-03-12
	 * @see StartContainer
	 * @objective Test 'isActivated' function with Resource ConfigFile
	 * @target string isActivated()
	 * @test_data configFile
	 * @pre_condition Set valid bundle version with config file
	 * @procedure Perform isActivated() API
	 * @post_condition StopContainer
	 * @expected getActivatorName value successfully.
	 **/
	public void testIsActivated_SRC_P() {
		m_ResourceContainer.startContainer(configFile);

		try {
			RcsBundleInfo bundleInfo = m_ResourceContainer.listBundles().get(0);

			if (bundleInfo != null) {
				bundleInfo.isActivated();
			} else {
				fail("Precondition failed. Bundle info is null");
			}
		} catch (Exception e) {
			fail("Exception occurred inside testIsActivated_P. "
					+ e.getLocalizedMessage());
		}

		m_ResourceContainer.stopContainer();
	}

	/**
	 * @since 2016-03-09
	 * @see None
	 * @objective Test 'getAttributeKeys' function with positive way
	 * @target String getAttributeKeys()
	 * @test_data None
	 * @pre_condition BundleResource class should be initialize with attribute.
	 * @procedure Perform getAttributeKeys() API
	 * @post_condition None
	 * @expected 1. Keys count should be more than 0. 2. No crash occurs
	 **/
	public void testGetAttributeKeys_SRC_P() {
		try {
			String[] keys = m_BundleResource.getAttributeKeys();

			if (keys.length <= ZERO_VALUE) {
				fail("Attribute key list should have attribute.");
			}
		} catch (Exception e) {
			Log.e(TAG, "Exception occurred inside testGetAttributeKeys_SRC_P. "
					+ e.getLocalizedMessage());
			fail("Exception occur when calling getAttributeKeys() API.");
		}
	}

	/**
	 * @since 2016-03-09
	 * @see None
	 * @objective Test 'setURI' function with HUE Bundle URI
	 * @target void setURI(String uri)
	 * @test_data BUNDLE_URI
	 * @pre_condition BundleResource class should be initialize with attribute.
	 * @procedure Perform setURI() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testSetBundleResourceURI_SRC_P() {
		try {
			m_BundleResource.setURI(HUE_BUNDLE_URI);
		} catch (Exception e) {
			Log.e(TAG,
					"Exception occurred inside testSetBundleResourceURI_SRC_P. "
							+ e.getLocalizedMessage());
			fail("Exception occur when calling setURI() API.");
		}
	}

	/**
	 * @since 2016-03-09
	 * @see None
	 * @objective Test 'setURI' function with empty string instead of HUE Bundle
	 *            URI
	 * @target void setURI(String uri)
	 * @test_data uri = ""
	 * @pre_condition BundleResource class should be initialize with attribute.
	 * @procedure Perform setURI() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testSetBundleResourceURI_ESV_N() {
		try {
			m_BundleResource.setURI(EMPTY_STRING);
		} catch (Exception e) {
			Log.e(TAG,
					"Exception occurred inside testSetBundleResourceURI_ESV_P. "
							+ e.getLocalizedMessage());
			fail("Exception occur when calling setURI() API with empty uri");
		}
	}

	/**
	 * @since 2016-03-09
	 * @see None
	 * @objective Test 'setURI' function with NULL instead of HUE Bundle URI
	 * @target void setURI(String uri)
	 * @test_data uri = null
	 * @pre_condition BundleResource class should be initialize with attribute.
	 * @procedure Perform setURI() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testSetBundleResourceURI_NV_N() {
		try {
			m_BundleResource.setURI(null);
		} catch (Exception e) {
			Log.e(TAG,
					"Exception occurred inside testSetBundleResourceURI_NV_P. "
							+ e.getLocalizedMessage());
			fail("Exception occur when calling setURI() API with null uri");
		}
	}

	/**
	 * @since 2016-03-09
	 * @see None
	 * @objective Test 'getURI' function without passing any URI
	 * @target String getURI()
	 * @test_data None
	 * @pre_condition BundleResource class should be initialize with attribute.
	 * @procedure Perform getURI() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testGetBundleResourceURI_SRC_P() {
		try {
			m_BundleResource.getURI();
		} catch (Exception e) {
			Log.e(TAG,
					"Exception occurred inside testGetBundleResourceURI_SRC_P. "
							+ e.getLocalizedMessage());
			fail("Exception occur when calling getURI() API");
		}
	}

	/**
	 * @since 2016-03-10
	 * @see None
	 * @objective Test 'setResourceType' function with Temperature Sensor Type
	 *            resource
	 * @target void setResourceType(String resourceType)
	 * @test_data ResourceType = "oic.r.temperature"
	 * @pre_condition BundleResource class should be initialize with attribute.
	 * @procedure Perform setResourceType() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testSetBundleResourceType_SRC_P() {
		try {
			m_BundleResource.setResourceType(TEMPERATURE_SENSOR_TYPE);
		} catch (Exception e) {
			Log.e(TAG,
					"Exception occurred inside testSetBundleResourceType_SRC_P. "
							+ e.getLocalizedMessage());
			fail("Exception occur when calling setResourceType() API");
		}
	}

	/**
	 * @since 2016-03-10
	 * @see None
	 * @objective Test 'setResourceType' function with empty strng instead of
	 *            Temperature Sensor resource
	 * @target void setResourceType(String resourceType)
	 * @test_data ResourceType = ""
	 * @pre_condition BundleResource class should be initialize with attribute.
	 * @procedure Perform setResourceType() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testSetBundleResourceType_ESV_N() {
		try {
			m_BundleResource.setResourceType(EMPTY_STRING);
		} catch (Exception e) {
			Log.e(TAG,
					"Exception occurred inside testSetBundleResourceType_SRC_P. "
							+ e.getLocalizedMessage());
			fail("Exception occur when calling setResourceType() API with empty type");
		}
	}

	/**
	 * @since 2016-03-10
	 * @see None
	 * @objective Test 'setResourceType' function with NULL instead of
	 *            Temperature Sensor Type resource
	 * @target void setResourceType(String resourceType)
	 * @test_data ResourceType = null
	 * @pre_condition BundleResource class should be initialize with attribute.
	 * @procedure Perform setResourceType() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testSetBundleResourceType_NV_N() {
		try {
			m_BundleResource.setResourceType(null);
		} catch (Exception e) {
			Log.e(TAG,
					"Exception occurred inside testSetBundleResourceType_NV_P. "
							+ e.getLocalizedMessage());
			fail("Exception occur when calling setResourceType() API with null type");
		}
	}

	/**
	 * @since 2016-03-10
	 * @see None
	 * @objective Test 'getResourceType' function without passing resource type
	 * @target String getResourceType()
	 * @test_data None
	 * @pre_condition BundleResource class should be initialize with attribute.
	 * @procedure Perform getResourceType() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testGetBundleResourceType_SRC_P() {
		try {
			m_BundleResource.getResourceType();
		} catch (Exception e) {
			Log.e(TAG,
					"Exception occurred inside testGetBundleResourceType_SRC_P. "
							+ e.getLocalizedMessage());
			fail("Exception occur when calling getResourceType() API");
		}
	}

	/**
	 * @since 2016-03-10
	 * @see None
	 * @objective Test 'setAddress' function with HUE Bundle URI
	 * @target void setAddress(String address)
	 * @test_data Address = "bundle/uri"
	 * @pre_condition BundleResource class should be initialize with attribute.
	 * @procedure Perform setAddress() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testSetBundleAddress_SRC_P() {
		try {
			m_BundleResource.setAddress(HUE_BUNDLE_URI);
		} catch (Exception e) {
			Log.e(TAG, "Exception occurred inside testSetBundleAddress_SRC_P. "
					+ e.getLocalizedMessage());
			fail("Exception occur when calling setAddress() API");
		}
	}

	/**
	 * @since 2016-03-10
	 * @see None
	 * @objective Test 'setAddress' function with empty string instead of HUE
	 *            Bundle URI
	 * @target void setAddress(String address)
	 * @test_data Address = ""
	 * @pre_condition BundleResource class should be initialize with attribute.
	 * @procedure Perform setAddress() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testSetBundleAddress_ESV_N() {
		try {
			m_BundleResource.setAddress(EMPTY_STRING);
		} catch (Exception e) {
			Log.e(TAG, "Exception occurred inside testSetBundleAddress_ESV_P. "
					+ e.getLocalizedMessage());
			fail("Exception occur when calling setAddress() API with empty type");
		}
	}

	/**
	 * @since 2016-03-10
	 * @see None
	 * @objective Test 'setAddress' function with NULL instead of HUE Bundle URI
	 * @target void setAddress(String address)
	 * @test_data Address = null
	 * @pre_condition BundleResource class should be initialize with attribute.
	 * @procedure Perform setAddress() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testSetBundleAddress_NV_N() {
		try {
			m_BundleResource.setAddress(null);
		} catch (Exception e) {
			Log.e(TAG, "Exception occurred inside testSetBundleAddress_NV_P. "
					+ e.getLocalizedMessage());
			fail("Exception occur when calling setAddress() API with null type");
		}
	}

	/**
	 * @since 2016-03-10
	 * @see None
	 * @objective Test 'getAddress' function without passing resource URI
	 * @target String getAddress()
	 * @test_data None
	 * @pre_condition BundleResource class should be initialize with attribute.
	 * @procedure Perform getAddress() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testGetBundleAddress_SRC_P() {
		try {
			m_BundleResource.getAddress();
		} catch (Exception e) {
			Log.e(TAG, "Exception occurred inside testGetBundleAddress_SRC_P. "
					+ e.getLocalizedMessage());
			fail("Exception occur when calling getAddress() API");
		}
	}

	/**
	 * @since 2016-03-10
	 * @see None
	 * @objective Test 'setName' function with Temperature Sensor Name
	 * @target void setName(String name)
	 * @test_data name = "temperatureSensor"
	 * @pre_condition BundleResource class should be initialize with attribute.
	 * @procedure Perform setName() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testSetBundleName_SRC_P() {
		try {
			m_BundleResource.setName(TEMPERATURE_SENSOR_NAME);
		} catch (Exception e) {
			Log.e(TAG, "Exception occurred inside testSetBundleName_SRC_P. "
					+ e.getLocalizedMessage());
			fail("Exception occur when calling setName() API");
		}
	}

	/**
	 * @since 2016-03-10
	 * @see None
	 * @objective Test 'setName' function with empty string instead of
	 *            Temperature Sensor Name
	 * @target void setName(String name)
	 * @test_data name = ""
	 * @pre_condition BundleResource class should be initialize with attribute.
	 * @procedure Perform setName() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testSetBundleName_ESV_N() {
		try {
			m_BundleResource.setName(EMPTY_STRING);
		} catch (Exception e) {
			Log.e(TAG, "Exception occurred inside testSetBundleName_ESV_P. "
					+ e.getLocalizedMessage());
			fail("Exception occur when calling setName() API with empty type");
		}
	}

	/**
	 * @since 2016-03-10
	 * @see None
	 * @objective Test 'setName' function with NULL instead of Temperature
	 *            Sensor Name
	 * @target void setName(String name)
	 * @test_data name = null
	 * @pre_condition BundleResource class should be initialize with attribute.
	 * @procedure Perform setName() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testSetBundleName_NV_N() {
		try {
			m_BundleResource.setName(null);
		} catch (Exception e) {
			Log.e(TAG,
					"Exception occurred inside testSetBundleName_NV_P. "
							+ e.getLocalizedMessage());
			fail("Exception occur when calling setName() API with null type");
		}
	}

	/**
	 * @since 2016-03-10
	 * @see None
	 * @objective Test 'getName' function without passing resource type
	 * @target String getName()
	 * @test_data None
	 * @pre_condition BundleResource class should be initialize with attribute.
	 * @procedure Perform getName() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testGetBundleName_SRC_P() {
		try {
			m_BundleResource.getName();
		} catch (Exception e) {
			Log.e(TAG, "Exception occurred inside testGetBundleName_SRC_P. "
					+ e.getLocalizedMessage());
			fail("Exception occur when calling getName() API");
		}
	}
}