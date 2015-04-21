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
#include "ContextQuery.h"

result_model::result_model()
{
    modelID = 0;
}

CContextQuery::CContextQuery()
{
}

SSMRESULT CContextQuery::initialize(Token &input_root)
{
    m_root = input_root;
    return CreateGlobalInstance(OID_IPropagationEngine, (IBase **)&m_pPropagationEngine);
}

std::string CContextQuery::search_last_modelName(Token *temp)
{
    while (true)
    {
        if (temp->child_token.size() > 0)
        {
            temp = &(temp->child_token.at(0));
        }
        else
        {
            return temp->name;
        }
    }
}

void CContextQuery::integrate_result(std::vector<result_model> *result, int modelId,
                                     std::vector<int> *dataid, std::string modelName)
{
    bool flag = false;

    for (unsigned int i = 0 ; i < result->size() ; i++)
    {
        if (result->at(i).modelID == modelId)
        {
            flag = true;
            for (unsigned int k = 0 ; k < result->at(i).dataId.size() ; k++)
            {
                bool flag1 = false;
                for (unsigned int l = 0 ; l < dataid->size() ; l++)
                {
                    if (result->at(i).dataId.at(k) == dataid->at(l))
                    {
                        flag1 = true;
                        break;
                    }
                }
                if (flag1 == false)
                {
                    result->at(i).dataId.erase(result->at(i).dataId.begin() + k);
                    k--;
                }
            }
        }
    }

    if (flag == false)
    {
        result_model temp;
        temp.modelID = modelId;
        temp.dataId = *dataid;
        temp.modelName = modelName;
        result->push_back(temp);
    }
}



void CContextQuery::return_contextName(std::vector<std::string> *contextName)
{
    int k = m_root.child_token.at(0).child_token.size();

    for (int i = 0; i < k; i++)
    {
        Token *temp = &(m_root.child_token.at(0).child_token.at(i));
        contextName->push_back(search_last_modelName(temp));
    }

}

void CContextQuery::check_result_model()
{
    if (m_root.child_token.at(0).child_token.size() <= 0)
    {
        int k = m_root.child_token.at(1).child_token.size();
        std::vector<std::string> compare_string;
        bool compare_flag = true;
        std::string ModelName = "";
        unsigned int min_cnt = MIN_NUM;

        for (int i = 0 ; i < k; i++)
        {
            Token *temp = &(m_root.child_token.at(1).child_token.at(i));
            std::string model_name;

            if (temp->type == Context)
            {
                while (true)
                {
                    if (temp->child_token.size() > 0)
                    {
                        model_name += temp->name + ".";
                        temp = &(temp->child_token.at(0));
                    }
                    else
                    {
                        model_name += temp->name;
                        break;
                    }
                }

                compare_string.push_back(model_name);
                if (min_cnt > model_name.size())
                {
                    min_cnt = model_name.size();
                }
            }

        }

        unsigned int cnt = 0;

        while (true)
        {
            if ((unsigned int)cnt > min_cnt - 1)
            {
                break;
            }

            char char_temp = 0;

            for (unsigned int i = 0 ; i < compare_string.size(); i++)
            {
                if ( i == 0)
                {
                    char_temp = compare_string.at(i)[cnt];
                    compare_flag = true;
                }
                else
                {
                    if (char_temp != compare_string.at(i)[cnt])
                    {
                        compare_flag = false;
                        break;
                    }
                }
            }

            if (compare_flag == true)
            {
                ModelName += char_temp;
                cnt++;
            }
            else if (compare_flag == false)
            {
                break;
            }

        }


        std::string result_modelname = "";

        for (int i = (int)ModelName.size() - 1 ; i > -1 ; i--)
        {
            if (ModelName[i] != '.' || (unsigned int)i != ModelName.size() - 1)
            {
                result_modelname.insert(0, 1, ModelName[i]);
            }
        }

        if (result_modelname.size() > 0)
        {
            Token temp_token;
            temp_token.name = result_modelname;
            m_root.child_token.at(0).child_token.push_back(temp_token);
        }
    }
}

void CContextQuery::return_modelID(std::vector<int> *vector_int)
{
    int k = m_root.child_token.at(0).child_token.size();

    IContextModel *pContextModel = NULL;
    int pModel = 0;

    for (int i = 0; i < k; i++)
    {
        Token *temp = &(m_root.child_token.at(0).child_token.at(i));
        if (m_pPropagationEngine->getContextModel(search_last_modelName(temp),
                &pContextModel) == SSM_S_OK)
        {
            pModel = pContextModel->getModelId();
            vector_int->push_back(pModel);
        }
        SAFE_RELEASE(pContextModel);
    }
}

void CContextQuery::make_QueryCondition(QueryCondition *result)
{

    if (m_root.child_token.size() < 2)
    {
        ModelConditionVec modelcondition;
        ModelCondition model_data;
        int dataId = 0;

        for (unsigned int i = 0 ; i < m_root.child_token.at(0).child_token.size(); i++)
        {
            Token *temp_token = &(m_root.child_token.at(0).child_token.at(i));
            std::string temp_name;

            while (true)
            {
                if (temp_token->child_token.size() > 0)
                {
                    temp_token = &(temp_token->child_token.at(0));
                    continue;
                }
                else
                {
                    dataId = temp_token->number;
                    temp_name = temp_token->name;
                }

                modelcondition.clear();

                if (dataId == INDEX_ALL)
                {
                    ///m_pPropagationEngine->GetContextModel(temp_name,&contextmodel);
                    //contextmodel->GetModelSchema(modelpropertyVec);
                    model_data.modelProperty.propertyName = "dataId";
                    model_data.predicate = ModelCondition::PREDICATE_NEQ;
                    model_data.modelProperty.propertyType = ModelProperty::TYPE_INTEGER;
                    model_data.modelProperty.propertyValue = "0";
                    modelcondition.push_back(model_data);
                    result->push_back(std::pair<std::string, ModelConditionVec>(temp_name, modelcondition));
                }
                else if (dataId == INDEX_THIS)
                {

                }
                else
                {
                    std::stringstream sstream;
                    model_data.modelProperty.propertyName = "dataId";
                    model_data.predicate = ModelCondition::PREDICATE_EQ;
                    model_data.modelProperty.propertyType = ModelProperty::TYPE_INTEGER;
                    sstream << dataId << std::ends;
                    model_data.modelProperty.propertyValue = sstream.str();
                    modelcondition.push_back(model_data);
                    result->push_back(std::pair<std::string, ModelConditionVec>(temp_name, modelcondition));

                }

                break;
            }// end while
        }//end for
    }//end if
    else
    {
        int k = m_root.child_token.at(1).child_token.size();
        std::string model_name = "";

        std::map<std::string, ModelConditionVec> modelConditionList;

        for (int i = 0; i < k; i++)
        {
            Token *temp = &(m_root.child_token.at(1).child_token.at(i));

            if (temp->type == Context)
            {
                while (1)
                {
                    if (temp->child_token.size() > 0)
                    {
                        //  model_name[i/3] = temp->name + ".";
                        temp = &(temp->child_token.at(0));
                    }
                    else
                    {
                        model_name = temp->name;
                        break;
                    }
                }

                ModelCondition model_data;
                model_data.predicate = m_root.child_token.at(1).child_token.at(i + 1).condition;
                model_data.modelProperty.propertyName = temp->model_property.propertyName;
                model_data.modelProperty.propertyType = temp->model_property.propertyType;
                model_data.modelProperty.propertyValue = temp->model_property.propertyValue;
                modelConditionList[model_name].push_back(model_data);
            }
        }

        for (std::map<std::string, ModelConditionVec>::iterator itor = modelConditionList.begin();
             itor != modelConditionList.end(); ++itor)
        {
            result->push_back(std::pair<std::string, ModelConditionVec>(itor->first,
                              itor->second));
        }
    }
}
