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
#include "ResourceConnectivity.h"

SSMRESULT CResourceConnectivity::finalConstruct()
{
	SSMRESULT res = SSM_E_FAIL;
    OC::PlatformConfig cfg(OC::ServiceType::InProc, OC::ModeType::Both,
            "134.134.161.33", 56831, OC::QualityOfService::NonConfirmable);
	
	m_pPlatform = new OC::OCPlatform(cfg);
	
	SSM_CLEANUP_NULL_ASSERT(m_pPlatform);
	res = SSM_S_OK;

CLEANUP:
	return res;
}

void CResourceConnectivity::finalRelease()
{
}

void* CResourceConnectivity::getPlatform()
{
	return m_pPlatform;
}
