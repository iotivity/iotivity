/*
 * PresenceInfo.h
 *
 */

#ifndef PRESENCEINFO_H_
#define PRESENCEINFO_H_

#include <iostream>
#include <functional>
#include <pthread.h>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <list>
#include <condition_variable>

// Iotivity Base CAPI
#include "ocstack.h"
#include "logger.h"

#include "PrimitiveResource.h"
#include "ResourceBroker.h"

enum class BROKER_STATE;

typedef std::function<OCStackResult(BROKER_STATE)> BrokerCB;
typedef std::function<OCStackApplicationResult(void *, OCDoHandle, OCClientResponse *)> RequestCB;

struct BrokerRequesterInfo {
    BrokerCB brockerCB;
};

class PresenceInfo: public PrimitiveResource {
public:
    PresenceInfo(PrimitiveResource & pResource, BrokerCB _cbrequesterList);
    PresenceInfo(std::string _Uri, std::string _Address, BrokerCB _cb);
    ~PresenceInfo();

    const RequestCB pRequestCB;
    const BrokerCB brokerCB;

    std::list<BrokerRequesterInfo *> * requesterList;
private:
    OCStackApplicationResult requestCB(void * ctx, OCDoHandle handle,
            OCClientResponse * clientResponse);
};

#endif /* PRESENCEINFO_H_ */

