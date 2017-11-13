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
#include "oic_time.h"
#include "octhread.h"
#include "experimental/logger.h"
#include "iotivity_debug.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

/**
 * TAG
 * Logging tag for module name
 */
#define TAG "OIC_EVENT"

typedef struct oc_event_t
{
    /* Mutex for protecting the members. */
    oc_mutex mutex;
    /* The conditional variable to wait on. */
    oc_cond cond;
    /* Whether the event is signaled. */
    bool signaled;
} oc_event_t;

oc_event oc_event_new(void)
{
    oc_event event = (oc_event)OICCalloc(1, sizeof(oc_event_t));
    if (!event)
    {
        OIC_LOG(ERROR, TAG, "Failed to allocate oc_event");
        return NULL;
    }

    event->mutex = oc_mutex_new();
    event->cond = oc_cond_new();
    event->signaled = false;

    if (!event->mutex || !event->cond)
    {
        oc_event_free(event);
        return NULL;
    }

    return event;
}

void oc_event_free(oc_event event)
{
    if (event)
    {
        oc_mutex_free(event->mutex);
        oc_cond_free(event->cond);
        OICFree(event);
    }
}

void oc_event_wait(oc_event event)
{
    OC_VERIFY(OC_WAIT_SUCCESS == oc_event_wait_for(event, UINT32_MAX));
}

OCWaitResult_t oc_event_wait_for(oc_event event, uint32_t milliseconds)
{
    bool timedOut = false;
    oc_mutex_assert_owner(event->mutex, false);
    oc_mutex_lock(event->mutex);

    if (!event->signaled)
    {
        if (0 != milliseconds)
        {
            const uint64_t startTime = OICGetCurrentTime(TIME_IN_MS);
            uint64_t remaining = milliseconds;
            // This while loop is to filter spurious wakeups caused by conditional variable.
            while (!event->signaled)
            {
                oc_mutex_assert_owner(event->mutex, true);
                OCWaitResult_t waitResult = oc_cond_wait_for(event->cond, event->mutex,
                                                             (remaining * US_PER_MS));
                if (OC_WAIT_TIMEDOUT == waitResult)
                {
                    timedOut = true;
                    break;
                }
                assert(OC_WAIT_SUCCESS == waitResult);

                // Not timed out, see if the event is in signaled state and reset it.
                if (event->signaled)
                {
                    timedOut = false;
                    break;
                }

                // Not timed out and not signaled => spurious wakeup, see if we ran out of time.
                const uint64_t elapsed = (OICGetCurrentTime(TIME_IN_MS) - startTime);
                if (elapsed >= (uint64_t)milliseconds)
                {
                    timedOut = true;
                    break;
                }

                // Encountered spurious wakeup and still has time to wait, recalculate the
                // remaining time and wait again.
                // Spurious wakeup: depending on the platform, waiting on a Condition Variable can
                // occasionally (though rarely) return from the wait state even when the condition
                // isn't set, so we always need to revalidate the state in a loop here.
                remaining = (uint64_t)milliseconds - elapsed;
            }
        }
        else
        {
            // Zero timeout provided and the event has not been signaled.
            timedOut = true;
        }
    }
    oc_mutex_assert_owner(event->mutex, true);
    event->signaled = false;
    oc_mutex_unlock(event->mutex);
    return timedOut ? OC_WAIT_TIMEDOUT : OC_WAIT_SUCCESS;
}

void oc_event_signal(oc_event event)
{
    oc_mutex_assert_owner(event->mutex, false);
    oc_mutex_lock(event->mutex);
    if (!event->signaled)
    {
        event->signaled = true;
        oc_cond_signal(event->cond);
    }
    oc_mutex_unlock(event->mutex);
}
