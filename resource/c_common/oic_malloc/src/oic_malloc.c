//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include "oic_malloc.h"

// Enable extra debug logging for malloc.  Comment out to disable
#ifdef ENABLE_MALLOC_DEBUG
#include "logger.h"
#define TAG "OICMalloc"
#endif

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Internal API function
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private internal function prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public APIs
//-----------------------------------------------------------------------------
#ifdef ENABLE_MALLOC_DEBUG
static uint32_t count;
#endif

void *OICMalloc(size_t size)
{
#ifdef ENABLE_MALLOC_DEBUG
    void *ptr = 0;

    if (0 == size)
    {
        return NULL;
    }

    ptr = malloc(size);
    count++;
    OIC_LOG_V(INFO, TAG, "malloc: ptr=%p, size=%u, count=%u", ptr, size, count);
    return ptr;
#else
    if (0 == size)
    {
        return NULL;
    }
    return malloc(size);
#endif
}

void *OICCalloc(size_t num, size_t size)
{
    if(0 == size || 0 == num)
    {
        return NULL;
    }

#ifdef ENABLE_MALLOC_DEBUG
    void *ptr = 0;

    ptr = calloc(num, size);
    OIC_LOG_V(INFO, TAG, "calloc: ptr=%p, num=%u, size=%u", ptr, num, size);
    return ptr;
#else
    return calloc(num, size);
#endif
}

void *OICRealloc(void* ptr, size_t size)
{
    if(size == 0)
    {
        OICFree(ptr);
        return NULL;
    }

#ifdef ENABLE_MALLOC_DEBUG
    if(ptr == NULL)
    {
        return OICMalloc(size);
    }

    void* newptr = NULL;
    newptr = realloc(ptr, size);
    OIC_LOG_V(INFO, TAG, "realloc: ptr=%p, newptr=%p, size=%u", ptr, newptr, size);
    return ptr;
#else
    return realloc(ptr, size);
#endif
}

void OICFree(void *ptr)
{
#ifdef ENABLE_MALLOC_DEBUG
    OIC_LOG_V(INFO, TAG, "free: ptr=%p, count=%u", ptr, --count);
#endif

    free(ptr);
}


