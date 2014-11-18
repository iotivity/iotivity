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
#ifndef _ConditionedQueryResult_H_
#define _ConditionedQueryResult_H_

#include "SSMInterface/SSMCore.h"
#include "Common/PlatformLayer.h"
#include "Common/InternalInterface.h"

/**
* @class    CConditionedQueryResult
* @brief    CConditionedQueryResult Interface
*            This class represents executed CQL's result data
*
* @see
*/
class CConditionedQueryResult :
    public CObjectRoot<CObjectMultiThreadModel>
    , public IConditionedQueryResult
{
    private:
        std::vector<IConditionedModel *>         m_conditionedModels;

    public:
        SSMRESULT finalConstruct();

        void finalRelease();

        /**
        * @fn     addConditionedModel
        * @brief Add conditioned model interface for tracking trigger
        *
        * @param [in] IConditionedModel *pConditionedModel - Conditioned Model interface that contains results
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT addConditionedModel(IN IConditionedModel *pConditionedModel);

        SSMRESULT queryInterface(const OID &objectID, IBase **ppObject)
        {
            if (ppObject == NULL)
                return SSM_E_POINTER;

            if (IsEqualOID(objectID, OID_IConditionedQueryResult))
            {
                IBase *pBase = this;
                pBase->addRef();
                *ppObject = pBase;
                return SSM_S_OK;
            }

            return SSM_E_NOINTERFACE;
        }

        /**
        * @fn     getConditionedModelCount
        * @brief Get conditioned model count
        *
        * @param NONE
        *
        * @return unsigned int
        * @warning
        * @exception
        * @see
        */
        unsigned int getConditionedModelCount();

        /**
        * @fn     getConditionedContextModel
        * @brief Get conditioned model by index
        *
        * @param [in] unsigned int conditionedModelIndex - Index of Conditioned Model interface for retrieving
        * @param [out] IConditionedModel *pConditionedModel - Conditioned Model interface that contains results
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT getConditionedContextModel(IN unsigned int conditionedModelIndex,
                                             OUT IConditionedModel **ppConditionedModel);
};

#endif
