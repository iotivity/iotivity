/*
 * testWifiResource.cpp
 *
 *  Created on: 07-Nov-2014
 *      Author: manjunath.d
 */

#include "testWifiResource.h"
#include "Client/IWiFiResourceResponseListener.h"
#include "xsd/UsageLimit.h"
#include "xsd/Doors.h"

using namespace std;

class WifiResourceResponseListener : public IWiFiResourceResponseListener
{

    private:

        testWifi *mRes;
        OC::Cm::Device *mDevice ;

    public:

        WifiResourceResponseListener(testWifi *tempRes , OC::Cm::Device *device)
        {
            mRes = tempRes ;
            mDevice = device ;
        }

        bool ongetWiFi( int &requestId , int status, ::WiFi *respData)
        {
            if (status == 204 || status == 200)
            {
                cout << "ongetWifi : Success. Status:" << status;
                mRes->testSubscription(mDevice);
                return true;
            }
            else
            {
                cout << "ongetWifi : failed. Status" << status;
                return false;
            }
        }
        bool onputWiFi( int &requestId, int status )
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

        bool ondeleteWifi( int &requestId, int status )
        {

            if (status == 204 || status == 200)
            {
                cout << "ondeleteWifi : Success. Status:" << status;
                return true;
            }
            else
            {
                cout << "ondeleteWifi : failed. Status" << status;
                return false;
            }
        }

};


testWifi::testWifi( OC::Cm::Device *cmDevice)
{
    // TODO Auto-generated constructor stub

    mWifi = (WiFiResource *)cmDevice->getResource("WiFi");

    mResListener  = new WifiResourceResponseListener(this, cmDevice);
    mSubsListener = new SubscriptionResponseListener(this);

    mWifi->addResponseListener(*mResListener);
    mWifi->buildPath("0", "1");

}

testWifi::~testWifi()
{
    delete mSubsListener ;
    delete mResListener ;
    delete mWifi ;
}

bool
testWifi::testGet()
{
    int requestId  = 0;
    bool result = mWifi->getWiFi(requestId);
    if (true == result)
    {
        cout << "testWifi::testGet::executed successfully" << std::endl;
    }
    else
    {
        cout << "testWifi::testGet:: FAILED" << std::endl;
    }

    return result ;
}

bool testWifi::testPut()
{
    cout << "testWifi::testPut not implemented\n" ;
    return true ;
}

bool testWifi::testSubscription(OC::Cm::Device *cmDevice)
{

    std::string deviceId = "0" ;
    int requestId ;

    mWifi->buildPath(deviceId, "0");
    OC::Cm::CMError error =  mWifi->subscribe(requestId, *mSubsListener , "test subscription");
    if (OC::Cm::ErrorCode::CM_SUCCESS ==   error.getErrorCode() )
    {
        cout << "testWifi::testSubscription  is called successfully\n";
    }
    else
    {
        cout << "testWifi::testSubscription " << error.getErrorMessage() << "\n";
        return false;
    }

    return true ;
}

bool testWifi::testUnSubscribe()
{
    cout << "testWifi::testUnSubscribe not implemented\n" ;
    return true ;
}
