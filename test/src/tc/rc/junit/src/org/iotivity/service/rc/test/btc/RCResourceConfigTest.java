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

import java.util.List;

import static org.iotivity.service.rc.test.helper.RCCommonUtilBTC.*;
import org.iotivity.service.rc.test.helper.RCHelperBTC;

import org.iotivity.service.resourcecontainer.RcsResourceContainer;
import org.iotivity.service.resourcecontainer.ResourceConfig;

import android.content.Context;
import android.test.InstrumentationTestCase;
import android.util.Log;

public class RCResourceConfigTest extends InstrumentationTestCase {
	private RcsResourceContainer m_ResourceContainer;
	private Context m_Context = null;
	private RCHelperBTC m_RCHelper;
	private static boolean isCopied = false;
	private String CONFIG_FILE;
	private String sdCardPath;
	public static String name;
	public static String resURI;
	public static String resType;
	public static String address;
	public static String resString;

	protected void setUp() throws Exception {
		super.setUp();

		m_Context = getInstrumentation().getTargetContext();

		m_RCHelper = RCHelperBTC.getInstance();

		isCopied = m_RCHelper.copyFilesToDevice(m_Context);

		sdCardPath = m_RCHelper.getFilesPath(m_Context);

		CONFIG_FILE = sdCardPath + RESOURCE_CONTAINER_CONFIG_XML;

		if (!isCopied) {
			Log.d(TAG, "Can't copy the files from assets/lib folder");
		} else {
			Log.d(TAG, "Successfully copied files from assets/lib folder");
		}

		m_ResourceContainer = new RcsResourceContainer(m_Context);
	}

	protected void tearDown() throws Exception {
		super.tearDown();
	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'getName' function positively [SRC]
	 * @target public String getName()
	 * @test_data 1. Config File 2. DI_BUNDLE_ID
	 * @pre_condition Start Container
	 * @procedure Perform getName()
	 * @post_condition Stop Container
	 * @expected Return the name of a resource
	 **/
	public void testGetName_SRC_P() {

		try {

			m_ResourceContainer.startContainer(CONFIG_FILE);

			List<ResourceConfig> resourceConfigList = m_ResourceContainer
					.getConfiguredBundleResources(DI_BUNDLE_ID);
			Log.i(TAG, "Size of resource configuration list = "
					+ resourceConfigList.size());

			name = resourceConfigList.get(0).getName();

			Log.i(TAG, "Resoure Name = " + name);

			if (!name.equalsIgnoreCase(DI_BUNDLE_RESOURCE1_NAME)) {
				fail("getName API provided wrong name");
			}

			m_ResourceContainer.stopContainer();

		} catch (Exception e) {
			fail("Can't  get name because of the exception: "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'setName' function positively [SRC]
	 * @target public void setName()
	 * @test_data 1. Config File 2. DI_BUNDLE_ID
	 * @pre_condition Start Container
	 * @procedure Perform setName()
	 * @post_condition Stop Container
	 * @expected Set the name of a resource
	 **/
	public void testSetName_SRC_P() {

		try {

			m_ResourceContainer.startContainer(CONFIG_FILE);

			List<ResourceConfig> resourceConfigList = m_ResourceContainer
					.getConfiguredBundleResources(DI_BUNDLE_ID);
			Log.i(TAG, "Size of resource configuration list = "
					+ resourceConfigList.size());

			resourceConfigList.get(0).setName(DI_BUNDLE_RESOURCE1_NAME_SET);

			name = resourceConfigList.get(0).getName();

			Log.i(TAG, "Resoure Name = " + name);

			if (!name.equalsIgnoreCase(DI_BUNDLE_RESOURCE1_NAME_SET)) {
				fail("setName API provided wrong name");
			}

			m_ResourceContainer.stopContainer();

		} catch (Exception e) {
			fail("Can't set name because of the exception: "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'getURI' function positively [SRC]
	 * @target public String getName()
	 * @test_data 1. Config File 2. DI_BUNDLE_ID
	 * @pre_condition Start Container
	 * @procedure Perform getURI()
	 * @post_condition Stop Container
	 * @expected Return the URI of a resource
	 **/
	public void testGetURI_SRC_P() {

		try {

			m_ResourceContainer.startContainer(CONFIG_FILE);

			List<ResourceConfig> resourceConfigList = m_ResourceContainer
					.getConfiguredBundleResources(DI_BUNDLE_ID);
			Log.i(TAG, "Size of resource configuration list = "
					+ resourceConfigList.size());

			resURI = resourceConfigList.get(0).getURI();

			Log.i(TAG, "Resoure URI = " + resURI);

			if (!resURI.equalsIgnoreCase(DI_BUNDLE_RESOURCE1_URI)) {
				fail("getURI API provided wrong URI");
			}

			m_ResourceContainer.stopContainer();

		} catch (Exception e) {
			fail("Can't  get URI because of the exception: "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'setURI' function positively [SRC]
	 * @target public void setURI()
	 * @test_data 1. Config File 2. DI_BUNDLE_ID
	 * @pre_condition Start Container
	 * @procedure Perform setURI()
	 * @post_condition Stop Container
	 * @expected Set the URI of a resource
	 **/
	public void testSetURI_SRC_P() {

		try {
			m_ResourceContainer.startContainer(CONFIG_FILE);

			List<ResourceConfig> resourceConfigList = m_ResourceContainer
					.getConfiguredBundleResources(DI_BUNDLE_ID);
			Log.i(TAG, "Size of resource configuration list = "
					+ resourceConfigList.size());

			resourceConfigList.get(0).setURI(DI_BUNDLE_RESOURCE1_URI_SET);

			resURI = resourceConfigList.get(0).getURI();

			Log.i(TAG, "Resoure URI = " + resURI);

			if (!resURI.equalsIgnoreCase(DI_BUNDLE_RESOURCE1_URI_SET)) {
				fail("getURI API provided wrong URI");
			}

			m_ResourceContainer.stopContainer();

		} catch (Exception e) {
			fail("Can't  get URI because of the exception: "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'getResourceType' function positively [SRC]
	 * @target public String getResourceType()
	 * @test_data 1. Config File 2. DI_BUNDLE_ID
	 * @pre_condition Start Container
	 * @procedure Perform getResourceType()
	 * @post_condition Stop Container
	 * @expected Return the Resource Type of a resource
	 **/
	public void testGetResourceType_SRC_P() {

		try {

			m_ResourceContainer.startContainer(CONFIG_FILE);

			List<ResourceConfig> resourceConfigList = m_ResourceContainer
					.getConfiguredBundleResources(DI_BUNDLE_ID);
			Log.i(TAG, "Size of resource configuration list = "
					+ resourceConfigList.size());

			resType = resourceConfigList.get(0).getResourceType();

			Log.i(TAG, "Resoure URI = " + resType);

			if (!resType.equalsIgnoreCase(DI_RESOURCE_TYPE)) {
				fail("getURI API provided wrong URI");
			}

			m_ResourceContainer.stopContainer();

		} catch (Exception e) {
			fail("Can't  get URI because of the exception: "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'getResourceType' function positively [SRC]
	 * @target public void setResourceType()
	 * @test_data 1. Config File 2. DI_BUNDLE_ID
	 * @pre_condition Start Container
	 * @procedure Perform getResourceType()
	 * @post_condition Stop Container
	 * @expected Set the Resource Type of a resource
	 **/
	public void testSetResourceType_SRC_P() {

		try {

			m_ResourceContainer.startContainer(CONFIG_FILE);

			List<ResourceConfig> resourceConfigList = m_ResourceContainer
					.getConfiguredBundleResources(DI_BUNDLE_ID);
			Log.i(TAG, "Size of resource configuration list = "
					+ resourceConfigList.size());

			resourceConfigList.get(0).setResourceType(DI_RESOURCE_TYPE_SET);

			resType = resourceConfigList.get(0).getResourceType();

			Log.i(TAG, "Resoure URI = " + resType);

			if (!resType.equalsIgnoreCase(DI_RESOURCE_TYPE_SET)) {
				fail("getURI API provided wrong URI");
			}

			m_ResourceContainer.stopContainer();

		} catch (Exception e) {
			fail("Can't  get URI because of the exception: "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'getAddress' function positively [SRC]
	 * @target public String getAddress()
	 * @test_data 1. Config File 2. DI_BUNDLE_ID
	 * @pre_condition Start Container
	 * @procedure Perform getAddress()
	 * @post_condition Stop Container
	 * @expected Return the Address of a resource
	 **/
	public void testGetAddress_SRC_P() {

		try {

			m_ResourceContainer.startContainer(CONFIG_FILE);

			List<ResourceConfig> resourceConfigList = m_ResourceContainer
					.getConfiguredBundleResources(DI_BUNDLE_ID);
			Log.i(TAG, "Size of resource configuration list = "
					+ resourceConfigList.size());

			address = resourceConfigList.get(0).getAddress();

			Log.i(TAG, "Address = " + address);

			if (!address.equalsIgnoreCase(DI_RESOURCE_ADDRESS)) {
				fail("getURI API provided wrong URI");
			}

			m_ResourceContainer.stopContainer();

		} catch (Exception e) {
			fail("Can't  get URI because of the exception: "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'setAddress' function positively [SRC]
	 * @target public void setAddress()
	 * @test_data 1. Config File 2. DI_BUNDLE_ID
	 * @pre_condition Start Container
	 * @procedure Perform setAddress()
	 * @post_condition Stop Container
	 * @expected Set the Address Type of a resource
	 **/
	public void testSetAddress_SRC_P() {

		try {

			m_ResourceContainer.startContainer(CONFIG_FILE);

			List<ResourceConfig> resourceConfigList = m_ResourceContainer
					.getConfiguredBundleResources(DI_BUNDLE_ID);
			Log.i(TAG, "Size of resource configuration list = "
					+ resourceConfigList.size());

			resourceConfigList.get(0).setAddress(DI_RESOURCE_ADDRESS_SET);

			address = resourceConfigList.get(0).getAddress();

			Log.i(TAG, "Resoure URI = " + address);

			if (!address.equalsIgnoreCase(DI_RESOURCE_ADDRESS_SET)) {
				fail("getURI API provided wrong URI");
			}

			m_ResourceContainer.stopContainer();

		} catch (Exception e) {
			fail("Can't  get URI because of the exception: "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'toString' function positively [SRC]
	 * @target public toString()
	 * @test_data 1. Config File 2. DI_BUNDLE_ID
	 * @pre_condition Start Container
	 * @procedure Perform getAddress()
	 * @post_condition Stop Container
	 * @expected Return the string representation of a resource
	 **/
	public void testToString_SRC_P() {

		try {

			m_ResourceContainer.startContainer(CONFIG_FILE);

			List<ResourceConfig> resourceConfigList = m_ResourceContainer
					.getConfiguredBundleResources(DI_BUNDLE_ID);
			Log.i(TAG, "Size of resource configuration list = "
					+ resourceConfigList.size());

			resString = resourceConfigList.get(0).toString();

			Log.i(TAG, "Resoure String = " + resString);

			m_ResourceContainer.stopContainer();

		} catch (Exception e) {
			fail("Can't  convert to string because of the exception: "
					+ e.getLocalizedMessage());
		}
	}

}
