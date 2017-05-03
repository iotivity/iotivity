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

#include <limits.h>

#include "OCRepresentationInternal.h"
#include "OCException.h"
#include "StringConstants.h"

namespace OC {
    void checkValueInIntRange(int64_t int64Value)
    {
        if ((int64Value > INT_MAX) || (int64Value < INT_MIN))
        {
            throw OCException(Exception::OUT_OF_INT_RANGE);
        }
    }

    void checkValueInIntRange(const std::vector<int64_t>& int64Value)
    {
        for (int64_t value : int64Value)
        {
            checkValueInIntRange(value);
        }
    }

    void checkValueInIntRange(const std::vector<std::vector<int64_t>>& int64Value)
    {
        for (const auto& value : int64Value)
        {
            checkValueInIntRange(value);
        }
    }

    void checkValueInIntRange(const std::vector<std::vector<std::vector<int64_t>>>& int64Value)
    {
        for (const auto& value : int64Value)
        {
            checkValueInIntRange(value);
        }
    }

    void copyInt64Type(int64_t int64Value, int& intValue)
    {
        intValue = (int)int64Value;
    }

    void copyInt64Type(const std::vector<int64_t>& int64Value, std::vector<int>& intValue)
    {
        intValue.resize(int64Value.size());
        for (size_t index = 0; index < int64Value.size(); index++)
        {
            copyInt64Type(int64Value[index], intValue[index]);
        }
    }

    void copyInt64Type(const std::vector<std::vector<int64_t>>& int64Value, std::vector<std::vector<int>>& intValue)
    {
        intValue.resize(int64Value.size());
        for (size_t index = 0; index < int64Value.size(); index++)
        {
            copyInt64Type(int64Value[index], intValue[index]);
        }
    }

    void copyInt64Type(const std::vector<std::vector<std::vector<int64_t>>>& int64Value, std::vector<std::vector<std::vector<int>>>& intValue)
    {
        intValue.resize(int64Value.size());
        for (size_t index = 0; index < int64Value.size(); index++)
        {
            copyInt64Type(int64Value[index], intValue[index]);
        }
    }

    void copyIntType(int intValue, int64_t& int64Value)
    {
        int64Value = intValue;
    }

    void copyIntType(const std::vector<int>& intValue, std::vector<int64_t>& int64Value)
    {
        int64Value.resize(intValue.size());
        for (size_t index = 0; index < intValue.size(); index++)
        {
            copyIntType(intValue[index], int64Value[index]);
        }
    }

    void copyIntType(const std::vector<std::vector<int>>& intValue, std::vector<std::vector<int64_t>>& int64Value)
    {
        int64Value.resize(intValue.size());
        for (size_t index = 0; index < intValue.size(); index++)
        {
            copyIntType(intValue[index], int64Value[index]);
        }
    }

    void copyIntType(const std::vector<std::vector<std::vector<int>>>& intValue, std::vector<std::vector<std::vector<int64_t>>>& int64Value)
    {
        int64Value.resize(intValue.size());
        for (size_t index = 0; index < intValue.size(); index++)
        {
            copyIntType(intValue[index], int64Value[index]);
        }
    }
}