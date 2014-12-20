/*
 * testLightResource.h
 *
 *  Created on: Oct 14, 2014
 *  Author: kumar.vinay@samsung.com
 */

#ifndef TESTLIGHTRESOURCE_H_
#define TESTLIGHTRESOURCE_H_

#include "Client/LightResource.h"
#include "Device.h"
#include "testSubscriptionListener.h"
#include "ITestResource.h"


class testLightResource : public ITestResource
{

        ILightResourceResponseListener *mResListener;
        OC::Cm::Client::Resource::IResourceResponseListener *mSubsListener;

    public:
        testLightResource (OC::Cm::Device *cmDevice);
        virtual ~testLightResource();

        bool testGet();
        bool testPut();
        bool testSubscription(OC::Cm::Device *cmDevice);
        bool testUnSubscribe();

        LightResource *mLightResource;
};

#endif /* TESTLIGHTRESOURCE_H_ */
