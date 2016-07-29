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

#include "SMHelper.h"

std::mutex SMHelper::s_mutex;
bool SMHelper::s_checkExecuteCallback = false;

SMHelper::SMHelper() {
    IOTIVITYTEST_LOG(INFO, "[SMHelper] IN");
    PlatformConfig config { OC::ServiceType::InProc, ModeType::Both, "0.0.0.0",
            0, OC::QualityOfService::LowQos };
    OCPlatform::Configure(config);
    m_resourceTypes = {RESOURCE_TYPE_LIGHT, RESOURCE_TYPE_FAN};
    m_relativetUri = OC_RSRVD_WELL_KNOWN_URI;
}

SceneList* SMHelper::getSceneListInstance() {
    SceneList* sceneListInstance = nullptr;
    sceneListInstance = SceneList::getInstance();

    return sceneListInstance;
}

void SMHelper::onResourceDiscovered(
        std::shared_ptr<RCSRemoteResourceObject> foundResource) {
    s_mutex.lock();

    IOTIVITYTEST_LOG(INFO, "onResourceDiscovered callback");

    std::string resourceURI = foundResource->getUri();
    std::string hostAddress = foundResource->getAddress();

    IOTIVITYTEST_LOG(INFO, "Resource URI : %s", resourceURI.c_str());
    IOTIVITYTEST_LOG(INFO, "Resource Host : %s", hostAddress.c_str());

    g_foundResourceList.push_back(foundResource);
    IOTIVITYTEST_LOG(INFO, "size of resource list : %d",
            g_foundResourceList.size());

    s_mutex.unlock();
}

void SMHelper::onExecute(int /*Code*/) {
    IOTIVITYTEST_LOG(INFO, "%s is called", __func__);
    SMHelper::s_checkExecuteCallback = true;
}

void SMHelper::discoverResource() {
    std::string exceptionMsg = "";
    try {
        m_pDiscoveryTask =
                RCSDiscoveryManager::getInstance()->discoverResourceByTypes(
                        RCSAddress::multicast(), m_relativetUri, m_resourceTypes,
                        std::bind(&SMHelper::onResourceDiscovered, this,
                                PH::_1));
    } catch (const RCSPlatformException& e) {
        exceptionMsg = e.what();
        IOTIVITYTEST_LOG(ERROR, "%s", exceptionMsg.c_str());
    } catch (const RCSException& e) {
        exceptionMsg = e.what();
        IOTIVITYTEST_LOG(ERROR, "%s", exceptionMsg.c_str());
    }
    IOTIVITYTEST_LOG(INFO, "Wait 5 seconds until resources discovered.");
    CommonUtil::waitInSecond(MAX_SLEEP_TIME);
}

void SMHelper::stopDiscovery() {
    m_pDiscoveryTask->cancel();
}
