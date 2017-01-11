//******************************************************************
//
// Copyright 2017 Samsung Mobile Communications GmbH All Rights Reserved.
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

#ifndef TRACE_H_
#define TRACE_H_

#ifdef __ANDROID__
#include "logger.h"
#elif defined(__TIZEN__)
#include <ttrace.h>
#elif defined(ARDUINO)
#endif

#ifdef __cplusplus
extern "C"
{
#endif
#ifdef __ANDROID__

#define OIC_TRACE_BEGIN(MSG, ...) \
        oic_trace_begin("OIC:"#MSG, ##__VA_ARGS__)
#define OIC_TRACE_END() \
        oic_trace_end()

#elif defined(__TIZEN__)
/* trace macro for Tizen. this will call ttrace api internally*/
#define OIC_TRACE_BEGIN(MSG, ...) \
        traceBegin(TTRACE_TAG_APP, "IOTIVITY:"#MSG, ##__VA_ARGS__)
#define OIC_TRACE_END() \
        traceEnd(TTRACE_TAG_APP)

#elif defined(ARDUINO)
/* trace macro for Arduino. currently this will call nothing*/
#define OIC_TRACE_BEGIN(MSG, ...)
#define OIC_TRACE_END()

#else
#define OIC_TRACE_BEGIN(MSG, ...)
#define OIC_TRACE_END()

#endif //endif

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* TRACE_H_ */
