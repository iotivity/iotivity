/*
 * PresenceChecker.h
 *
 *  Created on: May 20, 2015
 *      Author: root
 */

#ifndef PRESENCECHECKER_H_
#define PRESENCECHECKER_H_

// Iotivity Base CAPI
#include "ocstack.h"
#include "logger.h"
class PresenceChecker {
public:
    PresenceChecker();
    virtual ~PresenceChecker();
    OCStackResult addRequestCB(BrokerCB _cb);
    OCStackResult deleteRequestCB(BrokerCB _cb);
};

#endif /* PRESENCECHECKER_H_ */
