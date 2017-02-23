/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include "CommonTestUtil.h"

using namespace boost::assign;


long CommonTestUtil::s_setUpDynamicMemoryUsage = 0;
long CommonTestUtil::s_tearDownDynamicMemoryUsage = 0;
long CommonTestUtil::s_memoryDiffSum = 0;

long CommonTestUtil::getCurrentAllocatedMemory()
{
#ifndef __WINDOWS__
    return mallinfo().uordblks;
#else
    return 0;
#endif
}

void CommonTestUtil::runCommonTCSetUpPart()
{
    s_setUpDynamicMemoryUsage = getCurrentAllocatedMemory();
}

void CommonTestUtil::runCommonTCTearDownPart()
{
    s_tearDownDynamicMemoryUsage = getCurrentAllocatedMemory();
    s_memoryDiffSum += s_tearDownDynamicMemoryUsage - s_setUpDynamicMemoryUsage;

    //TODO: It will be printed using new logger
    //printf("[Memory] Dynamic Memory Diff Sum : %ld bytes\n", s_memoryDiffSum);

    ::testing::Test::RecordProperty("MemoryUsage", s_memoryDiffSum);
    s_tearDownDynamicMemoryUsage = s_setUpDynamicMemoryUsage = 0;
}
