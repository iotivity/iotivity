//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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

#ifndef __OC_OCDEMO_UTILITY_H
 #define __OC_OCDEMO_UTILITY_H

#include <map>
#include <tuple>
#include <chrono>
#include <ostream>

#include "OCApi.h"

namespace Intel { namespace OCDemo {

/* A static observation counter: */
int observe_count();

/* Helpers for measuring request times: */
typedef std::pair<
                    std::chrono::time_point<std::chrono::high_resolution_clock>,
                    std::chrono::time_point<std::chrono::high_resolution_clock>
                 > clock_interval;

struct call_times
{
 public:
 bool display_reports;

 public:
 std::map<std::string, clock_interval> timings;

 public:
 call_times()
  : display_reports(true)
 {}

 call_times(const bool& display_reports_)
  : display_reports(display_reports_)
 {}

 public:
 void reset(const std::string& entry);
 void mark(const std::string& name);

 void report();
 void report_and_reset(const std::string& name);
};

extern call_times call_timer;

}} // namespace Intel::OCDemo

#endif
