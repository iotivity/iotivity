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
#include "ContextExecutor.h"

SSMRESULT CContextExecutor::finalConstruct()
{
	SSMRESULT res = SSM_E_FAIL;

	ctxEvent = NULL;

	SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_IThreadPool, (IBase**)&m_pTaskWorker));
	SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_IContextRepository, (IBase**)&m_pContextRepository));
	SSM_CLEANUP_ASSERT(CreateGlobalInstance(OID_IContextDataReader, (IBase**)&m_pContextDataReader));

	SSM_CLEANUP_ASSERT(m_pContextRepository->registerResourceFinderEvent(this));
	
	SSM_CLEANUP_ASSERT(m_pContextRepository->startResourceFinder());

CLEANUP:
	return res;
}

void CContextExecutor::finalRelease()
{
}

void CContextExecutor::registerCallback(ICtxEvent *pEvent)
{
	ctxEvent = pEvent;
}

//Data from soft sensors
void CContextExecutor::addOutput(std::vector<ContextData> contextData)
{
	//LOGE("ADDOUTPUT");
	std::map<std::string, CallbackData>::iterator	itor;
	std::string type = contextData.at(0).rootName;

	//TODO: name must be a this soft sensors identifier
	//m_mtxRequestedContextCallback.Lock();
	itor = m_requestedContextCallback.find(type);
	if(itor != m_requestedContextCallback.end())
	{
		TypeofEvent callType = itor->second.m_callType;
		itor->second.m_pCallbackEvent->onEvent(type, callType, contextData);
		if(callType == SSM_ONCE)
		{
			unregisterContext(callType, m_registeredResources[type], this);
		}

	}
	//m_mtxRequestedContextCallback.Unlock();
}

void CContextExecutor::getDataFromDatabase(std::string modelName, int startIndex, int count, std::vector<ContextData> *data, int *pLastIndex)
{
	m_pContextDataReader->getContextData(modelName, startIndex, count, data, pLastIndex);
}

std::string CContextExecutor::checkError(std::vector<ContextData> data)
{
	std::string errorMsg = "OK";

	for(unsigned int i = 0 ; i < data.size() ; ++i)
	{
		if(data.at(i).outputProperty.at(0).find("error") != data.at(i).outputProperty.at(0).end())
		{
			errorMsg = data.at(i).outputProperty.at(0).find("error")->second ;
			break;
		}
	}

	return errorMsg;
}

void CContextExecutor::registerContext(TypeofEvent callType, ISSMResource *pSSMResource, IEvent *pEvent)
{  
	//will make check routine for One resource has callbacks.
	std::vector<ISSMResource*>	baseList;

	CallbackData callbackData(callType, pSSMResource->name, pEvent);
	
	//m_mtxRequestedContextCallback.Lock();
	m_registeredResources[pSSMResource->type] = pSSMResource;
	m_requestedContextCallback[pSSMResource->type] = callbackData;
	//m_mtxRequestedContextCallback.Unlock();

	//This is primitive sensor
	if (pSSMResource->inputList.size() == 0)
	{
		//Let sensors work
		std::vector<ContextData> inputData;
		switch (pSSMResource->location)
		{
		case SENSOR_LOCATION_LOCAL:
			runLogic(inputData, pSSMResource->type);
			break;

		case SENSOR_LOCATION_REMOTE:
			//Let observer work!
			m_pContextRepository->startObserveResource(pSSMResource, this);
			break;
		}
	}

	m_pContextRepository->getPrimitiveSensorList(&baseList);
	m_pContextRepository->getSoftSensorList(&baseList);

	for (unsigned int i = 0; i < pSSMResource->inputList.size(); ++i)
	{
		ISSMResource *pLowerResource = NULL;
		if (findString(&baseList, pSSMResource->inputList.at(i), &pLowerResource) != SSM_E_FAIL) //if element of inputList is in the lower resources.
		{
			if (m_relatedContextModel.find(pLowerResource->type) != m_relatedContextModel.end()) //already exists
			{
				//EXIST!!!!
				//check related Context needs insert or not. if high resource is not a member of related context then insert.
				std::vector<std::string> highList = m_relatedContextModel[pLowerResource->type];

				for(unsigned int j = 0 ; j < highList.size() ; ++j)
				{
					if (highList.at(j).compare(pSSMResource->type) != 0)
					{
						m_relatedContextModel[pLowerResource->type].push_back(pSSMResource->type);
						break;
					}
				}
			}
			else // If first time
			{
				//NO EXIST!!!!
				//insert resource in the all map and vector
				m_relatedContextModel[pLowerResource->type].push_back(pSSMResource->type);

				//Recursive call
				registerContext(callType, pLowerResource, this);
			}
		}
		else //unable to find installed sensors. take it to keep list
		{
			m_mapResourceLookup[pSSMResource->inputList.at(i)].push_back(CallbackData(callType, pSSMResource->type, this));
		}
	}
}

void CContextExecutor::onExecute(IN void* pArg)
{
	int			*pMessage = (int*)pArg;

	RESOURCE_EVENT_TYPE		eventType = (RESOURCE_EVENT_TYPE)pMessage[0];
	ISSMResource			*pResource = (ISSMResource*)pMessage[1];

	switch (eventType)
	{
	case SSM_EVENT_NORMAL:
		break;

	case SSM_EVENT_ADDED:
		if (m_mapResourceLookup.find(pResource->type) != m_mapResourceLookup.end())
		{
			CallbackData		*callBack = NULL;

			for (size_t i = 0; i < m_mapResourceLookup[pResource->type].size(); i++)
			{
				callBack = &m_mapResourceLookup[pResource->type].at(i);

				m_relatedContextModel[pResource->type].push_back(callBack->m_name);

				registerContext(callBack->m_callType, pResource, callBack->m_pCallbackEvent);
			}

			m_mapResourceLookup.erase(m_mapResourceLookup.find(pResource->type));
		}
		break;

	case SSM_REMOVED:
		break;

	case SSM_UPDATED:
		break;

	default:
		break;
	}
}

void CContextExecutor::onTerminate(IN void* pArg)
{
	int			*pMessage = (int*)pArg;

	delete[] pMessage;
}

int CContextExecutor::onResourceEvent(RESOURCE_EVENT_TYPE eventType, ISSMResource *pSSMResource, std::string info)
{
	int			*pMessage = new int[2];

	pMessage[0] = eventType;
	pMessage[1] = (int)pSSMResource;
	return (int)m_pTaskWorker->addTask(this, (void*)pMessage);
}

SSMRESULT CContextExecutor::findString(std::vector<ISSMResource*> *sList, const std::string str, ISSMResource **ppResource)
{	
	SSMRESULT ret = SSM_E_FAIL;
	for(unsigned int i = 0 ; i < sList->size() ; ++i)
	{
		if(sList->at(i)->type == str)
		{
			*ppResource = sList->at(i);
			ret = SSM_S_OK;
			break;
		}
	}
	return ret;
}

std::map<std::string, std::vector<ContextData> >  CContextExecutor::getPreparedContextList(std::string lowContextName)
{
	//check m_relatedContextModel / apply timestamp

	std::vector<std::string> relatedHigherContextList = m_relatedContextModel[lowContextName];
	std::map<std::string, std::vector<ContextData> > returnData;

	SSMRESULT ret = SSM_E_FAIL;

	for (unsigned int i = 0; i < relatedHigherContextList.size(); ++i)
	{
		std::string highContextName = relatedHigherContextList.at(i);
		if (m_registeredResources.find(highContextName) != m_registeredResources.end())
		{
			ret = SSM_S_OK;
			std::vector<std::string> inputList = m_registeredResources[highContextName]->inputList;
			std::vector<ContextData> contextDataList;
			for (unsigned int j = 0; j < inputList.size(); j++) //check all "inputlist" arrived or not
			{
				if (m_storedLowerContextData.find(inputList.at(j)) == m_storedLowerContextData.end())
				{
					ret = SSM_E_FAIL;
					break;
				}
				else
				{
					std::vector<ContextData> lowerData = m_storedLowerContextData[inputList.at(j)];
					for (unsigned k = 0; k < lowerData.size(); k++)
					{
						contextDataList.push_back(lowerData.at(k));
					}
				}
			}

			if (ret == SSM_S_OK)
			{
				returnData.insert(std::make_pair(highContextName, contextDataList));
			}
			contextDataList.clear();
		}
	}

	return returnData;
}

//Data from every primitive sensors
int CContextExecutor::onEvent(std::string type, TypeofEvent callType, std::vector<ContextData> ctxData)
{
	//now getting data from low level 

	//std::string root_name = ctxData.at(0).root_name; //-> deviceId+ctxData.root_name

	if (m_relatedContextModel.find(type) != m_relatedContextModel.end()) //already registered?
	{
		//update recent values(overwrite)		
		m_storedLowerContextData[type] = ctxData;

		//find high context model
		std::map<std::string, std::vector<ContextData> > readyContextList = getPreparedContextList(type);

		if (readyContextList.size() > 0)
		{
			//Run HighContext!  readyContextList has all data for run
			std::map<std::string, std::vector<ContextData> >::iterator iter = readyContextList.begin();
			for (; iter != readyContextList.end(); ++iter)
			{
				std::string highContextName = iter->first;
				std::vector<ContextData> inputData = iter->second;
				std::string errorMsg = checkError(inputData);

				if (!errorMsg.compare("OK"))
				{
					runLogic(inputData, highContextName);
				}
				else
				{
					inputData.clear();
					inputData.push_back(makeErrorContextData(highContextName, errorMsg));
					addOutput(inputData);
				}
			}
		}
	}
	else //This data is primitive
	{
		//TODO: Temporally added for primitive data testing
		addOutput(ctxData);
	}

	return SSM_S_OK;
}

void  CContextExecutor::unregisterContext(TypeofEvent callType, ISSMResource *pSSMResource, IEvent *pEvent)
{
	std::vector<ISSMResource*> baseList;	

	//This is primitive sensor
	if (pSSMResource->inputList.size() == 0)
	{
		//Let sensors work
		std::vector<ContextData> inputData;
		switch (pSSMResource->location)
		{
		case SENSOR_LOCATION_LOCAL:
			//TODO: Must free soft sensor
			if (m_libraryList.find(pSSMResource->type) != m_libraryList.end() && callType != SSM_ONCE)
			{
#ifdef WIN32
				FreeLibrary((HINSTANCE)m_libraryList[pSSMResource->type]);
#else
				dlclose(m_libraryList[pSSMResource->type]);
#endif
				m_libraryList.erase(m_libraryList.find(pSSMResource->type));
			}

			if (m_ctxEventList.find(pSSMResource->type) != m_ctxEventList.end())
			{
				m_ctxEventList.erase(m_ctxEventList.find(pSSMResource->type));
			}
			break;

		case SENSOR_LOCATION_REMOTE:
			//Let observer stop work!
			m_pContextRepository->stopObserveResource(pSSMResource);
			break;
		}
	}

	//m_mtxRequestedContextCallback.Lock();
	if (m_requestedContextCallback.find(pSSMResource->type) != m_requestedContextCallback.end())
	{
		m_requestedContextCallback.erase(m_requestedContextCallback.find(pSSMResource->type));
	}
	if (m_registeredResources.find(pSSMResource->type) != m_registeredResources.end())
	{
		m_registeredResources.erase(m_registeredResources.find(pSSMResource->type));
	}
	//m_mtxRequestedContextCallback.Unlock();

	//TODO: support multiple concurrent query reference
	m_pContextRepository->getPrimitiveSensorList(&baseList);
	m_pContextRepository->getSoftSensorList(&baseList);


	for(unsigned int i = 0 ;i < pSSMResource->inputList.size() ; ++i)
	{
		ISSMResource *pLowerResource = NULL;
		if (findString(&baseList, pSSMResource->inputList.at(i), &pLowerResource) != SSM_E_FAIL)
		{
			std::vector<std::string> *highList = &m_relatedContextModel[pLowerResource->type];
			for(unsigned int j = 0 ; j < highList->size() ; ++ j)
			{
				if (!highList->at(j).compare(pSSMResource->type))
				{
					highList->erase(highList->begin()+j);

					if(highList->size() == 0) //if highList->size() == 0, then no more related context.
					{
						m_relatedContextModel.erase(m_relatedContextModel.find(pLowerResource->type));

						//Recursive call
						unregisterContext(callType, pLowerResource, this);
						break;
					}
				}
			}
		}
	}
}

//Called when soft sensor try to work
void CContextExecutor::runLogic(std::vector<ContextData> inputData,std::string highContextName )
{	
	m_mtxLibraryIO.lock();
	if(m_ctxEventList.find(highContextName) == m_ctxEventList.end())
	{	
		if (loadModelLibrary(highContextName) == SSM_S_OK)
		{
			m_ctxEventList[highContextName] = ctxEvent;
			m_ctxEventList[highContextName]->onCtxEvent(SPF_START, inputData);
		}
	}
	else
	{
		m_ctxEventList[highContextName]->onCtxEvent(SPF_START, inputData);
	}
	m_mtxLibraryIO.unlock();
}

ContextData CContextExecutor::makeErrorContextData(std::string rootName, std::string errMsg)
{
	std::map<std::string,std::string> errorMap;
	ContextData errorContextData;
					
	errorMap.insert(std::make_pair("error",errMsg));
	errorMap.insert(std::make_pair("available","false"));
	errorContextData.rootName = rootName;
	errorContextData.outputPropertyCount = 2;
	errorContextData.outputProperty.push_back(errorMap);
					
	return errorContextData;
}

SSMRESULT CContextExecutor::loadModelLibrary(std::string modelName)
{
	std::stringstream	sstream;
	SSMRESULT ret = SSM_E_FAIL;

	typedef void (*InitContext)(IN ICtxDelegate *);
	InitContext InitializeContextFunction = NULL;

	// load dll(so)
	for(unsigned int i = 1 ; i <= SSM_MODEL_RETRY; ++i)
	{
		sstream.str("");
	
#ifdef WIN32
		sstream << MODEL_DIRECTORY << modelName.c_str() << ".dll" << std::ends;
		
		HINSTANCE m_hdll_MyModule = NULL;			
		m_hdll_MyModule = LoadLibraryA(sstream.str().c_str());

		if(m_hdll_MyModule != NULL)
		{
			InitializeContextFunction = (InitContext)GetProcAddress(m_hdll_MyModule, "InitializeContext");
		}
#else
		//sstream << "/data/data/com.example.javaproject/lib/lib" << modelName <<".so" << std::ends;
		sstream << MODEL_DIRECTORY << "lib" << modelName.c_str() <<".so" << std::ends;

		void* m_hdll_MyModule = NULL;
		m_hdll_MyModule = dlopen(sstream.str().c_str(), RTLD_LOCAL | RTLD_LAZY);

		if(m_hdll_MyModule != NULL)
		{
			InitializeContextFunction = (InitContext)dlsym(m_hdll_MyModule, "InitializeContext");
		}
#endif
		if (m_hdll_MyModule == NULL)
		{
			InitializeContextFunction = NULL;
			continue;
		}

		if (InitializeContextFunction != NULL)
		{
			InitializeContextFunction(this);
			m_libraryList[modelName] = m_hdll_MyModule;
			ret = SSM_S_OK;
		}
		else
		{
#ifdef WIN32
			FreeLibrary((HINSTANCE)m_hdll_MyModule);
#else
			dlclose(m_hdll_MyModule);
#endif
			ret = SSM_E_FAIL;
		}

		break;
	}

	return ret;
}

