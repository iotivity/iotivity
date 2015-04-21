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
#ifndef _DataReader_H_
#define _DataReader_H_

#include "SSMInterface/SSMCore.h"
#include "Common/PlatformLayer.h"
#include "Common/InternalInterface.h"
#include "ModelData.h"
/**
* @class    CDataReader
* @brief    CDataReader Interface
*            This class represents context model data package's reader
*
* @see
*/
class CDataReader : public IDataReader
{
    private:
        std::map<std::string, std::vector<CModelData *> >    m_modelDataList;

    public:
        CDataReader();

        ~CDataReader();

        /**
        * @fn     addModelData
        * @brief Add affected model data. This method is only for constructing data.
        *
        * @param [in] std::string modelName - affected model name to add
        * @param [in] std::vector<CModelData*> *pModelData - affected model data list to add
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT addModelData(std::string modelName, std::vector<CModelData *> *pModelData);

        /**
        * @fn     getAffectedModels
        * @brief Get affected ContextModels. The CQL can specify multiple ContextModels for retrieving data.
        *
        * @param [out] std::vector<std::string> *pAffectedModels - affected ContextModel list
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT getAffectedModels(std::vector<std::string> *pAffectedModels);

        /**
        * @fn     getModelDataCount
        * @brief Get affected data count. There are multiple data can exist from given condition.
        *
        * @param [in] std::string modelName - affected ContextModel name
        *
        * @param [out] int *pDataCount - affected dataId count
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT getModelDataCount(std::string modelName, int *pDataCount);

        /**
        * @fn     getModelData
        * @brief Get actual Context Model data
        *
        * @param [in] std::string modelName - affected ContextModel name
        *
        *
        * @param [in] int dataIndex - affected dataId index
        *
        *
        * @param [out] IModelData **ppModelData - affected ContextModel data reader
        *
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT getModelData(std::string modelName, int dataIndex, IModelData **ppModelData);
};

#endif
