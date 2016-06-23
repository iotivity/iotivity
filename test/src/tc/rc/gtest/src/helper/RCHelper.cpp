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

#include "RCHelper.h"

RCHelper* RCHelper::s_rcHelperInstance = NULL;
std::mutex RCHelper::s_mutex;

RCHelper::RCHelper() {
	PlatformConfig config { OC::ServiceType::InProc, ModeType::Both, "0.0.0.0",
			0, OC::QualityOfService::LowQos };
	OCPlatform::Configure(config);
	IOTIVITYTEST_LOG(INFO, "Platform Configuration Done!!");

}

RCHelper::~RCHelper() {

}

RCHelper* RCHelper::getInstance(void) {
	if (s_rcHelperInstance == NULL) {
		s_mutex.lock();

		if (s_rcHelperInstance == NULL) {
			s_rcHelperInstance = new RCHelper();
		}

		s_mutex.unlock();
	}

	return s_rcHelperInstance;
}

bool RCHelper::isAddedBundle(RCSResourceContainer *container,
		std::string bundleID) {
	if (getBundle(container, bundleID) == NULL) {
		m_errorMessage = "Bundle is not added in container. ID : " + bundleID;
		return false;
	}

	return true;
}

unique_ptr<RCSBundleInfo> RCHelper::getBundle(RCSResourceContainer *container,
		std::string bundleID) {
	list < unique_ptr<RCSBundleInfo> > bundles = container->listBundles();
	list<unique_ptr<RCSBundleInfo> >::iterator bundleIt;
	for (bundleIt = bundles.begin(); bundleIt != bundles.end(); bundleIt++) {
		if (bundleID.compare((*bundleIt)->getID()) == 0)
			return move(*bundleIt);
	}
	return NULL;
}

bool RCHelper::startBundle(RCSResourceContainer *container,
		std::string bundleID) {
	container->startBundle(bundleID);

	unique_ptr < RCSBundleInfo > bundleInfo = getBundle(container, bundleID);

	if (bundleInfo == NULL) {
		m_errorMessage = "Bundle is not created. ID : " + bundleID;
		return false;
	}

	return true;
}

bool RCHelper::stopBundle(RCSResourceContainer *container,
		std::string bundleID) {
	container->stopBundle(bundleID);

	unique_ptr < RCSBundleInfo > bundleInfo = getBundle(container, bundleID);

	if (bundleInfo == NULL) {
		m_errorMessage = "Bundle is not created. ID : " + bundleID;
		return false;
	}

	return true;
}

bool RCHelper::removeBundle(RCSResourceContainer *container,
		std::string bundleID) {
	container->removeBundle(bundleID);

	unique_ptr < RCSBundleInfo > bundleInfo = getBundle(container, bundleID);

	if (bundleInfo != NULL) {
		m_errorMessage = "Bundle is not removed. ID : " + bundleID;
		return false;
	}
	return true;
}

bool RCHelper::addResourceConfigVersion(RCSResourceContainer *container,
		std::string bundleID, std::string value) {
	std::map < string, string > params;
	params.insert(std::make_pair("Version", value));

	container->addResourceConfig(bundleID, "", params);

	unique_ptr < RCSBundleInfo > bundle = getBundle(container, bundleID);

	if (bundle == NULL) {
		m_errorMessage = "Bundle is not created. ID : " + bundleID;
		return false;
	}

	if (bundle->getVersion().compare(value) != 0) {
		m_errorMessage = "Bundle version is not matched. ID : "
				+ bundle->getVersion() + ", value : " + value;
		return false;
	}
	return true;
}

std::string RCHelper::getFailureMessage() {
	return m_errorMessage;
}
