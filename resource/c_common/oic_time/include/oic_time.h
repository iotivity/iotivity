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

/**
 * @file
 * Support for working with elapsed time.
 */

#ifndef OIC_TIME_H_
#define OIC_TIME_H_

#include <stdint.h>

/**
 * @name
 * Useful constants for time unit conversions.
 *
 * @{
 */
#define MS_PER_SEC  (1000)
#define US_PER_SEC  (1000000)
#define US_PER_MS   (1000)
#define NS_PER_US   (1000)
#define NS_PER_MS   (1000000)
#define HNS_PER_US  (10)
/** @} */


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef enum
{
    TIME_IN_MS = 0,     //!< milliseconds
    TIME_IN_US,         //!< microseconds
} OICTimePrecision;

/**
 * Get the current time using the specified precision.
 *
 * Return the time in units of `precision`.
 * If the implementation supports a monotonic clock, then
 * the returned value will be from the monotonic clock,
 * and the difference between two retrieved times can be considered
 * an accurate elapsed time. The time base is unspecified
 * in this case. If a monotonic clock is not supported,
 * the time returned will be `precision` time units since the epoch,
 * without adjustment for any external changes to the clock.
 *
 * @param     precision   based on this parameter, current time is
 * returned in milliseconds or microseconds
 *
 * @warning   This function may be sensitive to system time changes on
 * platforms which do not support a monotonic clock.
 *
 * @return
 *         Returns current time in milliseconds or microseconds.
 */
uint64_t OICGetCurrentTime(OICTimePrecision precision);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // OIC_TIME_H_
