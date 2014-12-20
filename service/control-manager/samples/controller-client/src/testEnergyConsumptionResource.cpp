/*
 * testEnergyConsumptionResource.h
 *
 *  Created on: Oct 14, 2014
 *  Author: kumar.vinay@samsung.com
 */

#include "testEnergyConsumptionResource.h"

using namespace std;

class EnergyConsumptionResourceListener : public IEnergyConsumptionResourceResponseListener
{
    private:
        testEnergyConsumptionResource *mRes;
        OC::Cm::Device *mDevice ;

    public:
        EnergyConsumptionResourceListener(testEnergyConsumptionResource *tempRes , OC::Cm::Device *device)
        {
            mRes = tempRes ;
            mDevice = device ;
        }

        bool ongetEnergyConsumption( int &requestId , int status, ::EnergyConsumption *respData)
        {
            if (status == 204 || status == 200)
            {
                cout << "OnGetResponseReceived Success. status: " << status;
                mRes->testSubscription(mDevice);
                return true;
            }
            else
            {
                cout << "OnGetResponseReceived failed. Status: " << status;
                return false;
            }
        }

        bool onputEnergyConsumption( int &requestId, int status )
        {
            if (status == 204)
            {
                cout << "onputEnergyConsumption success. Status: " << status;
                return true;
            }
            else
            {
                cout << "onputEnergyConsumption failed. Status: " << status;
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


testEnergyConsumptionResource::testEnergyConsumptionResource (OC::Cm::Device *cmDevice)
{
    mEnergyConsumptionRes = (EnergyConsumptionResource *)cmDevice->getResource("EnergyConsumption");

    if (NULL == mEnergyConsumptionRes)
    {
        cout << "testEnergyConsumptionResource :: [Error] EnergyConsumption resource object is NULL\n";
        mResListener = NULL;
        mSubsListener = NULL;
    }
    else
    {
        mResListener  = new EnergyConsumptionResourceListener(this, cmDevice);
        mSubsListener = new SubscriptionResponseListener(this);
        if (mResListener != NULL)
            mEnergyConsumptionRes->addResponseListener(*mResListener);
        mEnergyConsumptionRes->buildPath("0");
    }
}

testEnergyConsumptionResource::~testEnergyConsumptionResource()
{
    if (NULL != mSubsListener)
        delete mSubsListener ;
    if (NULL != mResListener)
        delete mResListener ;
    if (NULL != mEnergyConsumptionRes)
        delete mEnergyConsumptionRes ;
}

bool testEnergyConsumptionResource::testGet()
{

    if (NULL == mEnergyConsumptionRes)
    {
        cout << "testEnergyConsumptionResource::testGet :: [Error] EnergyConsumption resource object is NULL\n";
        return false;
    }

    int requestId ;
    bool result = mEnergyConsumptionRes->getEnergyConsumption(requestId);

    if (true == result)
    {
        cout << "testEnergyConsumptionResource::testGet:: executed successfully" << std::endl;
    }
    else
    {
        cout << "testEnergyConsumptionResource::testGet:: FAILED" << std::endl;
    }

    return true ;
}

bool testEnergyConsumptionResource::testPut()
{

    if (NULL == mEnergyConsumptionRes)
    {
        cout << "testEnergyConsumptionResource::testPut :: [Error] EnergyConsumption resource object is NULL\n";
        return false;
    }


    EnergyConsumption repData;
    int requestId ;

    repData.mpReportInterval = new uint32_t();
    *(repData.mpReportInterval) = 2;
    bool result = mEnergyConsumptionRes->putEnergyConsumption(requestId, repData );

    if (true == result)
    {
        cout << "testEnergyConsumptionResource::testPut:: executed successfully" << std::endl;
    }
    else
    {
        cout << "testEnergyConsumptionResource::testPut:: FAILED" << std::endl;
    }

    return result ;
}

bool testEnergyConsumptionResource::testSubscription(OC::Cm::Device *cmDevice)
{
    if (NULL == mEnergyConsumptionRes)
    {
        cout << "testEnergyConsumptionResource::testSubscription :: [Error] EnergyConsumption resource object is NULL\n";
        return false;
    }

    std::string deviceId = "0" ;
    int requestId ;
    mEnergyConsumptionRes->buildPath(deviceId);
    OC::Cm::CMError error =  mEnergyConsumptionRes->subscribe(requestId, *mSubsListener ,
                             "test subscription");
    if (OC::Cm::ErrorCode::CM_SUCCESS ==   error.getErrorCode() )
    {
        cout << "testEnergyConsumptionResource::testSubscription  is called successfully\n";
    }
    else
    {
        cout << "testEnergyConsumptionResource::testSubscription " << error.getErrorMessage() << "\n";
        return false;
    }
    return true ;
}

bool testEnergyConsumptionResource::testUnSubscribe()
{
    cout << "testEnergyConsumptionResource::testUnSubscribe not implemented\n" ;
    return true ;
}
