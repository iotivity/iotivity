//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "UnitTestHelper.h"

#include "ResourceEncapsulationTestSimulator.h"
#include "RequestObject.h"

using namespace testing;
using namespace OIC::Service;

namespace
{
    bool isStarted = false;
    bool isFinished = false;

    ResourceEncapsulationTestSimulator testObject;
    RCSRemoteResourceObject::Ptr remoteObject;

    void setRequestCB(const RCSResourceAttributes &, int, const RCSRequest &, RequestObject::Ptr)
    {
    }

    void setup()
    {
        if(!isStarted)
        {
            testObject.defaultRunSimulator();
            remoteObject = testObject.getRemoteResource();

            isStarted = true;
        }
    }

    void tearDown()
    {
        if(isFinished)
        {
            testObject.destroy();
            isStarted = false;
        }
    }
}

class RequestObjectTest : public TestWithMock
{
public:
    std::mutex mutexForCondition;
    std::condition_variable responseCon;

protected:

    void SetUp()
    {
        TestWithMock::SetUp();
        setup();
    }

    void TearDown()
    {
        TestWithMock::TearDown();
        tearDown();
    }

public:
    void waitForCondition(int waitingTime = 1000)
    {
        std::unique_lock< std::mutex > lock{ mutexForCondition };
        responseCon.wait_for(lock, std::chrono::milliseconds{ waitingTime });
    }

    void notifyCondition()
    {
        responseCon.notify_all();
    }
};

TEST_F(RequestObjectTest, invokeRequestExpectCallwithSetter)
{
   bool isCalled = false;

   mocks.ExpectCallFunc(setRequestCB).Do(
           [this, &isCalled](const RCSResourceAttributes &, int,
                   const RCSRequest &, RequestObject::Ptr)
           {
               isCalled = true;
               notifyCondition();
           });

   RCSResourceAttributes att;
   std::shared_ptr<OC::OCResourceRequest> request;
   RequestObject::invokeRequest(remoteObject, RCSRequest(testObject.getResourceServer(), request),
           RequestObject::RequestMethod::Set, att, setRequestCB);

   waitForCondition();

   ASSERT_TRUE(isCalled);
}
