/*
 * testHumidityResource.cpp
 *
 *  Created on: Oct 9, 2014
 *      Author: Shunmuga Sundar
 */

#include "testHumidityResource.h"

using namespace std;

class HumidityResourceResponseListener : public IHumidityResourceResponseListener
{

    private:
        testHumidityResource *mRes;
        OC::Cm::Device *mDevice ;

    public:

        HumidityResourceResponseListener(testHumidityResource *tempRes , OC::Cm::Device *device)
        {
            mRes = tempRes ;
            mDevice = device ;
        }

        bool ongetHumidity( int &requestId , int status, ::Humidity *respData)
        {
            if (status == 204 || status == 200)
            {
                cout << "ongetHumidity Success. Status: " << status;
                mRes->testSubscription(mDevice);//subscribe to  the resource
                return true;
            }
            else
            {
                cout << "ongetHumidity failed. Status: " << status;
                return false;
            }
        }

        bool onputHumidity( int &requestId, int status )
        {
            if (status == 204)
            {
                cout << "putHumidity success with return code" << status;

            }
            else
            {
                cout << "onputHumidity failed with error code: " << status;
            }
            return true;
        }

        void onSubscribeResponseReceived( int requestId, int status, std::string &subscriptionUri )
        {
            cout << "subscribe Humidity return code" << status << endl;
            mRes->testUnSubscribe();

        }

        void onUnSubscribeResponseReceived(int requestId, int status )
        {
            cout << "unsubscribe Humidity return code" << status << endl;
        }

        void onError( int requestId, int statusCode, OC::Cm::CMError error,
                      OC::Cm::Serialization::Xsd::CMErrorMessage *errorMessage  )
        {
            cout << "ERROR Humidity return code" << statusCode << endl;
        }

};

testHumidityResource::testHumidityResource (OC::Cm::Device *cmDevice)
{
    mHumidityRes = (HumidityResource *)cmDevice->getResource("Humidity");

    mResListener  = new HumidityResourceResponseListener(this, cmDevice);
    mSubsListener = new SubscriptionResponseListener(this);

    mHumidityRes->addResponseListener(*mResListener);
    mHumidityRes->buildPath("0");
}

testHumidityResource::~testHumidityResource()
{
    delete mSubsListener ;
    delete mResListener ;
    delete mHumidityRes ;

}

bool testHumidityResource::testGet()
{
    int requestId ;
    bool result = mHumidityRes->getHumidity(requestId);

    if (true == result)
    {
        cout << "testHumidityResource::testGet::executed successfully" << std::endl;
    }
    else
    {
        cout << "testHumidityResource::testGet:: FAILED" << std::endl;
    }

    return result ;
}

bool testHumidityResource::testPut()
{
    int requestId ;
    Humidity repData;
    Float32 *mCurrent = new Float32();
    HumidityUnitType *mUnit = new HumidityUnitType();

    mCurrent->value = 90.0;
    repData.mpDesired = mCurrent;
    mUnit->value = "Percentage";
    repData.mpUnit = mUnit;
    return mHumidityRes->putHumidity(requestId, repData );
}

bool testHumidityResource::testSubscription(OC::Cm::Device *cmDevice)
{
    std::string deviceId = "0";
    int requestId ;

    mHumidityRes->buildPath(deviceId);
    OC::Cm::CMError error = mHumidityRes->subscribe(requestId, *mSubsListener , "test subscription");
    if (OC::Cm::ErrorCode::CM_SUCCESS ==   error.getErrorCode() )
    {
        cout << "testHumidityResource::testSubscription  is called successfully\n";
    }
    else
    {
        cout << "testHumidityResource::testSubscription " << error.getErrorMessage() << "\n";
        return false;
    }
    return true ;
}


bool testHumidityResource::testUnSubscribe()
{
    int requestId ;

    OC::Cm::CMError error = mHumidityRes->unSubscribe(requestId, *mSubsListener );
    if (OC::Cm::ErrorCode::CM_SUCCESS ==   error.getErrorCode() )
    {
        cout << "testHumidityResource::testUnSubscribe  is called successfully\n";
    }
    else
    {
        cout << "testHumidityResource::testUnSubscribe " << error.getErrorMessage() << "\n";
        return false;
    }

    return true;

}
