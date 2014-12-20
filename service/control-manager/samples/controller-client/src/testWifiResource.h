/*
 * testWifiResource.h
 *
 *  Created on: 07-Nov-2014
 *      Author: user
 */

#ifndef TESTWIFIRESOURCE_H_
#define TESTWIFIRESOURCE_H_

#include "ITestResource.h"
#include "Device.h"
#include "Client/WiFiResource.h"
#include "testSubscriptionListener.h"

class testWifi : public ITestResource
{

        IWiFiResourceResponseListener *mResListener;
        OC::Cm::Client::Resource::IResourceResponseListener *mSubsListener;

    public:
        testWifi(OC::Cm::Device *cmDevice);
        virtual ~testWifi();

        bool testGet();
        bool testPut();
        bool testSubscription(OC::Cm::Device *cmDevice);
        bool testUnSubscribe();

        WiFiResource *mWifi;

};


#endif /* TESTWIFIRESOURCE_H_ */
