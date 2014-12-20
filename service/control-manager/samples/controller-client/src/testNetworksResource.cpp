/*
 * testNetworksResource.h
 *
 *  Created on: Oct 14, 2014
 *  Author: kumar.vinay@samsung.com
 */

#include "testNetworksResource.h"

using namespace std;

class NetworksResourceListener : public INetworksResourceResponseListener
{

    private:

        testNetworksResource *mTempRes;
        OC::Cm::Device *mDevice ;

    public:

        NetworksResourceListener(testNetworksResource *tempRes , OC::Cm::Device *device)
        {
            mTempRes = tempRes ;
            mDevice = device ;
        }

        bool ongetNetworks(int &requestId , int status, ::Networks *respData)
        {

            if (status == 204 || status == 200)
            {
                cout << "ongetNetworks : Success. Status: " << status;
                mTempRes->testSubscription(mDevice);
                return false;
            }
            else
            {
                cout << "ongetTemperatures failed. Status: " << status;
                return true;
            }
        }

        bool onputNetworks( int &requestId, int status )
        {
            if (status == 204 || status == 200)
            {
                cout << "onputTemperatures success. Status: " << status;
                return true;
            }
            else
            {
                cout << "onputTemperatures failed. Status: " << status;
                return false;
            }
        }

        void onSubscribeResponseReceived( int requestId, int status, std::string &subscriptionUri )
        {

        }

        void onUnSubscribeResponseReceived(int requestId, int status )
        {

        }

        void onError( int requestId, int statusCode, OC::Cm::CMError error,
                      OC::Cm::Serialization::Xsd::CMErrorMessage *errorMessage  )
        {

        }

};


testNetworksResource::testNetworksResource (OC::Cm::Device *cmDevice)
{
    mNetworksResource = (NetworksResource *)cmDevice->getResource("Networks");

    if (NULL == mNetworksResource)
    {
        cout << "testTemperaturesResource :: [Error] TemperaturesResource resource object is NULL\n";
        mResListener = NULL;
        mSubsListener = NULL;
    }

    else
    {
        mResListener  = new NetworksResourceListener(this, cmDevice);
        mSubsListener = new SubscriptionResponseListener(this);
        if (mResListener != NULL)
            mNetworksResource->addResponseListener(*mResListener);
        mNetworksResource->buildPath("0");
    }

}

testNetworksResource::~testNetworksResource()
{
    if (NULL != mSubsListener)
        delete mSubsListener ;
    if (NULL != mResListener)
        delete mResListener ;
    if (NULL != mNetworksResource)
        delete mNetworksResource ;
}

bool
testNetworksResource::testGet()
{
    if (NULL == mNetworksResource)
    {
        cout << "testGet :: [Error] NetworksResource resource object is NULL\n";
        return false;
    }

    int requestId ;
    bool result = mNetworksResource->getNetworks(requestId);

    if (true == result)
    {
        cout << "testNetworksResource::testGet:: executed successfully" << std::endl;
    }

    else
    {
        cout << "testNetworksResource::testGet:: FAILED" << std::endl;
    }

    return result ;
}

bool testNetworksResource::testPut()
{

    if (NULL == mNetworksResource)
    {
        cout << "testNetworksResource::testPut :: [Error] NetworksResource resource object is NULL\n";
        return false;
    }
    cout << "testNetworksResource::testPut not implemented\n" ;
    return true ;
}

bool testNetworksResource::testSubscription(OC::Cm::Device *cmDevice)
{

    if (NULL == mNetworksResource)
    {
        cout << "testSubscription :: [Error] NetworksResource resource object is NULL\n";
        return false;
    }

    std::string deviceId = "0" ;
    int requestId ;
    mNetworksResource->buildPath(deviceId);
    OC::Cm::CMError error =  mNetworksResource->subscribe(requestId, *mSubsListener ,
                             "test subscription");
    if (OC::Cm::ErrorCode::CM_SUCCESS ==   error.getErrorCode() )
    {
        cout << "testNetworksResource::testSubscription  is called successfully\n";
    }
    else
    {
        cout << "testNetworksResource::testSubscription " << error.getErrorMessage() << "\n";
        return false;
    }
    return true ;
}
bool testNetworksResource::testUnSubscribe()
{
    cout << "testNetworksResource::testUnSubscribe not implemented\n" ;
    return true ;
}

