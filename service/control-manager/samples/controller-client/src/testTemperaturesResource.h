/*
 * testTemperaturesResource.h
 *
 *  Created on: Oct 15, 2014
 *  Author: kumar.vinay@samsung.com
 */

#ifndef TESTTEMPERATURESRESOURCE_H_
#define TESTTEMPERATURESRESOURCE_H_

#include "Client/TemperaturesResource.h"
#include "Device.h"
#include "testSubscriptionListener.h"
#include "ITestResource.h"


class testTemperaturesResource : public ITestResource
{

        ITemperaturesResourceResponseListener *mResListener;
        OC::Cm::Client::Resource::IResourceResponseListener *mSubsListener;

    public:
        testTemperaturesResource (OC::Cm::Device *cmDevice);
        virtual ~testTemperaturesResource();

        bool testGet();
        bool testPut();
        bool testSubscription(OC::Cm::Device *cmDevice);
        bool testUnSubscribe();

        TemperaturesResource *mTemperaturesResource;
};

#endif /* TESTTEMPERATURESRESOURCE_H_ */
