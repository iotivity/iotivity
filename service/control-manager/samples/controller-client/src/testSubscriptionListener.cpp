/*
 * Information_Controller.cpp
 *
 *  Created on: 13-Oct-2014
 *  Author: kumar.vinay@samsung.com
 */



#include "testConfiguration.h"

using namespace std;

SubscriptionResponseListener::SubscriptionResponseListener(ITestResource *resObj)
{
    mResObj = resObj ;
}
void SubscriptionResponseListener::OnGetResponseReceived(int requestId, int status,
        OC::Cm::Connector::HeadersList &headers,  OC::Cm::Serialization::ISerializable *response)
{
    cout << "OnGetResponseReceived\n";
}

void SubscriptionResponseListener::OnPutResponseReceived(int requestId, int status,
        OC::Cm::Connector::HeadersList &headers, OC::Cm::Serialization::ISerializable *response)
{
    cout << "OnPutResponseReceived\n";
}

void SubscriptionResponseListener::OnPostResponseReceived(int requestId, int status,
        OC::Cm::Connector::HeadersList &headers,  OC::Cm::Serialization::ISerializable *response)
{
    cout << "OnPostResponseReceived\n";
}

void SubscriptionResponseListener::onDeleteResponseReceived(int requestId, int status,
        OC::Cm::Connector::HeadersList &headers, OC::Cm::Serialization::ISerializable *response)
{
    cout << "onDeleteResponseReceived\n";
}

void SubscriptionResponseListener::onSubscribeResponseReceived( int requestId, int status,
        std::string &subscriptionUri )
{
    cout << "\nonSubscribeResponseReceived : " << subscriptionUri.c_str() << "\n";
    //mResObj->testUnSubscribe();

}

void SubscriptionResponseListener::onUnSubscribeResponseReceived(int requestId, int status )
{
    cout << "\nonUnSubscribeResponseReceived\n";
}

void SubscriptionResponseListener::onErrorReceived( int requestId, int status,
        OC::Cm::CMError error, OC::Cm::Serialization::ISerializable *errorMessage )
{
    cout << "onErrorReceived\n";
}

void SubscriptionResponseListener::onRequestComplete(int requestId, std::string requestPayload,
        std::string responsePayload)
{
    cout << "onRequestComplete\n";
    ProgramUtils::DisplayAvailableResources();

}
