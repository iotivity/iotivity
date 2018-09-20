//******************************************************************
//
// Copyright 2018 Kistler Group All Rights Reserved.
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

#include "iotivity_config.h"

extern "C" {
    #include "oc_refcounter.h"
}

#include <gtest/gtest.h>

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <stdint.h>
using namespace std;

//-----------------------------------------------------------------------------
//  Tests
//-----------------------------------------------------------------------------

TEST(RefCounterTests, OCRefCounterCreateNormalNonDtor)
{
    oc_refcounter result = oc_refcounter_create(NULL, NULL);
    EXPECT_TRUE(result != NULL);

    result = oc_refcounter_dec(result);
    EXPECT_TRUE(result == NULL);
}

static void dtorVal(int* v)
{
    *v = 0;
}

TEST(RefCounterTests, OCRefCounterIncDecWithDtor)
{
    int val = 1;
    oc_refcounter result = oc_refcounter_create(&val, (oc_refcounter_dtor_data_func) dtorVal);
    EXPECT_TRUE(result != NULL);
    EXPECT_EQ(1, oc_refcounter_get_count(result));

    result = oc_refcounter_inc(result);
    EXPECT_TRUE(result != NULL);
    EXPECT_EQ(2, oc_refcounter_get_count(result));

    result = oc_refcounter_dec(result);
    EXPECT_TRUE(result != NULL);
    EXPECT_EQ(1, oc_refcounter_get_count(result));

    result = oc_refcounter_dec(result);
    EXPECT_TRUE(result == NULL);
    EXPECT_EQ(0, val);
}