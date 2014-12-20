/*
 * testDoors.cpp
 *
 *  Created on: Oct 14, 2014
 *      Author: manjunath.d
 */

#include "testDoors.h"
#include "Client/IDoorsResourceResponseListener.h"
#include "xsd/Door.h"
#include "xsd/Doors.h"

using namespace std;

class DoorsResourceResponseListener : public IDoorsResourceResponseListener
{

    private:

        testDoors *mRes;
        OC::Cm::Device *mDevice ;

    public:

        DoorsResourceResponseListener(testDoors *tempRes , OC::Cm::Device *device)
        {
            mRes = tempRes ;
            mDevice = device ;
        }

        bool ongetDoors( int &requestId , int status, ::Doors *respData)
        {
            if (status == 204 || status == 200)
            {
                cout << "ongetDoors : Success. Status:" << status;
                mRes->testSubscription(mDevice);
                return true;
            }
            else
            {
                cout << "ongetDoors : failed. Status" << status;
                return false;
            }
        }
        bool onputDoors( int &requestId, int status )
        {
            if (status == 204 || status == 200)
            {
                cout << "onputDoors : Success. Status:" << status;
                mRes->testSubscription(mDevice);
                return true;
            }
            else
            {
                cout << "onputDoors : failed. Status" << status;
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


testDoors::testDoors(OC::Cm::Device *cmDevice)
{
    // TODO Auto-generated constructor stub

    mpDoorsRes = (DoorsResource *)cmDevice->getResource("Doors");

    mResListener  = new DoorsResourceResponseListener(this, cmDevice);
    mSubsListener = new SubscriptionResponseListener(this);

    mpDoorsRes->addResponseListener(*mResListener);
    mpDoorsRes->buildPath("0");

}

testDoors::~testDoors()
{
    delete mSubsListener ;
    delete mResListener ;
    delete mpDoorsRes ;
}

bool
testDoors::testGet()
{
    int requestId  = 3454;
    bool result = mpDoorsRes->getDoors(requestId);
    if (true == result)
    {
        cout << "testDoors::testGet::executed successfully" << std::endl;
    }
    else
    {
        cout << "testDoors::testGet:: FAILED" << std::endl;
    }

    return result ;
}

bool testDoors::testPut()
{

    int requestId  = 34556;
    Doors *respData = new Doors();
    Door *pDoor1 = new Door();
    pDoor1->mpId = new String50(); pDoor1->mpId->value = "1";
    pDoor1->mpLock = new LockType(); pDoor1->mpLock->value = "Lock";

    Door *pDoor2 = new Door();
    pDoor2->mpId = new String50(); pDoor2->mpId->value = "2";
    pDoor2->mpLock = new LockType(); pDoor2->mpLock->value = "Lock";

    respData->mpDoor.push_back(pDoor1);
    respData->mpDoor.push_back(pDoor2);

    return mpDoorsRes->putDoors(requestId, *respData );

}
bool testDoors::testSubscription(OC::Cm::Device *cmDevice)
{

    std::string deviceId = "0" ;
    int requestId ;
    mpDoorsRes->buildPath(deviceId);
    OC::Cm::CMError error =  mpDoorsRes->subscribe(requestId, *mSubsListener , "test subscription");
    if (OC::Cm::ErrorCode::CM_SUCCESS ==   error.getErrorCode() )
    {
        cout << "testDoors::testSubscription  is called successfully\n";
    }
    else
    {
        cout << "testDoors::testSubscription " << error.getErrorMessage() << "\n";
        return false;
    }
    return true ;
}

bool testDoors::testUnSubscribe()
{
    cout << "testDoors::testUnSubscribe not implemented\n" ;
    return true;
}

