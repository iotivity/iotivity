/*
 * testConfiguration.h
 *
 *  Created on: Oct 14, 2014
 *      Author: manjunath.d
 */

#include "Device.h"
#include "Client/ConfigurationResource.h"
#include "testSubscriptionListener.h"
#include "ITestResource.h"

#ifndef TESTCONFIGURATION_H_
#define TESTCONFIGURATION_H_

class testConfiguration : public ITestResource
{

        IConfigurationResourceResponseListener *mResListener;
        OC::Cm::Client::Resource::IResourceResponseListener *mSubsListener;

    public:

        testConfiguration (OC::Cm::Device *cmDevice);
        virtual ~testConfiguration();

        bool testGet();
        bool testPut();
        bool testSubscription(OC::Cm::Device *cmDevice);
        bool testUnSubscribe();

        ConfigurationResource *mConfigRes;

};


#endif /* TESTCONFIGURATION_H_ */
