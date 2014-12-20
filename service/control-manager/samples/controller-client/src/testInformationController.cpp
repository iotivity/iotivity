/*
 * Information_Controller.cpp
 *
 *  Created on: 13-Oct-2014
 *  Author: kumar.vinay@samsung.com
 */



#include "testInformationController.h"

using namespace std;


class InfoResourceResponseListener : public IInformationResourceResponseListener
{

    private:

        testInformationController *mRes;
        OC::Cm::Device *mDevice ;

    public:

        InfoResourceResponseListener(testInformationController *tempRes , OC::Cm::Device *device)
        {
            mRes = tempRes ;
            mDevice = device ;
        }

        bool ongetInformation( int &requestId , int status, ::Information *respData)
        {
            if (status == 204 || status == 200)
            {
                cout << "ongetInformation:Success. Status:" << status;
                mRes->testSubscription(mDevice);
                return true;
            }
            else
            {
                cout << "ongetInformation : failed. Status" << status;
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


testInformationController::testInformationController(OC::Cm::Device *cmDevice)
{
    // TODO Auto-generated constructor stub

    mInfoRes = (InformationResource *)cmDevice->getResource("Information");

    if (NULL == mInfoRes)
    {
        cout << "testInformationController :: [Error] InformationResource resource object is NULL\n";
        mResListener = NULL;
        mSubsListener = NULL;
    }
    else
    {
        mResListener  = new InfoResourceResponseListener(this, cmDevice);
        mSubsListener = new SubscriptionResponseListener(this);
        if (mResListener != NULL)
            mInfoRes->addResponseListener(*mResListener);
        mInfoRes->buildPath("0");
    }

}

testInformationController::~testInformationController()
{
    if (NULL != mSubsListener)
        delete mSubsListener ;
    if (NULL != mResListener)
        delete mResListener ;
    if (NULL != mInfoRes)
        delete mInfoRes ;
}


bool testInformationController::testGet()
{
    if (NULL == mInfoRes)
    {
        cout << "testInformationController::testGet :: [Error] InformationResource resource object is NULL\n";
        return false;
    }
    int requestId ;
    bool result = mInfoRes->getInformation(requestId);

    if (true == result)
    {
        cout << "testInformationController::testGet:: executed successfully" << std::endl;
    }
    else
    {
        cout << "testInformationController::testGet:: FAILED" << std::endl;
    }

    return result ;
}

bool testInformationController::testPut()
{
    cout << "testInformationController::testPut not implemented\n" ;
    return true ;
}
bool testInformationController::testSubscription(OC::Cm::Device *cmDevice)
{
    if (NULL == mInfoRes)
    {
        cout << "testInformationController::testSubscription :: [Error] InformationResource resource object is NULL\n";
        return false;
    }
    std::string deviceId = "0" ;
    int requestId ;

    mInfoRes->buildPath(deviceId);
    OC::Cm::CMError error =  mInfoRes->subscribe(requestId, *mSubsListener , "test subscription");
    if (OC::Cm::ErrorCode::CM_SUCCESS ==   error.getErrorCode() )
    {
        cout << "testInformationController::testSubscription  is called successfully\n";
    }
    else
    {
        cout << "testInformationController::testSubscription " << error.getErrorMessage() << "\n";
        return false;
    }

    return true ;
}

bool testInformationController::testUnSubscribe()
{
    cout << "testInformationController::testUnSubscribe not implemented\n" ;
    return true ;
}

