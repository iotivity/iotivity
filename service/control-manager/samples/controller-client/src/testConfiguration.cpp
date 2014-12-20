/*
 * testConfiguration.cpp
 *
 *  Created on: Oct 14, 2014
 *      Author: manjunath.d
 */

#include "testConfiguration.h"

using namespace std;
class ConfigurationResourceResponseListener : public IConfigurationResourceResponseListener
{

    private:

        testConfiguration *mRes;
        OC::Cm::Device *mDevice ;

    public:

        ConfigurationResourceResponseListener(testConfiguration *tempRes , OC::Cm::Device *device)
        {
            mRes = tempRes ;
            mDevice = device ;
        }

        bool ongetConfiguration( int &requestId , int status, ::Configuration *respData)
        {
            if (status == 204 || status == 200)
            {
                cout << "ongetConfiguration : Success. Status:" << status;
                mRes->testSubscription(mDevice);
                return true;
            }
            else
            {
                cout << "ongetConfiguration : failed. Status" << status;
                return false;
            }

        }
        bool onputConfiguration( int &requestId, int status )
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





testConfiguration::testConfiguration (OC::Cm::Device *cmDevice)
{
    // TODO Auto-generated constructor stub

    mConfigRes = (ConfigurationResource *)cmDevice->getResource("Configuration");

    mResListener  = new ConfigurationResourceResponseListener(this, cmDevice);
    mSubsListener = new SubscriptionResponseListener(this);

    mConfigRes->addResponseListener(*mResListener);
    mConfigRes->buildPath("0");

}

testConfiguration::~testConfiguration()
{
    delete mSubsListener ;
    delete mResListener ;
    delete mConfigRes ;
}

bool
testConfiguration::testGet()
{
    int requestId ;
    bool result = mConfigRes->getConfiguration(requestId);
    if (true == result)
    {
        cout << "testConfiguration::testGet::executed successfully" << std::endl;
    }
    else
    {
        cout << "testConfiguration::testGet:: FAILED" << std::endl;
    }

    return true ;
}

bool testConfiguration::testPut()
{

    int requestId  = 34556;
    Configuration *respData = new Configuration();
    respData->mpPairingStart = new OnType();
    respData->mpPairingStart->value = "On";

    bool result = mConfigRes->putConfiguration(requestId, *respData );
    if (true == result)
    {
        cout << "testConfiguration::testPut::executed successfully" << std::endl;
    }
    else
    {
        cout << "testConfiguration::testPut:: FAILED" << std::endl;
    }
    return result ;
}
bool testConfiguration::testSubscription(OC::Cm::Device *cmDevice)
{
    std::string deviceId = "0" ;
    int requestId ;

    mConfigRes->buildPath(deviceId);
    OC::Cm::CMError error =  mConfigRes->subscribe(requestId, *mSubsListener , "test subscription");
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
bool testConfiguration::testUnSubscribe()
{
    cout << "testConfiguration::testUnSubscribe not implemented\n" ;
    return true ;
}


