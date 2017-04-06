//******************************************************************
//
// Copyright 2017 Samsung Electronics All Rights Reserved.
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
#include "CHPHelper.h"


CHPHelper *CHPHelper::s_pchpHelper = NULL;

CHPHelper::CHPHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[CHPHelper] IN");
    m_secureFlag = 1;
    m_nonsecureFlag = 0;
}

CHPHelper *CHPHelper::getInstance(void)
{
    if (s_pchpHelper == NULL)
        s_pchpHelper = new CHPHelper();
    return s_pchpHelper;
}

CHPHelper::~CHPHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[CHPHelper] OUT");
    delete s_pchpHelper;
}

OCStackResult  CHPHelper::initializeOC()
{
    OCStackResult ret;
    __FUNC_IN__
    ret = OCInit(NULL, 0, OC_SERVER);
    __FUNC_OUT__
    return ret;
}

OCStackResult  CHPHelper::stopOC()
{
    OCStackResult ret;
    __FUNC_IN__
    ret = OCStop();
    __FUNC_OUT__
    return ret;
}

OCStackResult CHPHelper::initializeCHPNonSecure()
{

    OCStackResult ret;
    __FUNC_IN__
    ret = CHPInitialize(m_nonsecureFlag);
    __FUNC_OUT__
    return ret;
}
OCStackResult CHPHelper::initializeCHPSecure()
{
    OCStackResult ret;
    __FUNC_IN__
    ret = CHPInitialize(m_secureFlag);
    __FUNC_OUT__
    return ret;
}
OCStackResult CHPHelper::terminateCHP()
{
    OCStackResult ret;
    __FUNC_IN__
    ret = CHPTerminate();
    __FUNC_OUT__
    return ret;
}
bool CHPHelper::isInitializedCHP()
{
    bool ret;
    __FUNC_IN__
    ret = CHPIsInitialized();
    __FUNC_OUT__
    return ret;
}
