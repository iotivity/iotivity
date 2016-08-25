/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include "ICHelper.h"

using namespace std;
using namespace OC;

std::mutex ICHelper::s_mutex;
ICHelper* ICHelper::s_ICHelper = NULL;

ICHelper::ICHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[ICHelper] IN");
}

ICHelper* ICHelper::getInstance(void)
{
    if (s_ICHelper == NULL)
    {
        s_mutex.lock();
        s_ICHelper = new ICHelper();
        s_mutex.unlock();
    }

    return s_ICHelper;
}

ICHelper::~ICHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[ICHelper] OUT");
    if (s_ICHelper != NULL)
    {
        delete s_ICHelper;
        s_ICHelper = NULL;
    }
}
