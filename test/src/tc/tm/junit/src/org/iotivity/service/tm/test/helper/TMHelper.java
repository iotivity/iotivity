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

package org.iotivity.service.tm.test.helper;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcResource.OnObserveListener;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.service.tm.Action;
import org.iotivity.service.tm.ActionSet;
import org.iotivity.service.tm.Capability;
import org.iotivity.service.tm.GroupManager;
import org.iotivity.service.tm.GroupManager.IActionListener;
import org.iotivity.service.tm.GroupManager.IFindCandidateResourceListener;
import org.iotivity.service.tm.GroupManager.ISubscribePresenceListener;
import org.iotivity.service.tm.OCStackResult;
import org.iotivity.service.tm.ThingsConfiguration;
import org.iotivity.service.tm.ThingsConfiguration.IConfigurationListener;
import org.iotivity.service.tm.ThingsMaintenance;
import org.iotivity.service.tm.ThingsMaintenance.IThingsMaintenanceListener;

import android.util.Log;

public class TMHelper implements IConfigurationListener,
		IThingsMaintenanceListener, ISubscribePresenceListener,
		IFindCandidateResourceListener {

	private static TMHelper tmHelperInstance = null;

	public final String RESOURCE_TYPE_LIGHT = "core.light";
	public final String RESOURCE_TYPE_FAN = "core.fan";
	public final String RESOURCE_TYPE_DUMMY = "core.dummy";
	public final String RESOURCE_TYPE_DUMMY_1 = "core.dummy-1";
	public final String RESOURCE_TYPE_DUMMY_2 = "core.dummy-2";
	public final String RESOURCE_TYPE_DUMMY_3 = "core.dummy-3";

	public final String COLLECTION_TYPE_ROOM_SMALL = "core.room-small";
	public final String COLLECTION_TYPE_ROOM_MEDIUM = "core.room-medium";
	public final String COLLECTION_TYPE_ROOM_LARGE = "core.room-large";
	public final String EMPTY_STRING = "";

	public final int RESOURCE_COUNT_LIGHT = 3;
	public final int RESOURCE_COUNT_FAN = 2;

	public final int MAX_RESOURCE_COUNT = 10;

	public final int INT_ZERO = 0;
	public final int INT_ONE = 1;
	public final int INT_TWO = 2;
	public final int INT_MINUS_ONE = -1;

	public final int CALLBACK_WAIT_DEFAULT = 5;
	public final int CALLBACK_WAIT_MAX = 10;
	public final int CALLBACK_WAIT_MIN = 1;
	public final int CALLBACK_WAIT_NONE = 0;
	public final int SUCCESS_RESPONSE = 0;
	private final String LOG_TAG = this.getClass().getSimpleName();

	public GroupManager groupManager;

	public ThingsConfiguration thingsConfiguration;
	public ThingsMaintenance thingsMaintenance;

	public String errorMsg = "";
	public OCStackResult result = OCStackResult.OC_STACK_OK;
	public int resourceFoundCallbackCount;

	public Map<OCStackResult, String> resultMap;
	public Vector<OcResource> resourceList = new Vector<OcResource>();
	public Vector<OcResourceHandle> resourceHandleList = new Vector<OcResourceHandle>();

	private Vector<OcHeaderOption> receivedHeaderOptions;
	private OcRepresentation representation;
	private OCStackResult presenceCallbackResult = OCStackResult.OC_STACK_OK;
	private int configReceivedCallbackCount;
	private int presenceReceivedCallbackCount;
	private int callbackErrorCode;

	private String defaultRegionValue;
	private String defaultTimeValue;
	private String defaultCurrentTimeValue;
	private String defaultNetworkValue;
	private String defaultIPAddressValue;
	private String defaultSecurityValue;
	private String defaultModeValue;
	private String defaultConfigurationValue;
	private String defaultFactorySetValue;

	private String logMessage = "";

	private String resourceString = "";

	private TMHelper() {
		initializeTMHelper();
	}

	/**
	 * @brief Function is for getting singleton instance of TMHelper
	 * @return singleton instance of TMHelper
	 */
	public static TMHelper getInstance() {
		Lock mutex = new ReentrantLock();

		if (tmHelperInstance == null) {
			mutex.lock();
			if (tmHelperInstance == null) {
				Log.i("TMHelper", "Inside Helper");
				tmHelperInstance = new TMHelper();
			}
			mutex.unlock();
		}

		return tmHelperInstance;
	}

	/**
	 * @brief Function is for intializing tm class objects
	 */
	void initializeTMHelper() {
		groupManager = new GroupManager();

		thingsConfiguration = ThingsConfiguration.getInstance();
		thingsMaintenance = ThingsMaintenance.getInstance();

		groupManager.setFindCandidateResourceListener(this);

		thingsConfiguration.setConfigurationListener(this);
		thingsMaintenance.setThingsMaintenanceListener(this);
		groupManager.setSubscribePresenceListener(this);
	}

	private void onConfigurationRecieved(Vector<OcHeaderOption> headerOptions,
			OcRepresentation rep, int errorValue) {
		callbackErrorCode = errorValue;
		configReceivedCallbackCount++;
		representation = rep;
		receivedHeaderOptions = headerOptions;
		String value = "";
		value = rep.getValueString("power");
		Log.i(LOG_TAG, "power is: = " + value);
		value = rep.getValueString("intensity");
		Log.i(LOG_TAG, "intensity is: = " + value);
		value = rep.getValueString("manufacturer");
		Log.i(LOG_TAG, "manufacturer is: = " + value);
		Log.i(LOG_TAG,
				"header option size is: = " + receivedHeaderOptions.size());
	}

	@Override
	public void onUpdateConfigurationsCallback(
			Vector<OcHeaderOption> headerOptions, OcRepresentation rep,
			int errorValue) {
		Log.i(LOG_TAG, "Got Callback : onUpdateConfigurationsCallback");
		onConfigurationRecieved(headerOptions, rep, errorValue);

	}

	@Override
	public void onGetConfigurationsCallback(
			Vector<OcHeaderOption> headerOptions, OcRepresentation rep,
			int errorValue) {

		Log.i(LOG_TAG, "Got Callback : onGetConfigurationsCallback");
		onConfigurationRecieved(headerOptions, rep, errorValue);

	}

	@Override
	public void onBootStrapCallback(Vector<OcHeaderOption> headerOptions,
			OcRepresentation rep, int errorValue) {
		Log.i(LOG_TAG, "Got Callback : onBootStrapCallback");

		callbackErrorCode = errorValue;
		representation = rep;

		if (errorValue == SUCCESS_RESPONSE) {
			Log.i(LOG_TAG, "\n\nGET request was successful");
			Log.i(LOG_TAG, "\tResource URI: " + rep.getUri());

			defaultRegionValue = rep.getValueString("regionValue");
			defaultTimeValue = rep.getValueString("timeValue");
			defaultCurrentTimeValue = rep.getValueString("currentTimeValue");
			defaultNetworkValue = rep.getValueString("networkValue");
			defaultIPAddressValue = rep.getValueString("IPAddressValue");
			defaultSecurityValue = rep.getValueString("securityValue");
			defaultModeValue = rep.getValueString("modeValue");
			defaultConfigurationValue = rep
					.getValueString("configurationValue");
			defaultFactorySetValue = rep.getValueString("factorySetValue");

			Log.i(LOG_TAG, "\tregionValue : " + defaultRegionValue);
			Log.i(LOG_TAG, "\ttimeValue : " + defaultTimeValue);
			Log.i(LOG_TAG, "\tcurrentTimeValue : " + defaultCurrentTimeValue);
			Log.i(LOG_TAG, "\tnetworkValue : " + defaultNetworkValue);
			Log.i(LOG_TAG, "\tIPAddressValue : " + defaultIPAddressValue);
			Log.i(LOG_TAG, "\tsecurityValue : " + defaultSecurityValue);
			Log.i(LOG_TAG, "\tmodeValue : " + defaultModeValue);
			Log.i(LOG_TAG, "\tconfigurationValue : "
					+ defaultConfigurationValue);
			Log.i(LOG_TAG, "\tfactorySetValue : " + defaultFactorySetValue);
		} else {
			Log.e(LOG_TAG, "onGET Response error: " + callbackErrorCode);
		}
	}

	@Override
	public void onRebootCallback(Vector<OcHeaderOption> headerOptions,
			OcRepresentation rep, int errorValue) {
		Log.i(LOG_TAG, "Got Callback : onRebootCallback");
		onConfigurationRecieved(headerOptions, rep, errorValue);

	}

	@Override
	public void onFactoryResetCallback(Vector<OcHeaderOption> headerOptions,
			OcRepresentation rep, int errorValue) {
		Log.i(LOG_TAG, "Got Callback : onFactoryResetCallback");
		onConfigurationRecieved(headerOptions, rep, errorValue);

	}

	@Override
	public void onPresenceCallback(String resource, int result) {
		resourceString = resource;

		presenceCallbackResult = OCStackResult.conversion(result);
		presenceReceivedCallbackCount++;

		Log.i(LOG_TAG, "Callback result is = " + result + "; from resource = "
				+ resourceString);
	}

	@Override
	public void onResourceFoundCallback(Vector<OcResource> resources) {

		Log.i(LOG_TAG, "FindCadidateResourceListener Callback");
		resourceFoundCallbackCount++;
		if (resources.size() > INT_ZERO) {

			for (int i = 0; i < resources.size(); i++) {
				Log.i(LOG_TAG, "Resource information");
				OcResource ocResource = resources.get(i);
				if (ocResource != null) {
					resourceList.add(ocResource);
					String resourceURI = ocResource.getUri();
					String hostAddress = ocResource.getHost();

					logMessage = "API RESULT : " + "OC_STACK_OK" + "\n";
					logMessage = logMessage + "URI: " + resourceURI + "\n";
					logMessage = logMessage + "Host:" + hostAddress;
					Log.i(LOG_TAG, logMessage);
				} else {
					Log.e(LOG_TAG, "Found resource is null!!");
				}

			}

		}
	}

	/**
	 * @brief function for unregister resource
	 */
	public boolean unregisterAllResources() {
		boolean isSuccess = true;
		int startPos = 0;

		while (resourceHandleList.size() > INT_ZERO) {
			try {
				OcPlatform.unregisterResource(resourceHandleList.get(startPos));
				resourceHandleList.remove(startPos);
			} catch (OcException e) {
				e.printStackTrace();
				startPos++;
				isSuccess = false;
			}
		}
		return isSuccess;
	}

	/**
	 * @brief Function to Find Candidate Resource and Verify
	 * @param resourceTypeList
	 *            - A vector of resourceType
	 * @param expectedResult
	 *            - Result that is expected from the findCandidateResource API
	 * @param expectedResourceCount
	 *            - Expected found resource count
	 * @return int - If verified returns true otherwise false
	 */
	public boolean findCandidateResourcesAndVerify(
			Vector<String> resourceTypeList, OCStackResult expectedResult,
			int expectedResourceCount) {
		int resourceCount = 0;

		errorMsg = "From \"findCandidateResources\" Expected : "
				+ expectedResult + " Actual : ";

		resourceList.clear();
		resourceFoundCallbackCount = 0;

		OCStackResult result = groupManager.findCandidateResources(
				resourceTypeList, INT_MINUS_ONE);

		waitForResourceFound(CALLBACK_WAIT_MAX * INT_TWO);

		if (result != expectedResult) {
			errorMsg += result;
			return false;
		}

		resourceCount = resourceList.size();

		if ((resourceCount > INT_ZERO) && (resourceFoundCallbackCount != INT_ONE)) {
			errorMsg = "From \"findCandidateResources\" Callback count Expected : 1, Actual : "
					+ resourceFoundCallbackCount;
			return false;
		}
		
		if ((resourceCount == INT_ZERO) && (resourceFoundCallbackCount != INT_ZERO)) {
			errorMsg = "From \"findCandidateResources\" Callback count Expected : 0, Actual : "
					+ resourceFoundCallbackCount;
			return false;
		}

		if (expectedResult == OCStackResult.OC_STACK_OK) {
			if ((expectedResourceCount > INT_ZERO && resourceCount == INT_ZERO)
					|| (expectedResourceCount == INT_ZERO && resourceCount > INT_ZERO)) {
				errorMsg += result + "; Expected ResourceCount : "
						+ expectedResourceCount + ", Actual Resource Count: "
						+ resourceCount;
				return false;
			}
		}

		errorMsg = "";
		return true;
	}

	/**
	 * @brief Function to bind resource to group and Verify
	 * @param[in] childHandle - child handle
	 * @param[in] expectedResult - Result that is expected from the joinGroup
	 *            API
	 * @return bool - If verified returns true otherwise false
	 */
	public boolean bindResourceToGroupAndVerify(OcResourceHandle childHandle,
			OCStackResult expectedResult) {
		errorMsg = "bindResourceToGroup returns Error. Expected : "
				+ expectedResult + ", Actual : ";

		Vector<String> groupTypeList = new Vector<String>();
		Vector<String> resourceTypeList = new Vector<String>();

		resourceTypeList.add(RESOURCE_TYPE_LIGHT);
		resourceTypeList.add(RESOURCE_TYPE_FAN);

		if (!findCandidateResourcesAndVerify(resourceTypeList,
				OCStackResult.OC_STACK_OK, INT_ONE)) {
			errorMsg = "bindResourceToGroup Failed and " + errorMsg;
			return false;
		}

		OcResourceHandle collectionHandle = null;
		try {
			collectionHandle = OcPlatform.registerResource(resourceList
					.get(INT_ONE));
		} catch (OcException ocException) {
			errorMsg = "bindResourceToGroup Failed. Failed to register Group.";
			return false;
		}

		try {
			childHandle = groupManager.bindResourceToGroup(
					resourceList.get(INT_ZERO), collectionHandle);
			if (childHandle != null) {
				result = OCStackResult.OC_STACK_OK;
			} else {
				result = OCStackResult.OC_STACK_ERROR;
			}
		} catch (OcException e) {
			e.printStackTrace();
			result = OCStackResult.OC_STACK_ERROR;
		}

		if (result != expectedResult) {
			errorMsg = "bindResourceToGroup Failed. Failed to bind resource to Group. Expected : OC_STACK_OK,  Actual : "
					+ result;
			return false;
		}

		return true;
	}

	/**
	 * @brief Function to getConfiguration and Verify
	 * @param resource
	 *            - reference to resource
	 * @param expectedResult
	 *            - Result that is expected from the getConfiguration API
	 * @param isCallbackExpected
	 *            - If callback call is expected, then true, else false
	 * @param expectedErrorCode
	 *            - expected error code from callback
	 * @param expectedValue
	 *            - expected value of configuration
	 * @return bool - If verified returns true otherwise false
	 */
	public boolean getConfigurationAndVerify(OcResource resource,
			OCStackResult expectedResult, boolean isCallbackExpected,
			int expectedErrorCode, String expectedValue) {
		Vector<String> configurationNameList = new Vector<String>();
		String keyPower = "power";
		String keyIntensity = "intensity";
		configurationNameList.add(keyPower);
		configurationNameList.add(keyIntensity);

		errorMsg = "GetResourceConfiguration, Expected : " + expectedResult;

		callbackErrorCode = -1;
		configReceivedCallbackCount = 0;

		OCStackResult result;
		try {
			result = thingsConfiguration.getConfigurations(resource,
					configurationNameList);
		} catch (OcException e) {
			result = OCStackResult.OC_STACK_ERROR;
			e.printStackTrace();
		}

		waitInSecond(CALLBACK_WAIT_DEFAULT);

		if (result != expectedResult) {
			errorMsg += " Actual : " + result;
			return false;
		}

		if (configReceivedCallbackCount != (isCallbackExpected ? INT_ONE
				: INT_ZERO)) {
			errorMsg = "From \"getConfigurations\" Callback count Expected : "
					+ (isCallbackExpected ? INT_ONE : INT_ZERO) + ", Actual : "
					+ configReceivedCallbackCount;
			return false;
		}

		if (callbackErrorCode != expectedErrorCode) {
			errorMsg = "From \"getConfigurations\", callback error code mismatches. Expected :"
					+ expectedErrorCode + ", Actual : " + callbackErrorCode;
			return false;
		}

		String configurationKey = "power";

		String configurationValue = representation
				.getValueString(configurationKey);		

		if (!configurationValue.equals(expectedValue)) {
			errorMsg = "From \"getConfigurations\", got configuration mismatches. Expected power state : "
					+ expectedValue
					+ ", Actual power state : "
					+ configurationValue;
			return false;
		}

		errorMsg = "";

		return true;
	}

	/**
	 * @brief Function to updateConfiguration and Verify
	 * @param resource
	 *            - reference to resource
	 * @param expectedResult
	 *            - Result that is expected from the updateConfiguration API
	 * @param isCallbackExpected
	 *            - If callback call is expected, then true, else false
	 * @param expectedErrorCode
	 *            - expected error code from callback
	 * @param updatedValue
	 *            - value of the configuration after updateConfiguration is
	 *            called
	 * @return bool - If verified returns true otherwise false
	 */
	public boolean updateConfigurationAndVerify(OcResource resource,
			OCStackResult expectedResult, boolean isCallbackExpected,
			int expectedErrorCode, String updatedValue) {
		callbackErrorCode = -1;
		Map<String, String> configurationMap = new HashMap<String, String>();
		String cofigurationKey = "power";
		String cofigurationValue = updatedValue;
		configurationMap.put(cofigurationKey, cofigurationValue);

		errorMsg = "UpdateResourceConfiguration, Expected : " + expectedResult;

		configReceivedCallbackCount = 0;

		OCStackResult result;
		try {
			result = thingsConfiguration.updateConfigurations(resource,
					configurationMap);
		} catch (OcException e) {
			result = OCStackResult.OC_STACK_ERROR;
			e.printStackTrace();
		}

		waitInSecond(CALLBACK_WAIT_DEFAULT);

		if (result != expectedResult) {
			errorMsg += " Actual : " + result;
			return false;
		}

		if (configReceivedCallbackCount != (isCallbackExpected ? INT_ONE
				: INT_ZERO)) {
			errorMsg = "From \"updateConfigurations\" Callback not called. count Expected : "
					+ (isCallbackExpected ? INT_ONE : INT_ZERO)
					+ ", Actual : "
					+ configReceivedCallbackCount;
			return false;
		}

		if (callbackErrorCode != expectedErrorCode) {
			errorMsg = "From \"updateConfigurations\", callback error code mismatches. Expected :"
					+ expectedErrorCode + ", Actual : " + callbackErrorCode;
			return false;
		}

		String configurationKey = "power";

		String configurationValue = representation
				.getValueString(configurationKey);

		if (!configurationValue.equals(updatedValue)) {
			errorMsg = "From \"updateConfigurations\", updated configuration mismatches. Expected power state : "
					+ updatedValue
					+ ", Actual power state : "
					+ configurationValue;
			return false;
		}

		errorMsg = "";
		return true;
	}

	/**
	 * @brief Function to reboot and Verify
	 * @param resource
	 *            - reference to resource
	 * @param expectedResult
	 *            - Result that is expected from the reboot API
	 * @param expectedErrorCode
	 *            - Error code that is expected from callback
	 * @return bool - If verified returns true otherwise false
	 */
	public boolean rebootAndVerify(OcResource resource,
			OCStackResult expectedResult, int expectedErrorCode,
			String expectedlValue) {
		errorMsg = "Reboot, Expected : " + expectedResult;
		callbackErrorCode = -1;

		OCStackResult result;
		try {
			result = thingsMaintenance.reboot(resource);
		} catch (OcException e) {
			result = OCStackResult.OC_STACK_ERROR;
			e.printStackTrace();
		}

		waitInSecond(CALLBACK_WAIT_DEFAULT);

		if (result != expectedResult) {
			errorMsg += " Actual : " + result;
			return false;
		}

		if (callbackErrorCode != expectedErrorCode) {
			errorMsg = "From \"Reboot\", callback error code mismatches. Expected :"
					+ expectedErrorCode + ", Actual : " + callbackErrorCode;
			return false;
		}

		String configurationKey = "rb";

		String configurationValue = representation
				.getValueString(configurationKey);
		if (!configurationValue.equals(expectedlValue)) {
			errorMsg = "From \"reboot\", unable to reboot resource. Expected reboot state : "
					+ expectedlValue
					+ ", Actual reboot state : "
					+ configurationValue;
			return false;
		}

		errorMsg = "";
		return true;
	}

	/**
	 * @brief Function to factoryReset and Verify
	 * @param resource
	 *            - reference to resource
	 * @param expectedResult
	 *            - Result that is expected from the factoryReset API
	 * @param expectedErrorCode
	 *            - Error code that is expected from callback
	 * @return bool - If verified returns true otherwise false
	 */
	public boolean factoryResetAndVerify(OcResource resource,
			OCStackResult expectedResult, int expectedErrorCode,
			boolean supportsReset) {
		errorMsg = "FactoryReset, Expected : " + expectedResult;
		callbackErrorCode = -1;

		OCStackResult result;
		try {
			result = thingsMaintenance.factoryReset(resource);
		} catch (OcException e) {
			result = OCStackResult.OC_STACK_ERROR;
			e.printStackTrace();
		}

		waitInSecond(CALLBACK_WAIT_DEFAULT);

		if (result != expectedResult) {
			errorMsg += " Actual : " + result;
			return false;
		}

		if (callbackErrorCode != expectedErrorCode) {
			errorMsg = "From \"factoryReset\", callback error code mismatches. Expected :"
					+ expectedErrorCode + ", Actual : " + callbackErrorCode;
			return false;
		}

		String configurationKey = "power";

		String configurationValue = representation
				.getValueString(configurationKey);
		String originalValue = "off";

		if (supportsReset && (!configurationValue.equals(originalValue))) {
			errorMsg = "From \"factoryReset\", unable to revert to factory settings. Expected power state : "
					+ originalValue
					+ ", Actual power state : "
					+ configurationValue;
			return false;
		}

		if (!supportsReset && (!configurationValue.equals(EMPTY_STRING))) {
			errorMsg = "From \"factoryReset\", unable to revert to factory settings. Expected power state : , Actual power state : "
					+ configurationValue;
			return false;
		}

		errorMsg = "";
		return true;
	}

	/**
	 * @brief Function to do bootstrap and Verify
	 * @param expectedResult
	 *            - Result that is expected from the factoryReset API
	 * @param expectedErrorCode
	 *            - Error code that is expected from callback
	 * @return bool - If verified returns true otherwise false
	 */
	public boolean doBootstrapAndVerify(OCStackResult expectedResult,
			int expectedErrorCode) {
		errorMsg = "doBootstrap  Returned error. Expected : " + expectedResult;
		callbackErrorCode = -1;

		OCStackResult result = thingsConfiguration.doBootstrap();

		errorMsg += " Actual : " + result;

		waitInSecond(CALLBACK_WAIT_DEFAULT);

		if (result != expectedResult) {
			return false;
		}

		if (callbackErrorCode != expectedErrorCode) {
			errorMsg += ". But the callback returned error code. Expected = "
					+ expectedErrorCode + "; Actual = " + callbackErrorCode;
			return false;
		}

		errorMsg = "";
		return true;

	}

	/**
	 * @brief Function to subscribeCollectionPresence and Verify
	 * @param resource
	 *            - reference to resource
	 * @param expectedResult
	 *            - Result that is expected from the subscribeCollectionPresence
	 *            API
	 * @param expectedErrorCode
	 *            - Error code that is expected from callback
	 * @return bool - If verified returns true otherwise false
	 */
	public boolean subscribeCollectionPresenceAndVerify(OcResource resource,
			OCStackResult expectedResult, int expectedErrorCode) {
		errorMsg = "subscribeCollectionPresence, Expected : OC_STACK_OK, ";

		callbackErrorCode = -1;
		presenceReceivedCallbackCount = 0;
		presenceCallbackResult = OCStackResult.OC_STACK_NOTIMPL;

		OCStackResult result;
		try {
			result = groupManager.subscribeCollectionPresence(resource);
		} catch (OcException e) {
			result = OCStackResult.OC_STACK_ERROR;
			e.printStackTrace();
		}

		waitInSecond(CALLBACK_WAIT_MAX);

		if (result != OCStackResult.OC_STACK_OK) {
			errorMsg += " Actual : " + result;
			return false;
		}

		if (presenceReceivedCallbackCount != INT_ONE) {
			errorMsg = "From \"subscribeCollectionPresence\" Callback count Expected : 1, Actual : "
					+ presenceReceivedCallbackCount;
			return false;
		}

		if (presenceCallbackResult != expectedResult) {
			errorMsg = "From \"subscribeClollectionPresence\", callback error code mismatches. Expected :"
					+ expectedResult + ", Actual : " + presenceCallbackResult;
			return false;
		}

		errorMsg = "";
		return true;
	}

	/**
	 * @brief Function to get ResourceList
	 * @return OcResource- returns resourceList that is used in the Helper Class
	 */
	public Vector<OcResource> getResourceList() {
		return resourceList;
	}

	/**
	 * @brief Function to Wait until resource Found
	 * @param foundCollectionResourceList
	 *            - Resource Pointer List
	 * @param timeOut
	 *            - Time limit for waiting
	 * @return bool - Returns true if the Resource is not found within timeOut
	 *         otherwise false
	 */
	public boolean waitForResourceFound(int timeOut) {
		boolean isTimeOut = false;
		int passedTime = 0;
		while (resourceList.isEmpty()) {
			waitInSecond(CALLBACK_WAIT_MIN);
			passedTime++;
			Log.d(LOG_TAG, "Waited " + passedTime
					+ " seconds for finding resources");

			if (passedTime > timeOut) {
				isTimeOut = true;
				break;
			}
		}

		return isTimeOut;
	}

	/**
	 * @brief Helper function to find resource type in resource list
	 * @param[in] resourceList - list containing the resources to find
	 * @param[in] resourceTypeList - resource type list to find from
	 * @param[out] errorMessage - String to return error message if is are any
	 * @return if all resources are found , true is returned, else false is
	 *         returned
	 */
	public boolean containsResource(Vector<OcResource> resourceList,
			Vector<String> resourceTypeList) {
		boolean isAllEnlisted = true;
		errorMsg += "TestCase Failed.";

		for (String resourceType : resourceTypeList) {
			boolean isEnlisted = false;
			for (OcResource resource : resourceList) {

				List<String> typeList = resource.getResourceTypes();
				String type = typeList.get(0);

				if (resourceType.equals(type)) {
					isEnlisted = true;
					Log.i("Resource Type Found: %s", resourceType);
					break;
				}
			}

			if (!isEnlisted) {
				isAllEnlisted = false;
				errorMsg += " The following resource type not found: "
						+ resourceType + "\n";

			}

		}

		return isAllEnlisted;
	}

	/**
	 * @brief function for waiting
	 * @param[in] second - seconds to wait
	 */
	public void waitInSecond(int second) {
		try {
			Thread.sleep(second * 1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

}
