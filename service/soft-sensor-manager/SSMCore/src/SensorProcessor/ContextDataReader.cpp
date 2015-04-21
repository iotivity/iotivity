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
#include "ContextDataReader.h"

SSMRESULT CContextDataReader::finalConstruct()
{
    m_pContextModelAccessor = NULL;
    return SSM_S_OK;
}

void CContextDataReader::finalRelease()
{
    m_pContextModelAccessor = NULL;
}

SSMRESULT CContextDataReader::registerContextModelAccessor(IContextModelAccessor
        *pContextModelAccessor)
{
    m_pContextModelAccessor = pContextModelAccessor;

    return SSM_S_OK;
}

SSMRESULT CContextDataReader::getContextData(std::string modelName, int startIndex,
        int count, std::vector<ContextData> *data, int *pLastIndex)
{
    SSMRESULT res = SSM_E_FAIL;
    std::vector<ModelPropertyVec>   modelDataSet;
    CObjectPtr<IContextModel>   pContextModel;

    SSM_CLEANUP_ASSERT(m_pContextModelAccessor->onQueryContextModel(modelName, &pContextModel));

    SSM_CLEANUP_ASSERT(pContextModel->getModelDataSet(startIndex, count, &modelDataSet, pLastIndex));

    for (std::vector<ModelPropertyVec>::iterator itorModelPropertyVec = modelDataSet.begin();
         itorModelPropertyVec != modelDataSet.end(); ++itorModelPropertyVec)
    {
        ContextData contextData;

        contextData.rootName = pContextModel->getModelName();
        contextData.outputPropertyCount = (int)itorModelPropertyVec->size();

        for (ModelPropertyVec::iterator itor = itorModelPropertyVec->begin();
             itor != itorModelPropertyVec->end(); ++itor)
        {
            std::map<std::string, std::string> propertySet;

            switch (itor->propertyType)
            {
                case ModelProperty::TYPE_INTEGER:
                case ModelProperty::TYPE_NUMERIC:
                    propertySet["type"] = "int";
                    break;

                case ModelProperty::TYPE_REAL:
                    propertySet["type"] = "double";
                    break;

                case ModelProperty::TYPE_TEXT:
                    propertySet["type"] = "string";
                    break;

                default:
                    propertySet["type"] = "string";
            }

            propertySet["name"] = itor->propertyName;
            propertySet["value"] = itor->propertyValue;

            contextData.outputProperty.push_back(propertySet);
        }

        data->push_back(contextData);
    }

CLEANUP:
    return res;
}
