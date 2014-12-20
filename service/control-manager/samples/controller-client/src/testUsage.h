/*
 * testDoor.h
 *
 *  Created on: Oct 14, 2014
 *      Author: manjunath.d
 */

#ifndef TESTUSAGE_H_
#define TESTUSAGE_H_

#include "ITestResource.h"
#include "Device.h"
#include "Client/UsageLimitResource.h"
#include "testSubscriptionListener.h"

class testUsage : public ITestResource
{

        IUsageLimitResourceResponseListener *mResListener;
        OC::Cm::Client::Resource::IResourceResponseListener *mSubsListener;

    public:
        testUsage(OC::Cm::Device *cmDevice);
        virtual ~testUsage();

        bool testGet();
        bool testPut();
        bool testSubscription(OC::Cm::Device *cmDevice);
        bool testUnSubscribe();

        UsageLimitResource *mUsageLimit;

};


#endif /* TESTDOOR_H_ */
