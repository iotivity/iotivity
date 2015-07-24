
#include <iostream>
#include <vector>
#include <unistd.h>

#include "gtest/gtest.h"
#include "HippoMocks/hippomocks.h"

#include "OCResource.h"
#include "OCPlatform.h"

#include "PrimitiveResource.h"
#include "ResponseStatement.h"
#include "RCSResourceAttributes.h"
#include "ResourcePresence.h"
#include "UnitTestHelper.h"

using namespace testing;
using namespace OIC::Service;
using namespace OC;

typedef OCStackResult (*subscribePresenceSig1)(OC::OCPlatform::OCPresenceHandle&,
        const std::string&, OCConnectivityType, SubscribeCallback);

class ResourcePresenceTest : public TestWithMock
{
public:

    typedef std::function<void(const HeaderOptions&, const ResponseStatement&, int)> GetCallback;

    ResourcePresence * instance;
    PrimitiveResource::Ptr pResource;
    BrokerCB cb;
    BrokerID id;

protected:

    void SetUp() 
    {
        TestWithMock::SetUp();

        instance = (ResourcePresence*)new ResourcePresence();
        pResource = PrimitiveResource::Ptr(mocks.Mock< PrimitiveResource >(), [](PrimitiveResource*){});
        cb = ([](BROKER_STATE)->OCStackResult{return OC_STACK_OK;});
        id = 0;
    }

    void TearDown() 
    {
        TestWithMock::TearDown();
        pResource.reset();
        id = 0;
        cb = nullptr;
    }

    void MockingFunc()
    {
        mocks.OnCall(pResource.get(), PrimitiveResource::requestGet).Do([](GetCallback cb){});
        mocks.OnCall(pResource.get(), PrimitiveResource::getHost).Return(std::string());
        mocks.OnCallFuncOverload(static_cast< subscribePresenceSig1 >(OC::OCPlatform::subscribePresence)).Return(OC_STACK_OK);
    }

};

TEST_F(ResourcePresenceTest,initializeResourcePresence_NormalhandlingIfNormalResource)
{

    MockingFunc();

    instance->initializeResourcePresence(pResource);
    ASSERT_NE(nullptr,instance->getPrimitiveResource());

}

TEST_F(ResourcePresenceTest,addBrokerRequester_ReturnNormalValueIfNormalParams)
{

    MockingFunc();

    instance->initializeResourcePresence(pResource);
    id = 1;
    instance->addBrokerRequester(id,cb);
    EXPECT_FALSE(instance->isEmptyRequester());

}

TEST_F(ResourcePresenceTest,removeBrokerRequester_NormalHandlingIfNormalId)
{

    MockingFunc();

    instance->initializeResourcePresence(pResource);
    id = 1;
    instance->addBrokerRequester(id,cb);
    id = 2;
    instance->addBrokerRequester(id,cb);

    instance->removeBrokerRequester(id);
    ASSERT_EQ(1,instance->requesterListSize());

}

TEST_F(ResourcePresenceTest,removeAllBrokerRequester_NormalHandling)
{

    MockingFunc();

    instance->initializeResourcePresence(pResource);
    id = 1;
    instance->addBrokerRequester(id,cb);
    id = 2;
    instance->addBrokerRequester(id,cb);

    instance->removeAllBrokerRequester();
    ASSERT_TRUE(instance->isEmptyRequester());

}

TEST_F(ResourcePresenceTest,removeAllBrokerRequester_ErrorHandlingIfListNull)
{

    MockingFunc();

    instance->initializeResourcePresence(pResource);
    instance->removeAllBrokerRequester();

}

TEST_F(ResourcePresenceTest,requestResourceState_NormalHandling)
{

    MockingFunc();

    instance->initializeResourcePresence(pResource);

    ASSERT_NO_THROW(instance->requestResourceState());

}

TEST_F(ResourcePresenceTest,changePresenceMode_NormalHandlingIfNewModeDifferent)
{

    MockingFunc();

    instance->initializeResourcePresence(pResource);

    instance->changePresenceMode(BROKER_MODE::DEVICE_PRESENCE_MODE);

}

TEST_F(ResourcePresenceTest,getResourceState_NormalHandling)
{

    ASSERT_EQ(BROKER_STATE::REQUESTED,instance->getResourceState());

}

TEST_F(ResourcePresenceTest,changePresenceMode_NormalHandlingIfNewModeSame)
{

    MockingFunc();

    instance->initializeResourcePresence(pResource);

    instance->changePresenceMode(BROKER_MODE::NON_PRESENCE_MODE);

}

TEST_F(ResourcePresenceTest,getPrimitiveResource_NormalHandling)
{

    MockingFunc();

    instance->initializeResourcePresence(pResource);
    ASSERT_NE(nullptr,instance->getPrimitiveResource());

}

TEST_F(ResourcePresenceTest,getCB_NormalHandlingIfMessageOC_STACK_OK)
{

    mocks.ExpectCall(pResource.get(), PrimitiveResource::requestGet).Do(
                [](GetCallback callback){

        OIC::Service::HeaderOptions op;
        RCSResourceAttributes attr;
        OIC::Service::ResponseStatement res(attr);
        callback(op,res,OC_STACK_OK);

    });
    mocks.OnCall(pResource.get(), PrimitiveResource::requestGet).Do(
                                [](GetCallback callback){
        std::cout <<"End call requestGetFunc()\n";
                    });
    mocks.ExpectCall(pResource.get(), PrimitiveResource::getHost).Return("address1");
    mocks.ExpectCall(pResource.get(), PrimitiveResource::getHost).Return("address2");
    mocks.OnCallFuncOverload(static_cast< subscribePresenceSig1 >(OC::OCPlatform::subscribePresence)).Do(
            [](OC::OCPlatform::OCPresenceHandle&,
                    const std::string&, OCConnectivityType, SubscribeCallback callback)->OCStackResult{

        callback(OC_STACK_OK,0,std::string());
        return OC_STACK_OK;

    }).Return(OC_STACK_OK);

    instance->initializeResourcePresence(pResource);
    sleep(3);

}

TEST_F(ResourcePresenceTest,isEmptyRequester_NormalHandling)
{

    MockingFunc();

    instance->initializeResourcePresence(pResource);
    id = 1;
    instance->addBrokerRequester(id,cb);
    instance->removeAllBrokerRequester();
    ASSERT_TRUE(instance->isEmptyRequester());

}
