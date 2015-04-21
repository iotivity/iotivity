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
#include "DataReader.h"

CDataReader::CDataReader()
{
}

CDataReader::~CDataReader()
{
    for (std::map<std::string, std::vector<CModelData *> >::iterator itorModel =
             m_modelDataList.begin();
         itorModel != m_modelDataList.end(); ++itorModel)
    {
        for (std::vector<CModelData *>::iterator itorData = itorModel->second.begin();
             itorData != itorModel->second.end(); ++itorData)
        {
            SAFE_DELETE(*itorData);
        }
    }
}

SSMRESULT CDataReader::addModelData(std::string modelName,
                                    std::vector<CModelData *> *pModelData)
{
    m_modelDataList[modelName] = *pModelData;

    return SSM_S_OK;
}

SSMRESULT CDataReader::getAffectedModels(std::vector<std::string> *pAffectedModels)
{
    for (std::map<std::string, std::vector<CModelData *> >::iterator itorModel =
             m_modelDataList.begin();
         itorModel != m_modelDataList.end(); ++itorModel)
    {
        pAffectedModels->push_back(itorModel->first);
    }

    return SSM_S_OK;
}

SSMRESULT CDataReader::getModelDataCount(std::string modelName, int *pDataCount)
{
    std::map<std::string, std::vector<CModelData *> >::iterator itor;

    itor = m_modelDataList.find(modelName);

    if (itor == m_modelDataList.end())
    {
        return SSM_E_FAIL;
    }

    *pDataCount = itor->second.size();

    return SSM_S_OK;
}

SSMRESULT CDataReader::getModelData(std::string modelName, int dataIndex,
                                    IModelData **ppModelData)
{
    std::map<std::string, std::vector<CModelData *> >::iterator itor;

    itor = m_modelDataList.find(modelName);

    if (itor == m_modelDataList.end())
    {
        return SSM_E_FAIL;
    }

    if (dataIndex > (int)itor->second.size() - 1)
    {
        return SSM_E_FAIL;
    }

    *ppModelData = itor->second[dataIndex];

    return SSM_S_OK;
}
