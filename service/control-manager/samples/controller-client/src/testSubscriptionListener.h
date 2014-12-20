/*
 * testTemperaturesResource.h
 *
 *  Created on: Oct 15, 2014
 *  Author: kumar.vinay@samsung.com
 */

#ifndef TESTRESOURCERESPONSELISTENER_H
#define TESTRESOURCERESPONSELISTENER_H

#include "ProgramUtils.h"
#include <iostream>
#include "ITestResource.h"



class SubscriptionResponseListener : public OC::Cm::Client::Resource::IResourceResponseListener
{
        ITestResource *mResObj;

    public :

        SubscriptionResponseListener(ITestResource *resObj) ;
        virtual void OnGetResponseReceived(int requestId, int status,
                                           OC::Cm::Connector::HeadersList &headers,  OC::Cm::Serialization::ISerializable *response);
        virtual void OnPutResponseReceived(int requestId, int status,
                                           OC::Cm::Connector::HeadersList &headers, OC::Cm::Serialization::ISerializable *response);
        virtual void OnPostResponseReceived(int requestId, int status,
                                            OC::Cm::Connector::HeadersList &headers,  OC::Cm::Serialization::ISerializable *response);
        virtual void onDeleteResponseReceived(int requestId, int status,
                                              OC::Cm::Connector::HeadersList &headers, OC::Cm::Serialization::ISerializable *response);
        virtual void onSubscribeResponseReceived( int requestId, int status, std::string &subscriptionUri );
        virtual void onUnSubscribeResponseReceived(int requestId, int status );
        virtual void onErrorReceived( int requestId, int status, OC::Cm::CMError error,
                                      OC::Cm::Serialization::ISerializable *errorMessage );
        virtual void onRequestComplete(int requestId, std::string requestPayload,
                                       std::string responsePayload);
};

#endif /* TESTRESOURCERESPONSELISTENER_H */
