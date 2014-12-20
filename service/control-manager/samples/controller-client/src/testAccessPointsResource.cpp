/*
 * testAccessPointsResource.h
 *
 *  Created on: Nov 11, 2014
 *  Author: kumar.vinay@samsung.com
 */

#include "testAccessPointsResource.h"

using namespace std;

class AccessPointsListener : public IAccessPointsResourceResponseListener
{

    private:

        testAccessPoints *mTempRes;
        OC::Cm::Device *mDevice ;

    public:

        AccessPointsListener(testAccessPoints *tempRes , OC::Cm::Device *device)
        {
            mTempRes = tempRes ;
            mDevice = device ;
        }

        bool ongetAccessPoints(int &requestId , int status, ::AccessPoints *respData)
        {

            if (status == 204 || status == 200)
            {
                cout << "ongetAccessPoints : Success. Status: " << status;
                mTempRes->testSubscription(mDevice);
                return false;
            }
            else
            {
                cout << "ongetAccessPoints failed. Status: " << status;
                return true;
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


testAccessPoints::testAccessPoints (OC::Cm::Device *cmDevice)
{
    mAccessPointsResource = (AccessPointsResource *)cmDevice->getResource("AccessPoints");

    if (NULL == mAccessPointsResource)
    {
        cout << "testAccessPoints :: [Error] AccessPointsResource resource object is NULL\n";
        mResListener = NULL;
        mSubsListener = NULL;
    }

    else
    {
        mResListener  = new AccessPointsListener(this, cmDevice);
        mSubsListener = new SubscriptionResponseListener(this);
        mAccessPointsResource->addResponseListener(*mResListener);
        mAccessPointsResource->buildPath("0", "0");
    }

}

testAccessPoints::~testAccessPoints()
{
    if (mSubsListener != NULL)
        delete mSubsListener ;
    if (mResListener != NULL)
        delete mResListener ;
    if (mAccessPointsResource != NULL)
        delete mAccessPointsResource ;
}

bool
testAccessPoints::testGet()
{
    if (NULL == mAccessPointsResource)
    {
        cout << "testGet :: [Error] AccessPointsResource resource object is NULL\n";
        return false;
    }

    int requestId ;
    bool result = mAccessPointsResource->getAccessPoints(requestId);

    if (true == result)
    {
        cout << "testAccessPoints::testGet:: executed successfully" << std::endl;
    }
    else
    {
        cout << "testAccessPoints::testGet:: FAILED" << std::endl;
    }

    return result ;
}

bool testAccessPoints::testPut()
{

    if (NULL == mAccessPointsResource)
    {
        cout << "testAccessPoints::testPut :: [Error] AccessPointsResource resource object is NULL\n";
        return false;
    }
    cout << "testAccessPoints::testPut not implemented\n" ;
    return true ;
}

bool testAccessPoints::testSubscription(OC::Cm::Device *cmDevice)
{

    if (NULL == mAccessPointsResource)
    {
        cout << "testAccessPoints::testSubscription :: [Error] AccessPointsResource resource object is NULL\n";
        return false;
    }

    std::string deviceId = "0" ;
    int requestId ;

    mAccessPointsResource->buildPath(deviceId, "0");
    OC::Cm::CMError error =  mAccessPointsResource->subscribe(requestId, *mSubsListener ,
                             "test subscription");
    if (OC::Cm::ErrorCode::CM_SUCCESS ==   error.getErrorCode() )
    {
        cout << "testAccessPoints::testSubscription  is called successfully\n";
    }
    else
    {
        cout << "testAccessPoints::testSubscription " << error.getErrorMessage() << "\n";
        return false;
    }
    return true ;
}
bool testAccessPoints::testUnSubscribe()
{
    cout << "testAccessPoints::testUnSubscribe not implemented\n" ;
    return true ;
}

