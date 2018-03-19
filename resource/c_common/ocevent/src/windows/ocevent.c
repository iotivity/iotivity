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
 * This file implements Event object for allowing threads to wait on.
 */

#include "ocevent.h"
#include "oic_malloc.h"
#include "experimental/logger.h"
#include "iotivity_debug.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <windows.h>

/**
 * TAG
 * Logging tag for module name
 */
#define TAG "OIC_EVENT"

typedef struct oc_event_t
{
    /* The event handle */
    HANDLE event;
} oc_event_t;

oc_event oc_event_new()
{
    oc_event event = (oc_event)OICCalloc(1, sizeof(oc_event_t));
    if (!event)
    {
        OIC_LOG(ERROR, TAG, "Failed to allocate oc_event");
        return NULL;
    }

    /* Create an auto-reset event */
    event->event = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!event->event)
    {
        oc_event_free(event);
        return NULL;
    }

    return event;
}

void oc_event_free(oc_event event)
{
    if (event && event->event)
    {
        OC_VERIFY(0 != CloseHandle(event->event));
    }
    OICFree(event);
}

void oc_event_wait(oc_event event)
{
    OC_VERIFY(OC_WAIT_SUCCESS == WaitForSingleObject(event->event, INFINITE));
}

OCWaitResult_t oc_event_wait_for(oc_event event, uint32_t milliseconds)
{
    DWORD waitResult = WaitForSingleObject(event->event, milliseconds);
    switch (waitResult)
    {
    case WAIT_OBJECT_0:
        return OC_WAIT_SUCCESS;
    case WAIT_TIMEOUT:
        return OC_WAIT_TIMEDOUT;
    default:
        return OC_WAIT_INVAL;
    }
}

void oc_event_signal(oc_event event)
{
    OC_VERIFY(0 != SetEvent(event->event));
}