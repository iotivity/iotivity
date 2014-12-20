/*
 * testOperationResource.h
 *
 *  Created on: Oct 14, 2014
 *  Author: kumar.vinay@samsung.com
 */

#ifndef TESTOPERATIONSRESOURCE_H_
#define TESTOPERATIONSRESOURCE_H_

#include "Client/OperationResource.h"
#include "Device.h"
#include "testSubscriptionListener.h"
#include "ITestResource.h"


class testOperationResource : public ITestResource
{

        IOperationResourceResponseListener *mResListener;
        OC::Cm::Client::Resource::IResourceResponseListener *mSubsListener;

    public:
        testOperationResource (OC::Cm::Device *cmDevice);
        virtual ~testOperationResource();

        bool testGet();
        bool testPut();
        bool testSubscription(OC::Cm::Device *cmDevice);
        bool testUnSubscribe();

        OperationResource *mOperationResource;
};

#endif /* TESTOPERATIONSRESOURCE_H_ */
