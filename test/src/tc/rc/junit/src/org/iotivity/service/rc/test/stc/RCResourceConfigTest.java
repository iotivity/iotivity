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

import java.util.List;

import org.iotivity.service.rc.test.helper.RCHelper;

import static org.iotivity.service.rc.test.helper.RCCommonUtil.*;

import org.iotivity.service.resourcecontainer.RcsResourceContainer;
import org.iotivity.service.resourcecontainer.ResourceConfig;

import android.content.Context;
import android.test.InstrumentationTestCase;
import android.util.Log;

public class RCResourceConfigTest extends InstrumentationTestCase {

	private RcsResourceContainer m_ResourceContainer;
	private Context m_Context = null;
	private RCHelper m_RCHelper;
	private static boolean isCopied = false;
	private String configFile;
	private String sdCardPath;

	protected void setUp() throws Exception {
		super.setUp();

		m_Context = getInstrumentation().getTargetContext();

		m_RCHelper = RCHelper.getInstance();

		isCopied = m_RCHelper.copyFilesToDevice(m_Context);

		sdCardPath = m_RCHelper.getFilesPath(getInstrumentation()
				.getTargetContext()) + PATH_INDICATOR;

		configFile = sdCardPath + RESOURCE_CONTAINER_CONFIG_XML;

		if (!isCopied) {
			Log.d(TAG, "Can't copy the files from assets/lib folder");
		} else {
			Log.d(TAG, "Successfully copied files from assets/lib folder");
		}

		m_ResourceContainer = new RcsResourceContainer(m_Context);
	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'setName' function with DI Bundle Resource Name
	 * @target public void setName()
	 * @test_data configFile
	 * @pre_condition Start Container
	 * @procedure Perform setName()
	 * @post_condition Stop Container
	 * @expected Set the name of a resource
	 **/
	public void testSetGetName_GSRV_P() {

		try {

			m_ResourceContainer.startContainer(configFile);

			List<ResourceConfig> resourceConfigList = m_ResourceContainer
					.getConfiguredBundleResources(DI_BUNDLE_ID);

			resourceConfigList.get(0).setName(DI_BUNDLE_RESOURCE1_NAME_SET);

			String name = resourceConfigList.get(0).getName();

			assertEquals(name, DI_BUNDLE_RESOURCE1_NAME_SET);

			m_ResourceContainer.stopContainer();

		} catch (Exception e) {
			fail("Can't set name because of the exception: "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'setURI' function with DI Bundle Resource URI
	 * @target public void setURI()
	 * @test_data configFile
	 * @pre_condition Start Container
	 * @procedure Perform setURI()
	 * @post_condition Stop Container
	 * @expected Set the URI of a resource
	 **/
	public void testSetGetURI_GSRV_P() {

		try {

			m_ResourceContainer.startContainer(configFile);

			List<ResourceConfig> resourceConfigList = m_ResourceContainer
					.getConfiguredBundleResources(DI_BUNDLE_ID);

			resourceConfigList.get(0).setURI(DI_BUNDLE_RESOURCE1_URI_SET);

			String resURI = resourceConfigList.get(0).getURI();

			assertEquals(resURI, DI_BUNDLE_RESOURCE1_URI_SET);

			m_ResourceContainer.stopContainer();

		} catch (Exception e) {
			fail("Can't  get URI because of the exception: "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'getResourceType' function with DI Bundle Resource Type
	 * @target public void getResourceType()
	 * @test_data configFile
	 * @pre_condition Start Container
	 * @procedure Perform getResourceType()
	 * @post_condition Stop Container
	 * @expected Set the Resource Type of a resource
	 **/
	public void testSetGetResourceType_GSRV_P() {

		try {

			m_ResourceContainer.startContainer(configFile);

			List<ResourceConfig> resourceConfigList = m_ResourceContainer
					.getConfiguredBundleResources(DI_BUNDLE_ID);

			resourceConfigList.get(0).setResourceType(
					DI_SENSOR_RESOURCE_TYPE_SET);

			String resType = resourceConfigList.get(0).getResourceType();

			assertEquals(resType, DI_SENSOR_RESOURCE_TYPE_SET);

			m_ResourceContainer.stopContainer();

		} catch (Exception e) {
			fail("Can't  get URI because of the exception: "
					+ e.getLocalizedMessage());
		}
	}

	/**
	 * @since 2016-03-10
	 * @see StartContainer
	 * @objective Test 'setAddress' function with DI Bundle Resource Address
	 * @target public void setAddress()
	 * @test_data configFile
	 * @pre_condition Start Container
	 * @procedure Perform setAddress()
	 * @post_condition Stop Container
	 * @expected Set the Address Type of a resource
	 **/
	public void testSetGetAddress_GSRV_P() {

		try {

			m_ResourceContainer.startContainer(configFile);

			List<ResourceConfig> resourceConfigList = m_ResourceContainer
					.getConfiguredBundleResources(DI_BUNDLE_ID);

			resourceConfigList.get(0)
					.setAddress(DI_SENSOR_RESOURCE_ADDRESS_SET);

			String address = resourceConfigList.get(0).getAddress();

			assertEquals(address, DI_SENSOR_RESOURCE_ADDRESS_SET);

			m_ResourceContainer.stopContainer();

		} catch (Exception e) {
			fail("Can't  get URI because of the exception: "
					+ e.getLocalizedMessage());
		}
	}

}
