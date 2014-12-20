/*
 * testMode.cpp
 *
 *  Created on: Oct 14, 2014
 *      Author: manjunath.d
 */

#include "testMode.h"
#include "Client/IModeResourceResponseListener.h"

using namespace std;

class ModeResourceResponseListener : public IModeResourceResponseListener
{

    private:

        testMode *mRes;
        OC::Cm::Device *mDevice ;

    public:

        ModeResourceResponseListener(testMode *tempRes , OC::Cm::Device *device)
        {
            mRes = tempRes ;
            mDevice = device ;
        }

        bool ongetMode( int &requestId , int status, ::Mode *respData)
        {

            if (status == 204 || status == 200)
            {
                cout << "ongetMode : Success. Status:" << status;
                mRes->testSubscription(mDevice);
                return true;
            }
            else
            {
                cout << "ongetMode : failed. Status" << status;
                return false;
            }
        }
        bool onputMode( int &requestId, int status )
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

};





testMode::testMode(OC::Cm::Device *cmDevice)
{
    // TODO Auto-generated constructor stub

    mModeRes = (ModeResource *)cmDevice->getResource("Mode");

    mResListener  = new ModeResourceResponseListener(this, cmDevice);
    mSubsListener = new SubscriptionResponseListener(this);

    mModeRes->addResponseListener(*mResListener);
    mModeRes->buildPath("0");

}

testMode::~testMode()
{
    delete mSubsListener ;
    delete mResListener ;
    delete mModeRes ;
}

bool
testMode::testGet()
{
    int requestId  = 3454;
    bool result = mModeRes->getMode(requestId);
    if (true == result)
    {
        cout << "testMode::testGet::executed successfully" << std::endl;
    }
    else
    {
        cout << "testMode::testGet:: FAILED" << std::endl;
    }

    return true ;
}

bool testMode::testPut()
{

    int requestId  = 34556;
    Mode *respData = new Mode();
    respData->mpModes = new ArrayOfString();
    respData->mpModes->members.push_back("Opmode_Cool");
    respData->mpModes->members.push_back("Opmode_Sleep");

    return mModeRes->putMode(requestId, *respData );

}
bool testMode::testSubscription(OC::Cm::Device *cmDevice)
{
    std::string deviceId = "0" ;
    int requestId ;

    mModeRes->buildPath(deviceId);
    OC::Cm::CMError error =  mModeRes->subscribe(requestId, *mSubsListener , "test subscription");
    if (OC::Cm::ErrorCode::CM_SUCCESS ==   error.getErrorCode() )
    {
        cout << "testConfiguration::testSubscription  is called successfully\n";
    }
    else
    {
        cout << "testConfiguration::testSubscription " << error.getErrorMessage() << "\n";
        return false;
    }

    return true ;
}

bool testMode::testUnSubscribe()
{
    cout << "testMode::testUnSubscribe not implemented\n" ;
    return true ;
}

