#include <iostream>

#include "gtest/gtest.h"
#include "HippoMocks/hippomocks.h"

#include "OCPlatform.h"

#include "DevicePresence.h"
#include "DeviceAssociation.h"
#include "ResourcePresence.h"
#include "PrimitiveResource.h"
#include "ResponseStatement.h"

using namespace testing;
using namespace OIC::Service;
using namespace OC;

#define STRING_VALUE "10.242.34.235"

typedef OCStackResult (*subscribePresenceSig1)(OC::OCPlatform::OCPresenceHandle&,
        const std::string&, OCConnectivityType, SubscribeCallback);

class DeviceAssociationTest : public Test
{
public:

    MockRepository mocks;
    DeviceAssociation * instance;
    DevicePresencePtr device;
    PrimitiveResource::Ptr pResource;
protected:

    void setMockingFunc()
    {
        mocks.OnCall(pResource.get(), PrimitiveResource::requestGet);
        mocks.OnCall(pResource.get(), PrimitiveResource::getHost).Return(STRING_VALUE);
        mocks.OnCallFuncOverload(static_cast< subscribePresenceSig1 >(OC::OCPlatform::subscribePresence)).Return(OC_STACK_OK);
    }

    void SetAssociationDevice()
    {
        setMockingFunc();
        device->initializeDevicePresence(pResource);
        instance->addDevice(device);
    }

    void SetUp()
    {
        instance = DeviceAssociation::getInstance();
        device = (DevicePresencePtr)new DevicePresence();
        pResource = PrimitiveResource::Ptr(mocks.Mock< PrimitiveResource >(), [](PrimitiveResource*){});
    }

    void TearDown()
    {
        device.reset();
        pResource.reset();

    }
    virtual ~DeviceAssociationTest() noexcept(true)
    {
    }


};

TEST_F(DeviceAssociationTest,findDevice_ReturnNormalValueIfNormalParam)
{
    SetUp();
    SetAssociationDevice();
    ASSERT_NE(nullptr,instance->findDevice(pResource->getHost()));
    TearDown();

}

TEST_F(DeviceAssociationTest,addDevice_NormalHandlingIfNormalParam)
{
    SetUp();
    SetAssociationDevice();
    ASSERT_FALSE(instance->isEmptyDeviceList());
    TearDown();

}

TEST_F(DeviceAssociationTest,removeDevice_NormalHandlingIfNormalParam)
{
    SetUp();
    SetAssociationDevice();
    instance->removeDevice(device);
    ASSERT_TRUE(instance->isEmptyDeviceList());
    TearDown();
}

