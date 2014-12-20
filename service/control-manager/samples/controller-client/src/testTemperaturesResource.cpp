/*
 * testTemperaturesResource.h
 *
 *  Created on: Oct 14, 2014
 *  Author: kumar.vinay@samsung.com
 */

#include "testTemperaturesResource.h"

using namespace std;

class TemperaturesResourceListener : public ITemperaturesResourceResponseListener
{

    private:

        testTemperaturesResource *mTempRes;
        OC::Cm::Device *mDevice ;

    public:

        TemperaturesResourceListener(testTemperaturesResource *tempRes , OC::Cm::Device *device)
        {
            mTempRes = tempRes ;
            mDevice = device ;
        }

        bool ongetTemperatures(int &requestId , int status, ::Temperatures *respData)
        {

            if (status == 204 || status == 200)
            {
                cout << "ongetTemperatures : Success. Status: " << status;
                mTempRes->testSubscription(mDevice);
                return false;
            }
            else
            {
                cout << "ongetTemperatures failed. Status: " << status;
                return true;
            }
        }

        bool onputTemperatures( int &requestId, int status )
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


testTemperaturesResource::testTemperaturesResource (OC::Cm::Device *cmDevice)
{
    mTemperaturesResource = (TemperaturesResource *)cmDevice->getResource("Temperatures");

    if (NULL == mTemperaturesResource)
    {
        cout << "testTemperaturesResource :: [Error] TemperaturesResource resource object is NULL\n";
        mResListener = NULL;
        mSubsListener = NULL;
    }

    else
    {
        mResListener  = new TemperaturesResourceListener(this, cmDevice);
        mSubsListener = new SubscriptionResponseListener(this);
        if (mResListener != NULL)
            mTemperaturesResource->addResponseListener(*mResListener);
        mTemperaturesResource->buildPath("0");
    }

}

testTemperaturesResource::~testTemperaturesResource()
{
    if (NULL != mSubsListener)
        delete mSubsListener ;
    if (NULL != mResListener)
        delete mResListener ;
    if (NULL != mTemperaturesResource)
        delete mTemperaturesResource ;
}

bool
testTemperaturesResource::testGet()
{
    if (NULL == mTemperaturesResource)
    {
        cout << "testTemperaturesResource::testGet :: [Error] TemperaturesResource resource object is NULL\n";
        return false;
    }

    int requestId ;
    bool result = mTemperaturesResource->getTemperatures(requestId);

    if (true == result)
    {
        cout << "testTemperaturesResource::testGet:: executed successfully" << std::endl;
    }

    else
    {
        cout << "testTemperaturesResource::testGet:: FAILED" << std::endl;
    }

    return result ;
}

bool testTemperaturesResource::testPut()
{

    if (NULL == mTemperaturesResource)
    {
        cout << "testTemperaturesResource::testPut :: [Error] TemperaturesResource resource object is NULL\n";
        return false;
    }


    Temperatures repData;
    int requestId  = 5456;

    Temperature *temp1 = new Temperature();
    temp1->mpId = new String50();
    temp1->mpId->value = "1";
    temp1->mpDescription = new String50();
    temp1->mpDescription->value = "ID ONE";
    temp1->mpDesired = new Float32();
    temp1->mpDesired->value = -17.0;

    Temperature *temp2 = new Temperature();
    temp2->mpId = new String50();
    temp2->mpId->value = "2";
    temp2->mpDescription = new String50();
    temp2->mpDescription->value = "ID TWO";
    temp2->mpDesired = new Float32();
    temp2->mpDesired->value = 4.0;

    repData.mpTemperature.push_back(temp1);
    repData.mpTemperature.push_back(temp2);

    bool result = mTemperaturesResource->putTemperatures(requestId, repData );

    if (true == result)
    {
        cout << "test_putTemperatures:: executed successfully" << std::endl;
    }
    else
    {
        cout << "test_putTemperatures:: FAILED" << std::endl;
    }

    return result ;
}

bool testTemperaturesResource::testSubscription(OC::Cm::Device *cmDevice)
{
    if (NULL == mTemperaturesResource)
    {
        cout << "testTemperaturesResource::testSubscription :: [Error] TemperaturesResource resource object is NULL\n";
        return false;
    }

    std::string deviceId = "0" ;
    int requestId ;
    mTemperaturesResource->buildPath(deviceId);
    OC::Cm::CMError error =  mTemperaturesResource->subscribe(requestId, *mSubsListener ,
                             "test subscription");
    if (OC::Cm::ErrorCode::CM_SUCCESS ==   error.getErrorCode() )
    {
        cout << "testTemperaturesResource::testSubscription  is called successfully\n";
    }
    else
    {
        cout << "testTemperaturesResource::testSubscription " << error.getErrorMessage() << "\n";
        return false;
    }

    return true ;
}

bool testTemperaturesResource::testUnSubscribe()
{
    cout << "testTemperaturesResource::testUnSubscribe not implemented\n" ;
    return true ;
}

