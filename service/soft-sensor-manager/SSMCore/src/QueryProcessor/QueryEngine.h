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
#ifndef _QueryEngine_H_
#define _QueryEngine_H_

#include "SSMInterface/SSMCore.h"
#include "Common/PlatformLayer.h"
#include "Common/InternalInterface.h"
#include "ContextQuery.h"
#include "ConditionedQueryResult.h"

/**
* @class    CQueryEngine
* @brief    CQueryEngine Interface
*            This class manages ContextQuery.
*
* @see
*           CObject<CQueryEngine>   *pQueryEngine;
*           CreateInstance(OID_IQueryEngineInternal, (IBase**)&pQueryEngine);
*           pQeuryEngine->ExecuteContextQuery(contextquery,cqid);
*/
class CQueryEngine :
    public CObjectRoot<CObjectMultiThreadModel>
    , public IQueryEngineInternal
    , public IThreadClient
    , public IConditionedQueryEvent
{
    private:
        CObjectPtr<IPropagationEngine>      m_pPropagationEngine;
        std::map<int, IConditionedQuery *>   m_conditionedQueries;
        CSimpleMutex                        m_mtxQueries;
        int                                 m_cqid;
        IQueryEngineEvent                   *m_pQueryEngineEvent;
        std::map<int, CContextQuery *>       m_contextQueries;
        CObjectPtr<ITasker>                 m_pTasker;

    private:
        SSMRESULT processQueryResult(IN int userTriggerId, IN std::vector<result_model> *result);
        SSMRESULT validateQueryResult(IN IConditionedQueryResult *pConditionedQueryResult,
                                      OUT std::vector<result_model> *resultData);

    public:
        enum EventType {EVENT_TYPE_INNER, EVENT_TYPE_OUTER};
        SSMRESULT finalConstruct();
        void finalRelease();
        void onExecute(void *pArg);
        void onTerminate(void *pArg);

        /**
        * @fn onConditionedQueryEvent
        * @brief IConditionedQueryEvent interface's event callbacks for receiving query results
        *
        * @param [in] std::string ContextQuery - Entered ContetxQuery
        * @param [out] int* cqid - ID of ContextQuery
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT onConditionedQueryEvent(IN int userTriggerId,
                                          IN IConditionedQueryResult *pConditionedQueryResult);

        SSMRESULT queryInterface(const OID &objectID, IBase **ppObject)
        {
            if (ppObject == NULL)
                return SSM_E_POINTER;

            if (IsEqualOID(objectID, OID_IQueryEngineInternal))
            {
                IBase *pBase = (IQueryEngineInternal *)this;
                pBase->addRef();
                *ppObject = pBase;
                return SSM_S_OK;
            }

            return SSM_E_NOINTERFACE;
        }

        /**
        * @fn executeContextQuery
        * @brief Execute ContextQuery and return ContextQuery ID
        *
        * @param [in] std::string ContextQuery - Entered ContetxQuery
        * @param [out] int* cqid - ID of ContextQuery
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT executeContextQuery(IN std::string contextQuery, OUT int *cqid);

        //TODO: Registration with multiple instance support
        /**
        * @fn registerQueryEvent
        * @brief Register QueryEngineEvent to QueryEngine.
        *
        * @param [in] IQueryEngineEvent* pQueryEngineEvent - register QueryEngineEvent
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT registerQueryEvent(IN IQueryEngineEvent *pQueryEngineEvent);


        /**
        * @fn unregisterQueryEvent
        * @brief Unregister QueryEngineEvent to QueryEngine.
        *
        * @param [in] IQueryEngineEvent* pQueryEngineEvent - unregister QueryEngineEvent
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT unregisterQueryEvent(IN IQueryEngineEvent *pQueryEngineEvent);

        /**
        * @fn killContextQuery
        * @brief Kill registered ContextQuery according to cqid
        *
        * @param [in] int cqid - Context Query corresponding to the cqid will be terminated
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT killContextQuery(IN int cqid);
};

#endif /*_QueryEngine_H_*/
