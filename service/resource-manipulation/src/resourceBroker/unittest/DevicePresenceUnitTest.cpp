
#include "gtest/gtest.h"
#include "HippoMocks/hippomocks.h"

#include "PrimitiveResource.h"
#include "ResponseStatement.h"
#include "OCPlatform.h"
#include "DevicePresence.h"
#include "ResourcePresence.h"

using namespace testing;
using namespace OIC::Service;
using namespace OC;

typedef OCStackResult (*subscribePresenceSig1)(OC::OCPlatform::OCPresenceHandle&,
        const std::string&, OCConnectivityType, SubscribeCallback);

class DevicePresenceTest : public Test
{
public:

    MockRepository mocks;
    DevicePresence * instance;
    PrimitiveResource::Ptr pResource;
    BrokerCB cb;
    BrokerID id;

protected:

    void SetUp()
    {
        instance = (DevicePresence*)new DevicePresence();
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

    virtual ~DevicePresenceTest() noexcept(true)
    {
    }

};

TEST_F(DevicePresenceTest,initializeDevicePresence_NormalHandlingIfNormalResource)
{
    SetUp();
    MockingFunc();

    ASSERT_NO_THROW(instance->initializeDevicePresence(pResource));
    TearDown();
}

TEST_F(DevicePresenceTest,initializeDevicePresence_ErrorHandlingIfAbnormalResource)
{
    SetUp();
    MockingFunc();
    mocks.OnCallFuncOverload(static_cast< subscribePresenceSig1 >(OC::OCPlatform::subscribePresence)).Return(OC_STACK_ERROR);

    ASSERT_THROW(instance->initializeDevicePresence(pResource),PlatformException);
    TearDown();
}

TEST_F(DevicePresenceTest,addPresenceResource_NormalHandlingIfNormalResource)
{
    SetUp();
    ResourcePresence * resource = (ResourcePresence *)new ResourcePresence();
    instance->addPresenceResource(resource);

    ASSERT_EQ(false,instance->isEmptyResourcePresence());
    TearDown();
}

TEST_F(DevicePresenceTest,isEmptyResourcePresence_NormalHandling)
{
    SetUp();
    MockingFunc();

    ASSERT_TRUE(instance->isEmptyResourcePresence());
    TearDown();
}

TEST_F(DevicePresenceTest,getAddress_NormalHandling)
{
    SetUp();
    MockingFunc();

    instance->initializeDevicePresence(pResource);
    instance->getAddress();
    TearDown();
}

TEST_F(DevicePresenceTest,NormalHandlingWhenReceivedCallbackMessage)
{
    SetUp();
    MockingFunc();
    TearDown();
}
