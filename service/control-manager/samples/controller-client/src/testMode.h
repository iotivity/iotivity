/*
 * testMode.h
 *
 *  Created on: Oct 14, 2014
 *      Author: manjunath.d
 */

#ifndef TESTMODE_H_
#define TESTMODE_H_

#include "Client/ModeResource.h"
#include "Device.h"
#include "testSubscriptionListener.h"
#include "ITestResource.h"

class testMode : public ITestResource
{

        IModeResourceResponseListener *mResListener;
        OC::Cm::Client::Resource::IResourceResponseListener *mSubsListener;

    public:

        testMode(OC::Cm::Device *cmDevice);
        virtual ~testMode();

        bool testGet();
        bool testPut();
        bool testSubscription(OC::Cm::Device *cmDevice);
        bool testUnSubscribe();

        ModeResource *mModeRes;

};


#endif /* TESTMODE_H_ */
