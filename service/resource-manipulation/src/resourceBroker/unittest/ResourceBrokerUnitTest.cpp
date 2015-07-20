/*
 * 01.[P]HostResource_ReturnNormalValueIfNormalParams
 * 02.[N]HostResource_NormalErrorHandlingIfResourceNull
 * 03.[N]HostResource_NormalErrorHandlingIfCbFuncNull
 * 04.[P]CancelHostResource_NoThrowIfNormalParams
 * 05.[N]CancelHostResource_NormalErrorHandlingIfAbNormalId
 * 06.[N]CancelHostResource_NormalErrorHandlingIfAbNormalIdOutOfRangeValue
 * 07.[P]getResourceState_ReturnNormalValueIfNormalId
 * 08.[P]getResourceState_ReturnNormalValueIfNormalResource
 * 09.[N]getResourceState_NormalErrorHandlingIfResourceNull
 * 10.[N]getResourceState_NormalErrorHandlingIfAbnormalResource
 * 11.[N]getResourceState_NormalErrorHandlingIfAbnormalId
 */

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
    void SetUp() override {
        brokerInstance = ResourceBroker::getInstance();
        pResource = PrimitiveResource::Ptr(mocks.Mock< PrimitiveResource >(), [](PrimitiveResource*){});
        cb = ([](BROKER_STATE)->OCStackResult{return OC_STACK_OK;});
        id = 0;
    }

    void TearDown() override
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
/*
 * [P]HostResource_ReturnNormalValueIfNormalParams
 */
 
TEST_F(ResourceBrokerTest,HostResource_ReturnNormalValueIfNormalParams)
{
    MockingFunc();

    BrokerID ret;
    ASSERT_NE(ret = brokerInstance->hostResource(pResource,cb),0);

    brokerInstance->cancelHostResource(ret);
}
/*
 * [N]HostResource_NormalErrorHandlingIfResourceNull
 */
TEST_F(ResourceBrokerTest, HostResource_NormalErrorHandlingIfResourceNull)
{
    ASSERT_THROW(brokerInstance->hostResource(nullptr, cb),ResourceBroker::InvalidParameterException);
}
/*
 * [N]HostResource_NormalErrorHandlingIfCbFuncNull
 */
TEST_F(ResourceBrokerTest, HostResource_NormalErrorHandlingIfCbFuncNull)
{
    ASSERT_THROW(brokerInstance->hostResource(pResource,nullptr),ResourceBroker::InvalidParameterException);
}
/*
 * [P]CancelHostResource_NoThrowIfNormalParams
 */
TEST_F(ResourceBrokerTest,CancelHostResource_NoThrowIfNormalParams)
{
    MockingFunc();

    BrokerID ret;
    ret = brokerInstance->hostResource(pResource,cb);

    ASSERT_NO_THROW(brokerInstance->cancelHostResource(ret));

}
/*
 * [N]CancelHostResource_NormalErrorHandlingIfAbNormalIdZero
 */
TEST_F(ResourceBrokerTest,CancelHostResource_NormalErrorHandlingIfAbNormalIdZero)
{
    id = 0;
    ASSERT_THROW(brokerInstance->cancelHostResource(id),ResourceBroker::InvalidParameterException);
}
/*
 * [N]CancelHostResource_NormalErrorHandlingIfAbNormalIdOutOfRangeValue
 */
TEST_F(ResourceBrokerTest,CancelHostResource_NormalErrorHandlingIfAbNormalIdOutOfRangeValue)
{
    id = -1;
    ASSERT_THROW(brokerInstance->cancelHostResource(id),ResourceBroker::InvalidParameterException);
}
/*
 * [P]getResourceState_ReturnNormalValueIfNormalId
 */
TEST_F(ResourceBrokerTest,getResourceState_ReturnNormalValueIfNormalId)
{
    MockingFunc();

    BrokerID ret;
    ret = brokerInstance->hostResource(pResource,cb);

    ASSERT_NE(brokerInstance->getResourceState(ret),BROKER_STATE::NONE);

    brokerInstance->cancelHostResource(ret);

}

TEST_F(ResourceBrokerTest,getResourceState_NormalErrorHandlingIfIdZero)
{
    id = 0;
    ASSERT_THROW(brokerInstance->getResourceState(id),ResourceBroker::InvalidParameterException);
}

/*
 * [P]getResourceState_ReturnNormalValueIfNormalResource
 */
TEST_F(ResourceBrokerTest,getResourceState_ReturnNormalValueIfNormalResource)
{
    MockingFunc();

    BrokerID ret;
    ret = brokerInstance->hostResource(pResource,cb);

    ASSERT_NE(brokerInstance->getResourceState(pResource),BROKER_STATE::NONE);

    brokerInstance->cancelHostResource(ret);
}
/*
 * [N]getResourceState_NormalErrorHandlingIfResourceNull
 */
TEST_F(ResourceBrokerTest,getResourceState_NormalErrorHandlingIfResourceNull)
{
    ASSERT_THROW(brokerInstance->getResourceState((PrimitiveResource::Ptr)nullptr),ResourceBroker::InvalidParameterException);
}
/*
 * [N]getResourceState_NormalErrorHandlingIfAbnormalResource
 */
TEST_F(ResourceBrokerTest,getResourceState_NormalErrorHandlingIfAbnormalResource)
{

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
}
/*
 * [N]getResourceState_NormalErrorHandlingIfAbnormalId
 */
TEST_F(ResourceBrokerTest,getResourceState_NormalErrorHandlingIfAbnormalId)
{
    id = -1;
    ASSERT_THROW(brokerInstance->getResourceState(id),ResourceBroker::InvalidParameterException);
}

