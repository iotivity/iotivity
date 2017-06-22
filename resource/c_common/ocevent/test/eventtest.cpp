/* *****************************************************************
 *
 * Copyright 2017 Microsoft
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

/**
 * @file
 *
 * This file implement tests for the Event object.
 */

#include "ocevent.h"
#include "gtest/gtest.h"
#include <limits.h>
#include <thread>

class EventTester : public testing::Test
{
  protected:
    virtual void SetUp()
    {
        m_event = oc_event_new();
        ASSERT_TRUE(nullptr != m_event);
    }
    virtual void TearDown()
    {
        oc_event_free(m_event);
    }
    void TestSignalBeforeWait(uint32_t timeout);
    oc_event m_event;
};

void EventTester::TestSignalBeforeWait(uint32_t timeout)
{
    std::thread thread([this]()
    {
        oc_event_signal(m_event);
    });

    // Make sure the signal occurs before the wait.
    thread.join();

    if (UINT_MAX == timeout)
    {
        oc_event_wait(m_event);
    }
    else
    {
        EXPECT_EQ(OC_WAIT_SUCCESS, oc_event_wait_for(m_event, timeout));
    }
}

TEST_F(EventTester, InfiniteTimeout_SignaledAfterWait)
{
    std::thread thread([this]()
    {
        // This sleep allows the main thread to enter the wait state before the signal.
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        oc_event_signal(m_event);
    });
    oc_event_wait(m_event);
    thread.join();
}

TEST_F(EventTester, InfiniteTimeout_SignaledBeforeWait)
{
    TestSignalBeforeWait(UINT_MAX);
}

TEST_F(EventTester, ZeroTimeout_NotSignaled)
{
    EXPECT_EQ(OC_WAIT_TIMEDOUT, oc_event_wait_for(m_event, 0));
}

TEST_F(EventTester, ZeroTimeout_Signaled)
{
    TestSignalBeforeWait(0);
}

TEST_F(EventTester, TimedOut)
{
    EXPECT_EQ(OC_WAIT_TIMEDOUT, oc_event_wait_for(m_event, 10));
}
