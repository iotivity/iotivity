/*
 * testDoor.h
 *
 *  Created on: Oct 14, 2014
 *  Author: manjunath.d
 */

#ifndef TESTDOOR_H_
#define TESTDOOR_H_

#include "ITestResource.h"
#include "Device.h"
#include "Client/DoorsResource.h"
#include "testSubscriptionListener.h"

class testDoors : public ITestResource
{

        IDoorsResourceResponseListener *mResListener;
        OC::Cm::Client::Resource::IResourceResponseListener *mSubsListener;

    public:
        testDoors(OC::Cm::Device *cmDevice);
        virtual ~testDoors();

        bool testGet();
        bool testPut();
        bool testSubscription(OC::Cm::Device *cmDevice);
        bool testUnSubscribe();

        DoorsResource *mpDoorsRes;

};


#endif /* TESTDOOR_H_ */
