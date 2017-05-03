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

#include <vector>
#include <stdint.h>

namespace OC {

   /**
    *  Checks if passed 'int64_t' values are within a regular 'int' range.
    *
    *  @param[in]   int64Value An 'int64_t' variable or a vector containing such values.
    *
    *  @throws OCException if at least one verified value is outside of 'int' range.
    */
    void checkValueInIntRange(int64_t int64Value);
    void checkValueInIntRange(const std::vector<int64_t>& int64Value);
    void checkValueInIntRange(const std::vector<std::vector<int64_t>>& int64Value);
    void checkValueInIntRange(const std::vector<std::vector<std::vector<int64_t>>>& int64Value);

   /**
    *  Copies 'int64_t' values into an 'int' variable.
    *
    *  @param[in]   int64Value  An 'int64_t' variable or a vector containing such values.
    *  @param[out]  intValue    An 'int' variable or a vector containing such values.
    */
    void copyInt64Type(int64_t int64Value, int& intValue);
    void copyInt64Type(const std::vector<int64_t>& int64Value, std::vector<int>& intValue);
    void copyInt64Type(const std::vector<std::vector<int64_t>>& int64Value, std::vector<std::vector<int>>& intValue);
    void copyInt64Type(const std::vector<std::vector<std::vector<int64_t>>>& int64Value, std::vector<std::vector<std::vector<int>>>& intValue);

   /**
    *  Copies 'int' values into an 'int64_t' variable.
    *
    *  @param[in]   intValue    An 'int' variable or a vector containing such values.
    *  @param[out]  int64Value  An 'int64_t' variable or a vector containing such values.
    */
    void copyIntType(int intValue, int64_t& int64Value);
    void copyIntType(const std::vector<int>& intValue, std::vector<int64_t>& int64Value);
    void copyIntType(const std::vector<std::vector<int>>& intValue, std::vector<std::vector<int64_t>>& int64Value);
    void copyIntType(const std::vector<std::vector<std::vector<int>>>& intValue, std::vector<std::vector<std::vector<int64_t>>>& int64Value);
}