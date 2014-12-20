/*
 * testEnergyConsumptionResource.h
 *
 *  Created on: Oct 14, 2014
 *  Author: kumar.vinay@samsung.com
 */

#include "testOperationResource.h"

using namespace std;

class OperationResourceListener : public IOperationResourceResponseListener
{
    private:

        testOperationResource *mRes;
        OC::Cm::Device *mDevice ;

    public:

        OperationResourceListener(testOperationResource *tempRes , OC::Cm::Device *device)
        {
            mRes = tempRes ;
            mDevice = device ;
        }

        bool ongetOperation( int &requestId , int status, ::Operation *respData)
        {
            if (status == 204 || status == 200)
            {
                cout << "ongetOperation:Success. Status:" << status;
                mRes->testSubscription(mDevice);
                return true;
            }
            else
            {
                cout << "ongetOperation : failed. Status" << status;
                return false;
            }
        }

        bool onputOperation( int &requestId, int status )
        {
            if (status == 204)
            {
                cout << "onputOperation : Success. Status: " << status;
                return true;
            }
            else
            {
                cout << "onputOperation failed. Status: " << status;
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


testOperationResource::testOperationResource (OC::Cm::Device *cmDevice)
{
    mOperationResource = (OperationResource *)cmDevice->getResource("Operation");

    if (NULL == mOperationResource)
    {
        cout << "testOperationResource :: [Error] OperationResource resource object is NULL\n";
        mResListener = NULL;
        mSubsListener = NULL;
    }

    else
    {
        mResListener  = new OperationResourceListener(this, cmDevice);
        mSubsListener = new SubscriptionResponseListener(this);
        if (mResListener != NULL)
            mOperationResource->addResponseListener(*mResListener);
        mOperationResource->buildPath("0");
    }
}

testOperationResource::~testOperationResource()
{
    if (NULL != mSubsListener)
        delete mSubsListener ;
    if (NULL != mResListener)
        delete mResListener ;
    if (NULL != mOperationResource)
        delete mOperationResource ;
}

bool testOperationResource::testGet()
{
    if (NULL == mOperationResource)
    {
        cout << "test_getOperation :: [Error] OperationResource resource object is NULL\n";
        return false;
    }

    int requestId ;
    bool result = mOperationResource->getOperation(requestId);

    if (true == result)
    {
        cout << "testOperationResource::testGet:: executed successfully" << std::endl;
    }
    else
    {
        cout << "testOperationResource::testGet:: FAILED" << std::endl;
    }

    return true ;
}

bool testOperationResource::testPut()
{

    if (NULL == mOperationResource)
    {
        cout << "testOperationResource::testPut :: [Error] OperationResource resource object is NULL\n";
        return false;
    }


    Operation repData;
    int requestId ;
    repData.mpState = new StateType();
    repData.mpState->value = "Pause";

    bool result = mOperationResource->putOperation(requestId, repData );

    if (true == result)
    {
        cout << "testOperationResource::testPut:: executed successfully" << std::endl;
    }
    else
    {
        cout << "testOperationResource::testPut:: FAILED" << std::endl;
    }

    return result ;
}

bool testOperationResource::testSubscription(OC::Cm::Device *cmDevice)
{
    if (NULL == mOperationResource)
    {
        cout << "testOperationResource::testSubscription :: [Error] OperationResource resource object is NULL\n";
        return false;
    }

    std::string deviceId = "0" ;
    int requestId ;
    mOperationResource->buildPath(deviceId);

    OC::Cm::CMError error =  mOperationResource->subscribe(requestId, *mSubsListener ,
                             "test subscription");
    if (OC::Cm::ErrorCode::CM_SUCCESS ==   error.getErrorCode() )
    {
        cout << "testOperationResource::testSubscription  is called successfully\n";
    }
    else
    {
        cout << "testOperationResource::testSubscription " << error.getErrorMessage() << "\n";
        return false;
    }

    return true ;
}

bool testOperationResource::testUnSubscribe()
{
    cout << "testOperationResource::testUnSubscribe not implemented\n" ;
    return true ;
}

