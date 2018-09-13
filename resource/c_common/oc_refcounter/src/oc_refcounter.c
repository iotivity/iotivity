/******************************************************************
 *
 * Copyright 2018 Kistler Group All Rights Reserved.
 *
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
#include "oc_refcounter.h"

#include "oic_malloc.h"
#include "ocatomic.h"
#include "iotivity_debug.h"

// Enable extra debug logging for malloc.  Comment out to disable
#ifdef ENABLE_REFCOUNTER_DEBUG
#include "experimental/logger.h"
#define TAG "OC_REFCOUNT"
#endif

typedef struct oc_refcounter_t
{
    /* count of the references*/
    int32_t                      count;
    /* data owned by refcount */
    void*                        data;
    /* data destructor*/
    oc_refcounter_dtor_data_func dtor;
} oc_refcounter_t;

oc_refcounter oc_refcounter_create(void* data, oc_refcounter_dtor_data_func dtor)
{
    oc_refcounter ref = (oc_refcounter)OICMalloc(sizeof(*ref));
    if (ref == NULL)
    {
        return ref;
    }
    ref->count = 1;
#ifdef ENABLE_REFCOUNTER_DEBUG
    OIC_LOG_V(INFO, TAG, "oc_refcounter_create: ref=%p count=%d", ref, 1);
#endif
    ref->data = data;
    ref->dtor = dtor;

    return ref;
}

oc_refcounter oc_refcounter_inc(oc_refcounter ref)
{
    if (ref == NULL)
    {
        return NULL;
    }
    OC_VERIFY(ref->count > 0);
    int32_t count = oc_atomic_increment(&ref->count);
#ifdef ENABLE_REFCOUNTER_DEBUG
    OIC_LOG_V(INFO, TAG, "oc_refcounter_inc: ref=%p count=%d", ref, count);
#else
    (void) count;
#endif
    return ref;
}

oc_refcounter oc_refcounter_dec(oc_refcounter ref)
{
    if (ref == NULL)
    {
        return NULL;
    }
    OC_VERIFY(ref->count > 0);
    int32_t count = oc_atomic_decrement(&ref->count);
#ifdef ENABLE_REFCOUNTER_DEBUG
    OIC_LOG_V(INFO, TAG, "oc_refcounter_dec: ref=%p count=%d", ref, count);
#endif
    if (count == 0)
    {
        if (ref->dtor)
        {
            ref->dtor(ref->data);
        }
        OICFree(ref);
        return NULL;
    }
    return ref;
}

int32_t oc_refcounter_get_count(oc_refcounter ref)
{
    if (ref == NULL)
    {
        return -1;
    }
    OC_VERIFY(ref->count > 0);
    return ref->count;
}

void* oc_refcounter_get_data(oc_refcounter ref)
{
    if (ref == NULL)
    {
        return NULL;
    }
    OC_VERIFY(ref->count > 0);
    return ref->data;
}