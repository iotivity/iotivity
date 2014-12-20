/*
 * testEnergyConsumptionResource.h
 *
 *  Created on: Oct 14, 2014
 *  Author: kumar.vinay@samsung.com
 */

#ifndef TESTENERGYCONSUMPTIONRESOURCE_H_
#define TESTENERGYCONSUMPTIONRESOURCE_H_

#include "Client/EnergyConsumptionResource.h"
#include "Device.h"
#include "testSubscriptionListener.h"
#include "ITestResource.h"

class testEnergyConsumptionResource : public ITestResource
{

        IEnergyConsumptionResourceResponseListener *mResListener;
        OC::Cm::Client::Resource::IResourceResponseListener *mSubsListener;

    public:
        testEnergyConsumptionResource (OC::Cm::Device *cmDevice);
        virtual ~testEnergyConsumptionResource();

        bool testGet();
        bool testPut();
        bool testSubscription(OC::Cm::Device *cmDevice);
        bool testUnSubscribe();

        EnergyConsumptionResource *mEnergyConsumptionRes;
};

#endif /* TESTENERGYCONSUMPTIONRESOURCE_H_ */
