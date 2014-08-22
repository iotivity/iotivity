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

#if ! defined (OCCLIENTTESTAPP__INCLUDED_)
#define OCCLIENTTESTAPP__INCLUDED_

#pragma once

#define ElementCount(a)     (sizeof(a) / sizeof(a[0]))

enum
{
    IndentSpaces = 5,
    MaxParams = 6,
    MaxResultSize = 16,
    MaxNameSize = 64,
    ResultColumn = 80,                  // Start of 'Results' Column 
    ResultIndex = ResultColumn + 1,     // 'Results' Column + 1
    MaxFunctionNameSize = 64,
};

typedef void (Test_OCFunc)(int);

typedef struct
{
    Test_OCFunc             *m_pTest;
    char                    m_FunctionName[MaxNameSize];
    char                    m_Result[MaxResultSize];

}FunctionStruct;


#endif

