/*
 * testDoors.cpp
 *
 *  Created on: Oct 14, 2014
 *      Author: manjunath.d
 */

#include "testUsage.h"
#include "Client/IUsageLimitResourceResponseListener.h"
#include "xsd/UsageLimit.h"
#include "xsd/Doors.h"

using namespace std;

class UsageResourceResponseListener : public IUsageLimitResourceResponseListener
{

    private:

        testUsage *mRes;
        OC::Cm::Device *mDevice ;

    public:

        UsageResourceResponseListener(testUsage *tempRes , OC::Cm::Device *device)
        {
            mRes = tempRes ;
            mDevice = device ;
        }

        bool ongetUsageLimit( int &requestId , int status, ::UsageLimit *respData)
        {
            if (status == 204 || status == 200)
            {
                cout << "ongetUsageLimit : Success. Status:" << status;
                mRes->testSubscription(mDevice);
                return true;
            }
            else
            {
                cout << "ongetDoors : failed. Status" << status;
                return false;
            }
        }
        bool onputUsageLimit( int &requestId, int status )
        {
            if (status == 204)
            {
                // Light powered off successfully
            }
            return true;
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

        bool ondeleteUsageLimit( int &requestId, int status )
        {

            if (status == 204 || status == 200)
            {
                cout << "ondeleteUsageLimit : Success. Status:" << status;
                return true;
            }
            else
            {
                cout << "ongetDoors : failed. Status" << status;
                return false;
            }
        }

};


testUsage::testUsage( OC::Cm::Device *cmDevice)
{
    // TODO Auto-generated constructor stub

    mUsageLimit = (UsageLimitResource *)cmDevice->getResource("UsageLimit");

    mResListener  = new UsageResourceResponseListener(this, cmDevice);
    mSubsListener = new SubscriptionResponseListener(this);

    mUsageLimit->addResponseListener(*mResListener);
    mUsageLimit->buildPath("0");

}

testUsage::~testUsage()
{
    delete mSubsListener ;
    delete mResListener ;
    delete mUsageLimit ;
}

bool
testUsage::testGet()
{
    int requestId  = 0;
    bool result = mUsageLimit->getUsageLimit(requestId);
    if (true == result)
    {
        cout << "testUsage::testGet::executed successfully" << std::endl;
    }
    else
    {
        cout << "testUsage::testGet:: FAILED" << std::endl;
    }

    return result ;
}

bool testUsage::testPut()
{
    cout << "testUsage::testPut not implemented\n" ;
    return true ;
}
bool testUsage::testSubscription(OC::Cm::Device *cmDevice)
{

    std::string deviceId = "0" ;
    int requestId ;

    mUsageLimit->buildPath(deviceId);
    OC::Cm::CMError error =  mUsageLimit->subscribe(requestId, *mSubsListener , "test subscription");
    if (OC::Cm::ErrorCode::CM_SUCCESS ==   error.getErrorCode() )
    {
        cout << "testUsage::testSubscription  is called successfully\n";
    }
    else
    {
        cout << "testUsage::testSubscription " << error.getErrorMessage() << "\n";
        return false;
    }

    return true ;
}

bool testUsage::testUnSubscribe()
{
    cout << "testDoors::testUnSubscribe not implemented\n" ;
    return true ;
}

