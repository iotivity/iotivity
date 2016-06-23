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

#include "TMHelper.h"

TMHelper* TMHelper::s_tmHelperInstance = NULL;
std::mutex TMHelper::s_mutex;

TMHelper::TMHelper(void) {
	m_resourceFoundCallbackCount = 0;
	m_configReceivedCallbackCount = 0;
	m_presenceReceivedCallbackCount = 0;
	m_pThingsConfiguration = ThingsConfiguration::getInstance();
	m_pThingsMaintenance = ThingsMaintenance::getInstance();
}

TMHelper::~TMHelper() {
	if (s_tmHelperInstance != NULL) {
		delete s_tmHelperInstance;
		s_tmHelperInstance = NULL;
	}
}

TMHelper* TMHelper::getInstance(void) {
	if (s_tmHelperInstance == NULL) {
		s_mutex.lock();

		if (s_tmHelperInstance == NULL) {
			s_tmHelperInstance = new TMHelper();
		}

		s_mutex.unlock();
	}

	return s_tmHelperInstance;
}

void TMHelper::onFoundCandidate(
		vector<std::shared_ptr<OCResource> > resourceList) {
	s_mutex.lock();
	IOTIVITYTEST_LOG(INFO, "....Inside onFoundCandidateResources....");

	m_resourceFoundCallbackCount++;

	unsigned int resourceCount = resourceList.size();
	IOTIVITYTEST_LOG(INFO, "Total resource found: %u", resourceCount);
	if (resourceCount > (unsigned int) MAX_RESOURCE_COUNT) {
		IOTIVITYTEST_LOG(INFO, "....Leaving onFoundCandidateResources....");
		s_mutex.unlock();
		return;
	}

	m_resourceList.clear();
	m_resourceList = resourceList;
	m_isResourceEnlisted = true;
	for (auto resource : m_resourceList) {
		IOTIVITYTEST_LOG(INFO, "Found Resource URI = %s",
				resource->uri().c_str());
	}

	IOTIVITYTEST_LOG(INFO, "....Leaving onFoundCandidateResources....");
	s_mutex.unlock();
}

void TMHelper::onConfigurationReceived(const HeaderOptions& headerOptions,
		const OCRepresentation& rep, const int eCode) {
	IOTIVITYTEST_LOG(INFO,
			"....Inside onConfigurationReceived.... , with error code: %d",
			eCode);

	s_mutex.lock();

	m_callbackErrorCode = eCode;
	m_configReceivedCallbackCount++;
	m_headerOption = headerOptions;
	m_representation = rep;

	string value = "";
	m_representation.getValue("power", value);
	IOTIVITYTEST_LOG(INFO, "power is: = %s", value.c_str());
	m_representation.getValue("intensity", value);
	IOTIVITYTEST_LOG(INFO, "intensity is: = %s", value.c_str());
	m_representation.getValue("manufacturer", value);
	IOTIVITYTEST_LOG(INFO, "manufacturer is: = %s", value.c_str());

	s_mutex.unlock();
}

void TMHelper::onPresenceReceived(std::string presence, OCStackResult result) {
	IOTIVITYTEST_LOG(INFO, "....Inside onPresenceReceived....");

	m_presenceReceivedCallbackCount++;
	m_presenceCallbackResult = result;

	if (m_presenceCallbackResult == OC_STACK_OK) {
		IOTIVITYTEST_LOG(INFO,
				"Successfully subscribed collection(group) for presence, presence message is: %s",
				presence.c_str());
	} else {
		IOTIVITYTEST_LOG(ERROR,
				"Unable to subscribe collection(group) for presence, result is =  %s",
				CommonUtil::s_OCStackResultString.at(m_presenceCallbackResult).c_str());
	}
}

void TMHelper::onBootstrap(const HeaderOptions& headerOptions,
		const OCRepresentation& rep, const int eCode) {
	IOTIVITYTEST_LOG(INFO, "Bootstrap callback called");
	m_callbackErrorCode = eCode;

	if (eCode == SUCCESS_RESPONSE) {
		IOTIVITYTEST_LOG(INFO, "\n\nGET request was successful");
		IOTIVITYTEST_LOG(INFO, "\tResource URI: %s", rep.getUri().c_str());

		m_defaultRegionValue = rep.getValue<std::string>("regionValue");
		m_defaultTimeValue = rep.getValue<std::string>("timeValue");
		m_defaultCurrentTimeValue = rep.getValue<std::string>(
				"currentTimeValue");
		m_defaultNetworkValue = rep.getValue<std::string>("networkValue");
		m_defaultIPAddressValue = rep.getValue<std::string>("IPAddressValue");
		m_defaultSecurityValue = rep.getValue<std::string>("securityValue");
		m_defaultModeValue = rep.getValue<std::string>("modeValue");
		m_defaultConfigurationValue = rep.getValue<std::string>(
				"configurationValue");
		m_defaultFactorySetValue = rep.getValue<std::string>("factorySetValue");

		IOTIVITYTEST_LOG(INFO, "\tregionValue : %s",
				m_defaultRegionValue.c_str());
		IOTIVITYTEST_LOG(INFO, "\ttimeValue : %s", m_defaultTimeValue.c_str());
		IOTIVITYTEST_LOG(INFO, "\tcurrentTimeValue : %s",
				m_defaultCurrentTimeValue.c_str());
		IOTIVITYTEST_LOG(INFO, "\tnetworkValue : %s",
				m_defaultNetworkValue.c_str());
		IOTIVITYTEST_LOG(INFO, "\tIPAddressValue : %s",
				m_defaultIPAddressValue.c_str());
		IOTIVITYTEST_LOG(INFO, "\tsecurityValue : %s",
				m_defaultSecurityValue.c_str());
		IOTIVITYTEST_LOG(INFO, "\tmodeValue : %s", m_defaultModeValue.c_str());
		IOTIVITYTEST_LOG(INFO, "\tconfigurationValue : %s",
				m_defaultConfigurationValue.c_str());
		IOTIVITYTEST_LOG(INFO, "\tfactorySetValue : %s",
				m_defaultFactorySetValue.c_str());
	} else {
		IOTIVITYTEST_LOG(ERROR, "onGET Response error: %d", eCode);
		std::exit(INT_MINUS_ONE);
	}
}

bool TMHelper::findCandidateResourcesAndVerify(vector<string> resourceTypeList,
		OCStackResult expectedResult, int expectedResourceCount,
		string& errorMsg) {
	int resourceCount = 0;
	errorMsg = "From \"findCandidateResources\" Expected : "
			+ CommonUtil::s_OCStackResultString.at(expectedResult) + " Actual : ";
	m_resourceList.clear();
	m_resourceFoundCallbackCount = 0;
	m_isResourceEnlisted = false;
	OCStackResult result = m_groupManager.findCandidateResources(
			resourceTypeList,
			std::bind(&TMHelper::onFoundCandidate, this, std::placeholders::_1),
			INT_MINUS_ONE);
	waitForResourceFound(m_resourceList, CALLBACK_WAIT_MAX * 2);
	IOTIVITYTEST_LOG(INFO, "Wait Complete");

	if (result != expectedResult) {
		errorMsg += CommonUtil::s_OCStackResultString.at(result);
		return false;
	}

	if ((m_resourceList.size() > INT_ZERO)
			&& (m_resourceFoundCallbackCount != INT_ONE)) {
		errorMsg =
				"From \"findCandidateResources\" Callback count Expected : 1, Actual : "
						+ to_string(m_resourceFoundCallbackCount);
		return false;
	}

	if ((m_resourceList.size() == INT_ZERO)
						&& (m_resourceFoundCallbackCount != INT_ZERO)) {
			errorMsg =
					"From \"findCandidateResources\" Callback count Expected : 0, Actual : "
							+ to_string(m_resourceFoundCallbackCount);
			return false;
		}

	if (expectedResult == OC_STACK_OK) {
		resourceCount = m_resourceList.size();

		if ((expectedResourceCount > INT_ZERO && resourceCount == INT_ZERO)
				|| (expectedResourceCount == INT_ZERO
						&& resourceCount > INT_ZERO)) {
			errorMsg += CommonUtil::s_OCStackResultString.at(result) + "; Expected ResourceCount : "
					+ to_string(expectedResourceCount)
					+ ", Actual Resource Count: " + to_string(resourceCount);
			return false;
		}
	}

	errorMsg = "";
	return true;
}

bool TMHelper::bindResourceToGroupAndVerify(string resourceType,
		OCResourceHandle& childHandle, OCStackResult expectedResult,
		string& errorMsg) {
	errorMsg = "bindResourceToGroup returns Error. Expected : "
			+ CommonUtil::s_OCStackResultString.at(expectedResult) + ", Actual : ";

	vector<string> resourceTypeList;

	resourceTypeList.push_back(resourceType);
	resourceTypeList.push_back(RESOURCE_TYPE_FAN);

	IOTIVITYTEST_LOG(INFO,
			"-------------------Finding Candidate Resource-------------------\n");

	if (!findCandidateResourcesAndVerify(resourceTypeList, OC_STACK_OK, INT_ONE,
			errorMsg)) {
		errorMsg = "bindResourceToGroup Failed and " + errorMsg;
		return false;
	}

	OCResourceHandle collectionHandle;
	OCStackResult result = OCPlatform::registerResource(collectionHandle,
			m_resourceList.at(INT_ONE));

	if (result != OC_STACK_OK) {
		errorMsg =
				"bindResourceToGroup Failed. Failed to register Group. Expected : OC_STACK_OK,  Actual : "
						+ CommonUtil::s_OCStackResultString.at(result);
		return false;
	}

	IOTIVITYTEST_LOG(INFO,
			"-------------------Binding Resource to Group-------------------\n");

	result = m_groupManager.bindResourceToGroup(childHandle,
			m_resourceList.at(INT_ZERO), collectionHandle);

	if (result != expectedResult) {
		errorMsg =
				"bindResourceToGroup Failed. Failed to bind resource to Group. Expected : OC_STACK_OK,  Actual : "
						+ CommonUtil::s_OCStackResultString.at(result);
		return false;
	}

	return true;
}

bool TMHelper::getConfigurationAndVerify(
		const std::shared_ptr<OCResource> resource,
		OCStackResult expectedResult, bool isCallbackExpected,
		int expectedErrorCode, string expectedValue, string& errorMsg) {
	vector<string> configurationNameList;
	configurationNameList.push_back("power");
	configurationNameList.push_back("intensity");

	errorMsg = "GetResourceConfiguration, Expected : "
			+ CommonUtil::s_OCStackResultString.at(expectedResult);

	m_callbackErrorCode = -1;
	m_configReceivedCallbackCount = 0;

	OCStackResult result = m_pThingsConfiguration->getConfigurations(resource,
			configurationNameList,
			std::bind(&TMHelper::onConfigurationReceived, this,
					std::placeholders::_1, std::placeholders::_2,
					std::placeholders::_3));

	CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

	if (result != expectedResult) {
		errorMsg += " Actual : " + CommonUtil::s_OCStackResultString.at(result);
		return false;
	}

	if (m_configReceivedCallbackCount != (int) isCallbackExpected) {
		errorMsg = "From \"getConfigurations\" Callback count Expected : "
				+ to_string(isCallbackExpected) + ", Actual : "
				+ to_string(m_configReceivedCallbackCount);
		return false;
	}

	if (m_callbackErrorCode != expectedErrorCode) {
		errorMsg =
				"From \"getConfigurations\", callback error code mismatches. Expected :"
						+ to_string(expectedErrorCode) + ", Actual : "
						+ to_string(m_callbackErrorCode);
		return false;
	}

	string configurationValue = "";
	m_representation.getValue("power", configurationValue);

	if (configurationValue.compare(expectedValue) != INT_ZERO) {
		errorMsg =
				"From \"getConfigurations\", got configuration mismatches. Expected power state : "
						+ expectedValue + ", Actual power state : "
						+ configurationValue;
		return false;
	}
	errorMsg = "";
	return true;
}

bool TMHelper::updateConfigurationAndVerify(
		const std::shared_ptr<OCResource> resource,
		OCStackResult expectedResult, bool isCallbackExpected,
		int expectedErrorCode, string updatedValue, string& errorMsg) {
	m_callbackErrorCode = -1;
	map<string, string> configurationMap;
	string cofigurationKey = "power";
	string cofigurationValue = updatedValue;
	configurationMap[cofigurationKey] = cofigurationValue;

	errorMsg = "UpdateResourceConfiguration, Expected : "
			+ CommonUtil::s_OCStackResultString.at(expectedResult);

	m_configReceivedCallbackCount = 0;

	OCStackResult result = m_pThingsConfiguration->updateConfigurations(
			resource, configurationMap,
			std::bind(&TMHelper::onConfigurationReceived, this,
					std::placeholders::_1, std::placeholders::_2,
					std::placeholders::_3));

	CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

	if (result != expectedResult) {
		errorMsg += " Actual : " + CommonUtil::s_OCStackResultString.at(result);
		return false;
	}

	if (m_configReceivedCallbackCount != (int) isCallbackExpected) {
		errorMsg =
				"From \"updateConfigurations\" Callback not called. count Expected : "
						+ to_string(isCallbackExpected) + ", Actual : "
						+ to_string(m_configReceivedCallbackCount);
		return false;
	}

	if (m_callbackErrorCode != expectedErrorCode) {
		errorMsg =
				"From \"updateConfigurations\", callback error code mismatches. Expected :"
						+ to_string(expectedErrorCode) + ", Actual : "
						+ to_string(m_callbackErrorCode);
		return false;
	}

	string configurationValue = "";
	string configurationKey = "power";
	m_representation.getValue(configurationKey, configurationValue);

	if (configurationValue.compare(updatedValue) != INT_ZERO) {
		errorMsg =
				"From \"updateConfigurations\", updated configuration mismatches. Expected power state : "
						+ updatedValue + ", Actual power state : "
						+ configurationValue;
		return false;
	}

	errorMsg = "";
	return true;
}

bool TMHelper::rebootAndVerify(const std::shared_ptr<OCResource> resource,
		OCStackResult expectedResult, int expectedErrorCode,
		string expectedlValue, string& errorMsg) {
	errorMsg = "Reboot, Expected : " + CommonUtil::s_OCStackResultString.at(expectedResult);
	m_callbackErrorCode = -1;

	OCStackResult result = m_pThingsMaintenance->reboot(resource,
			std::bind(&TMHelper::onConfigurationReceived, this,
					std::placeholders::_1, std::placeholders::_2,
					std::placeholders::_3));

	CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

	if (result != expectedResult) {
		errorMsg += " Actual : " + CommonUtil::s_OCStackResultString.at(result);
		return false;
	}

	if (m_callbackErrorCode != expectedErrorCode) {
		errorMsg = "From \"Reboot\", callback error code mismatches. Expected :"
				+ to_string(expectedErrorCode) + ", Actual : "
				+ to_string(m_callbackErrorCode);
		return false;
	}

	string configurationValue = "";
	string configurationKey = "rb";
	m_representation.getValue(configurationKey, configurationValue);

	if (configurationValue.compare(expectedlValue) != INT_ZERO) {
		errorMsg =
				"From \"reboot\", unable to reboot resource. Expected reboot state : "
						+ expectedlValue + ", Actual reboot state : "
						+ configurationValue;
		return false;
	}

	errorMsg = "";
	return true;
}

bool TMHelper::factoryResetAndVerify(const std::shared_ptr<OCResource> resource,
		OCStackResult expectedResult, int expectedErrorCode, bool supportsReset,
		string& errorMsg) {
	errorMsg = "FactoryReset, Expected : " + CommonUtil::s_OCStackResultString.at(expectedResult);
	m_callbackErrorCode = -1;

	OCStackResult result = m_pThingsMaintenance->factoryReset(resource,
			std::bind(&TMHelper::onConfigurationReceived, this,
					std::placeholders::_1, std::placeholders::_2,
					std::placeholders::_3));

	CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

	if (result != expectedResult) {
		errorMsg += " Actual : " + CommonUtil::s_OCStackResultString.at(result);
		return false;
	}

	if (m_callbackErrorCode != expectedErrorCode) {
		errorMsg =
				"From \"factoryReset\", callback error code mismatches. Expected :"
						+ to_string(expectedErrorCode) + ", Actual : "
						+ to_string(m_callbackErrorCode);
		return false;
	}

	string configurationValue = "";
	string originalValue = DEFAULT_POWER_STATE;
	string configurationKey = "power";
	m_representation.getValue(configurationKey, configurationValue);

	if (supportsReset
			&& (configurationValue.compare(originalValue) != INT_ZERO)) {
		errorMsg =
				"From \"factoryReset\", unable to revert to factory settings. Expected power state : "
						+ originalValue + ", Actual power state : "
						+ configurationValue;
		return false;
	}

	if (!supportsReset && (configurationValue.compare("") != INT_ZERO)) {
		errorMsg =
				"From \"factoryReset\", unable to revert to factory settings. Expected power state : , Actual power state : "
						+ configurationValue;
		return false;
	}

	errorMsg = "";
	return true;
}

bool TMHelper::doBootstrapAndVerify(OCStackResult expectedResult,
		int expectedErrorCode, string& errorMsg) {
	errorMsg = "doBootstrap  Returned error. Expected : "
			+ CommonUtil::s_OCStackResultString.at(expectedResult);
	m_callbackErrorCode = -1;

	OCStackResult result = m_pThingsConfiguration->doBootstrap(
			std::bind(&TMHelper::onBootstrap, this, PH::_1, PH::_2, PH::_3));
	errorMsg += " Actual : " + CommonUtil::s_OCStackResultString.at(result);

	CommonUtil::waitInSecond(CALLBACK_WAIT_DEFAULT);

	if (result != expectedResult) {
		return false;
	}

	if (m_callbackErrorCode != expectedErrorCode) {
		errorMsg += ". But the callback returned error code. Expected = "
				+ to_string(expectedErrorCode) + "; Actual = "
				+ std::to_string(m_callbackErrorCode);
		return false;
	}

	errorMsg = "";
	return true;

}

bool TMHelper::subscribeCollectionPresenceAndVerify(
		const std::shared_ptr<OCResource> resource,
		OCStackResult expectedResult, int expectedErrorCode, string& errorMsg,
		bool isCallbackExpected) {
	errorMsg = "subscribeCollectionPresence, Expected : OC_STACK_OK, ";

	m_callbackErrorCode = -1;
	m_presenceReceivedCallbackCount = 0;
	m_presenceCallbackResult = OC_STACK_NOTIMPL;

	OCStackResult result = m_groupManager.subscribeCollectionPresence(resource,
			std::bind(&TMHelper::onPresenceReceived, this,
					std::placeholders::_1, std::placeholders::_2));

	CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);

	if (result != OC_STACK_OK) {
		errorMsg += " Actual : " + CommonUtil::s_OCStackResultString.at(result);
		return false;
	}

	if (m_presenceReceivedCallbackCount != (int) isCallbackExpected) {
		errorMsg =
				"From \"subscribeCollectionPresence\" Callback count Expected : "
						+ to_string((int) isCallbackExpected) + " , Actual : "
						+ to_string(m_presenceReceivedCallbackCount);
		return false;
	}

	if (isCallbackExpected) {
		if (m_presenceCallbackResult != expectedResult) {
			errorMsg =
					"From \"subscribeClollectionPresence\", callback error code mismatches. Expected :"
							+ to_string(expectedResult) + ", Actual : "
							+ to_string(m_presenceCallbackResult);
			return false;
		}
	}

	errorMsg = "";
	return true;
}

vector<shared_ptr<OCResource> > TMHelper::getResourceList(void) {
	return m_resourceList;
}

bool TMHelper::waitForResourceFound(
		std::vector<std::shared_ptr<OCResource> > &foundResourceList,
		int timeOut) {
	bool isTimeOut = false;
	int passedTime = 0;

	while (foundResourceList.empty()) {
		CommonUtil::waitInSecond(CALLBACK_WAIT_MIN);
		passedTime++;
		IOTIVITYTEST_LOG(INFO, "Waited %d seconds for finding resources",
				passedTime);

		if (passedTime > timeOut) {
			isTimeOut = true;
			break;
		}
	}

	return isTimeOut;
}

void TMHelper::launchApp(std::string app) {

#ifdef __LINUX__
	string cmd = "nohup gnome-terminal -x sh -c ";
	cmd += app;
	cmd += " &";
	system(cmd.c_str());
#endif

}

void TMHelper::killApp(std::string app) {
	string appName = app.substr(app.find_last_of("/\\") + INT_ONE);

#ifdef __LINUX__
	std::string prefix = "kill $(pgrep ";
	std::string postfix = ")";
	std::string command = prefix + appName + postfix;
	system(command.c_str());
#endif

}

bool TMHelper::containsResource(vector<shared_ptr<OCResource> > resourceList,
		vector<string> resourceTypeList, string& errorMessage) {
	bool isAllEnlisted = true;
	errorMessage += "TestCase Failed.";

	for (auto resourceType : resourceTypeList) {
		bool isEnlisted = false;
		for (auto resource : resourceList) {
			string fromType = "";
			string uri = "";
			string fromUri = "";
			QueryParamsMap paramsMap;
			fromType = resourceType.substr(
					resourceType.find_last_of(".") + INT_ONE, string::npos);
			uri = resource->uri();
			fromUri = uri.substr(uri.find_last_of("/") + INT_ONE,
					uri.find_last_of("-") - uri.find_last_of("/") - INT_ONE);

			if (fromUri.compare(fromType) == INT_ZERO) {
				isEnlisted = true;
				IOTIVITYTEST_LOG(INFO, "Resource Type Found: %s",
						resourceType.c_str());
				break;
			}
		}

		if (!isEnlisted) {
			isAllEnlisted = false;
			errorMessage += " The following resource type not found: "
					+ resourceType + "\n";

		}

	}

	return isAllEnlisted;
}
