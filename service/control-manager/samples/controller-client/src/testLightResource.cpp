/*
 * testLightResource.h
 *
 *  Created on: Oct 14, 2014
 *  Author: kumar.vinay@samsung.com
 */

#include "testLightResource.h"

using namespace std;

class LightResourceListener : public ILightResourceResponseListener
{
    private:

        testLightResource *mRes;
        OC::Cm::Device *mDevice ;

    public:

        LightResourceListener(testLightResource *tempRes , OC::Cm::Device *device)
        {
            mRes = tempRes ;
            mDevice = device ;
        }

        bool ongetLight( int &requestId , int status, ::Light *respData)
        {
            if (status == 204 || status == 200)
            {
                cout << "ongetLight:Success. Status:" << status;
                mRes->testSubscription(mDevice);
                return true;
            }
            else
            {
                cout << "ongetLight : failed. Status" << status;
                return false;
            }
        }

        bool onputLight( int &requestId, int status )
        {
            if (status == 204)
            {
                cout << "onputLight : Success. Status: " << status;
                return true;
            }
            else
            {
                cout << "onputLight failed. Status: " << status;
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


testLightResource::testLightResource (OC::Cm::Device *cmDevice)
{
    mLightResource = (LightResource *)cmDevice->getResource("Light");

    if (NULL == mLightResource)
    {
        cout << "testLightResource :: [Error] LightResource resource object is NULL\n";
        mResListener = NULL;
        mSubsListener = NULL;
    }

    else
    {
        mResListener  = new LightResourceListener(this, cmDevice);
        mSubsListener = new SubscriptionResponseListener(this);
        if (mResListener != NULL)
            mLightResource->addResponseListener(*mResListener);
        mLightResource->buildPath("0");
    }
}

testLightResource::~testLightResource()
{
    if (mSubsListener)
        delete mSubsListener ;
    if (mResListener)
        delete mResListener ;
    if (mLightResource)
        delete mLightResource ;
}

bool testLightResource::testGet()
{
    if (NULL == mLightResource)
    {
        cout << "testLightResource::testGet :: [Error] LightResource resource object is NULL\n";
        return false;
    }

    int requestId ;
    bool result = mLightResource->getLight(requestId);

    if (true == result)
    {
        cout << "testLightResource::testGet:: executed successfully" << std::endl;
    }
    else
    {
        cout << "testLightResource::testGet:: FAILED" << std::endl;
    }

    return true ;
}

bool testLightResource::testPut()
{
    if (NULL == mLightResource)
    {
        cout << "testLightResource::testPut :: [Error] LightResource resource object is NULL\n";
        return false;
    }

    Light reqData ;
    reqData.mpDimmingLevel = new uint8_t() ;
    *(reqData.mpDimmingLevel) = 70;
    int requestId;

    bool result = mLightResource->putLight(requestId, reqData );
    if (true == result)
    {
        cout << "testLightResource::testPut:: executed successfully" << std::endl;
    }
    else
    {
        cout << "testLightResource::testPut:: FAILED" << std::endl;
    }

    return result ;
}

bool testLightResource::testSubscription(OC::Cm::Device *cmDevice)
{
    if (NULL == mLightResource)
    {
        cout << "testLightResource::testSubscription :: [Error] LightResource resource object is NULL\n";
        return false;
    }

    std::string deviceId = "0" ;
    int requestId ;
    mLightResource->buildPath(deviceId);
    OC::Cm::CMError error =   mLightResource->subscribe(requestId, *mSubsListener ,
                              "test subscription");
    if (OC::Cm::ErrorCode::CM_SUCCESS ==   error.getErrorCode() )
    {
        cout << "testLightResource::testSubscription  is called successfully\n";
    }
    else
    {
        cout << "testLightResource::testSubscription " << error.getErrorMessage() << "\n";
        return false;
    }

    return true ;
}

bool testLightResource::testUnSubscribe()
{

    if (NULL == mLightResource)
    {
        cout << "testLightResource::testUnSubscribe :: [Error] LightResource resource object is NULL\n";
        return false;
    }

    int requestId ;
    OC::Cm::CMError error =  mLightResource->unSubscribe(requestId, *mSubsListener );
    if (OC::Cm::ErrorCode::CM_SUCCESS ==   error.getErrorCode() )
    {
        cout << "testLightResource::testUnSubscribe  is called successfully\n";
    }
    else
    {
        cout << "testLightResource::testUnSubscribe " << error.getErrorMessage() << "\n";
        return false;
    }

    return true ;
}

