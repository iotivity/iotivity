/*
 * Information_Controller.h
 *
 *  Created on: 13-Oct-2014
 *  Author: kumar.vinay@samsung.com
 */

#ifndef INFORMATION_CONTROLLER_H_
#define INFORMATION_CONTROLLER_H_

#include "Device.h"
#include "Client/InformationResource.h"
#include "testSubscriptionListener.h"
#include "ITestResource.h"


class testInformationController : public ITestResource
{

        IInformationResourceResponseListener *mResListener;
        OC::Cm::Client::Resource::IResourceResponseListener *mSubsListener;

    public:

        testInformationController(OC::Cm::Device *cmDevice);
        virtual ~testInformationController();

        bool testGet();
        bool testPut();
        bool testSubscription(OC::Cm::Device *cmDevice);
        bool testUnSubscribe();

        InformationResource *mInfoRes;

};


#endif /* INFORMATION_CONTROLLER_H_ */
