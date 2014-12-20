/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
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

#ifndef BASE_SLEP_UTIL_TIME_UTIL_H_
#define BASE_SLEP_UTIL_TIME_UTIL_H_
#pragma once

#include <string>
#include "base/base_export.h"
#include "base/time.h"

// DateTimeType of version 1.1 : [yyyy]-[mm]-[dd] [hh]:[mm]:[ss]
// DateTimeType of version 1.5, 2.0 and 1.0 : [yyyy]-[mm]-[dd]T[hh]:[mm]:[ss]

namespace slep_util {
  /**
   * Converts the UNIX Time to Time Type (HH:MM:SS)
   * @param[in] double time_value
   * @return std::string
   */
  BASE_EXPORT std::string ConvertUnixTimeToTimeType(const double& time_value);
  /**
   * Converts the UNIX Time to Date-Time Type (YYYY-MM-DDTHH:MM:SS)
   * @param[in] double time_value
   * @return std::string
   */
  BASE_EXPORT std::string ConvertUnixTimeToDateTimeType(const double& time_value);
  /**
   * Converts the Time Type (HH:MM:SS) to UNIX Time
   * @param[in] std::string
   * @param[out] double unix_time
   * @return bool
   */
  BASE_EXPORT bool ConvertTimeTypeToUnixTime(const std::string& time_type, double* unix_time);
  /**
   * Converts the Date-Time Type (YYYY-MM-DDTHH:MM:SS) to UNIX Time
   * @param[in] std::string date_time_type
   * @param[out] double unix_time
   * @return bool
   */
  BASE_EXPORT bool ConvertDateTimeTypeToUnixTime(const std::string& date_time_type, double* unix_time);
  /**
   * Checks the string whether it is the Time Type (HH:MM:SS)
   * @param[in] std::string time_type
   * @return bool
   */
  BASE_EXPORT bool IsTimeType(const std::string& time_type);
  /**
   * Checks the string whether it is the Date Time Type (YYYY-MM-DDTHH:MM:SS)
   * @param[in] std::string date_time_type
   * @return bool
   */
  BASE_EXPORT bool IsDateTimeType(const std::string& date_time_type);
  /**
   * Converts the base::Time to string Date-Time Type (YYYY-MM-DDTHH:MM:SS)
   * @param[in] base::Time
   * @return std::string date_time_type
   */
  BASE_EXPORT std::string ConvertToDateTimeType(const base::Time& time);
  /**
   * Converts the std::string(YYYY-MM-DDTHH:MM:SS) to std::string Date-Time Type (YYYY-MM-DD HH:MM:SS)
   * @param[in] std::string date_time_type(YYYY-MM-DDTHH:MM:SS)
   * @return std::string date_time_type(YYYY-MM-DD HH:MM:SS)
   */
  BASE_EXPORT std::string ConvertToDateTimeType(const std::string& dataTimeString);
  /**
   * Checks if the given base::Time time has already passed
   * @param[in] base::Time time
   * @return bool
   */
  BASE_EXPORT bool IsPastTime(const base::Time& time);
  /**
   * Converts the string Date-Time Type (YYYY-MM-DDTHH:MM:SS) to base::Time time
   * @param[in] string Date-Time Type (YYYY-MM-DDTHH:MM:SS)
   * @param[out] base::Time time
   * @return void
   */
  BASE_EXPORT void StringTimeInTime(const std::string& time_string, base::Time* time);
  /**
   * Find the difference between start_time and end_time of string Date-Time Type (YYYY-MM-DDTHH:MM:SS) in seconds
   * @param[in] base::Time start_time
   * @param[in] base::Time end_time
   * @return int64 seconds
   */
  BASE_EXPORT int64 FindTimeDifferenceInSeconds(const base::Time& start_time, const base::Time& end_time);
  /**
   * Find the difference between start_time and end_time of string Date-Time Type (YYYY-MM-DDTHH:MM:SS) in minutes
   * @param[in] base::Time start_time
   * @param[in] base::Time end_time
   * @return int64 minutes
   */
  BASE_EXPORT int64 FindTimeDifferenceInMinutes(const base::Time& start_time, const base::Time& end_time);
  /**
   * Find the difference between start_time and end_time of string Date-Time Type (YYYY-MM-DDTHH:MM:SS) in days
   * @param[in] base::Time start_time
   * @param[in] base::Time end_time
   * @return int64 days
   */
  BASE_EXPORT int64 FindTimeDifferenceInDays(const base::Time& start_time, const base::Time& end_time);
  /**
   * Find the difference between start_time and end_time of string Date-Time Type (YYYY-MM-DDTHH:MM:SS) in years
   * @param[in] base::Time start_time
   * @param[in] base::Time end_time
   * @return int64 years
   */
  BASE_EXPORT int64 FindTimeDifferenceInYears(const base::Time& start_time, const base::Time& end_time);
  /**
   * Finds what day of the week it is now.
   * @return int (1-Sunday, 2-Monday,..)
   */
  BASE_EXPORT int GetDayOfWeek();
  /**
   * Checks if the given year is a leap year.
   * @return bool
   */
  BASE_EXPORT bool IsLeapYear(int year);
  /**
   * Checks if the current year(now) is a leap year.
   * @return bool
   */
  BASE_EXPORT bool GetCurrentTimeAndCheckIfLeapYear();

}  // namespace slep_util
#endif  // BASE_SLEP_UTIL_TIME_UTIL_H_