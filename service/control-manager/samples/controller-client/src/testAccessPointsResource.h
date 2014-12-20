/*
 * testAccessPointsResource.h
 *
 *  Created on: Oct 15, 2014
 *  Author: kumar.vinay@samsung.com
 */

#ifndef TESTACCESSPOINTS_H_
#define TESTACCESSPOINTS_H_

#include "Client/AccessPointsResource.h"
#include "Device.h"
#include "testSubscriptionListener.h"
#include "ITestResource.h"


class testAccessPoints : public ITestResource
{

        IAccessPointsResourceResponseListener *mResListener;
        OC::Cm::Client::Resource::IResourceResponseListener *mSubsListener;


    public:
        testAccessPoints (OC::Cm::Device *cmDevice);
        virtual ~testAccessPoints();

        bool testGet();
        bool testPut();
        bool testSubscription(OC::Cm::Device *cmDevice);
        bool testUnSubscribe();

        AccessPointsResource *mAccessPointsResource;

};

#endif /* TESTACCESSPOINTS_H_ */
