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
#ifndef _ContextDataReader_H_
#define _ContextDataReader_H_

#include "SSMInterface/SSMCore.h"
#include "Common/PlatformLayer.h"
#include "Common/InternalInterface.h"

/**
* @class    CContextDataReader
* @brief    CContextDataReader Interface
*            This class represents ContextData reader
*
* @see
*/
class CContextDataReader :
    public CObjectRoot<CObjectMultiThreadModel>
    , public IContextDataReader
{
    private:
        IContextModelAccessor       *m_pContextModelAccessor;

    public:
        SSMRESULT finalConstruct();

        void finalRelease();

        SSMRESULT queryInterface(const OID &objectID, IBase **ppObject)
        {
            if (ppObject == NULL)
                return SSM_E_POINTER;

            if (IsEqualOID(objectID, OID_IContextDataReader))
            {
                IBase *pBase = this;
                pBase->addRef();
                *ppObject = pBase;
                return SSM_S_OK;
            }

            return SSM_E_NOINTERFACE;
        }

        /**
        * @fn     registerContextModelAccessor
        * @brief Called when ContextModelAccessor wants to query installed ContextModel
        *
        * @param [in] IContextModelAccessor *pContextModelAccessor - Instance of implemented IContextModelAccessor interface
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT registerContextModelAccessor(IContextModelAccessor *pContextModelAccessor);

        /**
        * @fn     getContextData
        * @brief Retrieve ContextData using given ContextModel name and dataId
        *
        * @param [in] std::string modelName - ContextModel name
        * @param [in] int startIndex - Starting index of model data
        * @param [in] int count - Number of data to retrieve
        * @param [out] std::vector<ContextData> *data - ContextData set
        * @param [out] int *pLastIndex - Index of last data
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT getContextData(std::string modelName, int startIndex, int count,
                                 std::vector<ContextData> *data, int *pLastIndex);
};
#endif
