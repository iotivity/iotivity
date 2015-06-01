/*
 * ResourceBroker.h
 *
 *  Created on: May 20, 2015
 *      Author: root
 */

#ifndef RESOURCEBROKER_H_
#define RESOURCEBROKER_H_

#include <iostream>
#include <functional>
#include <pthread.h>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <algorithm>

// Iotivity Base CAPI
#include "ocstack.h"
#include "logger.h"

// internal class
#include "PresenceInfo.h"

#define CACHE_TAG PCF("BROKER")


typedef std::function<OCStackResult(PrimitiveResource * resource)> DiscoverCB;
enum class BROKER_STATE
{
    ALIVE = 0,
    REQUESTED,
    LOST_SIGNAL,
    DESTROYED
};
class ResourceBroker {
public:
    ResourceBroker * getInstance();

    OCStackResult discoverResource(PrimitiveResource & pResource, DiscoverCB cb);
    OCStackResult discoverResource(const std::string uri, const std::string address, DiscoverCB cb);

    OCStackResult hostResource(PrimitiveResource & pResource, BrokerCB cb);
    OCStackResult hostResource(
            const std::string Uri, const std::string address, BrokerCB cb);

    OCStackResult cancelHostResource(PrimitiveResource & pResource);
    OCStackResult cancelHostResource(
            const std::string uri, const std::string address);

    OCStackResult getResourceState(PrimitiveResource & pResource);
    OCStackResult getResourceState(
            const std::string address, const std::string uri);

private:
    ResourceBroker();
    ~ResourceBroker();
    bool isPrimitiveResource(PrimitiveResource& pResource, BrokerCB cb);
    static ResourceBroker * s_instance;
    static std::mutex s_mutexForCreation;
    static std::list< PresenceInfo * > s_presenceInfo;
};

#endif /* RESOURCEBROKER_H_ */
