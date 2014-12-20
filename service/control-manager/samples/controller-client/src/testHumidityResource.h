/*
 * testHumidityResource.h
 *
 *  Created on: Oct 9, 2014
 *  Author: Shunmuga Sundar
 */

#ifndef TESTHUMIDITYRESOURCE_H_
#define TESTHUMIDITYRESOURCE_H_

#include "Client/HumidityResource.h"
#include "Device.h"
#include "testSubscriptionListener.h"
#include "ITestResource.h"

class testHumidityResource : public ITestResource
{

        IHumidityResourceResponseListener *mResListener;
        OC::Cm::Client::Resource::IResourceResponseListener *mSubsListener;

    public:
        testHumidityResource (OC::Cm::Device *mDevice);
        virtual ~testHumidityResource();

        bool testGet();
        bool testPut();
        bool testSubscription(OC::Cm::Device *cmDevice);
        bool testUnSubscribe();

        HumidityResource *mHumidityRes;
};

#endif /* TESTHUMIDITYRESOURCE_H_ */
