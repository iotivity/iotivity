/*
 * PresenceInfo.cpp
 *
 */

#include "PresenceInfo.h"

PresenceInfo::PresenceInfo(PrimitiveResource & pResource, BrokerCB _cb) :
        Uri(pResource.getUri()), Address(pResource.getAddress()) {
    pRequestCB =(RequestCB) (std::bind(&PresenceInfo::requestCB, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    requesterList = new std::list<BrokerRequesterInfo *>();
    BrokerRequesterInfo *requester = new BrokerRequesterInfo();
    requester->brockerCB = _cb;
    requesterList.push_back(requester);
}

PresenceInfo::PresenceInfo(std::string _Uri, std::string _Address, BrokerCB _cb) :
        Uri(_Uri), Address(_Address) {
    pRequestCB =(RequestCB) (std::bind(&PresenceInfo::requestCB, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    requesterList = new std::list<BrokerRequesterInfo *>();
    BrokerRequesterInfo *requester = new BrokerRequesterInfo();
    requester->brockerCB = _cb;
    requesterList.push_back(requester);
}

PresenceInfo::~PresenceInfo() {
    // TODO Auto-generated destructor stub
    // delete requester all
//	auto it = requesterList->begin();

    requesterList->clear();
    delete requesterList;
}

OCStackApplicationResult PresenceInfo::requestCB(void * ctx, OCDoHandle handle,
        OCClientResponse * clientResponse) {
    OCStackApplicationResult ret = OC_STACK_DELETE_TRANSACTION;

    return ret;
}

OCStackResult PresenceInfo::addRequestCB(BrokerCB _cb) {
    OCStackResult ret = OC_STACK_ERROR;

    requesterList->push_back(_cb);

    return ret;
}
