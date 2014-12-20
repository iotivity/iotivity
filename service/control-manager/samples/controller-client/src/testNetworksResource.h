/*
 * testTemperaturesResource.h
 *
 *  Created on: Oct 15, 2014
 *  Author: kumar.vinay@samsung.com
 */

#ifndef TESTNETWORKSRESOURCE_H_
#define TESTNETWORKSRESOURCE_H_

#include "Client/NetworksResource.h"
#include "Device.h"
#include "testSubscriptionListener.h"
#include "ITestResource.h"


class testNetworksResource : public ITestResource
{

        INetworksResourceResponseListener *mResListener;
        OC::Cm::Client::Resource::IResourceResponseListener *mSubsListener;


    public:
        testNetworksResource (OC::Cm::Device *cmDevice);
        virtual ~testNetworksResource();

        bool testGet();
        bool testPut();
        bool testSubscription(OC::Cm::Device *cmDevice);
        bool testUnSubscribe();

        NetworksResource *mNetworksResource;

};

#endif /* TESTNETWORKSRESOURCE_H_ */
