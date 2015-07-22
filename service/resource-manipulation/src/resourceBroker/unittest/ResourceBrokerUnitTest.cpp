#include "gtest/gtest.h"
#include "HippoMocks/hippomocks.h"

#include "OCPlatform.h"
#include "PrimitiveResource.h"
#include "ResponseStatement.h"
#include "ResourceBroker.h"

using namespace testing;
using namespace OIC::Service;
using namespace OC;

typedef OCStackResult (*subscribePresenceSig1)(OC::OCPlatform::OCPresenceHandle&,
        const std::string&, OCConnectivityType, SubscribeCallback);

class ResourceBrokerTest : public Test
{
public:

    MockRepository mocks;
    ResourceBroker * brokerInstance;
    PrimitiveResource::Ptr pResource;
    BrokerCB cb;
    BrokerID id;

protected:

    void SetUp()
    {
        brokerInstance = ResourceBroker::getInstance();
        pResource = PrimitiveResource::Ptr(mocks.Mock< PrimitiveResource >(), [](PrimitiveResource*){});
        cb = ([](BROKER_STATE)->OCStackResult{return OC_STACK_OK;});
        id = 0;
    }

    void TearDown()
    {
        pResource.reset();
        id = 0;
        cb = nullptr;
    }

    void MockingFunc()
    {
        mocks.OnCall(pResource.get(), PrimitiveResource::requestGet);
        mocks.OnCall(pResource.get(), PrimitiveResource::getHost).Return(std::string());
        mocks.OnCallFuncOverload(static_cast< subscribePresenceSig1 >(OC::OCPlatform::subscribePresence)).Return(OC_STACK_OK);
    }

    virtual ~ResourceBrokerTest() noexcept(true)
    {
    }

};
 
TEST_F(ResourceBrokerTest,HostResource_ReturnNormalValueIfNormalParams)
{
    SetUp();
    MockingFunc();

    BrokerID ret;
    ASSERT_NE(ret = brokerInstance->hostResource(pResource,cb),0);

    brokerInstance->cancelHostResource(ret);
    TearDown();
}

TEST_F(ResourceBrokerTest, HostResource_NormalErrorHandlingIfResourceNull)
{
    SetUp();
    ASSERT_THROW(brokerInstance->hostResource(nullptr, cb),ResourceBroker::InvalidParameterException);
    TearDown();
}

TEST_F(ResourceBrokerTest, HostResource_NormalErrorHandlingIfCbFuncNull)
{
    SetUp();
    ASSERT_THROW(brokerInstance->hostResource(pResource,nullptr),ResourceBroker::InvalidParameterException);
    TearDown();
}

TEST_F(ResourceBrokerTest,CancelHostResource_NoThrowIfNormalParams)
{
    SetUp();
    MockingFunc();

    BrokerID ret;
    ret = brokerInstance->hostResource(pResource,cb);

    ASSERT_NO_THROW(brokerInstance->cancelHostResource(ret));
    TearDown();

}

TEST_F(ResourceBrokerTest,CancelHostResource_NormalErrorHandlingIfAbNormalIdZero)
{
    SetUp();
    id = 0;
    ASSERT_THROW(brokerInstance->cancelHostResource(id),ResourceBroker::InvalidParameterException);
    TearDown();
}

TEST_F(ResourceBrokerTest,CancelHostResource_NormalErrorHandlingIfAbNormalIdOutOfRangeValue)
{
    SetUp();
    id = -1;
    ASSERT_THROW(brokerInstance->cancelHostResource(id),ResourceBroker::InvalidParameterException);
    TearDown();
}

TEST_F(ResourceBrokerTest,getResourceState_ReturnNormalValueIfNormalId)
{
    SetUp();
    MockingFunc();

    BrokerID ret;
    ret = brokerInstance->hostResource(pResource,cb);

    ASSERT_NE(brokerInstance->getResourceState(ret),BROKER_STATE::NONE);

    brokerInstance->cancelHostResource(ret);
    TearDown();

}

TEST_F(ResourceBrokerTest,getResourceState_NormalErrorHandlingIfIdZero)
{
    SetUp();
    id = 0;
    ASSERT_THROW(brokerInstance->getResourceState(id),ResourceBroker::InvalidParameterException);
    TearDown();
}

TEST_F(ResourceBrokerTest,getResourceState_ReturnNormalValueIfNormalResource)
{
    SetUp();
    MockingFunc();

    BrokerID ret;
    ret = brokerInstance->hostResource(pResource,cb);

    ASSERT_NE(brokerInstance->getResourceState(pResource),BROKER_STATE::NONE);

    brokerInstance->cancelHostResource(ret);
    TearDown();
}

TEST_F(ResourceBrokerTest,getResourceState_NormalErrorHandlingIfResourceNull)
{
    SetUp();
    ASSERT_THROW(brokerInstance->getResourceState((PrimitiveResource::Ptr)nullptr),ResourceBroker::InvalidParameterException);
    TearDown();
}

TEST_F(ResourceBrokerTest,getResourceState_NormalErrorHandlingIfAbnormalResource)
{
    SetUp();
    MockingFunc();

    PrimitiveResource::Ptr resource[3];
    BrokerID id[3];

    for(int i=0;i!=3;i++)
    {
        resource[i] = PrimitiveResource::Ptr(mocks.Mock< PrimitiveResource >(), [](PrimitiveResource*){});
        mocks.OnCall(resource[i].get(), PrimitiveResource::requestGet);
        mocks.OnCall(resource[i].get(), PrimitiveResource::getHost).Return(std::string());
        mocks.OnCallFuncOverload(static_cast< subscribePresenceSig1 >(OC::OCPlatform::subscribePresence)).Return(OC_STACK_OK);
        id[i] = brokerInstance->hostResource(resource[i],cb);
    }


    EXPECT_EQ(brokerInstance->getResourceState(pResource),BROKER_STATE::NONE);

    for(int i=0;i!=3;i++)
    {
        brokerInstance->cancelHostResource(id[i]);
    }
    TearDown();
}

TEST_F(ResourceBrokerTest,getResourceState_NormalErrorHandlingIfAbnormalId)
{
    SetUp();
    id = -1;
    ASSERT_THROW(brokerInstance->getResourceState(id),ResourceBroker::InvalidParameterException);
    TearDown();
}

