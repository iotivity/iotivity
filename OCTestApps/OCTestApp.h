//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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

