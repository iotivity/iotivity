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
 * This file implements stubs for atomic functions. These stubs are not designed
 * to be used by multi-threaded OS.
 */

#include "ocatomic.h"

int32_t oc_atomic_increment(volatile int32_t *addend)
{
    (*addend)++;
    return *addend;
}

int32_t oc_atomic_decrement(volatile int32_t *addend)
{
    (*addend)--;
    return *addend;
}

int32_t oc_atomic_add(volatile int32_t *addend, int32_t value)
{
    (*addend) += value;
    return *addend;
}

bool oc_atomic_cmpxchg(volatile int32_t *destination, int32_t oldValue, int32_t newValue)
{
    if ((*destination) == oldValue)
    {
        *destination = newValue;
        return true;
    }
    return false;
}

int32_t oc_atomic_or(volatile int32_t *destination, int32_t value)
{
    (*destination) |= value;
    return *destination;
}