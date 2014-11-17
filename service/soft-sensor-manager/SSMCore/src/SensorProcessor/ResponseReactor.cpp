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
#include "ResponseReactor.h"

SSMRESULT CResponseReactor::finalConstruct()
{
	SSMRESULT res = SSM_S_OK;

	SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_IContextRepository, (IBase**)&m_pContextRepository));

	SSM_CLEANUP_ASSERT(CreateInstance(OID_IContextExecutor, (IBase**)&m_pContextExecutor));//TEMP

CLEANUP:
	return res;
}

void CResponseReactor::finalRelease()
{
}

void CResponseReactor::registerContext(TypeofEvent callType, ISSMResource *pSSMResource, IEvent *pEvent)
{
	m_mtxRequestedContextData.lock();
	// if already exists
	if (m_requestedCallbackData.find(pSSMResource->type) != m_requestedCallbackData.end())
	{
		//m_requestedCallbackData.erase(m_requestedCallbackData.find(nameString));
		unregisterContext(callType,pSSMResource,pEvent);
	}
	else
	{
		m_requestedCallbackData[pSSMResource->type] = CallbackData(callType, pSSMResource->type, pEvent);
	}
	
	m_pContextExecutor->registerContext(callType, pSSMResource, this);

	m_mtxRequestedContextData.unlock();
}

void CResponseReactor::unregisterContext(TypeofEvent callType, ISSMResource *pSSMResource, IEvent *pEvent)
{
	m_mtxUnregisterContext.lock();

	// if already exists
	if (m_requestedCallbackData.find(pSSMResource->type) != m_requestedCallbackData.end())
	{
		m_requestedCallbackData.erase(m_requestedCallbackData.find(pSSMResource->type));

		//and Call NextLayer
		m_pContextExecutor->unregisterContext(callType, pSSMResource, this);
	}

	m_mtxUnregisterContext.unlock();
}

void CResponseReactor::getList(std::vector<ISSMResource*> *pList)
{	
	pList->clear();

	m_pContextRepository->getSoftSensorList(pList);

	m_pContextRepository->getPrimitiveSensorList(pList);
}

//Dispatch to upper layer
int CResponseReactor::onEvent(std::string type, TypeofEvent callType, std::vector<ContextData> ctxData)
{
	std::map<std::string, CallbackData >::iterator	itor;

	//m_mtxRequestedContextData.Lock();

	itor = m_requestedCallbackData.find(type);
	if (itor != m_requestedCallbackData.end())
	{
		itor->second.m_pCallbackEvent->onEvent(type, callType, ctxData);
		if (callType == SSM_ONCE)
		{
			m_requestedCallbackData.erase(itor);
		}
	}
	//m_mtxRequestedContextData.Unlock();

	return 0;
}
