/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include "jni_re_rcs_discovery_manager.h"

#include "RCSDiscoveryManager.h"
#include "RCSAddress.h"

using namespace OIC::Service;

JniDiscoverResourceListener *AddDiscoverResourceListener(JNIEnv *env, jobject jListener)
{
    JniDiscoverResourceListener *onResourceDiscovered = NULL;

    discoverResourceMapLock.lock();

    for (auto it = discoverResourceListenerMap.begin(); it != discoverResourceListenerMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            onResourceDiscovered = refPair.first;
            refPair.second++;
            it->second = refPair;
            discoverResourceListenerMap.insert(*it);
            LOGD("onResourceDiscovered: ref. count incremented");
            break;
        }
    }

    if (!onResourceDiscovered)
    {
        onResourceDiscovered = new JniDiscoverResourceListener(env, jListener,
                RemoveDiscoverResourceListener);
        jobject jgListener = env->NewGlobalRef(jListener);

        discoverResourceListenerMap.insert(std::pair < jobject, std::pair < JniDiscoverResourceListener *,
                                           int >> (jgListener, std::pair<JniDiscoverResourceListener *, int>(onResourceDiscovered, 1)));
        LOGD("onResourceDiscovered: new listener");
    }
    discoverResourceMapLock.unlock();
    return onResourceDiscovered;
}

void RemoveDiscoverResourceListener(JNIEnv *env, jobject jListener)
{
    discoverResourceMapLock.lock();

    for (auto it = discoverResourceListenerMap.begin(); it != discoverResourceListenerMap.end(); ++it)
    {
        if (env->IsSameObject(jListener, it->first))
        {
            auto refPair = it->second;
            if (refPair.second > 1)
            {
                refPair.second--;
                it->second = refPair;
                discoverResourceListenerMap.insert(*it);
                LOGI("OnResourceDiscoveredListener: ref. count decremented");
            }
            else
            {
                env->DeleteGlobalRef(it->first);
                JniDiscoverResourceListener *listener = refPair.first;
                delete listener;
                discoverResourceListenerMap.erase(it);
                LOGI("OnResourceDiscoveredListener removed");
            }
            break;
        }
    }
    discoverResourceMapLock.unlock();
}

JNIEXPORT void JNICALL
Java_org_iotivity_ResourceEncapsulation_client_RCSDiscoveryManager_discoverResource
(JNIEnv *env, jclass interfaceClass, jstring jAddress, jint jTypeofDiscovery, jstring jRelativeURI,
 jstring jresourceType, jobject jDiscoveryListener)
{
    LOGI("NativeInterface_discoverResource entry");
    std::string address;
    if (jAddress)
    {
        address = env->GetStringUTFChars(jAddress, NULL);
    }
    std::string relativeUri;
    if (jRelativeURI)
    {
        relativeUri = env->GetStringUTFChars(jRelativeURI, NULL);
    }
    std::string resourceType;
    if (jresourceType)
    {
        resourceType = env->GetStringUTFChars(jresourceType, NULL);
    }

    JniDiscoverResourceListener *onResFoundListener = AddDiscoverResourceListener(env,
            jDiscoveryListener);
    LOGI("NativeInterface_discoverResource onResFoundListener added");
    RCSDiscoveryManager::ResourceDiscoveredCallback discoverCallback = [onResFoundListener](
                std::shared_ptr< OIC::Service::RCSRemoteResourceObject> resource)
    {
        onResFoundListener->discoverResourceCallback(resource);
    };
    if ( 0 == jTypeofDiscovery )
    {
        RCSDiscoveryManager::getInstance()->discoverResourceByType(RCSAddress::multicast(), relativeUri,
                resourceType, discoverCallback);
        LOGI("NativeInterface_discoverResource multicast");
    }
    else if ( 1 == jTypeofDiscovery)
    {
        RCSDiscoveryManager::getInstance()->discoverResourceByType(RCSAddress::unicast(address),
                relativeUri, resourceType, discoverCallback );
        LOGI("NativeInterface_discoverResource unicast");
    }

    LOGI("NativeInterface_discoverResource exit");

}
