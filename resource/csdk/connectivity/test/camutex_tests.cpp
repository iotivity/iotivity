//******************************************************************
//
// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
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
//
//
//*********************************************************************

#include "gtest/gtest.h"

#include <camutex.h>
#include <uthreadpool.h>

#include <time.h>
#include <sys/time.h>

static const uint64_t USECS_PER_SEC = 1000000;

uint64_t getAbsTime()
{
    uint64_t currentTime=0;
#if _POSIX_TIMERS > 0
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    currentTime = ts.tv_sec * USECS_PER_SEC + ts.tv_nsec / 1000;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    currentTime = tv.tv_sec * USECS_PER_SEC + tv.tv_usec;
#endif
    return currentTime;
}

TEST(MutexTests, TC_01_CREATE)
{
    ca_mutex mymutex = ca_mutex_new();

    EXPECT_TRUE(mymutex != NULL);

    ca_mutex_free(mymutex);
}

TEST(MutexTests, TC_02_TRY_LOCK)
{
    ca_mutex mymutex = ca_mutex_new();

    EXPECT_TRUE(mymutex != NULL);

    EXPECT_TRUE(ca_mutex_trylock(mymutex)); // acquire it

    ca_mutex_unlock(mymutex); // release it

    ca_mutex_lock(mymutex); // acquire it

    EXPECT_FALSE(ca_mutex_trylock(mymutex)); // he should be lock

    ca_mutex_free(mymutex);

    EXPECT_FALSE(ca_mutex_trylock(NULL));
}

typedef struct _tagFunc1
{
    ca_mutex mutex;
} _func1_struct;

void mutexFunc(void *context)
{
    _func1_struct* pData = (_func1_struct*) context;

    printf("Thread: trying to lock\n");
    ca_mutex_lock(pData->mutex);
    printf("Thread: got lock\n");
    sleep(5);
    printf("Thread:releasing\n");
    ca_mutex_unlock(pData->mutex);
}

TEST(MutexTests, DISABLED_TC_03_THREAD_LOCKING)
{
    u_thread_pool_t mythreadpool;

    EXPECT_EQ(CA_STATUS_OK, u_thread_pool_init(3, &mythreadpool));

    _func1_struct pData;

    pData.mutex = ca_mutex_new();

    EXPECT_TRUE(pData.mutex != NULL);

    printf("Holding mutex for 5 seconds\n");
    ca_mutex_lock(pData.mutex);

    printf("starting thread\n");
    EXPECT_EQ(CA_STATUS_OK, u_thread_pool_add_task(mythreadpool, mutexFunc, &pData)); //start thread

    printf("test: sleeping\n");

    sleep(5);

    printf("test: unlocking\n");

    ca_mutex_unlock(pData.mutex);

    printf("test: waiting for thread to release\n");
    sleep(1);

    ca_mutex_lock(pData.mutex);

    // Cleanup Everything

    ca_mutex_free(pData.mutex);

    u_thread_pool_free(mythreadpool);
}

TEST(ConditionTests, TC_01_CREATE)
{
    ca_cond mycond = ca_cond_new();

    EXPECT_TRUE(mycond != NULL);

    ca_cond_free(mycond);
}

typedef struct _tagFunc2
{
    int id;
    ca_mutex mutex;
    ca_cond condition;bool bFinished;
} _func2_struct;

void condFunc(void *context)
{
    _func2_struct* pData = (_func2_struct*) context;

    printf("Thread %d: waiting on condition\n", pData->id);

    ca_mutex_lock(pData->mutex);

    ca_cond_wait(pData->condition, pData->mutex);

    ca_mutex_unlock(pData->mutex);

    pData->bFinished = true;

    printf("Thread %d: got signaled\n", pData->id);
}

TEST(ConditionTests, DISABLED_TC_02_SIGNAL)
{
    u_thread_pool_t mythreadpool;

    EXPECT_EQ(CA_STATUS_OK, u_thread_pool_init(3, &mythreadpool));

    ca_mutex sharedMutex = ca_mutex_new();
    ca_cond sharedCond = ca_cond_new();

    _func2_struct pData1 =
    { 1, sharedMutex, sharedCond, false };
    _func2_struct pData2 =
    { 2, sharedMutex, sharedCond, false };

    EXPECT_TRUE(pData1.mutex != NULL);

    printf("starting thread\n");
    EXPECT_EQ(CA_STATUS_OK, u_thread_pool_add_task(mythreadpool, condFunc, &pData1)); //start thread
    EXPECT_EQ(CA_STATUS_OK, u_thread_pool_add_task(mythreadpool, condFunc, &pData2)); //start thread

    printf("test: sleeping\n");

    sleep(2);

    printf("test: signaling first thread\n");

    ca_mutex_lock(sharedMutex);
    ca_cond_signal(sharedCond);
    ca_mutex_unlock(sharedMutex);

    sleep(1);

    EXPECT_FALSE(pData1.bFinished && pData2.bFinished); // only one should be finished

    printf("test: signaling another thread\n");

    ca_mutex_lock(sharedMutex);
    ca_cond_signal(sharedCond);
    ca_mutex_unlock(sharedMutex);

    sleep(1);

    EXPECT_TRUE(pData2.bFinished && pData2.bFinished); // both should finally be finished

    // Cleanup Everything

    ca_mutex_free(pData1.mutex);

    ca_cond_free(pData1.condition);

    u_thread_pool_free(mythreadpool);
}

TEST(ConditionTests, DISABLE_TC_03_BROADCAST)
{
    u_thread_pool_t mythreadpool;

    EXPECT_EQ(CA_STATUS_OK, u_thread_pool_init(3, &mythreadpool));

    ca_mutex sharedMutex = ca_mutex_new();
    ca_cond sharedCond = ca_cond_new();

    _func2_struct pData1 =
    { 1, sharedMutex, sharedCond, false };
    _func2_struct pData2 =
    { 2, sharedMutex, sharedCond, false };

    EXPECT_TRUE(pData1.mutex != NULL);

    printf("starting thread\n");
    EXPECT_EQ(CA_STATUS_OK, u_thread_pool_add_task(mythreadpool, condFunc, &pData1)); //start thread
    EXPECT_EQ(CA_STATUS_OK, u_thread_pool_add_task(mythreadpool, condFunc, &pData2)); //start thread

    printf("test: sleeping\n");

    sleep(2);

    printf("test: signaling all threads\n");

    ca_mutex_lock(sharedMutex);
    ca_cond_broadcast(sharedCond);
    ca_mutex_unlock(sharedMutex);

    sleep(1);

    EXPECT_TRUE(pData1.bFinished && pData2.bFinished); // both should finally be finished

    // Cleanup Everything

    ca_mutex_free(sharedMutex);

    ca_cond_free(sharedCond);

    u_thread_pool_free(mythreadpool);
}

TEST(CondTests, TC_04_TIMECHECK)
{
    uint64_t begin = getAbsTime();

    usleep(1);

    uint64_t end = getAbsTime();

    EXPECT_LE(begin, end); // should never be the same value
}

void timedFunc(void *context)
{
    _func2_struct* pData = (_func2_struct*) context;

    printf("Thread %d: waiting for timeout \n", pData->id);

    ca_mutex_lock(pData->mutex);

    uint64_t abs = getAbsTime();
    abs += 2 * USECS_PER_SEC; // 2 seconds

    // test UTIMEDOUT
    CAWaitResult_t ret = ca_cond_wait_until(pData->condition, pData->mutex, abs);
    EXPECT_EQ(CA_WAIT_TIMEDOUT, ret);

    printf("Thread %d: waiting for signal \n", pData->id);

    abs = getAbsTime();
    abs += 5 * USECS_PER_SEC; // 5 seconds

    // test signal
    ret = ca_cond_wait_until(pData->condition, pData->mutex, abs);
    EXPECT_EQ(CA_WAIT_SUCCESS, ret);

    ca_mutex_unlock(pData->mutex);

    pData->bFinished = true;

    printf("Thread %d: stopping\n", pData->id);
}

TEST(ConditionTests, DISABLED_TC_05_WAIT)
{
    u_thread_pool_t mythreadpool;

    EXPECT_EQ(CA_STATUS_OK, u_thread_pool_init(3, &mythreadpool));

    ca_mutex sharedMutex = ca_mutex_new();
    ca_cond sharedCond = ca_cond_new();

    _func2_struct pData1 =
    { 1, sharedMutex, sharedCond, false };

    EXPECT_TRUE(sharedMutex != NULL);

    printf("starting thread\n");
    EXPECT_EQ(CA_STATUS_OK, u_thread_pool_add_task(mythreadpool, timedFunc, &pData1)); //start thread

    printf("test: sleeping for 3\n");

    sleep(4);

    printf("test: signaling first thread\n");

    ca_mutex_lock(sharedMutex);
    ca_cond_signal(sharedCond);
    ca_mutex_unlock(sharedMutex);

    sleep(1);

    EXPECT_TRUE(pData1.bFinished); // both should finally be finished

    // Cleanup Everything

    ca_mutex_free(sharedMutex);

    ca_cond_free(sharedCond);

    u_thread_pool_free(mythreadpool);
}

TEST(ConditionTests, TC_06_INVALIDWAIT)
{

    ca_mutex sharedMutex = ca_mutex_new();
    ca_cond sharedCond = ca_cond_new();

    ca_mutex_lock(sharedMutex);

    int ret = ca_cond_wait_until(NULL, sharedMutex, 5000);
    EXPECT_EQ(CA_WAIT_INVAL,ret);

    ret = ca_cond_wait_until(sharedCond, NULL, 5000);
    EXPECT_EQ(CA_WAIT_INVAL,ret);

    ret = ca_cond_wait_until(NULL, NULL, 5000);
    EXPECT_EQ(CA_WAIT_INVAL,ret);

    ca_mutex_unlock(sharedMutex);

    // Cleanup Everything

    ca_mutex_free(sharedMutex);

    ca_cond_free(sharedCond);
}

TEST(ConditionTests, TC_07_WAITDURATION)
{

    ca_mutex sharedMutex = ca_mutex_new();
    ca_cond sharedCond = ca_cond_new();

    ca_mutex_lock(sharedMutex);

    uint64_t abs = getAbsTime();
    uint64_t beg = abs;
    abs += 2 * USECS_PER_SEC;

    CAWaitResult_t ret = ca_cond_wait_until(sharedCond, sharedMutex, abs);
    EXPECT_EQ(CA_WAIT_TIMEDOUT,ret);

    uint64_t end = getAbsTime();

    double secondsDiff = (end - beg) / (double) USECS_PER_SEC;

    EXPECT_NEAR(2.0, secondsDiff, 0.05);

    ca_mutex_unlock(sharedMutex);

    // Cleanup Everything

    ca_mutex_free(sharedMutex);

    ca_cond_free(sharedCond);
}
